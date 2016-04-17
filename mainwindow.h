#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "glm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent*);
    bool eventFilter(QObject *, QEvent *);

private slots:
    void on_cb_mode_currentIndexChanged(int index);
    void on_cb_type_currentIndexChanged(int index);

    void on_cb_stereo_clicked(bool checked);
    void on_cb_spline_clicked(bool checked);

    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_itemActivated(QListWidgetItem *item);
    void on_listWidget_itemSelectionChanged();

    void on_pb_Ungrab_clicked();
    void on_pb_AddObject_clicked();

    void on_widgetScene_objectAdded(QObject *);
    void on_widgetScene_cursorPositionChanged(const glm::vec3 &, const glm::vec2 &);

    //torus manip
    void on_sb_torusnR_valueChanged(int arg1);
    void on_sb_torusnr_valueChanged(int arg1);
    void on_dsb_torusR_valueChanged(double arg1);
    void on_dsb_torusr_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
