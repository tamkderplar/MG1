#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_nRSpinBox_valueChanged(int arg1);

    void on_nrSpinBox_valueChanged(int arg1);

    void on_modeComboBox_activated(int index);

    void on_RSpinBox_valueChanged(double arg1);

    void on_rSpinBox_valueChanged(double arg1);

    void on_stereoCheckBox_clicked(bool checked);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
