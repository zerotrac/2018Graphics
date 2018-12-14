#include "raytracingobject.h"

RayTracingObject::RayTracingObject()
{

}

RayTracingObject::RayTracingObject(const QString& filename, bool normalized)
{
    QFile file(QString("model/") + filename);
    QTextStream in(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    float x_min = 1e10, x_max = -1e10;
    float y_min = 1e10, y_max = -1e10;
    float z_min = 1e10, z_max = -1e10;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList opt = line.split(" ");
        if (opt[0] == 'v')
        {
            float x = opt[1].toFloat();
            float y = opt[2].toFloat();
            float z = opt[3].toFloat();

            x_min = std::min(x_min, x);
            x_max = std::max(x_max, x);
            y_min = std::min(y_min, y);
            y_max = std::max(y_max, y);
            z_min = std::min(z_min, z);
            z_max = std::max(z_max, z);

            v.push_back(QVector3D(x, y, z));
        }
        else if (opt[0] == "vn")
        {
            float x = opt[1].toFloat();
            float y = opt[2].toFloat();
            float z = opt[3].toFloat();

            vn.push_back(QVector3D(x, y, z));
        }
        else if (opt[0] == "f")
        {
            int x = opt[1].split("/")[0].toInt() - 1;
            int y = opt[2].split("/")[0].toInt() - 1;
            int z = opt[3].split("/")[0].toInt() - 1;
            int xx = opt[1].split("/")[2].toInt() - 1;
            int yy = opt[2].split("/")[2].toInt() - 1;
            int zz = opt[3].split("/")[2].toInt() - 1;
            f.push_back(Tuple(x, y, z));
            findex.push_back(Tuple(xx, yy, zz));
        }
    }

    float diff = std::max(x_max - x_min, std::max(y_max - y_min, z_max - z_min));

    if (normalized)
    {
        for (int i = 0; i < v.size(); ++i)
        {
            float x = (v[i].x() * 2.0f - x_min - x_max) / diff;
            float y = (v[i].y() * 2.0f - y_min - y_max) / diff;
            float z = (v[i].z() * 2.0f - z_min - z_max) / diff;
            v[i] = QVector3D(x, y, z);
        }
    }

    file.close();

    // calculate normal

    for (int i = 0; i < f.size(); ++i)
    {
        QVector3D v1 = v[f[i].ptx];
        QVector3D v2 = v[f[i].pty];
        QVector3D v3 = v[f[i].ptz];
        fn.push_back(QVector3D::crossProduct(v2 - v1, v3 - v1).normalized());
    }

    opaque = false;
}

RayTracingObject::~RayTracingObject()
{
    // do nothing
}

void RayTracingObject::scale(float dd)
{
    for (int i = 0; i < v.size(); ++i)
    {
        v[i] = v[i] * dd;
    }
}

void RayTracingObject::translate(float dx, float dy, float dz)
{
    QVector3D dd(dx, dy, dz);

    for (int i = 0; i < v.size(); ++i)
    {
        v[i] += dd;
    }
}

void RayTracingObject::setMaterial(float rr, float gg, float bb)
{
    material = QVector3D(rr, gg, bb);
}

void RayTracingObject::setF(float _f)
{
    F = _f;
}

void RayTracingObject::setOpaque()
{
    opaque = true;
}

QVector3D RayTracingObject::getMaterial()
{
    return material;
}

float RayTracingObject::getF()
{
    return F;
}

bool RayTracingObject::isOpaque()
{
    return opaque;
}

void RayTracingObject::compile()
{
    float x_min = 1e10, x_max = -1e10;
    float y_min = 1e10, y_max = -1e10;
    float z_min = 1e10, z_max = -1e10;

    for (int i = 0; i < v.size(); ++i)
    {
        x_min = std::min(x_min, v[i].x());
        x_max = std::max(x_max, v[i].x());
        y_min = std::min(y_min, v[i].y());
        y_max = std::max(y_max, v[i].y());
        z_min = std::min(z_min, v[i].z());
        z_max = std::max(z_max, v[i].z());
    }

    box_min = QVector3D(x_min, y_min, z_min);
    box_max = QVector3D(x_max, y_max, z_max);
}

void RayTracingObject::print()
{
    /*qDebug() << box_min << box_max;
    for (int i = 0; i < f.size(); ++i)
    {
        QVector3D v1 = v[f[i].ptx];
        QVector3D v2 = v[f[i].pty];
        QVector3D v3 = v[f[i].ptz];
        qDebug() << "points:" << f[i].ptx << f[i].pty << f[i].ptz << v.size() << v1 << v2 << v3;
        qDebug() << QVector3D::crossProduct(v2 - v1, v3 - v1).normalized() << fn[i];
    }*/
}

IntersectInfo RayTracingObject::intersect(QVector3D _start, QVector3D _pos)
{
    // test bounding box

    float k_min = -1e10, k_max = 1e10;

    if (std::fabs(_pos.x()) > MYCONST::eps)
    {
        float _p = (box_min.x() - _start.x()) / _pos.x();
        float _q = (box_max.x() - _start.x()) / _pos.x();
        float _r = std::min(_p, _q);
        float _s = std::max(_p, _q);

        k_min = std::max(k_min, _r);
        k_max = std::min(k_max, _s);
    }
    if (std::fabs(_pos.y()) > MYCONST::eps)
    {
        float _p = (box_min.y() - _start.y()) / _pos.y();
        float _q = (box_max.y() - _start.y()) / _pos.y();
        float _r = std::min(_p, _q);
        float _s = std::max(_p, _q);

        k_min = std::max(k_min, _r);
        k_max = std::min(k_max, _s);
    }
    if (std::fabs(_pos.z()) > MYCONST::eps)
    {
        float _p = (box_min.z() - _start.z()) / _pos.z();
        float _q = (box_max.z() - _start.z()) / _pos.z();
        float _r = std::min(_p, _q);
        float _s = std::max(_p, _q);

        k_min = std::max(k_min, _r);
        k_max = std::min(k_max, _s);
    }

    // qDebug() << "k min max:" << k_min << k_max;

    if (!(k_min <= k_max + MYCONST::eps && k_min >= MYCONST::eps && k_max >= MYCONST::eps))
    {
        return IntersectInfo();
    }

    // test each triangular mesh

    IntersectInfo ret;

    for (int i = 0; i < f.size(); ++i)
    {
        QVector3D N = fn[i];
        float _chk = QVector3D::dotProduct(N, _pos);
        if (_chk + MYCONST::eps >= 0.0f) continue;

        QVector3D A = v[f[i].ptx];
        QVector3D B = v[f[i].pty];
        QVector3D C = v[f[i].ptz];
        float D = -QVector3D::dotProduct(A, N);

        float _this_k = -(D + QVector3D::dotProduct(N, _start)) / QVector3D::dotProduct(N, _pos);
        if (_this_k <= MYCONST::eps) continue;

        QVector3D P = _start + _this_k * _pos;
        QVector3D AB = B - A;
        QVector3D AC = C - A;
        QVector3D AP = P - A;

        QVector3D AN = vn[findex[i].ptx];
        QVector3D BN = vn[findex[i].pty];
        QVector3D CN = vn[findex[i].ptz];

        float _abab = QVector3D::dotProduct(AB, AB);
        float _acac = QVector3D::dotProduct(AC, AC);
        float _abac = QVector3D::dotProduct(AB, AC);
        float _abap = QVector3D::dotProduct(AB, AP);
        float _acap = QVector3D::dotProduct(AC, AP);

        float _down = _acac * _abab - _abac * _abac;
        float _u = (_abab * _acap - _abac * _abap) / _down;
        float _v = (_acac * _abap - _abac * _acap) / _down;

        if (_u >= -MYCONST::eps && _u < 1 + MYCONST::eps && _v >= -MYCONST::eps && _v < 1 + MYCONST::eps && _u + _v < 1 + MYCONST::eps)
        {
            if (!ret.check || _this_k < ret.k)
            {
                // ret = IntersectInfo(P, N, _this_k);
                ret = IntersectInfo(P, (_v * BN + _u * CN + (1.0f - _u - _v) * AN).normalized(), _this_k);
            }
        }
    }

    // qDebug() << ret.pos << ret.n << ret.k;

    return ret;
}

bool RayTracingObject::check(QVector3D _start, QVector3D _pos, float _k)
{
    // test bounding box

    float k_min = -1e10, k_max = 1e10;

    if (std::fabs(_pos.x()) > MYCONST::eps)
    {
        float _p = (box_min.x() - _start.x()) / _pos.x();
        float _q = (box_max.x() - _start.x()) / _pos.x();
        float _r = std::min(_p, _q);
        float _s = std::max(_p, _q);

        k_min = std::max(k_min, _r);
        k_max = std::min(k_max, _s);
    }
    if (std::fabs(_pos.y()) > MYCONST::eps)
    {
        float _p = (box_min.y() - _start.y()) / _pos.y();
        float _q = (box_max.y() - _start.y()) / _pos.y();
        float _r = std::min(_p, _q);
        float _s = std::max(_p, _q);

        k_min = std::max(k_min, _r);
        k_max = std::min(k_max, _s);
    }
    if (std::fabs(_pos.z()) > MYCONST::eps)
    {
        float _p = (box_min.z() - _start.z()) / _pos.z();
        float _q = (box_max.z() - _start.z()) / _pos.z();
        float _r = std::min(_p, _q);
        float _s = std::max(_p, _q);

        k_min = std::max(k_min, _r);
        k_max = std::min(k_max, _s);
    }

    // qDebug() << "k min max:" << k_min << k_max;

    if (!(k_min <= k_max + MYCONST::eps && k_min >= MYCONST::eps && k_max >= MYCONST::eps))
    {
        return true;
    }

    for (int i = 0; i < f.size(); ++i)
    {
        QVector3D N = fn[i];
        float _chk = QVector3D::dotProduct(N, _pos);
        if (_chk + MYCONST::eps >= 0.0f) continue;

        QVector3D A = v[f[i].ptx];
        QVector3D B = v[f[i].pty];
        QVector3D C = v[f[i].ptz];
        float D = -QVector3D::dotProduct(A, N);

        float _this_k = -(D + QVector3D::dotProduct(N, _start)) / QVector3D::dotProduct(N, _pos);
        if (_this_k <= MYCONST::eps) continue;

        QVector3D P = _start + _this_k * _pos;
        QVector3D AB = B - A;
        QVector3D AC = C - A;
        QVector3D AP = P - A;

        float _abab = QVector3D::dotProduct(AB, AB);
        float _acac = QVector3D::dotProduct(AC, AC);
        float _abac = QVector3D::dotProduct(AB, AC);
        float _abap = QVector3D::dotProduct(AB, AP);
        float _acap = QVector3D::dotProduct(AC, AP);

        float _down = _acac * _abab - _abac * _abac;
        float _u = (_abab * _acap - _abac * _abap) / _down;
        float _v = (_acac * _abap - _abac * _acap) / _down;

        if (_u >= -MYCONST::eps && _u < 1 + MYCONST::eps && _v >= -MYCONST::eps && _v < 1 + MYCONST::eps && _u + _v < 1 + MYCONST::eps)
        {
            if (_this_k + MYCONST::eps < _k)
            {
                return false;
            }
        }
    }

    return true;
}
