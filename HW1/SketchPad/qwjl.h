#ifndef QWJL_H
#define QWJL_H

class QPaintBoard;

#include "qpaintboard.h"
#include "qpolygonlabel.h"
#include "lib/Polygon.h"

#include <QWidget>

namespace Ui {
class QWJL;
}

class QWJL : public QWidget
{
    Q_OBJECT

public:
    static const int DOING_NOTHING = 0;
    static const int DRAWING_OUTER_LOOP = 1;
    static const int DRAWING_INNER_LOOP = 2;
    static const int SELECTING = 3;

    static const int IS_NOTHING = 0;
    static const int IS_TRANSLATE = 1;
    static const int IS_ROTATE = 2;

public:
    explicit QWJL(QWidget *parent = nullptr);
    ~QWJL();

public:
    QPaintBoard* paintBoard;
    std::vector<int> ids;
    std::vector<Polygon2f> polygons;
    std::vector<QPolygonLabel*> polygonLabels;

    OuterLoop2f cur_outer_loop;
    InnerLoop2f cur_inner_loop;

    int id;
    int select_id;
    int status;
    int tr_status;

    Point2f fixed;

public:
    void addNewPolygon();
    void addNewPolygons(const std::vector<Polygon2f>& polygons);
    void addInnerPolygon();
    void changeSelectId(int _select_id);
    void checkTR(const Point2f& a);
    void translateOrRotateFake(const Point2f& a, const Point2f& b);
    void translateOrRotate(const Point2f& a, const Point2f& b);
    void keyPressEvent(QKeyEvent* event);

public slots:
    void statusAddPolygon();
    void statusAddInnerPolygon();
    void statusDeletePolygon();
    void select(int);
    void polygonFlipH();
    void polygonFlipV();
    void zoomIn();
    void zoomOut();
    void doClip();
    void paintBoardUpdate();

private:
    Ui::QWJL *ui;
};

#endif // QWJL_H
