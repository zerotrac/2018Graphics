#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qwjl.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    QWJL* wjl;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
