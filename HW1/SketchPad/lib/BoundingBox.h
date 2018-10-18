#include "Point.h"

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

class BoundingBox
{
private:
    double xmin, xmax;
    double ymin, ymax;
    double sz;

public:
    BoundingBox();
    BoundingBox(const BoundingBox& that) = default;
    ~BoundingBox();

public:
    void clear();
    void addPoint(const Point2f& pt);
    Point2f getCenter() const;
    Point2f getUpperLeftCorner() const;
    Point2f getUpperRightCorner() const;
    Point2f getLowerLeftCorner() const;
    Point2f getLowerRightCorner() const;
};

#endif
