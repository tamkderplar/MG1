#include "sceneglwidget.h"
#include <QGLFormat>
#include <QMouseEvent>

SceneGLWidget::SceneGLWidget(QWidget *parent)
    :QOpenGLWidget(parent)
    ,torus()
    ,stereo(false)
    ,cursorPos(0,0,0,1)
    ,grabbed(-1)
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

void SceneGLWidget::updateCursor()
{
    QVector<glm::vec4> cursorPoints;
    glm::mat4 invWorld = glm::inverse(worldMat);
    cursorPoints.append(cursorPos+invWorld*glm::vec4{0,0, 0.1,0});
    cursorPoints.append(cursorPos+invWorld*glm::vec4{0,0,-0.1,0});
    cursorPoints.append(cursorPos+invWorld*glm::vec4{0, 0.1,0,0});
    cursorPoints.append(cursorPos+invWorld*glm::vec4{0,-0.1,0,0});
    cursorPoints.append(cursorPos+invWorld*glm::vec4{ 0.1,0,0,0});
    cursorPoints.append(cursorPos+invWorld*glm::vec4{-0.1,0,0,0});
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
            worldMat=glm::translate(glm::vec3{dir.x,-dir.y,-dir.z}/400.0f)
                    *worldMat;
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
            glm::vec4 moveVec = glm::inverse(worldMat)*
                    glm::vec4(glm::vec3{dir.x,-dir.y,-dir.z}/400.0f,0);
            cursorPos += moveVec;
            if(-1 != grabbed){
                if(PointCAM*point = qobject_cast<PointCAM*>(points[grabbed]))
                {
                    point->setPos(glm::vec3(point->pos() + moveVec));
                }
            }
        };
        break;
    case 4:
        mouseFunc =
            [this](glm::vec3 curpos){
            float x = 2*curpos.x/width() - 1.0;
            float y = 1.0 - 2*curpos.y/height();
            glm::mat4 m = perspMat*viewportMat*worldMat;
            for(int i=0; i<points.size();++i){
                PointCAM*point;
                if(!(point=qobject_cast<PointCAM*>(points[i]))){
                    continue;
                }
                glm::vec4 perspP = m * point->pos();
                if(perspP.z >= cameraPosZ){
                    continue;
                }
                perspP.z = 0;
                perspP = perspP/perspP.w;
                if( (perspP.x-x)*(perspP.x-x) + (perspP.y-y)*(perspP.y-y)
                        < 0.03*0.03){
                    cursorPos = point->pos();
                    grabbed = i;
                    return;
                }
            }
        };

    default:
        break;
    }
}

void SceneGLWidget::addPoint(glm::vec3 p)
{
    static int numbering = 1;
    PointCAM *point = new PointCAM(p,&manager);
    point->setObjectName( QString("Point%1").arg(numbering) );
    points.append(point);
    manager.addPoint(point);
    manager.addDrawable(point);
    emit pointAdded(point->objectName());
    numbering++;
}

void SceneGLWidget::removePoint(QString name)
{
    grabbed = -1;
    for(int i=0;i<points.size();++i){
        if(0 == points[i]->objectName().compare(name)){
            QObject *p = points[i];
            points.removeAt(i);
            emit pointRemoved(name);
            delete p;
            return;
        }
    }
}

void SceneGLWidget::removePointAt(int id)
{
    QObject *p = points[id];
    QString name = points[id]->objectName();
    points.removeAt(id);
    emit pointRemoved(name);
    grabbed = -1;
    delete p;
}

void SceneGLWidget::cursorGrab(QString name)
{
    if(0 == name.compare(QString())){
        grabbed = -1;
        return;
    }

    for(int i=0;i<points.size();++i){
        if(0 == points[i]->objectName().compare(name)){
            if(PointCAM*point=qobject_cast<PointCAM*>(points[i])){
                cursorPos = point->pos();
                grabbed = i;
                return;
            }
        }
    }
}

void SceneGLWidget::cursorGrabAt(int id)
{
    if(id>=points.size()){
        grabbed = -1;
        return;
    }
    if(-1 != id){
        if(PointCAM*point=qobject_cast<PointCAM*>(points[id])){
            cursorPos = point->pos();
        }
    }
    grabbed = id;
}

void SceneGLWidget::renamePoint(QString oldName, QString newName)
{
    for(int i=0;i<points.size();++i){
        if(0 == points[i]->objectName().compare(oldName)){
            points[i]->setObjectName(newName);
            return;
        }
    }
}

glm::vec3 SceneGLWidget::cursorPosition() const
{
    return glm::vec3(cursorPos);
}

void SceneGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    QSurfaceFormat glFormat = QOpenGLWidget::format();
    if(!glFormat.samples())
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
    if(!torusShader.initBuffers(64,128)){
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
    if(!cursorShader.initBuffers(6,3)){
        qWarning() << "Could not bind or create buffers";
        return;
    }
    //write
    updateCursor();
    cursorShader.release();

    manager.init();

    //hardcode-add some points
    addPoint({0.5,0,0});
    addPoint({-0.5,0,0});
    addPoint({0,0.5,0});
    addPoint({0,-0.5,0});

    manager.addDrawable(new Bezier3({(PointCAM*)points[0],
                                     (PointCAM*)points[2],
                                     (PointCAM*)points[1],
                                     (PointCAM*)points[3]}));

    int result = -1;
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS,&result);
    qWarning() << "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS:" << result;
    result = -1;
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS,&result);
    qWarning() << "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS:"
               << result;
    result = -1;
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,&result);
    qWarning() << "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS:"
               << result;
    result = -1;
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS,&result);
    qWarning() << "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS:"
               << result;
    result = -1;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES,&result);
    qWarning() << "GL_MAX_GEOMETRY_OUTPUT_VERTICES:"
               << result;
    result = -1;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,&result);
    qWarning() << "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS:"
               << result;
    result = -1;
    glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS,&result);
    qWarning() << "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS:"
               << result;
}

void SceneGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    sizes[0] = w;
    sizes[1] = h;
    sizes[2] = qMin(w,h);
    sizes[3] = qMax(w,h);
    viewportMat[0][0] = sizes[1]/sizes[3];
    viewportMat[1][1] = sizes[0]/sizes[3];
}

void SceneGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

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
    //qWarning() << worldMat[0][0];
    glm::vec4 screenPos = perspMat*viewportMat*worldMat*cursorPos;
    emit cursorPositionChanged(glm::vec3(cursorPos),
                               glm::vec2(screenPos/screenPos.w));
}

void SceneGLWidget::mousePressEvent(QMouseEvent *e)
{
    lastpos = e->pos();
    mouseFunc({e->pos().x(),e->pos().y(),0});
    update();
}

void SceneGLWidget::mouseMoveEvent(QMouseEvent *e)
{
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

void SceneGLWidget::drawScene(glm::mat4 viewMat, QColor c)
{
    /*torusShader.bind();
    torusShader.setColor(c);
    torusShader.draw(worldMat,viewMat,cameraPosZ);
    torusShader.release();*/

    updateCursor();
    cursorShader.bind();
    cursorShader.setColor(c);
    cursorShader.draw(worldMat,viewMat,cameraPosZ);
    cursorShader.release();

    /*updatePoints();
    pointsShader.bind();
    pointsShader.setColor(c);
    pointsShader.draw(worldMat,viewMat,cameraPosZ);
    pointsShader.release();*/

    manager.drawAll<PointCAM>({worldMat,viewMat,c,cameraPosZ},this);
    manager.drawAll<Bezier3>({worldMat,viewMat,c,cameraPosZ},this);
}
