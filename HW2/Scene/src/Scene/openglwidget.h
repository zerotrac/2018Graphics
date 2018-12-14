#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "myconst.h"
#include "globject.h"
#include "raytracingobject.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QKeyEvent>

class OpenGLWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = 0);

private:
    QOpenGLShaderProgram* program;
    GLObject** obj;
    GLObject** wall;
    QOpenGLVertexArrayObject vao;

    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    QVector2D mousePos;

private:
    float X_POS_MIN, X_POS_MAX, X_STEP;
    float Y_POS_MIN, Y_POS_MAX, Y_STEP;
    float Z_POS_MIN, Z_POS_MAX, Z_STEP;

    QVector3D cameraPos;
    QVector3D cameraFront;
    QVector3D cameraUp;

    QVector3D lights_pos[5];
    QVector3D lights_rgb[5];

    int lights_occ = 0;

    float yaw;
    float pitch;

    int cur_key;
    bool isPerspective;
    bool isChanging;

    float obj_f;
    float wall_f;

    QVector3D enviro;
    QOpenGLTexture* texture;
    bool isTexture;
    QString tex_name;

    QVector<RayTracingObject> ray_objects;

    int cube_cnt, cylinder_cnt, sphere_cnt;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    void updateView();
    bool checkInObject(const QVector3D& view);
    bool checkInView(const QVector3D& view);
    QVector3D rayDfs(int depth, QVector3D _start, QVector3D _pos, const QVector3D& para_enviro, const QVector<QVector3D>& para_light_pos, const QVector<QVector3D>& para_light_illumin);
    void doRayTracing();

public slots:
    void moveCamera();
};

#endif // OPENGLWIDGET_H
