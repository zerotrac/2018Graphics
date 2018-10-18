#ifndef QPAINTBOARD_H
#define QPAINTBOARD_H

class QWJL;

#include "qwjl.h"
#include <QWidget>

namespace Ui {
class QPaintBoard;
}

class QPaintBoard : public QWidget
{
    Q_OBJECT

public:
    QWJL* fa;
    int posX;
    int posY;
    int fixedposX;
    int fixedposY;
    bool fixed;

public:
    explicit QPaintBoard(QWidget *parent = nullptr);
    ~QPaintBoard();

public:
    void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

public:
    void setFa(QWJL* _fa);

private:
    Ui::QPaintBoard *ui;
};

#endif // QPAINTBOARD_H
