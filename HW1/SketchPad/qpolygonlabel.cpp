#include "qpolygonlabel.h"
#include "ui_qpolygonlabel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDebug>

QString QPolygonLabel::fromColorToQString(QColor color)
{
    int a = color.red();
    int b = color.green();
    int c = color.blue();

    QString a0 = QString::number(a, 16);
    QString b0 = QString::number(b, 16);
    QString c0 = QString::number(c, 16);

    if (a0.size() < 2) a0 = "0" + a0;
    if (b0.size() < 2) b0 = "0" + b0;
    if (c0.size() < 2) c0 = "0" + c0;

    return "#" + a0 + b0 + c0;
}

QPolygonLabel::QPolygonLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QPolygonLabel)
{
    ui->setupUi(this);

    selected = false;

    outer_color = Qt::black;
    inner_color = Qt::white;

    ui->button_outer_color->setStyleSheet("border: 0px; background-color: " + fromColorToQString(outer_color));
    ui->button_inner_color->setStyleSheet("border: 0px; background-color: " + fromColorToQString(inner_color));

    QObject::connect(ui->button_outer_color, SIGNAL(clicked(bool)), this, SLOT(changeOuterColor()));
    QObject::connect(ui->button_inner_color, SIGNAL(clicked(bool)), this, SLOT(changeInnerColor()));
}

QPolygonLabel::~QPolygonLabel()
{
    delete ui;
}

int QPolygonLabel::getId() const
{
    return id;
}

void QPolygonLabel::setId(int _id)
{
    id = _id;
    ui->label_id->setText("Polygon " + QString::number(id));
}

void QPolygonLabel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (selected)
    {
        painter.setPen(QPen(Qt::blue, 5));
    }
    else
    {
        painter.setPen(QPen(Qt::black, 5));
    }
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, this->width(), this->height());
}

void QPolygonLabel::mousePressEvent(QMouseEvent* event)
{
    emit selectLabel(id);
}

void QPolygonLabel::upd()
{
    selected = !selected;
    update();
}

QColor QPolygonLabel::getOuterColor() const
{
    return outer_color;
}

QColor QPolygonLabel::getInnerColor() const
{
    return inner_color;
}

void QPolygonLabel::changeOuterColor()
{
    QColor c = QColorDialog::getColor(outer_color);
    if (c.isValid())
    {
        outer_color = c;
        ui->button_outer_color->setStyleSheet("border: 0px; background-color: " + fromColorToQString(outer_color));
        emit(askForPaintBoardUpdate());
    }
}

void QPolygonLabel::changeInnerColor()
{
    QColor c = QColorDialog::getColor(inner_color);
    if (c.isValid())
    {
        inner_color = c;
        ui->button_inner_color->setStyleSheet("border: 0px; background-color: " + fromColorToQString(inner_color));
        emit(askForPaintBoardUpdate());
    }
}
