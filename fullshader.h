#ifndef FULLSHADER_H
#define FULLSHADER_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "torus.h"

//torus only
class FullShader
{
public:
    FullShader();
    bool prepareShaderProgram();
    bool bind();
    void release();
    bool initBuffers(int nR, int nr);
    void write(QVector<glm::vec4> points,QVector<glm::ivec2> edges);
    void draw(glm::mat4 transform,glm::mat4 perspective,float);
private:
    QOpenGLShaderProgram m_shader;
    QOpenGLBuffer m_vertexBuffer;
    int m_vertexCount;
    QOpenGLBuffer m_indexBuffer;
    int m_indexCount;
    static QString VSH;
    static QString GSH;
    static QString FSH;

};

#endif // FULLSHADER_H
