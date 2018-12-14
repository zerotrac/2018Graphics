#include "openglwidget.h"

#include <QOpenGLBuffer>
#include <QTimer>
#include <QMatrix4x4>
#include <QtMath>
#include <algorithm>
#include <cmath>

OpenGLWidget::OpenGLWidget(QWidget *parent): QOpenGLWidget(parent)
{
    X_STEP = Y_STEP = Z_STEP = 0.0125f;

    X_POS_MIN = Y_POS_MIN = Z_POS_MIN = -1.0f + X_STEP * 2;
    X_POS_MAX = Y_POS_MAX = Z_POS_MAX = 1.0f - X_STEP * 2;

    cameraPos = QVector3D(0.0f, 0.0f, Z_POS_MAX);
    // cameraPos = QVector3D(0.0f, 0.0f, Z_POS_MAX * 2.0);

    cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
    cameraUp = QVector3D(0.0f, 1.0f, 0.0f);

    for (int i = 0; i < 5; ++i)
    {
        lights_pos[i] = QVector3D(0.0, 0.0, 0.0);
        lights_rgb[i] = QVector3D(0.0, 0.0, 0.0);
    }

    lights_occ = 0;

    yaw = -90.0f;
    pitch = 0.0f;
    cur_key = -1;
    isPerspective = true;
    isChanging = false;
    isTexture = false;

    enviro = QVector3D(1.0, 1.0, 1.0);

    setFocusPolicy(Qt::StrongFocus);
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_DOUBLEBUFFER);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearDepth(1.0);

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile("./shader/basic.vert");

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile("./shader/basic.frag");

    program = new QOpenGLShaderProgram();
    program->addShader(vshader);
    program->addShader(fshader);

    program->link();
    program->bind();

    obj = new GLObject*[3];
    obj[0] = new GLObject(QString("model/cube.obj"), true);
    obj[1] = new GLObject(QString("model/cylinder.obj"), true);
    obj[2] = new GLObject(QString("model/sphere.obj"), true);

    obj[0]->scale(0.2f);
    obj[1]->scale(0.15f);
    obj[2]->scale(0.12f);

    obj[0]->translate(-0.2f, 0.35f, -0.1f);
    obj[1]->translate(0.3f, -0.2f, 0.05f);
    obj[2]->translate(-0.2f, -0.3f, 0.25f);

    for (int i = 0; i < 3; ++i)
    {
        obj[i]->compile();
    }

    wall = new GLObject*[6];
    for (int i = 0; i < 6; ++i)
    {
        wall[i] = new GLObject(QString("model/wall") + QString::number(i) + QString(".obj"), false);
        wall[i]->compile();
    }

    QFile file("scene.zerotrac");
    QTextStream in(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList opt = line.split(" ");

        if (opt[0] == "sphere" || opt[0] == "cube" || opt[0] == "cylinder")
        {
            double l_r = opt[1].toFloat();
            double l_g = opt[2].toFloat();
            double l_b = opt[3].toFloat();

            if (opt[0] == "cube")
            {
                obj[0]->setMaterial(l_r, l_g, l_b);
            }
            else if (opt[0] == "cylinder")
            {
                obj[1]->setMaterial(l_r, l_g, l_b);
            }
            else
            {
                obj[2]->setMaterial(l_r, l_g, l_b);
            }
        }
        else if (opt[0] == "wall")
        {
            int idd = opt[1].toInt();
            if (idd >= 0 && idd <= 5)
            {
                double l_r = opt[2].toFloat();
                double l_g = opt[3].toFloat();
                double l_b = opt[4].toFloat();

                wall[idd]->setMaterial(l_r, l_g, l_b);
            }
        }
        else if (opt[0] == "light")
        {
            double l_x = opt[1].toFloat();
            double l_y = opt[2].toFloat();
            double l_z = opt[3].toFloat();
            double l_r = opt[4].toFloat();
            double l_g = opt[5].toFloat();
            double l_b = opt[6].toFloat();

            lights_pos[lights_occ] = QVector3D(l_x, l_y, l_z);
            lights_rgb[lights_occ] = QVector3D(l_r, l_g, l_b);

            lights_occ += 1;
            if (lights_occ >= 5) lights_occ = 0;
        }
        else if (opt[0] == "f")
        {
            double ff = opt[2].toFloat();
            if (opt[1] == "obj")
            {
                obj_f = ff;
            }
            else if (opt[1] == "wall")
            {
                wall_f = ff;
            }
        }
        else if (opt[0] == "enviro")
        {
            double l_r = opt[1].toFloat();
            double l_g = opt[2].toFloat();
            double l_b = opt[3].toFloat();

            enviro = QVector3D(l_r, l_g, l_b);
            program->setUniformValue("La", enviro);
        }
        else if (opt[0] == "texture")
        {
            tex_name = opt[1];
            qDebug() << "name =" << tex_name;
        }
        else
        {
            // qDebug() << "no use:" << line;
        }
    }

    for (int i = 0; i < 5; ++i)
    {
        program->setUniformValue((QString("LightP") + QString::number(i + 1)).toStdString().c_str(), lights_pos[i]);
        program->setUniformValue((QString("Ld") + QString::number(i + 1)).toStdString().c_str(), lights_rgb[i]);
    }

    texture = new QOpenGLTexture(QImage(QString("texture/") + tex_name));
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);
    program->setUniformValue("Tex", 0);

    // model keeps identity
    model.setToIdentity();

    updateView();

    projection.setToIdentity();

    QTimer* timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(moveCamera()));
    timer->start(10);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    projection.setToIdentity();
    projection.perspective(60.0f, (GLfloat)w / (GLfloat)h, 0.001f, 5.0f);
}

void OpenGLWidget::paintGL()
{
    glShadeModel(GL_SMOOTH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model.setToIdentity();

    updateView();

    QMatrix4x4 MVP = projection * view * model;
    program->setUniformValue("MVP", MVP);

    program->setUniformValue("Eye", cameraPos);

    texture->bind(0);
    program->setUniformValue("Texture", isTexture);
    for (int i = 0; i < 3; ++i)
    {
        program->setUniformValue("Kd", obj[i]->getMaterial());
        program->setUniformValue("f", obj_f);
        obj[i]->render();
    }
    program->setUniformValue("Texture", false);
    texture->release();

    for (int i = 0; i < 6; ++i)
    {
        // if (i == 1) continue;
        program->setUniformValue("Kd", wall[i]->getMaterial());
        program->setUniformValue("f", wall_f);
        wall[i]->render();

        if (!isPerspective)
        {
            break;
        }
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        mousePos = QVector2D(event->pos());
        isChanging = true;
    }
    else
    {
        isChanging = false;
        if (isPerspective)
        {
            projection.setToIdentity();
            projection.ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.001f, 3.0f);
            isPerspective = false;
        }
        else
        {
            projection.setToIdentity();
            projection.perspective(60.0f, (GLfloat)this->width() / (GLfloat)this->height(), 0.001f, 5.0f);
            isPerspective = true;
        }
        this->update();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        isChanging = false;
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isChanging) return;

    QVector2D mousePosDiff = QVector2D(event->pos()) - mousePos;
    yaw += mousePosDiff.x() / 8.0;
    pitch -= mousePosDiff.y() / 8.0;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    else if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    cameraFront = QVector3D(qCos(qDegreesToRadians(yaw)) * qCos(qDegreesToRadians(pitch)),
                            qSin(qDegreesToRadians(pitch)),
                            qSin(qDegreesToRadians(yaw)) * qCos(qDegreesToRadians(pitch))).normalized();

    mousePos = QVector2D(event->pos());
    this->update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    int k = event->key();
    if (k == Qt::Key_T)
    {
        isTexture = !isTexture;
    }
    else if (k == Qt::Key_P)
    {
        int sz = 1000;
        QImage opt(sz, sz, QImage::Format_RGB32);
        for (int i = 0; i < sz; ++i)
        {
            for (int j = 0; j < sz; ++j)
            {
                int c = static_cast<int>((i + j + 2.0) / (sz * 2.0) * 255.0 + 0.5);
                opt.setPixelColor(i, j, QColor(c, c, c));
            }
        }
        opt.save("raytracing/test.jpg");
    }
    else if (k == Qt::Key_R)
    {
        doRayTracing();
    }
    else
    {
        cur_key = k;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    int k = event->key();
    if (cur_key == k)
    {
        cur_key = -1;
    }
}

void OpenGLWidget::updateView()
{
    view.setToIdentity();
    view.lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
    this->update();
}

bool OpenGLWidget::checkInObject(const QVector3D& view)
{
    return obj[0]->inBoundingBox(view, X_STEP) || obj[1]->inBoundingBox(view, X_STEP) || obj[2]->inBoundingBox(view, X_STEP);
}

bool OpenGLWidget::checkInView(const QVector3D& view)
{
    return X_POS_MIN <= view.x() && view.x() <= X_POS_MAX &&
           Y_POS_MIN <= view.y() && view.y() <= Y_POS_MAX &&
           Z_POS_MIN <= view.z() && view.z() <= Z_POS_MAX && !checkInObject(view);
}

QVector3D OpenGLWidget::rayDfs(int depth, QVector3D _start, QVector3D _pos, const QVector3D& para_enviro, const QVector<QVector3D>& para_light_pos, const QVector<QVector3D>& para_light_illumin)
{
    if (!depth)
    {
        return QVector3D(0.0f, 0.0f, 0.0f);
    }

    // _start + _k * _pos;

    IntersectInfo _true_info;
    int _obj_index = -1;

    for (int i = 0; i < ray_objects.size(); ++i)
    {
        // if (i != 1) continue;
        IntersectInfo _info = ray_objects[i].intersect(_start, _pos);
        if (_info.check)
        {
            // qDebug() << "intersect with" << i << _info.k;
            if (_obj_index == -1)
            {
                _true_info = _info;
                _obj_index = i;
            }
            else if (_info.k + MYCONST::eps <= _true_info.k)
            {
                _true_info = _info;
                _obj_index = i;
            }
        }
    }

    if (!_true_info.check)
    {
        // it is impossible!
        // qDebug() << "impossible for" << _start << _pos;
        // float _stp = (1.0f - _start.y()) / _pos.y();
        // QVector3D _stp2 = _start + _stp * _pos;
        // qDebug() << "trans:" << _stp << _stp2;
        return QVector3D(0.0f, 0.0f, 0.0f);
    }

    // qDebug() << "possible for" << _start << _pos;

    QVector3D ret = para_enviro * ray_objects[_obj_index].getMaterial();
    // qDebug() << "ret =" << ret;

    for (int i = 0; i < para_light_pos.size(); ++i)
    {
        // qDebug() << "light" << i << para_light_pos[i];
        QVector3D _light_start = para_light_pos[i];
        QVector3D _s_pre = _light_start - _true_info.pos;
        QVector3D _s = _s_pre.normalized();
        QVector3D _n = _true_info.n;
        QVector3D _v = -_pos;
        // qDebug() << _s << _n << _v;

        float _dummy = QVector3D::dotProduct(_s, _n);
        if (_dummy < MYCONST::eps) continue;

        QVector3D _r = -_s + 2.0f * _dummy * _n;

        float _k;
        if (std::fabs(_s_pre.x() - _s.x()) > MYCONST::eps)
        {
            _k = _s_pre.x() / _s.x();
        }
        else if (std::fabs(_s_pre.y() - _s.y()) > MYCONST::eps)
        {
            _k = _s_pre.y() / _s.y();
        }
        else
        {
            _k = _s_pre.z() / _s.z();
        }

        bool _check = true;
        for (int j = 0; j < ray_objects.size(); ++j)
        {
            // qDebug() << "test on ray" << i << "object" << j;
            if (!ray_objects[j].check(_light_start, -_s, _k))
            {
                // qDebug() << "dirty object" << j << "light" << i;
                _check = false;
                break;
            }
        }

        // _check = true;

        if (_check)
        {
            float _dummy2 = std::max(QVector3D::dotProduct(_r, _v), 0.0f);
            // qDebug() << "dummy:" << _dummy << _dummy2;
            ret += (_dummy + _dummy2) * para_light_illumin[i] * ray_objects[_obj_index].getMaterial();
        }
    }

    // qDebug() << "this ret:" << ret;

    QVector3D _next_pos = _pos - 2.0f * QVector3D::dotProduct(_pos, _true_info.n) * _true_info.n;
    if (!ray_objects[_obj_index].isOpaque()) //(true) //(_obj_index == 4 || _obj_index == 5 || _obj_index == 7)
    {
        // qDebug() << "good";
        return ret + ray_objects[_obj_index].getMaterial() * rayDfs(depth - 1, _true_info.pos, _next_pos, para_enviro, para_light_pos, para_light_illumin);
    }
    else
    {
        return ret;
    }
}

void OpenGLWidget::doRayTracing()
{
    QFile file("raytracing/settings.txt");
    QTextStream in(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    float para_m, para_n, para_t;
    int para_sz;
    int _depth;

    cube_cnt = 0;
    cylinder_cnt = 0;
    sphere_cnt = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList opt = line.split(" ");
        if (opt[0] == "m")
        {
            para_m = opt[1].toFloat();
            if (para_m <= 0.0f) return;
        }
        else if (opt[0] == "n")
        {
            para_n = opt[1].toFloat();
            if (para_n <= 0.0f) return;
        }
        else if (opt[0] == "t")
        {
            para_t = opt[1].toFloat();
            if (para_t <= 0.0f || para_t >= 1.0f) return;
        }
        else if (opt[0] == "size")
        {
            para_sz = opt[1].toInt();
            if (para_sz <= 0) return;
        }
        else if (opt[0] == "cube")
        {
            cube_cnt = opt[1].toInt();
        }
        else if (opt[0] == "cylinder")
        {
            cylinder_cnt = opt[1].toInt();
        }
        else if (opt[0] == "sphere")
        {
            sphere_cnt = opt[1].toInt();
        }
        else if (opt[0] == "depth")
        {
            _depth = opt[1].toInt();
        }
    }

    file.close();

    if ((para_m + para_n) / para_m * para_t >= 1.0f)
    {
        return;
    }

    qDebug() << para_m << para_n << para_t << para_sz;

    for (int i = 0; i < cube_cnt; ++i)
    {
        ray_objects.push_back(RayTracingObject("cube.obj", true));
    }

    for (int i = 0; i < cylinder_cnt; ++i)
    {
        ray_objects.push_back(RayTracingObject("cylinder.obj", true));
    }

    for (int i = 0; i < sphere_cnt; ++i)
    {
        ray_objects.push_back(RayTracingObject("sphere.obj", true));
    }

    ray_objects.push_back(RayTracingObject("wall0.obj", false));
    ray_objects.push_back(RayTracingObject("wall1.obj", false));
    ray_objects.push_back(RayTracingObject("wall2.obj", false));
    ray_objects.push_back(RayTracingObject("wall3.obj", false));
    ray_objects.push_back(RayTracingObject("wall4.obj", false));
    ray_objects.push_back(RayTracingObject("wall5.obj", false));

    QFile file2("raytracing/objects.txt");
    QTextStream in2(&file2);
    if (!file2.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QVector3D para_enviro;
    QVector<QVector3D> para_light_pos;
    QVector<QVector3D> para_light_illumin;

    while (!in2.atEnd())
    {
        QString line = in2.readLine();
        QStringList opt = line.split(" ");

        if (opt[0] == "wall")
        {
            int _index = cube_cnt + cylinder_cnt + sphere_cnt + opt[1].toInt();
            if (opt[2] == "material")
            {
                float x = opt[3].toFloat();
                float y = opt[4].toFloat();
                float z = opt[5].toFloat();
                ray_objects[_index].setMaterial(x, y, z);
            }
            else if (opt[2] == "f")
            {
                float _f = opt[3].toFloat();
                ray_objects[_index].setF(_f);
            }
            else if (opt[2] == "opaque")
            {
                ray_objects[_index].setOpaque();
            }
        }
        else if (opt[0] == "light")
        {
            float x = opt[2].toFloat();
            float y = opt[3].toFloat();
            float z = opt[4].toFloat();

            float r = opt[6].toFloat();
            float g = opt[7].toFloat();
            float b = opt[8].toFloat();

            para_light_pos.push_back(QVector3D(x, y, z));
            para_light_illumin.push_back(QVector3D(r, g, b));
        }
        else if (opt[0] == "environment")
        {
            float r = opt[2].toFloat();
            float g = opt[3].toFloat();
            float b = opt[4].toFloat();

            para_enviro = QVector3D(r, g, b);
        }
        else if (opt.size() > 2)
        {
            if (opt[2] == "opaque")
            {
                int _index;
                if (opt[0] == "cube")
                {
                    _index = opt[1].toInt();
                }
                else if (opt[0] == "cylinder")
                {
                    _index = cube_cnt + opt[1].toInt();
                }
                else if (opt[0] == "sphere")
                {
                    _index = cube_cnt + cylinder_cnt + opt[1].toInt();
                }

                ray_objects[_index].setOpaque();
            }
            else if (opt[2] == "f" || opt[2] == "scale")
            {
                float x = opt[3].toFloat();

                int _index;
                if (opt[0] == "cube")
                {
                    _index = opt[1].toInt();
                }
                else if (opt[0] == "cylinder")
                {
                    _index = cube_cnt + opt[1].toInt();
                }
                else if (opt[0] == "sphere")
                {
                    _index = cube_cnt + cylinder_cnt + opt[1].toInt();
                }

                if (opt[2] == "f")
                {
                    ray_objects[_index].setF(x);
                }
                else if (opt[2] == "scale")
                {
                    ray_objects[_index].scale(x);
                }
            }
            else if (opt[2] == "material" || opt[2] == "translate")
            {
                float x = opt[3].toFloat();
                float y = opt[4].toFloat();
                float z = opt[5].toFloat();

                int _index;
                if (opt[0] == "cube")
                {
                    _index = opt[1].toInt();
                }
                else if (opt[0] == "cylinder")
                {
                    _index = cube_cnt + opt[1].toInt();
                }
                else if (opt[0] == "sphere")
                {
                    _index = cube_cnt + cylinder_cnt + opt[1].toInt();
                }

                if (opt[2] == "material")
                {
                    ray_objects[_index].setMaterial(x, y, z);
                }
                else if (opt[2] == "translate")
                {
                    ray_objects[_index].translate(x, y, z);
                }
            }
            else
            {
                // qDebug() << "no use:" << line;
            }
        }
        else
        {
            // qDebug() << "no use:" << line;
        }
    }

    file2.close();

    for (int i = 0; i < ray_objects.size(); ++i)
    {
        ray_objects[i].compile();
        ray_objects[i].print();
    }

    QImage res(para_sz, para_sz, QImage::Format_RGB32);
    QVector3D _camera_pos(0, 0, para_m + para_n);
    // QVector3D _screen_pos(0, 0, para_n);

    for (int i = 0; i < para_sz; ++i)
    {
        qDebug() << i;
        for (int j = 0; j < para_sz; ++j)
        {
            // qDebug() << i << j;
            float _x = (i + 0.5f) / para_sz * para_t * 2.0f - para_t;
            float _y = -(j + 0.5f) / para_sz * para_t * 2.0f + para_t;
            float _z = para_n;

            QVector3D _start(_x, _y, _z);
            QVector3D _pos = (_start - _camera_pos).normalized();
            QVector3D _ray = rayDfs(_depth, _start, _pos, para_enviro, para_light_pos, para_light_illumin);
            QVector3D _color = _ray * 255.0f;

            if (_color.x() > 255.0f) _color.setX(255.0f);
            if (_color.y() > 255.0f) _color.setY(255.0f);
            if (_color.z() > 255.0f) _color.setZ(255.0f);

            // if (i == 0) qDebug() << "color:" << _ray;
            // if (_ray.z() > 0.5f) qDebug() << i << j << "color:" << _ray;
            // if (_ray.x() < MYCONST::eps && _ray.y() < MYCONST::eps && _ray.z() > 0.2f)
            // {
                // _color.setZ(255);
                // qDebug() << "cur color:" << _color;
            // }
            res.setPixelColor(i, j, QColor(_color.x(), _color.y(), _color.z()));
            // break;
        }
        // break;
    }
    res.save("raytracing/hey.jpg");
    qDebug() << "render complete";
}


void OpenGLWidget::moveCamera()
{
    if (cur_key == -1)
    {
        return;
    }

    if (cur_key == Qt::Key_W)
    {
        QVector3D cameraPosNew = cameraPos + cameraFront * X_STEP;
        if (checkInView(cameraPosNew))
        {
            cameraPos = cameraPosNew;
            this->update();
        }
    }
    else if (cur_key == Qt::Key_S)
    {
        QVector3D cameraPosNew = cameraPos - cameraFront * X_STEP;
        if (checkInView(cameraPosNew))
        {
            cameraPos = cameraPosNew;
            this->update();
        }
    }
    else if (cur_key == Qt::Key_A)
    {
        QVector3D cameraPosNew = cameraPos - QVector3D::crossProduct(cameraFront, cameraUp).normalized() * X_STEP;
        if (checkInView(cameraPosNew))
        {
            cameraPos = cameraPosNew;
            this->update();
        }
    }
    else if (cur_key == Qt::Key_D)
    {
        QVector3D cameraPosNew = cameraPos + QVector3D::crossProduct(cameraFront, cameraUp).normalized() * X_STEP;
        if (checkInView(cameraPosNew))
        {
            cameraPos = cameraPosNew;
            this->update();
        }
    }
    else if (cur_key == Qt::Key_Q)
    {
        QVector3D cameraPosNew = cameraPos + cameraUp * X_STEP;
        if (checkInView(cameraPosNew))
        {
            cameraPos = cameraPosNew;
            this->update();
        }
    }
    else if (cur_key == Qt::Key_E)
    {
        QVector3D cameraPosNew = cameraPos - cameraUp * X_STEP;
        if (checkInView(cameraPosNew))
        {
            cameraPos = cameraPosNew;
            this->update();
        }
    }
}
