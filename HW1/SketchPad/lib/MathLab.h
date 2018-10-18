#include "Point.h"

#include <vector>

namespace MathLab
{

const double EPS = 1e-7;
const double ZERO = 0.0;

bool isEqual(double a, double b);
bool isEqualToZero(double a);
bool isLeq(double a, double b);
bool isLeq(double a, double b, double c);
bool isLeq(const Point2f& a, const Point2f& b);
bool isLeq(const Point2f& a, const Point2f& b, const Point2f& c);
bool isLeqZero(double a);
bool isLessThan(double a, double b);
bool isLessThan(double a, double b, double c);
bool isLessThan(const Point2f& a, const Point2f& b);
bool isLessThan(const Point2f& a, const Point2f& b, const Point2f& c);
bool isGeq(double a, double b);
bool isGeq(double a, double b, double c);
bool isGeq(const Point2f& a, const Point2f& b);
bool isGeq(const Point2f& a, const Point2f& b, const Point2f& c);
bool isGeqZero(double a);
bool isGreaterThan(double a, double b);
bool isGreaterThan(double a, double b, double c);
bool isGreaterThan(const Point2f& a, const Point2f& b);
bool isGreaterThan(const Point2f& a, const Point2f& b, const Point2f& c);
bool isSamePoint(const Point2f& a, const Point2f& b);

int round(double a);

double norm(const Point2f& a);
Point2f normalized(const Point2f& a);

double dotProduct(const Point2f& a, const Point2f& b);
double crossProduct(const Point2f& a, const Point2f& b);
double cos(const Point2f& a);
double sin(const Point2f& b);
double cos(const Point2f& a, const Point2f& b);
double sin(const Point2f& a, const Point2f& b);
double acos(double a);
    
/*
a---c---b
*/
bool segmentCross(const Point2f& a, const Point2f& b, const Point2f& c);

/*
a---b c---d
*/
bool segmentCross(const Point2f& a, const Point2f& b, const Point2f& c, const Point2f& d);

bool segmentCrossNotStrict(const Point2f& a, const Point2f& b, const Point2f& c, const Point2f& d);

std::vector<Point2f> getIntersectionGeneral(const Point2f& a, const Point2f& b, const Point2f& c, const Point2f& d);

};
