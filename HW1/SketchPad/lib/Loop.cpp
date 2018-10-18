#include "Loop.h"
#include "MathLab.h"

#include <cmath>
#include <algorithm>

#include <QDebug>

Loop2f::Loop2f()
{
    ring.clear();
    sz = 0;
}

Loop2f::~Loop2f()
{
    ring.clear();
}

Loop2f& Loop2f::operator= (const Loop2f& that)
{
    ring.clear();
    for (const Point2f& a: that.ring)
    {
        ring.push_back(a);
    }
    sz = that.sz;
    return *this;
}

int Loop2f::size() const
{
    return sz;
}

bool Loop2f::hasEnoughPoint() const
{
    return sz > 2;
}

bool Loop2f::isSelfCross() const
{
    if (MathLab::segmentCross(ring[sz - 2], ring[sz - 1], ring[0]))
    {
        return true;
    }

    if (MathLab::segmentCross(ring[sz - 1], ring[0], ring[1]))
    {
        return true;
    }
    
    for (int i = 1; i < sz - 2; ++i)
    {
        if (MathLab::segmentCross(ring[sz - 1], ring[0], ring[i], ring[i + 1]))
        {
            return true;
        }
    }

    return false;
}

bool Loop2f::isReasonableLoop() const
{
    // printf("%d %d %d\n", hasEnoughPoint(), !isSelfCross(), isOrdered());
    return hasEnoughPoint() && !isSelfCross() && isOrdered();
}

bool Loop2f::existPoint(int id) const
{
    return id >= 0 && id < sz;
}

Point2f Loop2f::getPoint(int id) const
{
    if (existPoint(id))
    {
        return ring[id];
    }
    else
    {
        return ring[((id % sz) + sz) % sz];
    }
}

Point2f Loop2f::getPrevPoint(int id) const
{
    if (existPoint(id))
    {
        --id;
        if (id < 0) id = sz - 1;
        return ring[id];
    }
    else
    {
        return Point2f(MathLab::ZERO, MathLab::ZERO);
    }
}

Point2f Loop2f::getSuccPoint(int id) const
{
    if (existPoint(id))
    {
        ++id;
        if (id == sz) id = 0;
        return ring[id];
    }
    else
    {
        return Point2f(MathLab::ZERO, MathLab::ZERO);
    }
}

bool Loop2f::addPoint(const Point2f& pt)
{
    if (sz == 1)
    {
        if (MathLab::isSamePoint(ring[sz - 1], pt))
        {
            return false;
        }
    }
    else if (sz >= 2)
    {
        qDebug() << "pt = " << pt.getX() << pt.getY();
        if (MathLab::segmentCross(ring[sz - 2], ring[sz - 1], pt))
        {
            qDebug() << "failed on test" << ring[sz - 2].getX() << ring[sz - 2].getY() << ring[sz - 1].getX() << ring[sz - 1].getY();
            return false;
        }

        for (int i = 0; i < size() - 2; ++i)
        {
            if (MathLab::segmentCross(ring[sz - 1], pt, ring[i], ring[i + 1]))
            {
                qDebug() << "failed on test" << ring[sz - 1].getX() << ring[sz - 1].getY() << ring[i].getX() << ring[i].getY() << ring[i + 1].getX() << ring[i + 1].getY();
                return false;
            }
        }
    }
    
    ring.push_back(pt);
    ++sz;
    return true;
}

bool Loop2f::modifyPointUnsafe(int id, const Point2f& pt)
{
    if (id < 0 || id >= sz)
    {
        return false;
    }
    ring[id] = pt;
    return true;
}

int Loop2f::isInside(const Point2f& a) const
{
    // check if point "a" is inside this loop
    // "a" is also inside this loop if directly on one of the edges

    double sum_ang = 0.0;
    for (int i = 0; i < sz - 1; ++i)
    {
        if (MathLab::segmentCross(ring[i], ring[i + 1], a))
        {
            return POINT_ON_THE_EDGE;
        }
        Point2f u = MathLab::normalized(ring[i] - a);
        Point2f v = MathLab::normalized(ring[i + 1] - a);
        double det_ang = MathLab::acos(MathLab::dotProduct(u, v));
        // qDebug() << "ang = " << det_ang;
        if (MathLab::isGeqZero(MathLab::crossProduct(u, v)))
        {
            sum_ang += det_ang;
        }
        else
        {
            sum_ang -= det_ang;
        }
    }

    if (MathLab::segmentCross(ring[sz - 1], ring[0], a))
    {
        return POINT_ON_THE_EDGE;
    }

    Point2f u = MathLab::normalized(ring[sz - 1] - a);
    Point2f v = MathLab::normalized(ring[0] - a);
    double det_ang = MathLab::acos(MathLab::dotProduct(u, v));
    // qDebug() << "ang = " << det_ang;
    if (MathLab::isGeqZero(MathLab::crossProduct(u, v)))
    {
        sum_ang += det_ang;
    }
    else
    {
        sum_ang -= det_ang;
    }

    // qDebug() << "sum = " << sum_ang;

    if (MathLab::isLeq(std::fabs(sum_ang), THRESHOLD_IN_OR_OUT))
    {
        return POINT_OUTSIDE;
    }
    else
    {
        return POINT_INSIDE;
    }
}

bool Loop2f::isInside(const Loop2f& that) const
{
    // check if the "that" loop is inside this loop
    for (int i = 0; i < that.sz; ++i)
    {
        // qDebug() << "judge point" << i << that.sz;
        if (isInside(that.ring[i]) != POINT_INSIDE)
        {
            return false;
        }
    }

    // qDebug() << "judge point success";

    for (int i = 0; i < sz; ++i)
    {
        for (int j = 0; j < that.sz; ++j)
        {
            // qDebug() << "judge segment" << i << j;
            if (MathLab::segmentCross(getPoint(i), getPoint(i + 1), that.getPoint(j), that.getPoint(j + 1)))
            {
                return false;
            }
        }
    }

    // qDebug() << "judge segment success";
    
    return true;
}

bool Loop2f::isOutside(const Loop2f& that) const
{
    // check if the "that" loop is outside this loop
    for (int i = 0; i < that.sz; ++i)
    {
        if (isInside(that.ring[i]) != POINT_OUTSIDE)
        {
            return false;
        }
    }

    for (int i = 0; i < sz; ++i)
    {
        for (int j = 0; j < that.sz; ++j)
        {
            if (MathLab::segmentCross(getPoint(i), getPoint(i + 1), that.getPoint(j), that.getPoint(j + 1)))
            {
                return false;
            }
        }
    }
    
    return true;
}

bool Loop2f::isInsideNotStrict(const Loop2f& that) const
{
    // check if the "that" loop is inside this loop
    for (int i = 0; i < that.sz; ++i)
    {
        // qDebug() << "judge point" << i << that.sz;
        if (isInside(that.ring[i]) == POINT_OUTSIDE)
        {
            return false;
        }
    }

    // qDebug() << "judge point success";

    for (int i = 0; i < sz; ++i)
    {
        for (int j = 0; j < that.sz; ++j)
        {
            // qDebug() << "judge segment" << i << j;
            if (MathLab::segmentCrossNotStrict(getPoint(i), getPoint(i + 1), that.getPoint(j), that.getPoint(j + 1)))
            {
                return false;
            }
        }
    }

    // qDebug() << "judge segment success";

    return true;
}

bool Loop2f::isOutsideNotStrict(const Loop2f& that) const
{
    // check if the "that" loop is outside this loop
    for (int i = 0; i < that.sz; ++i)
    {
        if (isInside(that.ring[i]) == POINT_INSIDE)
        {
            return false;
        }
    }

    for (int i = 0; i < sz; ++i)
    {
        for (int j = 0; j < that.sz; ++j)
        {
            if (MathLab::segmentCrossNotStrict(getPoint(i), getPoint(i + 1), that.getPoint(j), that.getPoint(j + 1)))
            {
                return false;
            }
        }
    }

    return true;
}

OuterLoop2f::OuterLoop2f()
{
    // do nothing
}

OuterLoop2f::~OuterLoop2f()
{
    // do nothing
}

bool OuterLoop2f::isOrdered() const
{
    int pos = 0;
    for (int i = 1; i < sz; ++i)
    {
        if (MathLab::isLessThan(ring[i].getX(), ring[pos].getX()))
        {
            pos = i;
        }
    }
    return MathLab::isLeqZero(MathLab::crossProduct(ring[pos] - getPrevPoint(pos), getSuccPoint(pos) - ring[pos]));
}

InnerLoop2f::InnerLoop2f()
{
    // do nothing
}

InnerLoop2f::~InnerLoop2f()
{
    // do nothing
}

bool InnerLoop2f::isOrdered() const
{
    int pos = 0;
    for (int i = 1; i < sz; ++i)
    {
        if (MathLab::isLessThan(ring[i].getX(), ring[pos].getX()))
        {
            pos = i;
        }
    }
    return MathLab::isGeqZero(MathLab::crossProduct(ring[pos] - getPrevPoint(pos), getSuccPoint(pos) - ring[pos]));
}
