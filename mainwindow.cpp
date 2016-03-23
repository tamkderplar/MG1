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

void MainWindow::on_cb_mode_activated(int index)
{
    findChild<SceneGLWidget*>()->setMouseMode(index);
}

void MainWindow::on_sb_torusnR_valueChanged(int arg1)
{
    findChild<SceneGLWidget*>()->torus.setNR(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_sb_torusnr_valueChanged(int arg1)
{
    findChild<SceneGLWidget*>()->torus.setNr(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_dsb_torusR_valueChanged(double arg1)
{
    findChild<SceneGLWidget*>()->torus.setR(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_dsb_torusr_valueChanged(double arg1)
{
    findChild<SceneGLWidget*>()->torus.setr(arg1);
    //write
    findChild<SceneGLWidget*>()->updateTorus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_cb_stereo_clicked(bool checked)
{
    findChild<SceneGLWidget*>()->stereo = checked;
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_widgetScene_pointAdded(const QString &name)
{
    auto newitem = new QListWidgetItem(name);
    newitem->setFlags(newitem->flags()|Qt::ItemIsEditable);
    findChild<QListWidget*>()->addItem(newitem);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    findChild<SceneGLWidget*>()->removePointAt(findChild<QListWidget*>()->row(item));
    findChild<QListWidget*>()->takeItem(findChild<QListWidget*>()->row(item));
    delete item;
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    findChild<SceneGLWidget*>()->cursorGrabAt(findChild<QListWidget*>()->row(item));
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
    auto selectList = findChild<QListWidget*>()->selectedItems();
    if(selectList.empty()){
        //findChild<SceneGLWidget*>()->cursorGrab(QString());
        findChild<SceneGLWidget*>()->cursorGrabAt(-1);
    }else{
        //findChild<SceneGLWidget*>()->cursorGrab(selectList[0]->text());
        findChild<SceneGLWidget*>()->cursorGrabAt(findChild<QListWidget*>()->row(selectList[0]));
    }
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_pb_Ungrab_clicked()
{
    findChild<QListWidget*>()->clearSelection();
    findChild<QListWidget*>()->clearFocus();
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_listWidget_itemActivated(QListWidgetItem *item)
{
    findChild<QListWidget*>()->editItem(item);
}

void MainWindow::on_listWidget_itemChanged(QListWidgetItem *current)
{
    //findChild<SceneGLWidget*>()->renamePoint(previous->text(),current->text());
}
