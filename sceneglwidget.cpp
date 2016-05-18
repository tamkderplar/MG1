#include "sceneglwidget.h"
#include <QGLFormat>
#include <QMouseEvent>
#include "bezier3c0.h"
#include "bspline.h"
#include "intercurve.h"
#include "patchc0.h"
#include "patchc2.h"
#include "linesegment.h"

SceneGLWidget::SceneGLWidget(QWidget *parent)
    :QOpenGLWidget(parent)
    ,torus()
    ,stereo(false)
    ,cursorPos(0,0,0,1)
    ,grabbedPoint(nullptr)
    ,addObjType(0)
{
    cameraPosZ = 2.0f;
    perspMat[2][2] = 0;
    perspMat[2][3] = -1/cameraPosZ;
    setMouseMode(0);
    patch_m=patch_n=1;
    patch_width=0.5;
    patch_height=0.3;
    patch_wrap=false;
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

void SceneGLWidget::addObject(const QVector<int> &indices)
{
    QVector<PointCAM*> points;
    QVector<Bezier3C0*> beziers;
    QVector<BSpline*> splines;
    QVector<InterCurve*> inters;
    switch(addObjType){
    case 0:
        addPoint(cursorPosition());
        break;
    case 1:
        //ignore all but PointCAMs and Bezier3C0
        for(int i=0;i<indices.size();++i){
            if(PointCAM*p=
                    qobject_cast<PointCAM*>(listObjects[indices[i]])){
                points.append(p);
            }else if(Bezier3C0*p=
                     qobject_cast<Bezier3C0*>(listObjects[indices[i]])){
                beziers.append(p);
            }
        }
        //no selection - addPoint + construct Bezier3C0
        if(points.size()==0 && beziers.size()==0){
            addBezier3C0({addPoint(cursorPosition())});
        }
        //selected Bezier3C0(s) - addPoint + Bezier3C0.append
        else if (points.size()==0){
            PointCAM*point = addPoint(cursorPosition());
            for(int i=0;i<beziers.size();++i){
                beziers[i]->append(point);
            }
        }
        //selected PointCAM(s) - construct Bezier3C0
        else if (beziers.size()==0){
            addBezier3C0(points);
        }
        //selected Bezier3C0(s) and PointCAM(s) - Bezier3C0.append
        else {
            for(int i=0;i<beziers.size();++i)
                for(int j=0;j<points.size();++j){
                    beziers[i]->append(points[j]);
                }
        }
        //select Bezier3C0(s)
        break;
    case 2:
        //ignore all but PointCAMs and BSpline
        for(int i=0;i<indices.size();++i){
            if(PointCAM*p=
                    qobject_cast<PointCAM*>(listObjects[indices[i]])){
                points.append(p);
            }else if(BSpline*p=
                     qobject_cast<BSpline*>(listObjects[indices[i]])){
                splines.append(p);
            }
        }
        //no selection - addPoint + construct BSpline
        if(points.size()==0 && splines.size()==0){
            addBSpline({addPoint(cursorPosition())});
        }
        //selected BSpline(s) - addPoint + BSpline.append
        else if (points.size()==0){
            PointCAM*point = addPoint(cursorPosition());
            for(int i=0;i<splines.size();++i){
                splines[i]->append(point);
            }
        }
        //selected PointCAM(s) - construct BSpline
        else if (splines.size()==0){
            addBSpline(points);
        }
        //selected BSpline(s) and PointCAM(s) - BSpline.append
        else {
            for(int i=0;i<splines.size();++i)
                for(int j=0;j<points.size();++j){
                    splines[i]->append(points[j]);
                }
        }
        //select BSpline(s),unable
        break;
    case 3:
        //ignore all but PointCAMs and InterCurve
        for(int i=0;i<indices.size();++i){
            if(PointCAM*p=
                    qobject_cast<PointCAM*>(listObjects[indices[i]])){
                points.append(p);
            }else if(InterCurve*p=
                     qobject_cast<InterCurve*>(listObjects[indices[i]])){
                inters.append(p);
            }
        }
        //no selection - addPoint + construct InterCurve
        if(points.size()==0 && inters.size()==0){
            addInter({addPoint(cursorPosition())});
        }
        //selected InterCurve(s) - addPoint + InterCurve.append
        else if (points.size()==0){
            PointCAM*point = addPoint(cursorPosition());
            for(int i=0;i<inters.size();++i){
                inters[i]->append(point);
            }
        }
        //selected PointCAM(s) - construct InterCurve
        else if (inters.size()==0){
            addInter(points);
        }
        //selected InterCurve(s) and PointCAM(s) - InterCurve.append
        else {
            for(int i=0;i<inters.size();++i)
                for(int j=0;j<points.size();++j){
                    inters[i]->append(points[j]);
                }
        }
        //select InterCurve(s),unable
        break;
    case 4:
        addPatchC0();
        break;
    case 5:
        addPatchC2();
        break;
    default:
        break;
    }
}

PointCAM* SceneGLWidget::addPoint(glm::vec3 p)
{
    static int numbering = 1;
    PointCAM *point = new PointCAM(p,&manager);
    point->setObjectName( QString("Point%1").arg(numbering) );
    listObjects.append(point);
    manager.addPoint(point);
    manager.addDrawable(point);
    emit objectAdded(point);
    numbering++;
    return point;
}

void SceneGLWidget::addBezier3C0(const QVector<PointCAM *> &points)
{
    assert(points.size()>0);
    static int numbering = 1;
    Bezier3C0 *bezier = new Bezier3C0(points,&manager);
    bezier->setObjectName( QString("Bezier%1").arg(numbering) );
    listObjects.append(bezier);
    emit objectAdded(bezier);
    numbering++;
}

void SceneGLWidget::addBSpline(const QVector<PointCAM *> &points)
{
    assert(points.size()>0);
    static int numbering = 1;
    BSpline *spline = new BSpline(points,&manager);
    spline->setObjectName( QString("BSpline%1").arg(numbering) );
    listObjects.append(spline);
    emit objectAdded(spline);
    connect(this,&SceneGLWidget::bsplineBasisChanged,
            spline,&BSpline::changeBasis);
    numbering++;
}

void SceneGLWidget::addInter(const QVector<PointCAM *> &points)
{
    assert(points.size()>0);
    static int numbering = 1;
    InterCurve *inter = new InterCurve(points,&manager);
    inter->setObjectName( QString("Inter%1").arg(numbering) );
    listObjects.append(inter);
    emit objectAdded(inter);
    numbering++;
}

void SceneGLWidget::addPatchC0()
{
    static int numbering = 1;
    PatchC0 *patch = new PatchC0(cursorPos,patch_n,patch_m,patch_wrap,
                                 patch_width,patch_height,&manager);
    patch->setObjectName( QString("PatchBezier%1").arg(numbering) );
    listObjects.append(patch);
    emit objectAdded(patch);
    patch->updateRibsChanged(4,4);
    connect(this,&SceneGLWidget::patchDivisionChanged,
            patch,&PatchC0::updateRibsChanged);
    numbering++;
}

void SceneGLWidget::addPatchC2()
{
    static int numbering = 1;
    PatchC2 *patch = new PatchC2(cursorPos,patch_n,patch_m,patch_wrap,
                                 patch_width,patch_height,&manager);
    patch->setObjectName( QString("PatchSpline%1").arg(numbering) );
    listObjects.append(patch);
    emit objectAdded(patch);
    patch->updateRibsChanged(4,4);
    connect(this,&SceneGLWidget::patchDivisionChanged,
            patch,&PatchC2::updateRibsChanged);
    numbering++;
}

void SceneGLWidget::removeObjectAt(int id)
{
    QObject *p = listObjects[id];
    if(PointCAM*point=qobject_cast<PointCAM*>(p)){
        manager.removeDrawable(point);
        manager.removePoint(point);
    }
    listObjects.removeAt(id);
    emit objectRemoved(p);
    grabbedPoint = nullptr;
    delete p;
}

void SceneGLWidget::cursorGrabAt(int id)
{
    grabbedPoint = nullptr;
    if(id>=listObjects.size()){
        return;
    }
    if(-1 != id){
        if(PointCAM*point=qobject_cast<PointCAM*>(listObjects[id])){
            cursorPos = point->pos();
            grabbedPoint = point;
        }
    }
}

void SceneGLWidget::renamePoint(QString oldName, QString newName)
{
    for(int i=0;i<listObjects.size();++i){
        if(0 == listObjects[i]->objectName().compare(oldName)){
            listObjects[i]->setObjectName(newName);
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
    /*addPoint({0.5,0,0});
    addPoint({-0.5,0,0});
    addPoint({0,0.5,0});
    addPoint({0,-0.5,0});*/

    /*manager.addDrawable(new BSplineBasis({(PointCAM*)listObjects[0],
                                     (PointCAM*)listObjects[2],
                                     (PointCAM*)listObjects[1],
                                     (PointCAM*)listObjects[3]}));*/
    /*addBSpline({   (PointCAM*)listObjects[0],(PointCAM*)listObjects[2],
                   (PointCAM*)listObjects[1],(PointCAM*)listObjects[3],
                   (PointCAM*)listObjects[0],(PointCAM*)listObjects[2],
                   (PointCAM*)listObjects[1]
                   });*/
    //addPatchC0();
    /*auto p =new PatchC0(glm::vec4(0,0,0,1),1,1,false,0.7,0.7,&manager);
    p->updateRibsChanged(1,1);
    p->updateRibsChanged(2,1);
    p->updateRibsChanged(3,1);
    p->updateRibsChanged(4,1);
    p->updateRibsChanged(5,1);
    p->updateRibsChanged(6,1);
    p->updateRibsChanged(5,1);
    p->updateRibsChanged(4,1);*/

    //*
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
    result = -1;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&result);
    qWarning() << "GL_MAX_VERTEX_ATTRIBS:"
               << result;//*/
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
    if(showPolygons){
        manager.drawAll<LineSegment>({worldMat,viewMat,(c==qRgba(255,255,255,100)?Qt::green:c),cameraPosZ},this);
    }
    manager.drawAll<Bezier3>({worldMat,viewMat,c,cameraPosZ},this);
    manager.drawAll<BSplineBasis>({worldMat,viewMat,c,cameraPosZ},this);
}

void SceneGLWidget::setMouseMode(int mode)
{
    switch (mode) {
    case 0://rotate
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            worldMat=glm::rotate((dir.x)/400.0f,glm::vec3{0,1,0})*worldMat;
            worldMat=glm::rotate((dir.y)/400.0f,glm::vec3{1,0,0})*worldMat;
            worldMat=glm::rotate((dir.z)/400.0f,glm::vec3{0,0,1})*worldMat;
        };
        break;
    case 1://translate
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            worldMat=glm::translate(glm::vec3{dir.x,-dir.y,-dir.z}/400.0f)
                    *worldMat;
        };
        break;
    case 2://scale
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            float s = exp(dir.x/400.0f);
            worldMat=glm::scale(glm::vec3{s,s,s})*worldMat;
        };
        break;
    case 3://move cursor
        mouseFunc =
            [this](glm::vec3 curpos){
            glm::vec3 dir = curpos-glm::vec3{lastpos.x(),lastpos.y(),0.0};
            glm::vec4 moveVec = glm::inverse(worldMat)*
                    glm::vec4(glm::vec3{dir.x,-dir.y,-dir.z}/400.0f,0);
            cursorPos += moveVec;
            if(manager.listPoints().contains(grabbedPoint)){
                grabbedPoint->setPos(glm::vec3(grabbedPoint->pos() + moveVec));
            }
        };
        break;
    case 4://grab point
        mouseFunc =
            [this](glm::vec3 curpos){
            float x = 2*curpos.x/width() - 1.0;
            float y = 1.0 - 2*curpos.y/height();
            glm::mat4 m = perspMat*viewportMat*worldMat;
            auto list = manager.listPoints();
            for(int i=0; i<list.size();++i){
                PointCAM*point = list[i];
                glm::vec4 perspP = m * point->pos();
                if(perspP.z >= cameraPosZ){
                    continue;
                }
                perspP.z = 0;
                perspP = perspP/perspP.w;
                if( (perspP.x-x)*(perspP.x-x) + (perspP.y-y)*(perspP.y-y)
                        < 0.03*0.03){
                    cursorPos = point->pos();
                    grabbedPoint = point;
                    return;
                }
            }
        };

    default:
        break;
    }
}

void SceneGLWidget::setAddObjectType(int type)
{
    addObjType = type;
}

void SceneGLWidget::changeBSplineBasis(bool bezierBasis)
{
    emit bsplineBasisChanged(bezierBasis);
}

void SceneGLWidget::toggleControlPolygons(bool show)
{
    showPolygons = show;
    update();
}

void SceneGLWidget::changePatchDivision(int udiv, int vdiv)
{
    emit patchDivisionChanged(udiv,vdiv);
}

void SceneGLWidget::setPatchNM(int n,int m)
{
    patch_n = n;
    patch_m = m;
}

void SceneGLWidget::togglePatchWrap(bool wrap)
{
    patch_wrap = wrap;
}

void SceneGLWidget::setPatchSize(float w, float h)
{
    patch_width = w;
    patch_height = h;
}
