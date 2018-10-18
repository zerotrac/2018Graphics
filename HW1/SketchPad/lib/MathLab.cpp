#include "MathLab.h"

#include <cmath>
#include <algorithm>

#include <QDebug>

namespace MathLab
{

bool isEqual(double a, double b)
{
    return fabs(a - b) < EPS;
}

bool isEqualToZero(double a)
{
    return fabs(a) < EPS;
}

bool isLeq(double a, double b)
{
    return a < b + EPS;
}

bool isLeq(double a, double b, double c)
{
    return isLeq(a, b) && isLeq(b, c);
}

bool isLeq(const Point2f& a, const Point2f& b)
{
    return isLeq(a.getX(), b.getX()) && isLeq(a.getY(), b.getY());
}

bool isLeq(const Point2f& a, const Point2f& b, const Point2f& c)
{
    return isLeq(a, b) && isLeq(b, c);
}

bool isLeqZero(double a)
{
    return isLeq(a, ZERO);
}

bool isLessThan(double a, double b)
{
    return a + EPS <= b;
}

bool isLessThan(double a, double b, double c)
{
    return isLessThan(a, b) && isLessThan(b, c);
}

bool isLessThan(const Point2f& a, const Point2f& b)
{
    return isLessThan(a.getX(), b.getX()) && isLessThan(a.getY(), b.getY());
}

bool isLessThan(const Point2f& a, const Point2f& b, const Point2f& c)
{
    return isLessThan(a, b) && isLessThan(b, c);
}

bool isGeq(double a, double b)
{
    return a + EPS > b;
}

bool isGeq(double a, double b, double c)
{
    return isGeq(a, b) && isGeq(b, c);
}

bool isGeq(const Point2f& a, const Point2f& b)
{
    return isGeq(a.getX(), b.getX()) && isGeq(a.getY(), b.getY());
}

bool isGeq(const Point2f& a, const Point2f& b, const Point2f& c)
{
    return isGeq(a, b) && isGeq(b, c);
}

bool isGeqZero(double a)
{
    return isGeq(a, ZERO);
}

bool isGreaterThan(double a, double b)
{
    return a >= b + EPS;
}

bool isGreaterThan(double a, double b, double c)
{
    return isGreaterThan(a, b) && isGreaterThan(b, c);
}

bool isGreaterThan(const Point2f& a, const Point2f& b)
{
    return isGreaterThan(a.getX(), b.getX()) && isGreaterThan(a.getY(), b.getY());
}

bool isGreaterThan(const Point2f& a, const Point2f& b, const Point2f& c)
{
    return isGreaterThan(a, b) && isGreaterThan(b, c);
}

bool isSamePoint(const Point2f& a, const Point2f& b)
{
    return isEqual(a.getX(), b.getX()) && isEqual(a.getY(), b.getY());
}

int round(double a)
{
    return int(a + 0.5);
}

double norm(const Point2f& a)
{
    return std::sqrt(a.getX() * a.getX() + a.getY() * a.getY());
}

Point2f normalized(const Point2f& a)
{
    double _norm = norm(a);
    if (isEqualToZero(_norm))
    {
        return a;
    }
    return a / _norm;
}

double dotProduct(const Point2f& a, const Point2f& b)
{
    return a.getX() * b.getX() + a.getY() * b.getY();
}

double crossProduct(const Point2f& a, const Point2f& b)
{
    return a.getX() * b.getY() - a.getY() * b.getX();
}

double cos(const Point2f& a)
{
    double n_a = norm(a);
    if (isEqualToZero(n_a))
    {
        return 1.0;
    }
    return a.getX() / n_a;
}

double sin(const Point2f& a)
{
    double n_a = norm(a);
    if (isEqualToZero(n_a))
    {
        return 0.0;
    }
    return a.getY() / n_a;
}

double cos(const Point2f& a, const Point2f& b)
{
    double n_a = norm(a);
    double n_b = norm(b);
    if (isEqualToZero(n_a) || isEqualToZero(n_b))
    {
        return 1.0;
    }
    return dotProduct(a, b) / (n_a * n_b);
}

double sin(const Point2f& a, const Point2f& b)
{
    double n_a = norm(a);
    double n_b = norm(b);
    if (isEqualToZero(n_a) || isEqualToZero(n_b))
    {
        return 0.0;
    }
    return crossProduct(a, b) / (n_a * n_b);
}

double acos(double a)
{
    if (isGeq(a, 1.0)) a = 1.0;
    if (isLeq(a, -1.0)) a = -1.0;
    return std::acos(a);
}

bool segmentCross(const Point2f& a, const Point2f& b, const Point2f& c)
{
    Point2f u = a - b;
    Point2f v = c - b;
    qDebug() << "2 segs" << u.getX() << u.getY() << v.getX() << v.getY();
    if (!isEqualToZero(crossProduct(a - c, b - c)))
    {
        return false;
    }
    if (isLeqZero(dotProduct(a - c, b - c)))
    {
        return true;
    }
    return false;
}

bool segmentCross(const Point2f& a, const Point2f& b, const Point2f& c, const Point2f& d)
{
    double p = crossProduct(b - a, c - b);
    double q = crossProduct(b - a, d - b);
    double r = crossProduct(d - c, a - d);
    double s = crossProduct(d - c, b - d);

    qDebug() << p << q;
    if (isEqualToZero(p) && isEqualToZero(q))
    {
        // a, b, c, d are on the same line
        return segmentCross(a, b, c) || segmentCross(a, b, d) || segmentCross(c, d, a) || segmentCross(c, d, b);
    }

    return isLeqZero(p * q) && isLeqZero(r * s);
}

bool segmentCrossNotStrict(const Point2f& a, const Point2f& b, const Point2f& c, const Point2f& d)
{
    double p = crossProduct(b - a, c - b);
    double q = crossProduct(b - a, d - b);
    double r = crossProduct(d - c, a - d);
    double s = crossProduct(d - c, b - d);

    if (isEqualToZero(p) && isEqualToZero(q))
    {
        // a, b, c, d are on the same line

        /*if (isLeq(a, b))
        {
            if (isLeq(c, d))
            {
                return !isLeq(d, a) && !isLeq(b, c);
            }
            else
            {
                return !isLeq(c, a) && !isLeq(b, d);
            }
        }
        else
        {
            if (isLeq(c, d))
            {
                return !isLeq(d, b) || !isLeq(a, c);
            }
            else
            {
                return !isLeq(c, b) || !isLeq(a, d);
            }
        }*/

        return false;
    }

    return isLessThan(p * q, ZERO) && isLessThan(r * s, ZERO);
}

std::vector<Point2f> getIntersectionGeneral(const Point2f& a, const Point2f& b, const Point2f& c, const Point2f& d)
{
    if (!segmentCross(a, b, c, d))
    {
        std::vector<Point2f> empty;
        return empty;
    }

    std::vector<Point2f> ret;

    double p = crossProduct(b - a, c - b);
    double q = crossProduct(b - a, d - b);
    double r = crossProduct(d - c, a - d);
    double s = crossProduct(d - c, b - d);

    if (isEqualToZero(p) && isEqualToZero(q))
    {
        double p, q;
        // test b
        p = 1.0;
        if (isEqual(a.getX(), b.getX()))
        {
            q = (b.getY() - c.getY()) / (d.getY() - c.getY());
        }
        else
        {
            q = (b.getX() - c.getX()) / (d.getX() - c.getX());
        }
        if (isGreaterThan(q, ZERO) && isLeq(q, 1.0))
        {
            ret.push_back(Point2f(p, q));
        }

        // test d
        if (isEqual(a.getX(), b.getX()))
        {
            p = (d.getY() - a.getY()) / (b.getY() - a.getY());
        }
        else
        {
            p = (d.getX() - a.getX()) / (b.getX() - a.getX());
        }
        q = 1.0;
        if (isGreaterThan(p, ZERO) && isLeq(p, 1.0))
        {
            ret.push_back(Point2f(p, q));
        }

        return ret;
    }

    /*
    a_x + (b_x - a_x) * p = c_x + (d_x - c_x) * q
    a_y + (b_y - a_y) * p = c_y + (d_y - c_y) * q
    */

    if (isEqual(a.getX(), b.getX()))
    {
        double q = (a.getX() - c.getX()) / (d.getX() - c.getX());
        double p = (c.getY() + (d.getY() - c.getY()) * q - a.getY()) / (b.getY() - a.getY());
        if (!isEqualToZero(p) && !isEqualToZero(q)) ret.push_back(Point2f(p, q));
    }
    else if (isEqual(a.getY(), b.getY()))
    {
        double q = (a.getY() - c.getY()) / (d.getY() - c.getY());
        double p = (c.getX() + (d.getX() - c.getX()) * q - a.getX()) / (b.getX() - a.getX());
        if (!isEqualToZero(p) && !isEqualToZero(q)) ret.push_back(Point2f(p, q));
    }
    else
    {
        double u = (c.getX() - a.getX()) * (d.getY() - c.getY()) - (c.getY() - a.getY()) * (d.getX() - c.getX());
        double v = (b.getX() - a.getX()) * (d.getY() - c.getY()) - (b.getY() - a.getY()) * (d.getX() - c.getX());
        double p = u / v;
        double q = (a.getX() + (b.getX() - a.getX()) * p - c.getX()) / (d.getX() - c.getX());
        if (!isEqualToZero(p) && !isEqualToZero(q)) ret.push_back(Point2f(p, q));
    }

    return ret;
}

}
