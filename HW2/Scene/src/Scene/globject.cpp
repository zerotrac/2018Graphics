#include "globject.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVector>
#include <algorithm>

GLObject::GLObject(const QString& filename, bool normalized)
{
    initializeOpenGLFunctions();
    compiled = false;

    QFile file(filename);
    QTextStream in(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QVector<float> vec_v, vec_vt, vec_vn;
    QVector<unsigned int> vec_f;

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

            vec_v.push_back(x);
            vec_v.push_back(y);
            vec_v.push_back(z);
        }
        else if (opt[0] == "vt")
        {
            vec_vt.push_back(opt[1].toFloat());
            vec_vt.push_back(opt[2].toFloat());
        }
        else if (opt[0] == "vn")
        {
            vec_vn.push_back(opt[1].toFloat());
            vec_vn.push_back(opt[2].toFloat());
            vec_vn.push_back(opt[3].toFloat());
        }
        else if (opt[0] == "f")
        {
            for (int i = 1; i <= 3; ++i)
            {
                QStringList elements = opt[i].split("/");
                for (int j = 0; j < 3; ++j)
                {
                    vec_f.push_back(elements[j].toUInt() - 1);
                }
            }
        }
    }

    v_cnt = vec_v.size() / 3;
    vt_cnt = vec_vt.size() / 2;
    vn_cnt = vec_vn.size() / 3;
    f_cnt = vec_f.size() / 9;

    float diff = std::max(x_max - x_min, std::max(y_max - y_min, z_max - z_min));

    if (normalized)
    {
        for (int i = 0; i < v_cnt * 3; i += 3)
        {
            // qDebug() << "old:" << vec_v[i] << vec_v[i + 1] << vec_v[i + 2];
            vec_v[i] = (vec_v[i] * 2.0 - x_min - x_max) / diff;
            vec_v[i + 1] = (vec_v[i + 1] * 2.0 - y_min - y_max) / diff;
            vec_v[i + 2] = (vec_v[i + 2] * 2.0 - z_min - z_max) / diff;
            // qDebug() << "new:" << vec_v[i] << vec_v[i + 1] << vec_v[i + 2];
        }
    }

    v = new float[f_cnt * 9];
    vt = new float[f_cnt * 6];
    vn = new float[f_cnt * 9];
    f = new unsigned int[f_cnt * 9];

    /*for (int i = 0; i < f_cnt * 9; ++i)
    {
        qDebug() << "f_cnt[" << i << "] =" << vec_f[i];
    }*/

    for (int i = 0, j = 0; i < f_cnt * 9; i += 3, ++j)
    {
        // qDebug() << "sz:" << vec_f[i + 2] << vec_vn.size();
        for (int k = 0; k < 3; ++k) v[j * 3 + k] = vec_v[vec_f[i] * 3 + k];
        for (int k = 0; k < 2; ++k) vt[j * 2 + k] = vec_vt[vec_f[i + 1] * 2 + k];
        for (int k = 0; k < 3; ++k) vn[j * 3 + k] = vec_vn[vec_f[i + 2] * 3 + k];
    }

    /*
    for (int i = 0; i < f_cnt * 9; i += 3)
    {
        qDebug() << "vn:" << vn[i] << vn[i + 1] << vn[i + 2];
    }
    */

    /*for (int i = 0; i < f_cnt * 9; i += 3)
    {
        qDebug() << v[i] << v[i + 1] << v[i + 2];
    }*/

    for (int i = 0; i < f_cnt * 9; ++i) f[i] = i;

    rr = gg = bb = 0.0f;

    file.close();
}

GLObject::~GLObject()
{
    delete[] v;
    delete[] vt;
    delete[] vn;
    delete[] f;
}

void GLObject::scale(float dd)
{
    if (compiled)
    {
        return;
    }

    for (int i = 0; i < f_cnt * 9; ++i)
    {
        v[i] *= dd;
    }
}

void GLObject::translate(float dx, float dy, float dz)
{
    if (compiled)
    {
        return;
    }

    for (int i = 0; i < f_cnt * 9; i += 3)
    {
        v[i] += dx;
        v[i + 1] += dy;
        v[i + 2] += dz;
    }
}

void GLObject::compile()
{
    vao.create();
    vao.bind();

    glGenBuffers(1, &vbo_v);
    glGenBuffers(1, &vbo_vt);
    glGenBuffers(1, &vbo_vn);
    glGenBuffers(1, &ebo_f);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_v);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * f_cnt * 9, v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vt);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * f_cnt * 6, vt, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vn);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * f_cnt * 9, vn, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_f);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * f_cnt * 9, f, GL_STATIC_DRAW);

    vao.release();

    compiled = true;
}

void GLObject::render()
{
    // calculate bounding box

    float x_min = 1e10, x_max = -1e10;
    float y_min = 1e10, y_max = -1e10;
    float z_min = 1e10, z_max = -1e10;

    for (int i = 0; i < f_cnt * 9; i += 3)
    {
        x_min = std::min(x_min, v[i]);
        x_max = std::max(x_max, v[i]);
        y_min = std::min(y_min, v[i + 1]);
        y_max = std::max(y_max, v[i + 1]);
        z_min = std::min(z_min, v[i + 2]);
        z_max = std::max(z_max, v[i + 2]);
    }

    box_min = QVector3D(x_min, y_min, z_min);
    box_max = QVector3D(x_max, y_max, z_max);

    // qDebug() << "f_cnt = " << f_cnt;
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    vao.bind();
    glDrawElements(GL_TRIANGLES, f_cnt * 3, GL_UNSIGNED_INT, 0);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, v_cnt * 3);
    vao.release();
}

void GLObject::setMaterial(float _rr, float _gg, float _bb)
{
    rr = _rr;
    gg = _gg;
    bb = _bb;
}

QVector3D GLObject::getMaterial()
{
    return QVector3D(rr, gg, bb);
}

bool GLObject::inBoundingBox(const QVector3D& pos, float step)
{
    return box_min.x() - step <= pos.x() && pos.x() <= box_max.x() &&
           box_min.y() - step <= pos.y() && pos.y() <= box_max.y() &&
           box_min.z() - step <= pos.z() && pos.z() <= box_max.z();
}
