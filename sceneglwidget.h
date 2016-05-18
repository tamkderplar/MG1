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
    void updateCursor();
    GLManager manager;

    glm::mat4 worldMat,perspMat;
    glm::mat4 viewportMat;
    Torus torus;
    QVector<QObject*> listObjects;
    //shader things
    FullShader torusShader;
    FullShader cursorShader;
    //
    bool stereo;
    void addObject(const QVector<int> &indicesSelected);
    //void removePoint(QString name);
    void removeObjectAt(int id);
    //void cursorGrab(QString name);
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

    PointCAM* addPoint(glm::vec3);
    void addBezier3C0(const QVector<PointCAM*>&);
    void addBSpline(const QVector<PointCAM*>&);
    void addInter(const QVector<PointCAM*>&);
    void addPatchC0();
    void addPatchC2();
    void drawScene(glm::mat4 transform,QColor c);

    QPoint lastpos;
    float sizes[4];
    float cameraPosZ;
    glm::vec4 cursorPos;
    PointCAM* grabbedPoint;
    int addObjType;
    bool showPolygons;
    //patch options
    int patch_n,patch_m;
    bool patch_wrap;
    float patch_width,patch_height;

signals:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void cursorPositionChanged(glm::vec3,glm::vec2);
    void bsplineBasisChanged(bool bezierBasis);
    void patchDivisionChanged(int,int);


public slots:
    void setMouseMode(int mode);
    void setAddObjectType(int type);
    void changeBSplineBasis(bool bezierBasis);
    void toggleControlPolygons(bool show);
    void changePatchDivision(int,int);
    void setPatchNM(int,int);
    void togglePatchWrap(bool);
    void setPatchSize(float,float);

};

#endif // SCENEGLWIDGET_H
