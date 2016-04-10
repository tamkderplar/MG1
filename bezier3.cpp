#include "bezier3.h"

Bezier3::Bezier3(QVector<PointCAM *> controlPoints)
{
    if(controlPoints.size()==0){
        throw std::logic_error("empty control points");
    }
    for(int i=0;i<controlPoints.size() && i<4;++i)
        control.append(controlPoints[i]);
    for(int i=controlPoints.size();i<4;++i)
        control.append(controlPoints.last());
}

QList<const PointCAM *> Bezier3::children() const
{
    return control;
}

void Bezier3::setUniforms(QOpenGLShaderProgram &shader, SceneData sdata)
{
    typedef GLfloat (*parr44)[4][4];
    shader.setUniformValue("transform",*(parr44)&sdata.worldMat);
    shader.setUniformValue("perspective",*(parr44)&sdata.viewMat);
    shader.setUniformValue("color",sdata.color);
    shader.setUniformValue("cameraPosZ",sdata.cameraPosZ);
}

void Bezier3::setAttributes(QOpenGLShaderProgram &shader)
{
    shader.setAttributeBuffer("vertex", GL_FLOAT, 0*sizeof(float),
                              4, 4*sizeof(float));
}

void Bezier3::setTextures(QOpenGLShaderProgram &shader)
{

}

void Bezier3::preprocessor(QOpenGLBuffer& vBuf,
                           QOpenGLBuffer& iBuf,
                           SceneData sdata)
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
    std::function<void(glm::mat4,QVector<glm::vec4>&)> splitUntil128;
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
        //qWarning() << maxX <<" "<<minX<<" "<<maxY<<" "<<minY;
        //do not draw if out of bounds
        if(maxX < -1.0f || minX > 1.0f ||
                maxY < -1.0f || minY > 1.0f){
            return;
        }
        //split if too long
        if((maxX-minX)*viewport[2]>128 || (maxY-minY)*viewport[3]>128)
        {
            glm::vec4 vv[5] = { (vs[0]+vs[1])/2.0f,
                                (vs[0]+2.0f*vs[1]+vs[2])/4.0f,
                                (vs[0]+3.0f*vs[1]+3.0f*vs[2]+vs[3])/8.0f,
                                (vs[1]+2.0f*vs[2]+vs[3])/4.0f,
                                (vs[2]+vs[3])/2.0f};
            splitUntil128({vs[0],vv[0],vv[1],vv[2]},out);
            splitUntil128({vv[2],vv[3],vv[4],vs[3]},out);
        }
        //else append and leave
        out.append(vs[0]);
        out.append(vs[1]);
        out.append(vs[2]);
        out.append(vs[3]);
    };

    QVector<glm::vec4> out;
    for(int i=0;i<curveCount;++i){
        glm::vec4 v0 = vertices[indices[i*4+0]];
        glm::vec4 v1 = vertices[indices[i*4+1]];
        glm::vec4 v2 = vertices[indices[i*4+2]];
        glm::vec4 v3 = vertices[indices[i*4+3]];
        splitUntil128(glm::mat4(v0,v1,v2,v3),out);
    }
    iBuf.unmap();
    vBuf.unmap();

    for(int i=0;i<out.size();++i){
        //isubBuffer.write(i*sizeof(int),&i,sizeof(int));
    }
    //int vsubsize = subBuffer.size();
    //int isubsize = isubBuffer.size();
    //qWarning() << "vsubsize:" <<vsubsize<< "   isubsize:" <<isubsize;
    iBuf.bind();
    vBuf.bind();
}


using namespace glm;
#define GLSL(version, shader)  "#version " #version "\n" #shader

const QString Bezier3::VSH = GLSL(330,
    in vec4 vertex;
    uniform mat4 transform;

    void main (void)
    {
        gl_Position = transform*vertex;
    }
);
const QString Bezier3::GSH = GLSL(330,
    layout(lines_adjacency) in;
    layout(line_strip, max_vertices = 256) out;
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
        vec4 v2 = gl_in[2].gl_Position;
        vec4 v3 = gl_in[3].gl_Position;
        vec4 v = v0;
        for(int i=1;i<=128;++i){
            float t = i/float(128);
            float u = 1-t;
            vec4 vv = v0*(u*u*u) + v1*(3*u*u*t) + v2*(3*u*t*t) + v3*(t*t*t);
            postCulledSegment(v,vv);
            v = vv;
        }
    }

);
const QString Bezier3::FSH = GLSL(330,
    layout(location = 0, index = 0) out vec4 fragColor;
    uniform vec4 color;
    void main (void)
    {
        fragColor = color;
    }
);
