#include "pointcam.h"
#include "glmanager.h"

PointCAM::PointCAM(GLManager *manager,QObject *parent)
    :QObject(parent)
    ,
      man(manager)
{
}

PointCAM::PointCAM(glm::vec3 p,GLManager *manager, QObject *parent)
    :QObject(parent)
    ,
      man(manager)
    ,position(glm::vec4(p,1))
{
}

void PointCAM::setPos(glm::vec3 p)
{
    position = glm::vec4(p,1);
    man->updateData(this,position,0);
}

glm::vec4 PointCAM::pos() const
{
    return position;
}

QList<const PointCAM *> PointCAM::children() const
{
    return QList<const PointCAM *>({this});
}

/*constexpr GLDrawableID PointCAM::id()
{
    return 0;
}

constexpr int PointCAM::ibufferSize()
{
    return sizeof(int);
}

constexpr GLenum PointCAM::glmode()
{
    return GL_POINTS;
}*/

void PointCAM::setUniforms(QOpenGLShaderProgram &shader,SceneData sdata)
{
    typedef GLfloat (*parr44)[4][4];
    shader.setUniformValue("transform",*(parr44)&sdata.worldMat);
    glm::mat4 invWorldMat = glm::inverse(sdata.worldMat);
    shader.setUniformValue("invTransform",*(parr44)&invWorldMat);
    shader.setUniformValue("perspective",*(parr44)&sdata.viewMat);
    shader.setUniformValue("color",sdata.color);
    shader.setUniformValue("cameraPosZ",sdata.cameraPosZ);
}

void PointCAM::setAttributes(QOpenGLShaderProgram &shader)
{
    shader.setAttributeBuffer("vertex", GL_FLOAT, 0, 4, 4*sizeof(float));
    shader.enableAttributeArray("vertex");
}

#include <QOpenGLTexture>

void PointCAM::setTextures(QOpenGLShaderProgram &shader)
{
    //nothing for points
    //used for 1d textures as Bernstein basis data
    //below is prototype for later
    /*QOpenGLTexture tex(QOpenGLTexture::Target1DArray);
    tex.create();
    tex.setMagnificationFilter(QOpenGLTexture::Linear);
    tex.setLayers(4);
    tex.setSize(1025);
    tex.setFormat(QOpenGLTexture::R32F);
    tex.allocateStorage();
    float data[4][1025];
    for(int i=0;i<=1024;++i){
        float t=i/1024.0;
        data[0][i]=(1-t)*(1-t)*(1-t);
        data[1][i]=3*(t)*(1-t)*(1-t);
        data[2][i]=3*(t)*(t)*(1-t);
        data[3][i]=(t)*(t)*(t);
    }
    for(int i=0;i<4;++i){
        tex.setData(0,i,QOpenGLTexture::Red,QOpenGLTexture::Float32,data[i]);
    }
    tex.bind();
    shader.setUniformValue("texUnit", tex.textureId());*/
}

void PointCAM::preprocessor(QOpenGLBuffer&,
                            QOpenGLBuffer&,
                            SceneData)
{

}


using namespace glm;
#define GLSL(version, shader)  "#version " #version "\n" #shader

const QString PointCAM::VSH = GLSL(330,
    in vec3 vertex;
    uniform mat4 transform;

    void main (void)
    {
        gl_Position = transform*vec4(vertex,1.0);
    }
);
const QString PointCAM::GSH = GLSL(330,
    layout(points) in;
    layout(line_strip, max_vertices = 4) out;
    uniform mat4 perspective;
    uniform mat4 invTransform;
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
        postCulledSegment(v0+vec4(0.03,0.03,0,0)
                          ,v0+vec4(-0.03,-0.03,0,0));
        postCulledSegment(v0+vec4(-0.03,0.03,0,0)
                          ,v0+vec4(0.03,-0.03,0,0));
    }

);
const QString PointCAM::FSH = GLSL(330,
    layout(location = 0, index = 0) out vec4 fragColor;
    uniform vec4 color;
    void main (void)
    {
        fragColor = color;
    }
);
