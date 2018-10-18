#include "Point.h"

#ifndef ROTATIONMATRIX_H
#define ROTATIONMATRIX_H

class RotationMatrix
{
private:
    static constexpr double ZOOM_COEFFICIENT = 0.1;

private:
    double base[3][3];
    double show[3][3];
    double base_inv[3][3];
    double show_inv[3][3];

    double zooming;
    bool hasFlippedHorizontal;
    bool hasFlippedVertical;
    Point2f center;

public:
    RotationMatrix();
    RotationMatrix(const RotationMatrix& that) = default;
    ~RotationMatrix();

public:
    void clear();
    void copy();
    void addCenterPoint(const Point2f& a);
    Point2f getCenter() const;
    /*
       b
      /
     /
    a------a'
    */
    void rotateFake(const Point2f& a, const Point2f& b);
    void rotate(const Point2f& a, const Point2f& b);
    void translateFake(const Point2f& a);
    void translate(const Point2f& a);
    void zoomIn();
    void zoomOut();
    void flipHorizontal();
    void flipVertical();

    Point2f transform(const Point2f& a) const;
    Point2f transformInv(const Point2f& a) const;
};

#endif
