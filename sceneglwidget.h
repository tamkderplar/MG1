#ifndef SCENEGLWIDGET_H
#define SCENEGLWIDGET_H

#include <QOpenGLWidget>

#include <QOpenGLFunctions_3_3_Core>

//#include <functional>

#include "torus.h"
#include "fullshader.h"
#include "pointcam.h"
#include "glmanager.h"
#include "bezier3.h"

class SceneGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SceneGLWidget(QWidget *parent = 0);

    void updateTorus();
    void updatePoints();
    void updateCursor();
    GLManager manager;

    glm::mat4 worldMat,perspMat;
    glm::mat4 viewportMat;
    Torus torus;
    QVector<PointCAM*> points;
    //shader things
    FullShader torusShader;
    FullShader pointsShader;
    FullShader cursorShader;
    //
    bool stereo;
    void setMouseMode(int mode);
    void addPoint(glm::vec3);
    void removePoint(QString name);
    void removePointAt(int id);
    void cursorGrab(QString name);
    void cursorGrabAt(int id);
    void renamePoint(QString oldName, QString newName);
    glm::vec3 cursorPosition() const;

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
    int grabbed;

signals:
    void pointAdded(QString name);
    void pointRemoved(QString name);
    void cursorPositionChanged(glm::vec3,glm::vec2);


public slots:

};

#endif // SCENEGLWIDGET_H
