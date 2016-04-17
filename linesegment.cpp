#include "linesegment.h"

LineSegment::LineSegment(QVector<PointCAM *> controlPoints)
{
    if(controlPoints.size()==0){
        throw std::logic_error("empty control points");
    }
    control.append(controlPoints[0]);
    control.append(controlPoints[controlPoints.size()>1]);
}

const QList<PointCAM*> LineSegment::children() const
{
    return control;
}

void LineSegment::setUniforms(QOpenGLShaderProgram &shader, SceneData sdata)
{
    typedef GLfloat  (*parr44)[4][4];
    shader.setUniformValue("transform",*(parr44)&sdata.worldMat);
    shader.setUniformValue("perspective",*(parr44)&sdata.viewMat);
    shader.setUniformValue("color",sdata.color);
    shader.setUniformValue("cameraPosZ",sdata.cameraPosZ);
}

void LineSegment::setAttributes(QOpenGLShaderProgram &shader)
{
    shader.setAttributeBuffer("vertex", GL_FLOAT, 0*sizeof(float),
                              4, 4*sizeof(float));
}

void LineSegment::preprocessor(QOpenGLBuffer &vBuf,
                               QOpenGLBuffer &iBuf,
                               SceneData sdata)
{

}

using namespace glm;
#define GLSL(version, shader)  "#version " #version "\n" #shader

const QString LineSegment::VSH = GLSL(330,
    in vec4 vertex;
    uniform mat4 transform;

    void main (void)
    {
        gl_Position = transform*vertex;
    }
);
const QString LineSegment::GSH = GLSL(330,
    layout(lines) in;
    layout(line_strip, max_vertices = 2) out;
    uniform mat4 perspective;
    uniform float cameraPosZ;

    void postCulledSegment (vec4 v0, vec4 v1)
    {

        float r = cameraPosZ;
        float r99 = r*0.99;

        if (v0.z >= r99 && v1.z >= r99)
            return;
        /**/
        float t = (r99 - v0.z)/(v1.z - v0.z);
    /**/
        if (v0.z >= r99){
            gl_Position = v0 + t*(v1-v0);
        }else{
            gl_Position = v0;
        }
        gl_Position = perspective*gl_Position;
        gl_Position = gl_Position/gl_Position.w;
        EmitVertex();
    /**/
        if (v1.z >= r99){
            gl_Position = v0 + t*(v1-v0);
        }else{
            gl_Position = v1;
        }
        gl_Position = perspective*gl_Position;
        gl_Position = gl_Position/gl_Position.w;
        EmitVertex();
    /**/
        EndPrimitive();
    }

    void main (void)
    {
        vec4 v0 = gl_in[0].gl_Position;
        vec4 v1 = gl_in[1].gl_Position;
        postCulledSegment(v0,v1);
    }

);
const QString LineSegment::FSH = GLSL(330,
    layout(location = 0, index = 0) out vec4 fragColor;
    uniform vec4 color;
    void main (void)
    {
        fragColor = color;
    }
);

