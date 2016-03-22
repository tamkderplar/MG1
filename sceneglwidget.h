#ifndef SCENEGLWIDGET_H
#define SCENEGLWIDGET_H

#include <QGLWidget>

#include <QOpenGLFunctions_3_3_Core>

//#include <functional>

#include "torus.h"
#include "fullshader.h"
#include "pointcam.h"

class SceneGLWidget : public QGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SceneGLWidget(QWidget *parent = 0);

    void updateTorus();
    void updatePoints();
    void updateCursor();

    glm::mat4 worldMat,perspMat;
    Torus torus;
    QList<PointCAM> points;
    //shader things
    FullShader torusShader;
    FullShader pointsShader;
    FullShader cursorShader;
    //
    bool stereo;
    void setMouseMode(int mode);
    void addPoint(glm::vec3);
    void removePoint(PointCAM*);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    std::function<void(glm::vec3)> mouseFunc;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent*e);

    void drawScene(glm::mat4 transform,QColor c);

    QPoint lastpos;
    float sizes[4];
    float cameraPosZ;
    glm::vec4 cursorPos;

signals:
    void pointAdded(QString name);

public slots:

};

#endif // SCENEGLWIDGET_H
