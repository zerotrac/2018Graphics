#include "qpaintboard.h"
#include "ui_qpaintboard.h"

#include <QStyleOption>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <iostream>

QPaintBoard::QPaintBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QPaintBoard)
{
    ui->setupUi(this);

    fixed = false;
}

QPaintBoard::~QPaintBoard()
{
    delete ui;
}

void QPaintBoard::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // draw the unselected polygons
    int ii = -1;
    for (const Polygon2f& polygon: fa->polygons)
    {
        ++ii;
        if (fa->ids[ii] == fa->select_id)
        {
            continue;
        }

        painter.setPen(QPen(fa->polygonLabels[ii]->getInnerColor(), 1));
        std::vector<ParallelSegment2f> segments = polygon.getColoredSegment();
        for (const ParallelSegment2f& segment: segments)
        {
            double a = segment.getA();
            double b = segment.getB();
            double c = segment.getC();
            painter.drawLine(QPointF(b, a), QPointF(c, a));
        }

        painter.setPen(QPen(fa->polygonLabels[ii]->getOuterColor(), 1));
        OuterLoop2f outerloop = polygon.getOuterLoop();
        for (int i = 0; i < outerloop.size(); ++i)
        {
            Point2f a = outerloop.getPoint(i);
            Point2f b = outerloop.getPoint(i + 1);
            painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(b.getX(), b.getY()));
        }

        std::vector<InnerLoop2f> innerloops = polygon.getInnerLoop();
        for (const InnerLoop2f& innerloop: innerloops)
        {
            for (int i = 0; i < innerloop.size(); ++i)
            {
                Point2f a = innerloop.getPoint(i);
                Point2f b = innerloop.getPoint(i + 1);
                painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(b.getX(), b.getY()));
            }
        }
    }

    // draw the selected polygon (if exists)

    ii = -1;
    for (const Polygon2f& polygon: fa->polygons)
    {
        ++ii;
        if (fa->ids[ii] != fa->select_id)
        {
            continue;
        }

        painter.setPen(QPen(fa->polygonLabels[ii]->getInnerColor(), 1));
        std::vector<ParallelSegment2f> segments = polygon.getColoredSegment();
        for (const ParallelSegment2f& segment: segments)
        {
            double a = segment.getA();
            double b = segment.getB();
            double c = segment.getC();
            painter.drawLine(QPointF(b, a), QPointF(c, a));
        }

        painter.setPen(QPen(fa->polygonLabels[ii]->getOuterColor(), 2));
        OuterLoop2f outerloop = polygon.getOuterLoop();
        for (int i = 0; i < outerloop.size(); ++i)
        {
            Point2f a = outerloop.getPoint(i);
            Point2f b = outerloop.getPoint(i + 1);
            painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(b.getX(), b.getY()));
        }

        std::vector<InnerLoop2f> innerloops = polygon.getInnerLoop();
        for (const InnerLoop2f& innerloop: innerloops)
        {
            for (int i = 0; i < innerloop.size(); ++i)
            {
                Point2f a = innerloop.getPoint(i);
                Point2f b = innerloop.getPoint(i + 1);
                painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(b.getX(), b.getY()));
            }
        }

        std::vector<Point2f> bb = polygon.getBoundingBox();

        QPen p1;
        p1.setWidth(5);
        p1.setColor(Qt::black);
        painter.setPen(p1);
        for (size_t i = 0; i < bb.size() - 1; ++i)
        {
            painter.drawLine(QPointF(bb[i].getX(), bb[i].getY()), QPointF(bb[i].getX(), bb[i].getY()));
        }

        QPen p2;
        p2.setWidth(1);
        p2.setColor(Qt::black);
        p2.setStyle(Qt::DashDotDotLine);
        painter.setPen(p2);
        for (size_t i = 0; i < bb.size() - 1; ++i)
        {
            painter.drawLine(QPointF(bb[i].getX(), bb[i].getY()), QPointF(bb[i + 1].getX(), bb[i + 1].getY()));
        }
    }

    // draw the drawing polygon (if exists)
    painter.setPen(QPen(Qt::black, 2));

    if (fa->status == fa->DRAWING_OUTER_LOOP)
    {
        for (int i = 0; i < fa->cur_outer_loop.size(); ++i)
        {
            if (i < fa->cur_outer_loop.size() - 1)
            {
                Point2f a = fa->cur_outer_loop.getPoint(i);
                Point2f b = fa->cur_outer_loop.getPoint(i + 1);
                painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(b.getX(), b.getY()));
            }
            else
            {
                Point2f a = fa->cur_outer_loop.getPoint(i);
                painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(posX, posY));
            }
        }
    }
    else if (fa->status == fa->DRAWING_INNER_LOOP)
    {
        for (int i = 0; i < fa->cur_inner_loop.size(); ++i)
        {
            if (i < fa->cur_inner_loop.size() - 1)
            {
                Point2f a = fa->cur_inner_loop.getPoint(i);
                Point2f b = fa->cur_inner_loop.getPoint(i + 1);
                painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(b.getX(), b.getY()));
            }
            else
            {
                Point2f a = fa->cur_inner_loop.getPoint(i);
                painter.drawLine(QPointF(a.getX(), a.getY()), QPointF(posX, posY));
            }
        }
    }
}

void QPaintBoard::mouseMoveEvent(QMouseEvent* event)
{
    if (fa->status == fa->DRAWING_OUTER_LOOP || fa->status == fa->DRAWING_INNER_LOOP)
    {
        posX = event->x();
        posY = event->y();
        update();
    }
    else if (fixed)
    {
        fa->translateOrRotateFake(Point2f(fixedposX, fixedposY), Point2f(event->x(), event->y()));
        update();
    }
    else if (fa->select_id == -1)
    {
        fixed = false;
    }
}

void QPaintBoard::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (fa->status == fa->DRAWING_OUTER_LOOP)
        {
            fa->cur_outer_loop.addPoint(Point2f(event->x(), event->y()));
        }
        else if (fa->status == fa->DRAWING_INNER_LOOP)
        {
            fa->cur_inner_loop.addPoint(Point2f(event->x(), event->y()));
        }
        else if (fa->select_id != -1)
        {
            fixedposX = event->x();
            fixedposY = event->y();
            fixed = true;
            fa->checkTR(Point2f(fixedposX, fixedposY));
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (fa->status == fa->DRAWING_OUTER_LOOP)
        {
            if (fa->cur_outer_loop.isReasonableLoop())
            {
                fa->addNewPolygon();
            }
            fa->status = fa->DOING_NOTHING;
        }
        else if (fa->status == fa->DRAWING_INNER_LOOP)
        {
            qDebug() << "inner loop 1";
            if (fa->cur_inner_loop.isReasonableLoop())
            {
                qDebug() << "inner loop 2";
                fa->addInnerPolygon();
            }
            fa->status = fa->DOING_NOTHING;
        }
    }

    update();
}

void QPaintBoard::mouseReleaseEvent(QMouseEvent* event)
{
    if (fixed)
    {
        fa->translateOrRotate(Point2f(fixedposX, fixedposY), Point2f(event->x(), event->y()));
        update();
        fixed = false;
    }
}

void QPaintBoard::wheelEvent(QWheelEvent* event)
{
    if (fa->select_id == -1) return;

    if (event->delta() > 0)
    {
        fa->zoomIn();
    }
    else if (event->delta() < 0)
    {
        fa->zoomOut();
    }
}

void QPaintBoard::setFa(QWJL* _fa)
{
    fa = _fa;
}

