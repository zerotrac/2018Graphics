#ifndef WIDGET_H
#define WIDGET_H

#include "openglwidget.h"

#include <QWidget>
// #include <gl/GLU.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    OpenGLWidget* glWidget;

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
