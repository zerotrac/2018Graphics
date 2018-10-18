#include "Point.h"

#include <vector>

#ifndef LOOP_H
#define LOOP_H

class Loop2f
{
private:
    static const int POINT_INSIDE = 0;
    static const int POINT_ON_THE_EDGE = 1;
    static const int POINT_OUTSIDE = 2;

    static constexpr double THRESHOLD_IN_OR_OUT = 1.0;

protected:
    std::vector<Point2f> ring;
    int sz;
    
public:
    Loop2f();
    Loop2f(const Loop2f& that) = default;
    virtual ~Loop2f();

public:
    Loop2f& operator= (const Loop2f& that);

public:
    int size() const;
    bool hasEnoughPoint() const;
    bool isSelfCross() const;
    virtual bool isOrdered() const = 0;
    bool isReasonableLoop() const;
    bool existPoint(int id) const;
    Point2f getPoint(int id) const;
    Point2f getPrevPoint(int id) const;
    Point2f getSuccPoint(int id) const;

    bool addPoint(const Point2f& pt);
    bool modifyPointUnsafe(int id, const Point2f& pt);

    int isInside(const Point2f& a) const;
    bool isInside(const Loop2f& that) const;
    bool isOutside(const Loop2f& that) const;

    bool isInsideNotStrict(const Loop2f& that) const;
    bool isOutsideNotStrict(const Loop2f& that) const;
};

class OuterLoop2f : public Loop2f
{
public:
    OuterLoop2f();
    ~OuterLoop2f();

public:
    bool isOrdered() const;
};

class InnerLoop2f : public Loop2f
{
public:
    InnerLoop2f();
    ~InnerLoop2f();

public:
    bool isOrdered() const;
};

#endif
