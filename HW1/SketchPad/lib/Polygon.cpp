#include "Polygon.h"

#include <cmath>
#include <algorithm>

#include <QDebug>

ParallelSegment2f::ParallelSegment2f(double _a, double _b, double _c)
{
    a = _a;
    b = _b;
    c = _c;
}

ParallelSegment2f::~ParallelSegment2f()
{
    // do nothing
}

double ParallelSegment2f::getA() const
{
    return a;
}

double ParallelSegment2f::getB() const
{
    return b;
}

double ParallelSegment2f::getC() const
{
    return c;
}

Point2f ParallelSegment2f::getStartPoint() const
{
    return Point2f(b, a);
}

Point2f ParallelSegment2f::getEndPoint() const
{
    return Point2f(c, a);
}

AETUnit2f::AETUnit2f(double _x_cur, double _x_delta, int _y_start, int _y_max)
{
    x_cur = _x_cur;
    x_delta = _x_delta;
    y_start = _y_start;
    y_max = _y_max;
}

AETUnit2f::~AETUnit2f()
{
    // do nothing
}

bool cmpByXCurrentPosition(const AETUnit2f& a, const AETUnit2f& b)
{
    return a.x_cur < b.x_cur;
}

bool cmpByYStartPosition(const AETUnit2f& a, const AETUnit2f& b)
{
    return a.y_start < b.y_start;
}

void AETUnit2f::update()
{
    x_cur += x_delta;
}

double AETUnit2f::getXCur() const
{
    return x_cur;
}

int AETUnit2f::getYStart() const
{
    return y_start;
}

int AETUnit2f::getYMax() const
{
    return y_max;
}

Polygon2f::Polygon2f()
{
    innerLoop.clear();
}

Polygon2f::~Polygon2f()
{
    // do nothing
}

bool Polygon2f::addOuterLoop(const OuterLoop2f& _outerLoop)
{
    outerLoop = _outerLoop;
    outerLoop_realtime = _outerLoop;

    for (int i = 0; i < _outerLoop.size(); ++i)
    {
        boundingBox.addPoint(_outerLoop.getPoint(i));
    }
    rotationMatrix.addCenterPoint(boundingBox.getCenter());

    return true;
}

bool Polygon2f::addInnerLoop(const InnerLoop2f& _innerLoop)
{
    /*
    should return false if
    the inner-loop is not inside the outer-loop
    the inner-loop is not outside any other inner-loop
    */

    // check if the inner-loop is inside the outer-loop
    if (!outerLoop_realtime.size()) return false;
    qDebug() << "pro 0";
    if (!outerLoop_realtime.isInside(_innerLoop)) return false;

    qDebug() << "pro 1";

    // check if the inner-loop is outside any other inner-loop
    for (const InnerLoop2f& a: innerLoop_realtime)
    {
        if (!a.isOutside(_innerLoop)) return false;
    }

    innerLoop_realtime.push_back(_innerLoop);
    innerLoop.push_back(_innerLoop);
    
    InnerLoop2f& cur = innerLoop[innerLoop.size() - 1];
    for (int i = 0; i < cur.size(); ++i)
    {
        cur.modifyPointUnsafe(i, rotationMatrix.transformInv(cur.getPoint(i)));
    }
    return true;
}

bool Polygon2f::inBoundingBox(const Point2f& a) const
{
    qDebug() << "a = " << a.getX() << a.getY();
    Point2f b = rotationMatrix.transformInv(a);
    qDebug() << "b = " << b.getX() << b.getY();
    Point2f c = boundingBox.getLowerLeftCorner();
    Point2f d = boundingBox.getUpperRightCorner();
    qDebug() << "c, d = " << c.getX() << c.getY() << d.getX() << d.getY();
    return MathLab::isLeq(boundingBox.getLowerLeftCorner(), b, boundingBox.getUpperRightCorner());
}

OuterLoop2f Polygon2f::getOuterLoop() const
{
    return outerLoop_realtime;
}

std::vector<InnerLoop2f> Polygon2f::getInnerLoop() const
{
    return innerLoop_realtime;
}

void Polygon2f::rotateFake(const Point2f& a, const Point2f& b)
{
    rotationMatrix.rotateFake(a, b);
    update();
}

void Polygon2f::rotate(const Point2f& a, const Point2f& b)
{
    rotationMatrix.rotate(a, b);
    update();
}

void Polygon2f::translateFake(const Point2f& a)
{
    rotationMatrix.translateFake(a);
    update();
}

void Polygon2f::translate(const Point2f& a)
{
    rotationMatrix.translate(a);
    update();
}

void Polygon2f::zoomIn()
{
    rotationMatrix.zoomIn();
    update();
}

void Polygon2f::zoomOut()
{
    rotationMatrix.zoomOut();
    update();
}

void Polygon2f::flipHorizontal()
{
    rotationMatrix.flipHorizontal();
    update();
}

void Polygon2f::flipVertical()
{
    rotationMatrix.flipVertical();
    update();
}

void Polygon2f::update()
{
    outerLoop_realtime = outerLoop;
    for (int i = 0; i < outerLoop_realtime.size(); ++i)
    {
        outerLoop_realtime.modifyPointUnsafe(i, rotationMatrix.transform(outerLoop_realtime.getPoint(i)));
    }

    innerLoop_realtime.clear();
    for (InnerLoop2f a: innerLoop)
    {
        for (int i = 0; i < a.size(); ++i)
        {
            a.modifyPointUnsafe(i, rotationMatrix.transform(a.getPoint(i)));
        }
        innerLoop_realtime.push_back(a);
    }
}

std::vector<Point2f> Polygon2f::getBoundingBox() const
{
    std::vector<Point2f> ret;
    ret.push_back(rotationMatrix.transform(boundingBox.getUpperLeftCorner()));
    ret.push_back(rotationMatrix.transform(boundingBox.getLowerLeftCorner()));
    ret.push_back(rotationMatrix.transform(boundingBox.getLowerRightCorner()));
    ret.push_back(rotationMatrix.transform(boundingBox.getUpperRightCorner()));
    ret.push_back(rotationMatrix.transform(boundingBox.getUpperLeftCorner()));
    return ret;
}

Point2f Polygon2f::getBoundingBoxCenter() const
{
    return rotationMatrix.transform(boundingBox.getCenter());
}

std::vector<ParallelSegment2f> Polygon2f::getColoredSegment() const
{
    std::vector<ParallelSegment2f> ret;
    std::vector<AETUnit2f> all_aet;

    int ymin = 999999999;
    int ymax = -999999999;

    for (int i = 0; i < outerLoop_realtime.size(); ++i)
    {
        Point2f a = outerLoop_realtime.getPoint(i);
        Point2f b = outerLoop_realtime.getPoint(i + 1);

        // (p, q) --- (r, s)
        int p = MathLab::round(a.getX());
        int q = MathLab::round(a.getY());
        int r = MathLab::round(b.getX());
        int s = MathLab::round(b.getY());

        if (q != s)
        {
            if (q > s)
            {
                std::swap(p, r);
                std::swap(q, s);
            }
            all_aet.push_back(AETUnit2f(p, 1.0 * (r - p) / (s - q), q, s));
            ymin = std::min(ymin, q);
            ymax = std::max(ymax, s);
        }
    }

    for (const InnerLoop2f& inner: innerLoop_realtime)
    {
        for (int i = 0; i < inner.size(); ++i)
        {
            Point2f a = inner.getPoint(i);
            Point2f b = inner.getPoint(i + 1);

            int p = MathLab::round(a.getX());
            int q = MathLab::round(a.getY());
            int r = MathLab::round(b.getX());
            int s = MathLab::round(b.getY());

            if (q != s)
            {
                if (q > s)
                {
                    std::swap(p, r);
                    std::swap(q, s);
                }
                all_aet.push_back(AETUnit2f(p, 1.0 * (r - p) / (s - q), q, s));
                ymin = std::min(ymin, q);
                ymax = std::max(ymax, s);
            }
        }
    }
    
    std::sort(all_aet.begin(), all_aet.end(), cmpByYStartPosition);
    auto iter = all_aet.begin();
    std::vector<AETUnit2f> cur_aet;

    for (int i = ymin; i <= ymax; ++i)
    {
        std::vector<AETUnit2f> cur_aet_copy;
        for (AETUnit2f aet: cur_aet)
        {
            if (aet.getYMax() > i)
            {
                aet.update();
                cur_aet_copy.push_back(aet);
            }
        }

        while (iter != all_aet.end())
        {
            if (iter->getYStart() == i)
            {
                cur_aet_copy.push_back(*iter);
                ++iter;
            }
            else
            {
                break;
            }
        }

        std::sort(cur_aet_copy.begin(), cur_aet_copy.end(), cmpByXCurrentPosition);
        for (size_t j = 0; j < cur_aet_copy.size(); j += 2)
        {
            ret.push_back(ParallelSegment2f(i, cur_aet_copy[j].getXCur(), cur_aet_copy[j + 1].getXCur()));
        }

        cur_aet = cur_aet_copy;
    }

    return ret;
}

std::vector<Polygon2f> Polygon2f::clipping(const Polygon2f& that) const
{
    OuterLoop2f o1 = outerLoop_realtime;
    OuterLoop2f o2 = that.outerLoop_realtime;

    bool o1good = true;
    bool o2good = true;

    std::vector<InnerLoop2f> i1s = innerLoop_realtime;
    std::vector<InnerLoop2f> i2s = that.innerLoop_realtime;

    if (o1.isOutsideNotStrict(o2))
    {
        std::vector<Polygon2f> empty;
        return empty;
    }

    if (o1.isInsideNotStrict(o2))
    {
        o1good = false;
    }
    else if (o2.isInsideNotStrict(o1))
    {
        o2good = false;
    }

    while (true)
    {
        bool flag = false;

        for (auto iter1 = i1s.begin(); iter1 != i1s.end();)
        {
            bool judge = false;

            for (auto iter2 = i2s.begin(); iter2 != i2s.end();)
            {
                if (iter1->isInsideNotStrict(*iter2))
                {
                    i2s.erase(iter2);
                    flag = true;
                }
                else
                {
                    if (iter2->isInsideNotStrict(*iter1))
                    {
                        i1s.erase(iter1);
                        flag = true;
                        judge = true;
                        break;
                    }
                    ++iter2;
                }
            }

            if (!judge)
            {
                ++iter1;
            }
        }

        for (auto iter1 = i1s.begin(); iter1 != i1s.end();)
        {
            if (o1.isOutsideNotStrict(*iter1))
            {
                i1s.erase(iter1);
                flag = true;
            }
            else
            {
                ++iter1;
            }
        }

        for (auto iter2 = i2s.begin(); iter2 != i2s.end();)
        {
            if (o2.isOutsideNotStrict(*iter2))
            {
                i2s.erase(iter2);
                flag = true;
            }
            else
            {
                ++iter2;
            }
        }

        if (!flag)
        {
            break;
        }
    }

    qDebug() << o1good << o2good << i1s.size() << i2s.size();

    std::vector<ClipUnit2f> left;
    std::vector<ClipUnit2f> right;

    bool intero1 = false;
    bool intero2 = false;
    bool interi1[i1s.size()];
    bool interi2[i2s.size()];
    for (size_t i = 0; i < i1s.size(); ++i) interi1[i] = false;
    for (size_t i = 0; i < i2s.size(); ++i) interi2[i] = false;

    if (o1good)
    {
        for (int i = 0; i < o1.size(); ++i)
        {
            left.push_back(ClipUnit2f(0, 0, 0, i, 0.0, -1, -1));
        }
    }

    if (o2good)
    {
        for (int i = 0; i < o2.size(); ++i)
        {
            right.push_back(ClipUnit2f(1, 0, 0, i, 0.0, -1, -1));
        }
    }

    int ccc = -1;
    for (const InnerLoop2f& i1: i1s)
    {
        ++ccc;
        for (int i = 0; i < i1.size(); ++i)
        {
            left.push_back(ClipUnit2f(0, 1, ccc, i, 0.0, -1, -1));
        }
    }

    ccc = -1;
    for (const InnerLoop2f& i2: i2s)
    {
        ++ccc;
        for (int i = 0; i < i2.size(); ++i)
        {
            right.push_back(ClipUnit2f(1, 1, ccc, i, 0.0, -1, -1));
        }
    }

    ccc = -1;
    std::vector<int> ruorchu;

    if (o1good)
    {
        for (int i = 0; i < o1.size(); ++i)
        {
            Point2f a = o1.getPoint(i);
            Point2f b = o1.getPoint(i + 1);

            if (o2good)
            {
                for (int j = 0; j < o2.size(); ++j)
                {
                    Point2f c = o2.getPoint(j);
                    Point2f d = o2.getPoint(j + 1);
                    std::vector<Point2f> intersections = MathLab::getIntersectionGeneral(a, b, c, d);
                    if (intersections.size())
                    {
                        intero1 = true;
                        intero2 = true;
                    }
                    for (size_t k = 0; k < intersections.size(); ++k)
                    {
                        ++ccc;
                        if (o2.isInside(a + (b - a) * (intersections[k].getX() - 0.001)))
                        {
                            ruorchu.push_back(0);
                            left.push_back(ClipUnit2f(0, 0, 0, i, intersections[k].getX(), ccc, 0));
                            right.push_back(ClipUnit2f(1, 0, 0, j, intersections[k].getY(), ccc, 0));
                        }
                        else
                        {
                            ruorchu.push_back(1);
                            left.push_back(ClipUnit2f(0, 0, 0, i, intersections[k].getX(), ccc, 1));
                            right.push_back(ClipUnit2f(1, 0, 0, j, intersections[k].getY(), ccc, 1));
                        }
                    }
                }
            }

            int ddd = -1;
            for (const InnerLoop2f& i2: i2s)
            {
                ++ddd;
                for (int j = 0; j < i2.size(); ++j)
                {
                    Point2f c = i2.getPoint(j);
                    Point2f d = i2.getPoint(j + 1);
                    std::vector<Point2f> intersections = MathLab::getIntersectionGeneral(a, b, c, d);
                    if (intersections.size())
                    {
                        intero1 = true;
                        interi2[ddd] = true;
                    }
                    for (size_t k = 0; k < intersections.size(); ++k)
                    {
                        ++ccc;
                        if (i2.isInside(a + (b - a) * (intersections[k].getX() - 0.001)))
                        {
                            ruorchu.push_back(1);
                            left.push_back(ClipUnit2f(0, 0, 0, i, intersections[k].getX(), ccc, 1));
                            right.push_back(ClipUnit2f(1, 1, ddd, j, intersections[k].getY(), ccc, 1));
                        }
                        else
                        {
                            ruorchu.push_back(0);
                            left.push_back(ClipUnit2f(0, 0, 0, i, intersections[k].getX(), ccc, 0));
                            right.push_back(ClipUnit2f(1, 1, ddd, j, intersections[k].getY(), ccc, 0));
                        }
                    }
                }
            }
        }
    }

    int eee = -1;
    for (const InnerLoop2f& i1: i1s)
    {
        ++eee;
        for (int i = 0; i < i1.size(); ++i)
        {
            Point2f a = i1.getPoint(i);
            Point2f b = i1.getPoint(i + 1);

            if (o2good)
            {
                for (int j = 0; j < o2.size(); ++j)
                {
                    Point2f c = o2.getPoint(j);
                    Point2f d = o2.getPoint(j + 1);
                    std::vector<Point2f> intersections = MathLab::getIntersectionGeneral(a, b, c, d);
                    if (intersections.size())
                    {
                        interi1[eee] = true;
                        intero2 = true;
                    }
                    for (size_t k = 0; k < intersections.size(); ++k)
                    {
                        ++ccc;
                        if (o2.isInside(a + (b - a) * (intersections[k].getX() - 0.001)))
                        {
                            ruorchu.push_back(0);
                            left.push_back(ClipUnit2f(0, 1, eee, i, intersections[k].getX(), ccc, 0));
                            right.push_back(ClipUnit2f(1, 0, 0, j, intersections[k].getY(), ccc, 0));
                        }
                        else
                        {
                            ruorchu.push_back(1);
                            left.push_back(ClipUnit2f(0, 1, eee, i, intersections[k].getX(), ccc, 1));
                            right.push_back(ClipUnit2f(1, 0, 0, j, intersections[k].getY(), ccc, 1));
                        }
                    }
                }
            }

            int fff = -1;
            for (const InnerLoop2f& i2: i2s)
            {
                ++fff;
                for (int j = 0; j < i2.size(); ++j)
                {
                    Point2f c = i2.getPoint(j);
                    Point2f d = i2.getPoint(j + 1);
                    std::vector<Point2f> intersections = MathLab::getIntersectionGeneral(a, b, c, d);
                    if (intersections.size())
                    {
                        interi1[eee] = true;
                        interi2[fff] = true;
                    }
                    for (size_t k = 0; k < intersections.size(); ++k)
                    {
                        ++ccc;
                        if (i2.isInside(a + (b - a) * (intersections[k].getX() - 0.001)))
                        {
                            ruorchu.push_back(1);
                            left.push_back(ClipUnit2f(0, 1, eee, i, intersections[k].getX(), ccc, 1));
                            right.push_back(ClipUnit2f(1, 1, fff, j, intersections[k].getY(), ccc, 1));
                        }
                        else
                        {
                            ruorchu.push_back(0);
                            left.push_back(ClipUnit2f(0, 1, eee, i, intersections[k].getX(), ccc, 0));
                            right.push_back(ClipUnit2f(1, 1, fff, j, intersections[k].getY(), ccc, 0));
                        }
                    }
                }
            }
        }
    }

    std::vector<OuterLoop2f> canOuter;
    std::vector<InnerLoop2f> canInner;

    if (o1good && !intero1)
    {
        canOuter.push_back(o1);
    }
    if (o2good && !intero2)
    {
        canOuter.push_back(o2);
    }
    for (size_t i = 0; i < i1s.size(); ++i)
    {
        if (!interi1[i]) canInner.push_back(i1s[i]);
    }
    for (size_t i = 0; i < i2s.size(); ++i)
    {
        if (!interi2[i]) canInner.push_back(i2s[i]);
    }

    qDebug() << "left";
    for (const ClipUnit2f aa: left)
    {
        qDebug() << aa.left << aa.loopkind << aa.looplabel << aa.loopnode << aa.pos << aa.ilabel << aa.ikind;
    }

    qDebug() << "right";
    for (const ClipUnit2f aa: right)
    {
        qDebug() << aa.left << aa.loopkind << aa.looplabel << aa.loopnode << aa.pos << aa.ilabel << aa.ikind;
    }

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    qDebug() << "left";
    for (const ClipUnit2f aa: left)
    {
        qDebug() << aa.left << aa.loopkind << aa.looplabel << aa.loopnode << aa.pos << aa.ilabel << aa.ikind;
    }

    qDebug() << "right";
    for (const ClipUnit2f aa: right)
    {
        qDebug() << aa.left << aa.loopkind << aa.looplabel << aa.loopnode << aa.pos << aa.ilabel << aa.ikind;
    }

    bool used[ccc + 10];
    for (int i = 0; i <= ccc; ++i) used[i] = false;

    qDebug() << "ccc =" << ccc;

    for (int ii = 0; ii <= ccc; ++ii)
    {
        // qDebug() << "ii =" << ii << used[ii];
        if (used[ii]) continue;
        int isLeft;
        int posss;
        if (ruorchu[ii] == 0) // left
        {
            isLeft = true;
            for (size_t j = 0; j < left.size(); ++j)
            {
                if (left[j].ilabel == ii)
                {
                    posss = j;
                    break;
                }
            }
        }
        else
        {
            isLeft = false;
            for (size_t j = 0; j < right.size(); ++j)
            {
                if (right[j].ilabel == ii)
                {
                    posss = j;
                    break;
                }
            }
        }

        // qDebug() << "first node:" << ii << isLeft << posss;

        std::vector<ClipUnit2f> cur;
        bool outerquestion = false;
        while (true)
        {
            // for (int i = 0; i <= ccc; ++i) qDebug() << "used[" << i << "]=" << used[i];
            qDebug() << "cur node:" << isLeft << posss;
            // qDebug() << "good in loop";
            if (isLeft)
            {
                if (left[posss].ilabel > -1)
                {
                    qDebug() << "a I node:" << left[posss].ilabel << used[left[posss].ilabel];
                    if (used[left[posss].ilabel])
                    {
                        break;
                    }
                    used[left[posss].ilabel] = true;
                }

                cur.push_back(left[posss]);
                if (left[posss].loopkind == 0) outerquestion = true;

                if (posss == left.size() - 1 || (left[posss].loopkind != left[posss + 1].loopkind || left[posss].looplabel != left[posss + 1].looplabel))
                {
                    for (size_t i = 0; i < left.size(); ++i)
                    {
                        if (left[posss].loopkind == left[i].loopkind && left[posss].looplabel == left[i].looplabel)
                        {
                            posss = i;
                            break;
                        }
                    }
                }
                else
                {
                    ++posss;
                }
                if (left[posss].ikind == 1)
                {
                    isLeft = false;
                    for (size_t i = 0; i < right.size(); ++i)
                    {
                        if (right[i].ilabel == left[posss].ilabel)
                        {
                            posss = i;
                            break;
                        }
                    }
                }
            }
            else
            {
                if (right[posss].ilabel > -1)
                {
                    if (used[right[posss].ilabel])
                    {
                        break;
                    }
                    used[right[posss].ilabel] = true;
                }

                cur.push_back(right[posss]);
                if (right[posss].loopkind == 0) outerquestion = true;

                if (posss == right.size() - 1 || (right[posss].loopkind != right[posss + 1].loopkind || right[posss].looplabel != right[posss + 1].looplabel))
                {
                    for (size_t i = 0; i < right.size(); ++i)
                    {
                        if (right[posss].loopkind == right[i].loopkind && right[posss].looplabel == right[i].looplabel)
                        {
                            posss = i;
                            break;
                        }
                    }
                }
                else
                {
                    ++posss;
                }
                if (right[posss].ikind == 0)
                {
                    isLeft = true;
                    for (size_t i = 0; i < left.size(); ++i)
                    {
                        if (left[i].ilabel == right[posss].ilabel)
                        {
                            posss = i;
                            break;
                        }
                    }
                }
            }
        }

        qDebug() << "a loop:" << cur.size() << outerquestion;

        if (outerquestion)
        {
            OuterLoop2f f0;
            for (size_t i = 0; i < cur.size(); ++i)
            {
                if (cur[i].left == 0 && cur[i].loopkind == 0)
                {
                    Point2f a = o1.getPoint(cur[i].loopnode);
                    Point2f b = o1.getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
                else if (cur[i].left == 0 && cur[i].loopkind == 1)
                {
                    Point2f a = i1s[cur[i].looplabel].getPoint(cur[i].loopnode);
                    Point2f b = i1s[cur[i].looplabel].getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
                else if (cur[i].left == 1 && cur[i].loopkind == 0)
                {
                    Point2f a = o2.getPoint(cur[i].loopnode);
                    Point2f b = o2.getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
                else
                {
                    Point2f a = i2s[cur[i].looplabel].getPoint(cur[i].loopnode);
                    Point2f b = i2s[cur[i].looplabel].getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
            }

            qDebug() << "an outerloop";
            for (int i = 0; i < f0.size(); ++i)
            {
                qDebug() << "point:" << f0.getPoint(i).getX() << f0.getPoint(i).getY();
            }
            if (f0.isReasonableLoop()) canOuter.push_back(f0);
        }
        else
        {
            InnerLoop2f f0;
            for (size_t i = 0; i < cur.size(); ++i)
            {
                if (cur[i].left == 0 && cur[i].loopkind == 0)
                {
                    Point2f a = o1.getPoint(cur[i].loopnode);
                    Point2f b = o1.getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
                else if (cur[i].left == 0 && cur[i].loopkind == 1)
                {
                    Point2f a = i1s[cur[i].looplabel].getPoint(cur[i].loopnode);
                    Point2f b = i1s[cur[i].looplabel].getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
                else if (cur[i].left == 1 && cur[i].loopkind == 0)
                {
                    Point2f a = o2.getPoint(cur[i].loopnode);
                    Point2f b = o2.getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
                else
                {
                    Point2f a = i2s[cur[i].looplabel].getPoint(cur[i].loopnode);
                    Point2f b = i2s[cur[i].looplabel].getPoint(cur[i].loopnode + 1);
                    f0.addPoint(a + (b - a) * cur[i].pos);
                }
            }
            if (f0.isReasonableLoop()) canInner.push_back(f0);
        }
    }

    std::vector<Polygon2f> ans;
    for (size_t i = 0; i < canOuter.size(); ++i)
    {
        Polygon2f q;
        q.addOuterLoop(canOuter[i]);
        for (size_t j = 0; j < canInner.size(); ++j)
        {
            q.addInnerLoop(canInner[j]);
        }
        ans.push_back(q);
    }

    qDebug() << "final size = " << canOuter.size() << canInner.size() << ans.size();

    return ans;
};
