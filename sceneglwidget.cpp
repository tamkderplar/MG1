#include "sceneglwidget.h"
#include <QGLFormat>
#include <QMouseEvent>

SceneGLWidget::SceneGLWidget(QWidget *parent)
    :QGLWidget(parent)
    ,torus()
    ,stereo(false)
    ,cursorPos(0,0,0,1)
{
    cameraPosZ = 2.0f;
    perspMat[2][2] = 0;
    perspMat[2][3] = -1/cameraPosZ;
    setMouseMode(0);
}

void SceneGLWidget::updateTorus()
{
    torusShader.write(torus.points,torus.edges);
}

void SceneGLWidget::updatePoints()
{
    QVector<glm::vec4> pointsPoints;
    for (PointCAM& var : points) {
        pointsPoints.append(var.pos()+ glm::inverse(worldMat)*glm::vec4{ 0.03, 0.03,0,0});
        pointsPoints.append(var.pos()+ glm::inverse(worldMat)*glm::vec4{-0.03,-0.03,0,0});
        pointsPoints.append(var.pos()+ glm::inverse(worldMat)*glm::vec4{-0.03, 0.03,0,0});
        pointsPoints.append(var.pos()+ glm::inverse(worldMat)*glm::vec4{ 0.03,-0.03,0,0});
    }
    QVector<glm::ivec2> pointsEdges;
    for (int i=0;i<points.size();++i) {
        pointsEdges.append(glm::ivec2{4*i+0,4*i+1});
        pointsEdges.append(glm::ivec2{4*i+2,4*i+3});
    }
    pointsShader.write(pointsPoints,pointsEdges);
}

void SceneGLWidget::updateCursor()
{
    QVector<glm::vec4> cursorPoints;
    cursorPoints.append(cursorPos+glm::inverse(worldMat)*glm::vec4{0,0, 0.1,0});
    cursorPoints.append(cursorPos+glm::inverse(worldMat)*glm::vec4{0,0,-0.1,0});
    cursorPoints.append(cursorPos+glm::inverse(worldMat)*glm::vec4{0, 0.1,0,0});
    cursorPoints.append(cursorPos+glm::inverse(worldMat)*glm::vec4{0,-0.1,0,0});
    cursorPoints.append(cursorPos+glm::inverse(worldMat)*glm::vec4{ 0.1,0,0,0});
    cursorPoints.append(cursorPos+glm::inverse(worldMat)*glm::vec4{-0.1,0,0,0});
    QVector<glm::ivec2> cursorEdges;
    cursorEdges.append(glm::ivec2{0,1});
    cursorEdges.append(glm::ivec2{2,3});
    cursorEdges.append(glm::ivec2{4,5});
    cursorShader.write(cursorPoints,cursorEdges);
}

void SceneGLWidget::setMouseMode(int mode)
{
    switch (mode) {
    case 0:
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            worldMat=glm::rotate((dir.x)/400.0f,glm::vec3{0,1,0})*worldMat;
            worldMat=glm::rotate((dir.y)/400.0f,glm::vec3{1,0,0})*worldMat;
            worldMat=glm::rotate((dir.z)/400.0f,glm::vec3{0,0,1})*worldMat;
        };
        break;
    case 1:
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            worldMat=glm::translate(glm::vec3{dir.x,-dir.y,-dir.z}/400.0f)*worldMat;
        };
        break;
    case 2:
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            float s = exp(dir.x/400.0f);
            worldMat=glm::scale(glm::vec3{s,s,s})*worldMat;
        };
        break;
    case 3:
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            cursorPos += glm::inverse(worldMat)* glm::vec4(glm::vec3{dir.x,-dir.y,-dir.z}/400.0f,0);
        };
        break;
    default:
        break;
    }
}

void SceneGLWidget::addPoint(glm::vec3 p)
{
    static int numbering = 1;
    PointCAM point(p);
    point.name = QString("Point%1").arg(numbering);
    points.append(point);
    emit pointAdded(point.name);
    numbering++;
}

void SceneGLWidget::removePoint(QString name)
{
    for(int i=0;i<points.size();++i){
        if(0 == points[i].name.compare(name)){
            points.removeAt(i);
            emit pointRemoved(name);
            return;
        }
    }
}

void SceneGLWidget::removePointAt(int id)
{
    QString name = points[id].name;
    points.removeAt(id);
    emit pointRemoved(name);
}

void SceneGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    QGLFormat glFormat = QGLWidget::format();
    if(!glFormat.sampleBuffers())
        qWarning() << "Could not enable sample buffers";

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);

    {
        uint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        //QOpenGLVertexArrayObject vao;
        //vao.create();
        //vao.bind();
    }

    torusShader.prepareShaderProgram();
    if(!torusShader.bind()){
        qWarning() << "Could not bind shProgram";
        return;
    }
    if(!torusShader.initBuffers(8,8)){
        qWarning() << "Could not bind or create buffers";
        return;
    }
    //write
    torusShader.write(torus.points,torus.edges);
    torusShader.release();

    cursorShader.prepareShaderProgram();
    if(!cursorShader.bind()){
        qWarning() << "Could not bind shProgram";
        return;
    }
    if(!cursorShader.initBuffers(1,3)){
        qWarning() << "Could not bind or create buffers";
        return;
    }
    //write
    updateCursor();
    cursorShader.release();

    pointsShader.prepareShaderProgram();
    if(!pointsShader.bind()){
        qWarning() << "Could not bind shProgram";
        return;
    }
    if(!pointsShader.initBuffers(1,3)){
        qWarning() << "Could not bind or create buffers";
        return;
    }
    //write
    updatePoints();
    pointsShader.release();

}

void SceneGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    sizes[0] = w;
    sizes[1] = h;
    sizes[2] = qMin(w,h);
    sizes[3] = qMax(w,h);
}

void SceneGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glm::mat4 viewportMat = glm::mat4(1.0f);
    viewportMat[0][0] = sizes[1]/sizes[3];
    viewportMat[1][1] = sizes[0]/sizes[3];

    //
    //draw
    if(stereo){
        float eyeWidth = 0.01;
        glm::mat4 perspL = perspMat, perspR = perspMat;

        perspL[2][0] =  eyeWidth/cameraPosZ;
        drawScene(perspL*viewportMat,qRgba(150,  0,  0,255));

        perspR[2][0] = -eyeWidth/cameraPosZ;
        drawScene(perspR*viewportMat,qRgba(  0,  0,255,255));
    }else{
        drawScene(perspMat*viewportMat,qRgba(255,255,255,100));
    }
    //
    qWarning() << worldMat[0][0];
}

void SceneGLWidget::mousePressEvent(QMouseEvent *e)
{
    lastpos = e->pos();
}

void SceneGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPoint dir = e->pos()-lastpos;
    mouseFunc({e->pos().x(),e->pos().y(),0});
    lastpos = e->pos();
    update();
}

void SceneGLWidget::wheelEvent(QWheelEvent *e)
{
    mouseFunc({lastpos.x(),lastpos.y(),e->angleDelta().y()});
    lastpos = e->pos();
    update();
}

void SceneGLWidget::drawScene(glm::mat4 transform, QColor c)
{
    torusShader.bind();
    torusShader.setColor(c);
    torusShader.draw(worldMat,transform,cameraPosZ);
    torusShader.release();

    updateCursor();
    cursorShader.bind();
    cursorShader.setColor(c);
    cursorShader.draw(worldMat,transform,cameraPosZ);
    cursorShader.release();

    updatePoints();
    pointsShader.bind();
    pointsShader.setColor(c);
    pointsShader.draw(worldMat,transform,cameraPosZ);
    pointsShader.release();
}
