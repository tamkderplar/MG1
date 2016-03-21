#include "sceneglwidget.h"
#include <QGLFormat>
#include <QMouseEvent>

SceneGLWidget::SceneGLWidget(QWidget *parent)
    :QGLWidget(parent)
    ,torus()
    ,stereo(false)
{
    cameraPosZ = 2.0f;
    perspMat[2][2] = 0;
    perspMat[2][3] = -1/cameraPosZ;
    mouseFunc = [](glm::mat4 m,glm::vec3 dir){
        m=glm::rotate((dir.x)/400.0f,glm::vec3{0,1,0})*m;
        m=glm::rotate((dir.y)/400.0f,glm::vec3{1,0,0})*m;
        m=glm::rotate((dir.z)/400.0f,glm::vec3{0,0,1})*m;
        return m;
    };
}

void SceneGLWidget::updateTorus()
{
    torusShader.write(torus.points,torus.edges);
}

void SceneGLWidget::setMouseMode(std::function<glm::mat4 (glm::mat4,glm::vec3)> f)
{
    mouseFunc = f;
}

void SceneGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    QGLFormat glFormat = QGLWidget::format();
    if(!glFormat.sampleBuffers())
        qWarning() << "Could not enable sample buffers";

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    //glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    //glEnable(GL_DEPTH_TEST);
    //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
    //glBlendFunc(GL_ONE,GL_ONE);
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
    torusShader.bind();
    //draw
    if(stereo){
        float eyeWidth = 0.01;
        glm::mat4 perspL = perspMat;
        perspL[2][0] =  eyeWidth/cameraPosZ;
        torusShader.setColor(qRgba(150,  0,  0,255));
        torusShader.draw(worldMat,perspL*viewportMat,cameraPosZ);
        glm::mat4 perspR = perspMat;
        perspR[2][0] = -eyeWidth/cameraPosZ;
        torusShader.setColor(qRgba(  0,  0,255,255));
        torusShader.draw(worldMat,perspR*viewportMat,cameraPosZ);
    }else{
        torusShader.setColor(qRgba(255,255,255,100));
        torusShader.draw(worldMat,perspMat*viewportMat,cameraPosZ);
    }
    //
    torusShader.release();
    qWarning() << worldMat[0][0];
}

void SceneGLWidget::mousePressEvent(QMouseEvent *e)
{
    lastpos = e->pos();
}

void SceneGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    //worldMat=glm::rotate(-(lastpos.x()-e->pos().x())/400.0f,glm::vec3{0,1,0})*worldMat;
    //worldMat=glm::rotate(-(lastpos.y()-e->pos().y())/400.0f,glm::vec3{1,0,0})*worldMat;
    QPoint dir = e->pos()-lastpos;
    worldMat = mouseFunc(worldMat,{dir.x(),dir.y(),0});
    lastpos = e->pos();
    update();
}

void SceneGLWidget::wheelEvent(QWheelEvent *e)
{
    worldMat = mouseFunc(worldMat,{0,0,e->angleDelta().y()});
    lastpos = e->pos();
    update();
}
