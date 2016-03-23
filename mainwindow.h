#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_cb_mode_activated(int index);

    void on_sb_torusnR_valueChanged(int arg1);

    void on_sb_torusnr_valueChanged(int arg1);

    void on_dsb_torusR_valueChanged(double arg1);

    void on_dsb_torusr_valueChanged(double arg1);

    void on_cb_stereo_clicked(bool checked);

    void on_widgetScene_pointAdded(const QString &);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_listWidget_itemSelectionChanged();

    void on_pb_Ungrab_clicked();

    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_listWidget_itemChanged(QListWidgetItem *current);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
