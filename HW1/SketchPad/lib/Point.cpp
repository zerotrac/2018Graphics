#include "Point.h"

Point2f::Point2f()
{
    x = X_DEFAULT;
    y = Y_DEFAULT;
}

Point2f::Point2f(double _x, double _y)
{
    x = _x;
    y = _y;
}

Point2f::~Point2f()
{
    // do nothing
}

Point2f Point2f::operator+ (const Point2f& that) const
{
    return Point2f(x + that.x, y + that.y);
}

Point2f Point2f::operator- (const Point2f& that) const
{
    return Point2f(x - that.x, y - that.y);
}

Point2f Point2f::operator* (double scale) const
{
    return Point2f(x * scale, y * scale);
}

Point2f Point2f::operator/ (double scale) const
{
    return Point2f(x / scale, y / scale);
}

double Point2f::getX() const
{
    return x;
}

double Point2f::getY() const
{
    return y;
}

void Point2f::flipHorizontal()
{
    y = -y;
}

void Point2f::flipVertical()
{
    x = -x;
}
