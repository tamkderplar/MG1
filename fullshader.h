#ifndef FULLSHADER_H
#define FULLSHADER_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QColor>

#include "torus.h"

//torus only
class FullShader
{
public:
    FullShader();
    bool prepareShaderProgram();
    bool bind();
    void release();
    bool initBuffers(int pointCount, int edgeCount);
    void write(QVector<glm::vec4> points,QVector<glm::ivec2> edges);
    void draw(glm::mat4 worldMat,glm::mat4 v,float);
    void setColor(QColor c);
private:
    QOpenGLShaderProgram m_shader;
    QOpenGLBuffer m_vertexBuffer;
    int m_vertexCount;
    QOpenGLBuffer m_indexBuffer;
    int m_indexCount;
    static QString VSH;
    static QString GSH;
    static QString FSH;
    QColor color;

};

#endif // FULLSHADER_H
