#include "bsplinebasis.h"

int BSplineBasis::instanceSplit = 1;

BSplineBasis::BSplineBasis(QVector<PointCAM*> controlPoints)
{
    if(controlPoints.size()==0){
        throw std::logic_error("empty control points");
    }
    for(int i=0;i<controlPoints.size() && i<4;++i)
        control.append(controlPoints[i]);
    for(int i=controlPoints.size();i<4;++i)
        control.append(controlPoints.last());
}

const QList<PointCAM *> BSplineBasis::children() const
{
    return control;
}

void BSplineBasis::setUniforms(QOpenGLShaderProgram &shader, SceneData sdata)
{
    typedef GLfloat (*parr44)[4][4];
    shader.setUniformValue("split",instanceSplit);
    shader.setUniformValue("transform",*(parr44)&sdata.worldMat);
    shader.setUniformValue("perspective",*(parr44)&sdata.viewMat);
    shader.setUniformValue("color",sdata.color);
    shader.setUniformValue("cameraPosZ",sdata.cameraPosZ);
}

void BSplineBasis::setAttributes(QOpenGLShaderProgram &shader)
{
    shader.setAttributeBuffer("vertex", GL_FLOAT, 0*sizeof(float),
                              4, 4*sizeof(float));
}

void BSplineBasis::setTextures(QOpenGLShaderProgram &)
{

}

void BSplineBasis::preprocessor(QOpenGLBuffer &vBuf, QOpenGLBuffer &iBuf, SceneData sdata)
{
    int* indices = (int*)(
                iBuf.map(QOpenGLBuffer::ReadWrite));
    int isize = iBuf.size()/sizeof(int);
    int curveCount = isize/4;
    glm::vec4* vertices = (glm::vec4*)(
                vBuf.map(QOpenGLBuffer::ReadWrite));
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    glm::mat4 fullMat = sdata.viewMat*sdata.worldMat;
    std::function<int(glm::mat4,QVector<glm::vec4>&)> splitUntil128;
    splitUntil128 = [&splitUntil128,&viewport,&fullMat](
            glm::mat4 vs,
            QVector<glm::vec4>&out){
        float maxX,minX,maxY,minY;
        glm::mat4 v = fullMat*vs;
        for(int i=0;i<4;++i)v[i]/=v[i].w;
        maxX = glm::max(glm::max(v[0].x,v[1].x),glm::max(v[2].x,v[3].x));
        maxY = glm::max(glm::max(v[0].y,v[1].y),glm::max(v[2].y,v[3].y));
        minX = glm::min(glm::min(v[0].x,v[1].x),glm::min(v[2].x,v[3].x));
        minY = glm::min(glm::min(v[0].y,v[1].y),glm::min(v[2].y,v[3].y));
        if(maxX < -1.0f || minX > 1.0f ||
                maxY < -1.0f || minY > 1.0f){
            return 1;
        }
        return 1+int((maxX-minX)*viewport[2]+(maxY-minY)*viewport[3])/128;
    };

    int splitCount=0;
    QVector<glm::vec4> out;
    for(int i=0;i<curveCount;++i){
        glm::vec4 v0 = vertices[indices[i*4+0]];
        glm::vec4 v1 = vertices[indices[i*4+1]];
        glm::vec4 v2 = vertices[indices[i*4+2]];
        glm::vec4 v3 = vertices[indices[i*4+3]];
        splitCount = qMax(splitCount,
                          splitUntil128(glm::mat4(v0,v1,v2,v3),out));
    }
    iBuf.unmap();
    vBuf.unmap();
    instanceSplit = splitCount;
    iBuf.bind();
    vBuf.bind();
}



using namespace glm;
#define GLSL(version, shader)  "#version " #version "\n" #shader

const QString BSplineBasis::VSH = GLSL(330,
    in vec4 vertex;
    uniform mat4 transform;
    out int instanceID;

    void main (void)
    {
        instanceID = gl_InstanceID;
        gl_Position = transform*vertex;
    }
);
const QString BSplineBasis::GSH = GLSL(330,
    layout(lines_adjacency) in;
    layout(line_strip, max_vertices = 256) out;
    uniform mat4 perspective;
    uniform float cameraPosZ;
    uniform int split;
    in int instanceID[];

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
        vec4 v2 = gl_in[2].gl_Position;
        vec4 v3 = gl_in[3].gl_Position;
        float t0 = instanceID[0]/float(split); float u0 = 1-t0;
        vec4 v = v0*(u0*u0*u0/6) + v1*(2.0/3.0-u0*t0*t0-0.5*t0*t0*t0) + v2*(2.0/3.0-0.5*u0*u0*u0-u0*u0*t0) + v3*(t0*t0*t0/6);
        for(int i=1;i<=128;++i){
            float t = (instanceID[0]*128+i)/float(split*128);
            float u = 1-t;
            vec4 vv = v0*(u*u*u/6) + v1*(2.0/3.0-u*t*t-0.5*t*t*t) + v2*(2.0/3.0-0.5*u*u*u-u*u*t) + v3*(t*t*t/6);
            postCulledSegment(v,vv);
            v = vv;
        }
    }

);
const QString BSplineBasis::FSH = GLSL(330,
    layout(location = 0, index = 0) out vec4 fragColor;
    uniform vec4 color;
    void main (void)
    {
        fragColor = color;
    }
);
