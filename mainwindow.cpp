#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    findChild<SceneGLWidget*>()->installEventFilter(this);
    /*installEventFilter(findChild<SceneGLWidget*>());
    installEventFilter(centralWidget());*/

    //hardcode-add some points
    /*findChild<SceneGLWidget*>()->addPoint({0.5,0,0});
    findChild<SceneGLWidget*>()->addPoint({-0.5,0,0});
    findChild<SceneGLWidget*>()->addPoint({0,0.5,0});
    findChild<SceneGLWidget*>()->addPoint({0,-0.5,0});*/
    findChild<QListView*>("listView")->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()){
    case Qt::Key_1:
        findChild<QComboBox*>("cb_mode")->setCurrentIndex(0);
        break;
    case Qt::Key_2:
        findChild<QComboBox*>("cb_mode")->setCurrentIndex(1);
        break;
    case Qt::Key_3:
        findChild<QComboBox*>("cb_mode")->setCurrentIndex(2);
        break;
    case Qt::Key_4:
        findChild<QComboBox*>("cb_mode")->setCurrentIndex(3);
        break;
    case Qt::Key_5:
        findChild<QComboBox*>("cb_mode")->setCurrentIndex(4);
        break;
    case Qt::Key_Q:
        findChild<QComboBox*>("cb_type")->setCurrentIndex(0);
        break;
    case Qt::Key_W:
        findChild<QComboBox*>("cb_type")->setCurrentIndex(1);
        break;
    case Qt::Key_E:
        findChild<QComboBox*>("cb_type")->setCurrentIndex(2);
        break;
    case Qt::Key_R:
        findChild<QComboBox*>("cb_type")->setCurrentIndex(3);
        break;
    case Qt::Key_A:
        findChild<QPushButton*>("pb_AddObject")->animateClick();
        break;
    default:
        e->ignore();
        break;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{

    //qWarning() << "etype:"<<event->type();
    if (event->type() == QEvent::KeyPress)
    {
         this->keyPressEvent((QKeyEvent*)(event));
         return true;
    }

    /*if (event->type() == QEvent::KeyRelease)
    {
        this->keyReleaseEvent((QKeyEvent*)(event));
        return true;
    }*/

    return QObject::eventFilter(obj, event);
}

void MainWindow::on_cb_mode_currentIndexChanged(int index)
{
    findChild<SceneGLWidget*>()->setMouseMode(index);
}

void MainWindow::on_cb_type_currentIndexChanged(int index)
{
    findChild<SceneGLWidget*>()->setAddObjectType(index);
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

void MainWindow::on_widgetScene_objectAdded(QObject *obj)
{
    auto name = obj->objectName();
    auto newitem = new QListWidgetItem(name);
    newitem->setFlags(newitem->flags() | Qt::ItemIsEditable);
    findChild<QListWidget*>()->addItem(newitem);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    findChild<SceneGLWidget*>()->removeObjectAt(findChild<QListWidget*>()->row(item));
    findChild<QListWidget*>()->takeItem(findChild<QListWidget*>()->row(item));
    delete item;
    findChild<QListWidget*>()->clearSelection();
    findChild<SceneGLWidget*>()->cursorGrabAt(-1);
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
    findChild<SceneGLWidget*>()->cursorGrabAt(-1);
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_listWidget_itemActivated(QListWidgetItem *item)
{
    findChild<QListWidget*>()->editItem(item);
}

void MainWindow::on_pb_AddObject_clicked()
{
    QVector<int> indicesSelected;
    auto itemsSelected = findChild<QListWidget*>()->selectedItems();
    for(int i=0;i<itemsSelected.size();++i){
        indicesSelected.append(findChild<QListWidget*>()->row(itemsSelected[i]));
    }
    findChild<SceneGLWidget*>()->addObject(indicesSelected);
    findChild<SceneGLWidget*>()->update();
}

void MainWindow::on_widgetScene_cursorPositionChanged(const glm::vec3 &pos3, const glm::vec2 &pos2)
{
    findChild<QDoubleSpinBox*>("dsb_PointX")->setValue(pos3.x);
    findChild<QDoubleSpinBox*>("dsb_PointY")->setValue(pos3.y);
    findChild<QDoubleSpinBox*>("dsb_PointZ")->setValue(pos3.z);

    findChild<QDoubleSpinBox*>("dsb_ScreenX")->setValue(pos2.x);
    findChild<QDoubleSpinBox*>("dsb_ScreenY")->setValue(pos2.y);
}

void MainWindow::on_cb_spline_clicked(bool checked)
{
    findChild<SceneGLWidget*>()->changeBSplineBasis(checked);
    findChild<SceneGLWidget*>()->update();
}
