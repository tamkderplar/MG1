#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_nRSpinBox_valueChanged(int arg1)
{
    findChild<SceneGLWidget*>()->torus.setNR(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_nrSpinBox_valueChanged(int arg1)
{
    findChild<SceneGLWidget*>()->torus.setNr(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_modeComboBox_activated(int index)
{
    switch (index) {
    case 0:
        findChild<SceneGLWidget*>()->setMouseMode(
                    [](glm::mat4 m,glm::vec3 dir){
            m=glm::rotate((dir.x)/400.0f,glm::vec3{0,1,0})*m;
            m=glm::rotate((dir.y)/400.0f,glm::vec3{1,0,0})*m;
            m=glm::rotate((dir.z)/400.0f,glm::vec3{0,0,1})*m;
            return m;
        });
        break;
    case 1:
        findChild<SceneGLWidget*>()->setMouseMode(
                    [](glm::mat4 m,glm::vec3 dir){
            m=glm::translate(glm::vec3{dir.x,-dir.y,dir.z}/400.0f)*m;
            return m;
        });
        break;
    case 2:
        findChild<SceneGLWidget*>()->setMouseMode(
                    [](glm::mat4 m,glm::vec3 dir){
            float s = exp(dir.x/400.0f);
            m=glm::scale(glm::vec3{s,s,s})*m;
            return m;
        });
        break;
    default:
        break;
    }
}

void MainWindow::on_RSpinBox_valueChanged(double arg1)
{
    findChild<SceneGLWidget*>()->torus.setR(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_rSpinBox_valueChanged(double arg1)
{
    findChild<SceneGLWidget*>()->torus.setr(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_stereoCheckBox_clicked(bool checked)
{
    findChild<SceneGLWidget*>()->stereo = checked;
    findChild<SceneGLWidget*>()->update();
}
