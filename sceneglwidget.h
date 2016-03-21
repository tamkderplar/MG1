#ifndef SCENEGLWIDGET_H
#define SCENEGLWIDGET_H

#include <QGLWidget>

#include <QOpenGLFunctions_3_3_Core>

//#include <functional>

#include "torus.h"
#include "fullshader.h"

class SceneGLWidget : public QGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SceneGLWidget(QWidget *parent = 0);

    void updateTorus();

    glm::mat4 worldMat,perspMat;
    Torus torus;
    //shader things
    FullShader torusShader;
    //
    bool stereo;
    void setMouseMode(std::function<glm::mat4(glm::mat4,glm::vec3)>);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    std::function<glm::mat4(glm::mat4,glm::vec3)> mouseFunc;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent*e);

    QPoint lastpos;
    float sizes[4];
    float cameraPosZ;


/*
    QOpenGLShaderProgram m_shader;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    static QString VSH;
    static QString GSH;
    static QString FSH;
*/
signals:

public slots:

};

#endif // SCENEGLWIDGET_H
