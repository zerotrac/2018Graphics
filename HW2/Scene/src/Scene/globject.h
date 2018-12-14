#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "myconst.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QVector3D>

class GLObject: protected QOpenGLFunctions
{
public:
    GLObject(const QString& fileName, bool normalized);
    ~GLObject();

private:
    float* v;
    float* vt;
    float* vn;
    unsigned int* f;
    int v_cnt, vt_cnt, vn_cnt, f_cnt;
    float rr, gg, bb;

    QOpenGLVertexArrayObject vao;
    unsigned int vbo_v, vbo_vt, vbo_vn, ebo_f;
    bool compiled;

    QVector3D box_min;
    QVector3D box_max;

public:
    void scale(float dd);
    void translate(float dx, float dy, float dz);
    void compile();
    void render();
    void setMaterial(float _rr, float _gg, float _bb);
    QVector3D getMaterial();
    bool inBoundingBox(const QVector3D& pos, float step);
};

#endif // GLOBJECT_H
