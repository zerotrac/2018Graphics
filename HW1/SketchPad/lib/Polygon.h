#include "RotationMatrix.h"
#include "BoundingBox.h"
#include "Loop.h"

#include "MathLab.h"

#include <vector>

#ifndef POLYGON_H
#define POLYGON_H

class ParallelSegment2f
{

/*
    (b, a) --- (c, a)
*/

private:
    double a, b, c;

public:
    ParallelSegment2f(double _a, double _b, double _c);
    ~ParallelSegment2f();

public:
    double getA() const;
    double getB() const;
    double getC() const;
    Point2f getStartPoint() const;
    Point2f getEndPoint() const;
};

class AETUnit2f
{
private:
    double x_cur;
    double x_delta;
    int y_start;
    int y_max;

public:
    AETUnit2f(double _x_cur, double _x_delta, int _y_start, int _y_max);
    AETUnit2f(const AETUnit2f& that) = default;
    ~AETUnit2f();

public:
    friend bool cmpByXCurrentPosition(const AETUnit2f& a, const AETUnit2f& b);
    friend bool cmpByYStartPosition(const AETUnit2f& a, const AETUnit2f& b);

public:
    void update();
    double getXCur() const;
    int getYStart() const;
    int getYMax() const;
};

class ClipUnit2f
{
public:
    int left;
    int loopkind; // 0 waihuan 1 neihuan
    int looplabel;
    int loopnode;
    double pos;
    int ilabel;
    int ikind; // 0 rudian 1 chudian

public:
    ClipUnit2f(int _left, int _loopkind, int _looplabel, int _loopnode, double _pos, int _ilabel, int _ikind)
    {
        left = _left;
        loopkind = _loopkind;
        looplabel = _looplabel;
        loopnode = _loopnode;
        pos = _pos;
        ilabel = _ilabel;
        ikind = _ikind;
    }

    bool operator< (const ClipUnit2f& that) const
    {
        if (loopkind != that.loopkind) return loopkind < that.loopkind;
        if (looplabel != that.looplabel) return looplabel < that.looplabel;
        if (loopnode != that.loopnode) return loopnode < that.loopnode;
        if (!MathLab::isEqual(pos, that.pos)) return MathLab::isLeq(pos, that.pos);
        // return true;
        return false;
    }
};

class Polygon2f
{
private:
    OuterLoop2f outerLoop;
    std::vector<InnerLoop2f> innerLoop;

    OuterLoop2f outerLoop_realtime;
    std::vector<InnerLoop2f> innerLoop_realtime;

    RotationMatrix rotationMatrix;
    BoundingBox boundingBox;

public:
    Polygon2f();
    ~Polygon2f();

public:
    bool addOuterLoop(const OuterLoop2f& _outer_loop);  // should be counter-clockwise
    bool addInnerLoop(const InnerLoop2f& _inner_loop);  // should be clockwise
    bool inBoundingBox(const Point2f& a) const;

    OuterLoop2f getOuterLoop() const;
    std::vector<InnerLoop2f> getInnerLoop() const;

    void rotateFake(const Point2f& a, const Point2f& b);
    void rotate(const Point2f& a, const Point2f& b);
    void translateFake(const Point2f& a);
    void translate(const Point2f& a);
    void zoomIn();
    void zoomOut();
    void flipHorizontal();
    void flipVertical();
    void update();
    
    std::vector<Point2f> getBoundingBox() const;
    Point2f getBoundingBoxCenter() const;
    std::vector<ParallelSegment2f> getColoredSegment() const;
    std::vector<Polygon2f> clipping(const Polygon2f& that) const;
};

#endif
