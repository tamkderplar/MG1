#include "fullshader.h"


FullShader::FullShader()
    :m_vertexBuffer(QOpenGLBuffer::VertexBuffer)
    ,m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{

}

bool FullShader::prepareShaderProgram()
{
    bool result = m_shader.addShaderFromSourceCode(QOpenGLShader::Vertex, VSH);
    if(!result)
        qWarning() << m_shader.log();

    if(!GSH.isNull() && !GSH.isEmpty()){
        result = m_shader.addShaderFromSourceCode(QOpenGLShader::Geometry, GSH);
        if(!result)
            qWarning() << m_shader.log();
    }

    result = m_shader.addShaderFromSourceCode(QOpenGLShader::Fragment, FSH);
    if(!result)
        qWarning() << m_shader.log();

    result = m_shader.link();
    if(!result)
        qWarning() << "Could not link shProgram" << m_shader.log();

    return result;
}

bool FullShader::bind()
{
    if(!m_shader.bind()) return false;
    //if(!m_vertexBuffer.bind()) return false;
    //if(!m_indexBuffer.bind()) return false;
    return true;
}

void FullShader::release()
{
    //m_indexBuffer.release();
    //m_vertexBuffer.release();
    m_shader.release();
}

bool FullShader::initBuffers(int nR, int nr)
{
    m_shader.bind();
    if(!m_vertexBuffer.create()) return false;
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    if(!m_vertexBuffer.bind()) return false;
    m_vertexBuffer.allocate(nr*nR*sizeof(glm::vec4));
    m_vertexBuffer.release();

    if(!m_indexBuffer.create()) return false;
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    if(!m_indexBuffer.bind()) return false;
    m_indexBuffer.allocate((nr*(nR)+(nr)*nR)
                           *sizeof(glm::ivec2));
    m_indexBuffer.release();
    return true;
}

void FullShader::write(QVector<glm::vec4> points, QVector<glm::ivec2> edges)
{
    //DEBUG?:check index sanity, i.e. edges[i]<points.size
    m_vertexBuffer.bind();
    int newPsize = points.size()*sizeof(glm::vec4);
    if (newPsize > m_vertexBuffer.size()) {
        m_vertexBuffer.allocate(newPsize);
    }
    m_vertexBuffer.write(0,points.data(),newPsize);
    //
    m_indexBuffer.bind();
    int newEsize = edges.size()*sizeof(glm::ivec2);
    if (newEsize > m_indexBuffer.size()) {
        m_indexBuffer.allocate(newEsize);
    }
    m_indexBuffer.write(0,edges.data(),newEsize);
    //
    m_vertexCount = points.size();
    m_indexCount = edges.size();
}

void FullShader::draw(glm::mat4 transform,glm::mat4 perspective,float cameraPosZ)
{
    typedef GLfloat (*parr44)[4][4];
    m_vertexBuffer.bind();
    m_indexBuffer.bind();
    m_shader.setUniformValue("transform",*(parr44)&transform);
    m_shader.setUniformValue("perspective",*(parr44)&perspective);
    m_shader.setUniformValue("cameraPosZ",cameraPosZ);
    m_shader.setAttributeBuffer("vertex", GL_FLOAT, 0, 4, 4*sizeof(float));
    m_shader.enableAttributeArray("vertex");

    glDrawElements(GL_LINES, 2*m_indexCount ,GL_UNSIGNED_INT,0);
    m_vertexBuffer.release();
    m_indexBuffer.release();
}


using namespace glm;
#define GLSL(version, shader)  "#version " #version "\n" #shader

QString FullShader::VSH = GLSL(330,
    in vec3 vertex;
    uniform mat4 transform;

    void main (void)
    {
        gl_Position = transform*vec4(vertex,1.0);
    }
);
QString FullShader::GSH = GLSL(330,
    layout(lines) in;
    layout(line_strip, max_vertices = 2) out;
    uniform mat4 perspective;
    uniform float cameraPosZ;

    void main (void)
    {
        vec4 v0 = gl_in[0].gl_Position;
        vec4 v1 = gl_in[1].gl_Position;

        float r = cameraPosZ;
        float r99 = r*0.99;

        if (v0.z >= r99 && v1.z >= r99)
            return;
        float t = (r99 - v0.z)/(v1.z - v0.z);
    /**/
        if (v0.z >= r99){
            gl_Position = v0 + t*(v1-v0);
        }else{
            gl_Position = v0;
        }
        gl_Position = perspective*gl_Position;
        /*gl_Position.z = 0;*/
        gl_Position = gl_Position/gl_Position.w;
        EmitVertex();
    /**/
        if (v1.z >= r99){
            gl_Position = v0 + t*(v1-v0);
        }else{
            gl_Position = v1;
        }
        gl_Position = perspective*gl_Position;
        /*gl_Position.z = 0;*/
        gl_Position = gl_Position/gl_Position.w;
        EmitVertex();
    /**/
        EndPrimitive();

    }
);
QString FullShader::FSH = GLSL(330,
    layout(location = 0, index = 0) out vec4 fragColor;
    void main (void)
    {
        fragColor = vec4(1,1,1 ,0.7);
    }
);

