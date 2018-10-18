#include "RotationMatrix.h"
#include "MathLab.h"

#include <QDebug>

RotationMatrix::RotationMatrix()
{
    clear();
}

RotationMatrix::~RotationMatrix() 
{
    // do nothing
}

void RotationMatrix::clear()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (i == j)
            {
                base[i][j] = 1.0;
                base_inv[i][j] = 1.0;
            }
            else
            {
                base[i][j] = 0.0;
                base_inv[i][j] = 0.0;
            }
        }
    }

    zooming = 1.0;
    hasFlippedHorizontal = false;
    hasFlippedVertical = false;
    copy();
}

void RotationMatrix::copy()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            show[i][j] = base[i][j];
            show_inv[i][j] = base_inv[i][j];
        }
    }
}

void RotationMatrix::addCenterPoint(const Point2f& a)
{
    center = a;
}

Point2f RotationMatrix::getCenter() const
{
    return center;
}

void RotationMatrix::rotateFake(const Point2f& a, const Point2f& b)
{
    copy();
    double cos_v = MathLab::cos(a, b);
    double sin_v = MathLab::sin(a, b);
    qDebug() << cos_v << sin_v;
    qDebug() << "a = " << a.getX() << a.getY();
    qDebug() << "b = " << b.getX() << b.getY();

    if ((hasFlippedHorizontal && !hasFlippedVertical) || (!hasFlippedHorizontal && hasFlippedVertical))
    {
        sin_v = -sin_v;
    }

    show[0][0] = cos_v * base[0][0] - sin_v * base[1][0];
    show[0][1] = cos_v * base[0][1] - sin_v * base[1][1];
    show[1][0] = sin_v * base[0][0] + cos_v * base[1][0];
    show[1][1] = sin_v * base[0][1] + cos_v * base[1][1];
    show_inv[0][0] = cos_v * base_inv[0][0] + sin_v * base_inv[1][0];
    show_inv[0][1] = cos_v * base_inv[0][1] + sin_v * base_inv[1][1];
    show_inv[1][0] = -sin_v * base_inv[0][0] + cos_v * base_inv[1][0];
    show_inv[1][1] = -sin_v * base_inv[0][1] + cos_v * base_inv[1][1];
}

void RotationMatrix::rotate(const Point2f& a, const Point2f& b)
{
    double cos_v = MathLab::cos(a, b);
    double sin_v = MathLab::sin(a, b);

    if ((hasFlippedHorizontal && !hasFlippedVertical) || (!hasFlippedHorizontal && hasFlippedVertical))
    {
        sin_v = -sin_v;
    }

    double p = cos_v * base[0][0] - sin_v * base[1][0];
    double q = cos_v * base[0][1] - sin_v * base[1][1];
    double r = sin_v * base[0][0] + cos_v * base[1][0];
    double s = sin_v * base[0][1] + cos_v * base[1][1];
    base[0][0] = p;
    base[0][1] = q;
    base[1][0] = r;
    base[1][1] = s;

    double p0 = cos_v * base_inv[0][0] + sin_v * base_inv[1][0];
    double q0 = cos_v * base_inv[0][1] + sin_v * base_inv[1][1];
    double r0 = -sin_v * base_inv[0][0] + cos_v * base_inv[1][0];
    double s0 = -sin_v * base_inv[0][1] + cos_v * base_inv[1][1];
    base_inv[0][0] = p0;
    base_inv[0][1] = q0;
    base_inv[1][0] = r0;
    base_inv[1][1] = s0;

    copy();
}

void RotationMatrix::translateFake(const Point2f& a)
{
    copy();
    show[0][2] += a.getX();
    show[1][2] += a.getY();
    show_inv[0][2] -= a.getX();
    show_inv[1][2] -= a.getY();
}

void RotationMatrix::translate(const Point2f& a)
{
    base[0][2] += a.getX();
    base[1][2] += a.getY();
    base_inv[0][2] -= a.getX();
    base_inv[1][2] -= a.getY();
    copy();
}

void RotationMatrix::zoomIn()
{
    zooming += ZOOM_COEFFICIENT;
}

void RotationMatrix::zoomOut()
{
    if (MathLab::isGeqZero(zooming - ZOOM_COEFFICIENT))
    {
        zooming -= ZOOM_COEFFICIENT;
    }
}

void RotationMatrix::flipHorizontal()
{
    hasFlippedHorizontal = !hasFlippedHorizontal;
}

void RotationMatrix::flipVertical()
{
    hasFlippedVertical = !hasFlippedVertical;
}

Point2f RotationMatrix::transform(const Point2f& a) const
{
    // qDebug() << "show = " << show[0][2] << show[1][2];
    Point2f aa = a - center;
    Point2f ret = Point2f(show[0][0] * aa.getX() + show[0][1] * aa.getY(), show[1][0] * aa.getX() + show[1][1] * aa.getY()) * zooming;
    if (hasFlippedHorizontal) ret.flipHorizontal();
    if (hasFlippedVertical) ret.flipVertical();
    return ret + center + Point2f(show[0][2], show[1][2]);
}

Point2f RotationMatrix::transformInv(const Point2f& a) const
{
    Point2f aa = a - center + Point2f(show_inv[0][2], show_inv[1][2]);
    // qDebug() << "a0 = " << a.getX() << a.getY() << center.getX() << center.getY();
    // qDebug() << "aa = " << aa.getX() << aa.getY();
    // qDebug() << "showInv" << show_inv[0][0] << show_inv[0][1] << show_inv[1][0] << show_inv[1][1];
    Point2f ret = Point2f(show_inv[0][0] * aa.getX() + show_inv[0][1] * aa.getY(), show_inv[1][0] * aa.getX() + show_inv[1][1] * aa.getY()) / zooming;
    // qDebug() << "ret = " << ret.getX() << ret.getY();
    if (hasFlippedHorizontal) ret.flipHorizontal();
    if (hasFlippedVertical) ret.flipVertical();
    return ret + center;
}
