#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //hardcode-add some points
    findChild<SceneGLWidget*>()->addPoint({0.5,0,0});
    findChild<SceneGLWidget*>()->addPoint({-0.5,0,0});
    findChild<SceneGLWidget*>()->addPoint({0,0.5,0});
    findChild<SceneGLWidget*>()->addPoint({0,-0.5,0});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_modeComboBox_activated(int index)
{
    findChild<SceneGLWidget*>()->setMouseMode(index);
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

void MainWindow::on_widgetScene_pointAdded(const QString &name)
{
    findChild<QListWidget*>()->addItem(name);
}
