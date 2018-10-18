#include "qwjl.h"
#include "ui_qwjl.h"

#include <QColorDialog>
#include <QDebug>
#include <QKeyEvent>

QWJL::QWJL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QWJL)
{
    ui->setupUi(this);

    QObject::connect(ui->button_addPolygon, SIGNAL(clicked(bool)), this, SLOT(statusAddPolygon()));
    QObject::connect(ui->button_deletePolygon, SIGNAL(clicked(bool)), this, SLOT(statusDeletePolygon()));
    QObject::connect(ui->button_flipH, SIGNAL(clicked(bool)), this, SLOT(polygonFlipH()));
    QObject::connect(ui->button_flipV, SIGNAL(clicked(bool)), this, SLOT(polygonFlipV()));
    QObject::connect(ui->button_inner, SIGNAL(clicked(bool)), this, SLOT(statusAddInnerPolygon()));
    QObject::connect(ui->button_clip, SIGNAL(clicked(bool)), this, SLOT(doClip()));

    paintBoard = new QPaintBoard(this);
    paintBoard->setFa(this);
    paintBoard->setGeometry(240, 20, 540, 500);

    id = -1;
    select_id = -1;
    status = DOING_NOTHING;
    tr_status = IS_NOTHING;
}

QWJL::~QWJL()
{
    delete ui;
}

void QWJL::addNewPolygon()
{
    ++id;
    ids.push_back(id);
    Polygon2f polygon;
    polygon.addOuterLoop(cur_outer_loop);
    polygons.push_back(polygon);
    QPolygonLabel* label = new QPolygonLabel(ui->widget_layer);
    polygonLabels.push_back(label);
    label->show();
    label->setId(id);
    label->move(-1, 80 * (int)(ids.size() - 1) - 1);
    ui->widget_layer->setMinimumHeight(80 * (int)ids.size());

    select(id);

    QObject::connect(label, SIGNAL(selectLabel(int)), this, SLOT(select(int)));
    QObject::connect(label, SIGNAL(askForPaintBoardUpdate()), this, SLOT(paintBoardUpdate()));
}

void QWJL::addNewPolygons(const std::vector<Polygon2f>& polygons)
{

    for (const Polygon2f& polygon: polygons)
    {
        ++id;
        ids.push_back(id);
        this->polygons.push_back(polygon);
        QPolygonLabel* label = new QPolygonLabel(ui->widget_layer);
        polygonLabels.push_back(label);
        label->show();
        label->setId(id);
        label->move(-1, 80 * (int)(ids.size() - 1) - 1);
        ui->widget_layer->setMinimumHeight(80 * (int)ids.size());

        QObject::connect(label, SIGNAL(selectLabel(int)), this, SLOT(select(int)));
        QObject::connect(label, SIGNAL(askForPaintBoardUpdate()), this, SLOT(paintBoardUpdate()));
    }

    select(-1);
}

void QWJL::addInnerPolygon()
{
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            qDebug() << "inner loop 3";
            bool chk = polygons[i].addInnerLoop(cur_inner_loop);
            if (chk)
            {
                qDebug() << "inner loop 4";
                paintBoardUpdate();
            }
            break;
        }
    }
    status = DOING_NOTHING;
}

void QWJL::changeSelectId(int _select_id)
{
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id || (select_id != _select_id && ids[i] == _select_id))
        {
            polygonLabels[i]->upd();
        }
    }
    if (select_id == _select_id)
    {
        select_id = -1;
    }
    else
    {
        select_id = _select_id;
    }
}

void QWJL::checkTR(const Point2f& a)
{
    fixed = a;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            if (polygons[i].inBoundingBox(a))
            {
                tr_status = IS_TRANSLATE;
            }
            else
            {
                tr_status = IS_ROTATE;
            }
            break;
        }
    }
}

void QWJL::translateOrRotateFake(const Point2f& a, const Point2f& b)
{
    qDebug() << "fake" << tr_status;
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            if (tr_status == IS_TRANSLATE)
            {
                polygons[i].translateFake(b - a);
            }
            else
            {
                Point2f c = polygons[i].getBoundingBoxCenter();
                polygons[i].rotateFake(a - c, b - c);
            }
            break;
        }
    }
}

void QWJL::translateOrRotate(const Point2f& a, const Point2f& b)
{
    qDebug() << "true" << tr_status;
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            if (tr_status == IS_TRANSLATE)
            {
                polygons[i].translate(b - a);
            }
            else
            {
                Point2f c = polygons[i].getBoundingBoxCenter();
                polygons[i].rotate(a - c, b - c);
            }
            tr_status = IS_NOTHING;
            break;
        }
    }
}

void QWJL::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && tr_status == IS_NOTHING)
    {
        status = DOING_NOTHING;
        changeSelectId(-1);
        paintBoardUpdate();
    }
}

void QWJL::statusAddPolygon()
{
    status = DRAWING_OUTER_LOOP;
    changeSelectId(-1);
    cur_outer_loop = OuterLoop2f();
    paintBoardUpdate();
}

void QWJL::statusAddInnerPolygon()
{
    if (select_id != -1)
    {
        status = DRAWING_INNER_LOOP;
        cur_inner_loop = InnerLoop2f();
        paintBoardUpdate();
    }
}

void QWJL::statusDeletePolygon()
{
    if (select_id == -1) return;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            ids.erase(ids.begin() + i);
            polygons.erase(polygons.begin() + i);
            polygonLabels[i]->deleteLater();
            polygonLabels.erase(polygonLabels.begin() + i);

            for (size_t j = 0; j < polygonLabels.size(); ++j)
            {
                polygonLabels[j]->move(-1, 80 * j - 1);
            }
            ui->widget_layer->setMinimumHeight(80 * (int)ids.size());

            break;
        }
    }
    select_id = -1;
    paintBoard->update();
}

void QWJL::select(int _select_id)
{
    changeSelectId(_select_id);
    paintBoardUpdate();
}

void QWJL::polygonFlipH()
{
    if (select_id == -1) return;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            polygons[i].flipHorizontal();
            paintBoardUpdate();
            break;
        }
    }
}

void QWJL::polygonFlipV()
{
    if (select_id == -1) return;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            polygons[i].flipVertical();
            paintBoardUpdate();
            break;
        }
    }
}

void QWJL::zoomIn()
{
    if (select_id == -1) return;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            polygons[i].zoomIn();
            paintBoardUpdate();
            break;
        }
    }
}

void QWJL::zoomOut()
{
    if (select_id == -1) return;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            polygons[i].zoomOut();
            paintBoardUpdate();
            break;
        }
    }
}

void QWJL::doClip()
{
    if (select_id == -1) return;

    QString s = ui->line->text();
    bool ok;
    int t = s.toInt(&ok);
    if (ok) qDebug() << t;

    int posi = -1, posj = -1;
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (ids[i] == select_id)
        {
            posi = i;
        }
        else if (ids[i] == t)
        {
            posj = i;
        }
    }

    if (posj == -1 || posj == posi) return;

    std::vector<Polygon2f> ans = polygons[posi].clipping(polygons[posj]);
    qDebug() << "clip idea:" << ans.size();
    addNewPolygons(ans);
}

void QWJL::paintBoardUpdate()
{
    paintBoard->update();
    // paintBoard->repaint();
}
