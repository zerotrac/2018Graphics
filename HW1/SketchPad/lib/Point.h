#ifndef POINT_H
#define POINT_H

class Point2f
{
private:
    static constexpr double X_DEFAULT = 0.0;
    static constexpr double Y_DEFAULT = 0.0;
    
private:
    double x;
    double y;

public:
    Point2f();
    Point2f(double _x, double _y);
    Point2f(const Point2f& that) = default;
    ~Point2f();

    Point2f operator+ (const Point2f& that) const;
    Point2f operator- (const Point2f& that) const;
    Point2f operator* (double scale) const;
    Point2f operator/ (double scale) const;
    
public:
    double getX() const;
    double getY() const;
    void flipHorizontal();
    void flipVertical();
};

#endif
