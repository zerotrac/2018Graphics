#include "BoundingBox.h"

#include <cmath>
#include <algorithm>

BoundingBox::BoundingBox()
{
    clear();
}

BoundingBox::~BoundingBox()
{
    // nothing
}

void BoundingBox::clear()
{
    xmin = xmax = 0.0;
    ymin = ymax = 0.0;
    sz = 0;
}

void BoundingBox::addPoint(const Point2f& pt)
{
    if (!sz)
    {
        xmin = xmax = pt.getX();
        ymin = ymax = pt.getY();
    }
    else
    {
        xmin = std::min(xmin, pt.getX());
        xmax = std::max(xmax, pt.getX());
        ymin = std::min(ymin, pt.getY());
        ymax = std::max(ymax, pt.getY());
    }
    ++sz;
}

Point2f BoundingBox::getCenter() const
{
    return Point2f((xmin + xmax) / 2.0, (ymin + ymax) / 2.0);
}

Point2f BoundingBox::getUpperLeftCorner() const
{
    return Point2f(xmin, ymax);
}

Point2f BoundingBox::getUpperRightCorner() const
{
    return Point2f(xmax, ymax);
}

Point2f BoundingBox::getLowerLeftCorner() const
{
    return Point2f(xmin, ymin);
}

Point2f BoundingBox::getLowerRightCorner() const
{
    return Point2f(xmax, ymin);
}
