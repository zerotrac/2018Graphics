#ifndef QPOLYGONLABEL_H
#define QPOLYGONLABEL_H

#include <QWidget>

namespace Ui {
class QPolygonLabel;
}

class QPolygonLabel : public QWidget
{
    Q_OBJECT

public:
    int id;
    bool selected;
    QColor outer_color;
    QColor inner_color;

public:
    static QString fromColorToQString(QColor color);

public:
    explicit QPolygonLabel(QWidget *parent = nullptr);
    ~QPolygonLabel();

public:
    int getId() const;
    void setId(int _id);
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void upd();
    QColor getOuterColor() const;
    QColor getInnerColor() const;

signals:
    void selectLabel(int);
    void askForPaintBoardUpdate();

public slots:
    void changeOuterColor();
    void changeInnerColor();

private:
    Ui::QPolygonLabel *ui;
};

#endif // QPOLYGONLABEL_H
