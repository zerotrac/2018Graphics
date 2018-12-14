#ifndef RAYTRACINGOBJECT_H
#define RAYTRACINGOBJECT_H

#include "myconst.h"

#include <QVector>
#include <QVector3D>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <algorithm>
#include <cmath>

struct Tuple
{
    int ptx;
    int pty;
    int ptz;

    Tuple()
    {

    }

    Tuple(int _ptx, int _pty, int _ptz)
    {
        ptx = _ptx;
        pty = _pty;
        ptz = _ptz;
    }
};

struct IntersectInfo
{
    bool check;
    QVector3D pos;
    QVector3D n;
    float k;

    IntersectInfo()
    {
        check = false;
    }

    IntersectInfo(QVector3D _pos, QVector3D _n, float _k)
    {
        check = true;
        pos = _pos;
        n = _n;
        k = _k;
    }
};

class RayTracingObject
{
public:
    RayTracingObject();
    RayTracingObject(const QString& fileName, bool normalized);
    ~RayTracingObject();

private:
    QVector<QVector3D> v;    // vertices
    QVector<QVector3D> vn; // normal vector of vertices
    QVector<Tuple> f;        // faces with index to v
    QVector<QVector3D> fn;   // normal vector of faces
    QVector<Tuple> findex;

    QVector3D material;
    QVector3D box_min;
    QVector3D box_max;

    float F;
    bool opaque;

public:
    void scale(float dd);
    void translate(float dx, float dy, float dz);
    void setMaterial(float rr, float gg, float bb);
    void setF(float _f);
    void setOpaque();
    QVector3D getMaterial();
    float getF();
    bool isOpaque();
    void compile();
    void print();
    IntersectInfo intersect(QVector3D _start, QVector3D _pos);
    bool check(QVector3D _start, QVector3D _pos, float _k);
};

#endif // RAYTRACINGOBJECT_H
