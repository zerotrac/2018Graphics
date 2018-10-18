#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(800, 600);
    QWJL* wjl = new QWJL();
    this->setCentralWidget(wjl);
}

MainWindow::~MainWindow()
{
    delete ui;
}
