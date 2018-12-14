#include "widget.h"
#include "ui_widget.h"

#include <QGridLayout>
#include <QDebug>
#include <QPushButton>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);

    // glWidget = new OpenGLWidget(this);
    // glWidget->setGeometry(160, 25, 620, 560);

    QGridLayout *layout = new QGridLayout();
    glWidget = new OpenGLWidget();
    layout->addWidget(glWidget);
    setLayout(layout);
}

Widget::~Widget()
{
    delete ui;
}
