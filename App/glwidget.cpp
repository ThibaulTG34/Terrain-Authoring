/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <iostream>
#include <GL/glut.h>

bool GLWidget::m_transparent = false;
bool firstMouse = true;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0)
{
    setMouseTracking(true);

    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    if (m_transparent)
    {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }

    wireframe = false;

    timer.start(20);
    frame_count = 0;
    last_count = 0;
    last_time = QTime::currentTime();

    camera = new Camera();

}

GLWidget::~GLWidget()
{
    wireframe = false;
    cam_position = QVector3D(0, 0, -10.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up = QVector3D(0.0f, 1.0f, 0.0f);
    makeCurrent();
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot)
    {
        m_xRot = angle;
        // Completer pour emettre un signal
        emit objectRotChangeOnX(angle);

        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot)
    {
        m_yRot = angle;
        // Completer pour emettre un signal
        emit objectRotChangeOnY(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot)
    {
        m_zRot = angle;
        // Completer pour emettre un signal
        emit objectRotChangeOnZ(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    indexBuffer_.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

void GLWidget::initializeGL()
{

    QOpenGLWidget::initializeGL();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    connect(this, SIGNAL(objectRotChangeOnX(int)), this, SLOT(setXRotation(int)));
    connect(this, SIGNAL(objectRotChangeOnY(int)), this, SLOT(setYRotation(int)));

    initializeOpenGLFunctions();
    glClearColor(0.8, 0.8, 0.8, 1);

    QImage blackImage(300, 300, QImage::Format_RGB888);
    blackImage.fill(Qt::black);
    biome = new QOpenGLTexture(blackImage);
    m_program = new QOpenGLShaderProgram;
    // Compile vertex shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("texture_coordonnees", 2);

    // Link shader pipeline
    if (!m_program->link())
        close();

    // Bind shader pipeline for use
    if (!m_program->bind())
        close();

    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");

    vao_.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);

    vertexBuffer_.create();
    vertexBuffer_.bind();
    vertexBuffer_.allocate(object.vertices.constData(), object.vertices.size() * sizeof(QVector3D));
    vertexBuffer_.release();

    indexBuffer_.create();
    indexBuffer_.bind();
    indexBuffer_.allocate(object.indices.constData(), object.indices.size() * sizeof(short));
    indexBuffer_.release();

    m_texturebuffer.create();
    m_texturebuffer.bind();
    m_texturebuffer.allocate(object.uvs.constData(), sizeof(QVector2D) * object.uvs.size());
    m_texturebuffer.release();

    normalsBuffer.create();
    normalsBuffer.bind();
    normalsBuffer.allocate(object.normals.constData(), sizeof(QVector3D) * object.normals.size());
    normalsBuffer.release();

    resizeGL(16, 9);

    vao_.bind();
    vertexBuffer_.bind();
    indexBuffer_.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    normalsBuffer.bind();
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    m_texturebuffer.bind();
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    vao_.release();

    m_view.setToIdentity();
    m_model.setToIdentity();
    m_projection.setToIdentity();

    // m_view.translate(0.2, 0.15, -3);

    // Light position is fixed.
    // m_program->setUniformValue(m_light_pos_loc, QVector3D(1.2f, 1.0f, 2.0f));

    QImage desertBottom = QImage("desertBottom.jpg");
    desertB = new QOpenGLTexture(desertBottom);

    QImage desertMiddle = QImage("desertMiddle.jpg");
    desertM = new QOpenGLTexture(desertMiddle);

    desertT = new QOpenGLTexture(desertMiddle);

    QImage CanyonBottom = QImage("canyonBottom.jpg");
    canyonB = new QOpenGLTexture(CanyonBottom);

    QImage CanyonMiddle = QImage("canyonMiddle.jpg");
    canyonM = new QOpenGLTexture(CanyonMiddle);

    QImage CanyonTop = QImage("canyonMiddle.jpg");
    canyonT = new QOpenGLTexture(CanyonTop);

    QImage montagneBottom = QImage("montagneBottom.jpg");
    montagneB = new QOpenGLTexture(montagneBottom);

    QImage montagneMiddle = QImage("montagneMiddle.jpg");
    montagneM = new QOpenGLTexture(montagneMiddle);

    QImage montagneTop = QImage("montagneTop.jpg");
    montagneT = new QOpenGLTexture(montagneTop);

    glacialB = new QOpenGLTexture(montagneTop);

    QImage glacialMiddle = QImage("glacialMiddle.jpg");
    glacialM = new QOpenGLTexture(glacialMiddle);

    QImage glacialTop = QImage("glacialTop.jpg");
    glacialT = new QOpenGLTexture(glacialTop);

    m_program->release();
}

void GLWidget::paintGL()
{

    // // ---------- Compute FPS ---------
    ++frame_count;
    QTime new_time = QTime::currentTime();
    if (last_time.msecsTo(new_time) >= 1000)
    {
        last_count = frame_count;
        frame_count = 0;
        last_time = QTime::currentTime();
    }
    // // -------------------------------

    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);
    m_program->bind();
    m_view.setToIdentity();
    m_projection.setToIdentity();
    // m_model.setToIdentity();

    m_model.setToIdentity();
    m_model.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_model.rotate(m_yRot / 16.0f, 0, 1, 0);
    // m_model.rotate(m_zRot / 16.0f, 0, 0, 1);
    // std::cout << camera->getUp()[0] <<' '<< camera->getUp()[1] << ' '<<camera->getUp()[2] << std::endl;
    // if(!shift_press)
    // {

    // }
    // else
    // {
    m_view.lookAt(camera->getPosition(), camera->getPosition() + camera->getFront(), camera->getUp()); ///:CA CA MARCHE FREE CAM
    // }

    m_projection.perspective(camera->getFov(), width() / height(), 0.1f, 100.0f);

    // m_model.rotate(45.0f, 1, 1, 0);

    m_model.rotate(90, 1.0f, 0.0f, 0.0f);

    QMatrix3x3 normal_matrix = m_model.normalMatrix();

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_program->bind();

    if (hm_active)
    {
        glActiveTexture(GL_TEXTURE0);
        hmap->bind();
        m_program->setUniformValue("heightmap", 0);
    }


    glActiveTexture(GL_TEXTURE1);
    biome->bind();
    m_program->setUniformValue("biome", 1);
    

    glActiveTexture(GL_TEXTURE2);
    desertB->bind();
    m_program->setUniformValue("desertB", 2);

    glActiveTexture(GL_TEXTURE3);
    desertM->bind();
    m_program->setUniformValue("desertM", 3);

    glActiveTexture(GL_TEXTURE4);
    desertT->bind();
    m_program->setUniformValue("desertT", 4);

    glActiveTexture(GL_TEXTURE5);
    canyonB->bind();
    m_program->setUniformValue("canyonB", 5);

    glActiveTexture(GL_TEXTURE6);
    canyonM->bind();
    m_program->setUniformValue("canyonM", 6);

    glActiveTexture(GL_TEXTURE7);
    canyonT->bind();
    m_program->setUniformValue("canyonT", 7);

    glActiveTexture(GL_TEXTURE8);
    montagneB->bind();
    m_program->setUniformValue("montagneB", 8);

    glActiveTexture(GL_TEXTURE9);
    montagneM->bind();
    m_program->setUniformValue("montagneM", 9);

    glActiveTexture(GL_TEXTURE10);
    montagneT->bind();
    m_program->setUniformValue("montagneT", 10);

    glActiveTexture(GL_TEXTURE11);
    glacialB->bind();
    m_program->setUniformValue("glacialB", 11);

    glActiveTexture(GL_TEXTURE12);
    glacialM->bind();
    m_program->setUniformValue("glacialM", 12);

    glActiveTexture(GL_TEXTURE13);
    glacialT->bind();
    m_program->setUniformValue("glacialT", 13);


    m_program->setUniformValue(m_program->uniformLocation("tool_active"), tool_active);

    // if (mode_pres)
    // {
    //     m_view.rotate(angle_speed, 0, 1, 0);
    //     // m_projection.perspective(glm::radians(45.0f), (4.0f / 3.0f), 0.01f, 100.0f);
    //     // m_view.lookAt(QVector3D(0, 0, -3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    // }

    if (tool_active && !mouseLeftPressed && !mouseMiddlePressed && !mouseRightPressed)
    {

        // worldPosition = GetWorldPosition();

        m_program->setUniformValue(m_program->uniformLocation("center"), worldPosition);

        m_program->setUniformValue(m_program->uniformLocation("radius"), radius_sphere_selection);
    }

    m_program->setUniformValue(m_light_pos_loc, m_model * QVector3D(0, -2, 0));
    m_program->setUniformValue(m_program->uniformLocation("textureSize"), Image_biome_size);

    m_program->bind();
    m_program->setUniformValue(m_program->uniformLocation("model"), m_model);
    m_program->setUniformValue(m_program->uniformLocation("view"), m_view);
    m_program->setUniformValue(m_program->uniformLocation("projection"), m_projection);

    m_program->setUniformValue(m_program->uniformLocation("amplitudeMAX"), amplitude_max);
    m_program->setUniformValue(m_program->uniformLocation("amplitudeMIN"), amplitude_min);

    m_program->setUniformValue(m_program->uniformLocation("lightColor"), QVector3D(1.0f, 1.0f, 1.0f));
    m_program->setUniformValue(m_program->uniformLocation("viewPos"), m_model * QVector3D(0,-2,0));

    // if (tool_active)
    // {
    //     // object.taubinSmooth(1, 0.660f, -0.661f);
    //     QVector3D mean = object.MeanSmooth(radius_sphere_selection, QVector3D(worldPosition.x, 0, worldPosition.z), heightMapDATA);
    //     m_program->setUniformValue(m_program->uniformLocation("mean"), mean);

    //     // vertexBuffer_.bind();
    //     // vertexBuffer_.allocate(object.vertices.constData(), object.vertices.size() * sizeof(QVector3D));
    //     // vertexBuffer_.release();
    // }

    if (!mouseRightPressed && !mouseLeftPressed && !mouseMiddlePressed)
        setMouseTracking(true);
    else
        setMouseTracking(false);

    update();

    vao_.bind();

    glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_SHORT, (void *)0);
    glEnableVertexAttribArray(1);
    normalsBuffer.bind();
    glVertexAttribPointer(
        1,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );
    vao_.release();

    m_program->release();

    if (hm_active)
    {
        hmap->release();
    }

    biome->release();
    
    emit UpdateFPS(last_count);
}

void GLWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, w, h);

    float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
    m_projection.setToIdentity();
    m_projection.perspective(camera->getFov(), aspectRatio, 0.1f, 100.0f);
    
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_last_position = event->pos();
    mouseX = event->pos().x();
    mouseY = event->pos().y();

    if (event->button() == Qt::LeftButton)
    {
        mouseRightPressed = false;
        mouseLeftPressed = true;
        mouseMiddlePressed = false;

        if (tool_active)
        {
            worldPosition = GetWorldPosition(event->localPos());
            // object.SmoothMoyenneur(radius_sphere_selection, worldPosition, heightMapDATA, amplitude_max, amplitude_min);
            // std::cout << "smooth done !" << std::endl;
            // vertexBuffer_.bind();
            // vertexBuffer_.allocate(object.vertices.constData(), object.vertices.size() * sizeof(QVector3D));
            // vertexBuffer_.release();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        mouseRightPressed = true;
        mouseLeftPressed = false;
        mouseMiddlePressed = false;
    }
    else if (event->button() == Qt::MiddleButton)
    {
        if (!tool_active)
        {
            QPixmap customCursorPixmap("./dragClick.png");
            QCursor customCursor(customCursorPixmap);
            setCursor(customCursor);
        }
        lastX = event->x();
        lastY = event->y();
        if (!mouseMiddlePressed)
        {
            setMouseTracking(false);
            mouseRightPressed = false;
            mouseLeftPressed = false;
            mouseMiddlePressed = true;
        }
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_last_position.x();
    int dy = event->y() - m_last_position.y();

    mouseX = event->x();
    mouseY = event->y();

    if (mouseLeftPressed)
    {
    }
    else if (mouseRightPressed)
    {
    }
    else if (mouseMiddlePressed && shift_press)
    {
        int deltaX = mouseX - lastX;
        int deltaY = mouseY - lastY;

        camera->setPosition(camera->getPosition() + QVector3D(deltaX * 0.001, deltaY * 0.001, 0));

        lastX = mouseX;
        lastY = mouseY;
    }
    else if (mouseMiddlePressed && !hasMouseTracking())
    {
        if (firstMouse)
        {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = false;
        }

        // QVector3D right = QVector3D::crossProduct((camera->getTarget() - camera->getPosition()), camera->getUp());
        float deltaX = mouseX - lastX;
        float deltaY = lastY - mouseY;
        lastX = mouseX;
        lastY = mouseY;

        // deltaX *= camera->getMvtSpeed()*2;
        // deltaY *= camera->getMvtSpeed()*2;
        // camera->setYaw(camera->getYaw() + deltaX);
        // camera->setPitch(camera->getPitch() + deltaY);

        // // if (camera->getPitch() > 89.f)
        // // {
        // //     camera->setPitch(89.f);
        // // }

        // //   if (camera->getPitch() < -89.f)
        // // {
        // //     camera->setPitch(-89.f);
        // // }

        // QVector3D direction=QVector3D(0,0,0);
        // direction.setX(cos(glm::radians(camera->getYaw()))*cos(glm::radians(camera->getYaw())));
        // direction.setY(sin(glm::radians(camera->getPitch())));

        // direction.setZ(sin(glm::radians(camera->getYaw()) * cos(glm::radians(camera->getPitch()))));
        // direction.normalize();
        // std::cout << direction[0] <<' '<<direction[1]<<' '<<direction[2] <<std::endl;
        // frontdemerde=(-direction);

        setXRotation(m_xRot + 2.5 * deltaY);
        setYRotation(m_yRot + 2.5 * deltaX);
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPixmap customCursorPixmap("./drag.png");
    QCursor customCursor(customCursorPixmap);
    setCursor(customCursor);

    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        mouseLeftPressed = false;
    }
    else if (event->button() == Qt::RightButton)
    {
        mouseRightPressed = false;
    }
    else if (event->button() == Qt::MiddleButton)
    {
        mouseMiddlePressed = false;
    }
}
void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();

    // int delta = event->angleDelta().y();
    if (shift_press)
    {
        radius_sphere_selection += float(delta) / 2000.f;
    }
    else
    {
        camera->setPosition(camera->getPosition() + (float(delta) / 75.f) * camera->getMvtSpeed() * camera->getFront());
    }

    update();
}

void GLWidget::UpdateResolution(int res)
{
    if (res >= 2)
    {
        object.CreateFlatTerrain(1, res);
        object.setResolution(res);

        vertexBuffer_.bind();
        vertexBuffer_.allocate(object.vertices.constData(), object.vertices.size() * sizeof(QVector3D));
        vertexBuffer_.release();

        indexBuffer_.bind();
        indexBuffer_.allocate(object.indices.constData(), object.indices.size() * sizeof(short));
        indexBuffer_.release();

        m_texturebuffer.bind();
        m_texturebuffer.allocate(object.uvs.constData(), object.uvs.size() * sizeof(QVector2D));
        m_texturebuffer.release();
    }
}

void GLWidget::UpdateTerrain(QString imgname)
{
    heightMAP = QImage(imgname);
    hmap = new QOpenGLTexture(heightMAP);

    heightMapDATA.clear();

    QImage img = heightMAP.convertToFormat(QImage::Format_Grayscale8);

    for (size_t i = 0; i < img.width(); i++)
    {
        for (size_t j = 0; j < img.height(); j++)
        {
            int v = (int)qGray(img.pixel(i, j));

            heightMapDATA.append(v / 255.0f);
        }
    }

    amplitude_min = 0.1f;
    amplitude_max = 2.0f;
    m_program->bind();

    hm_active = true;
}

/**
- jaune (desert) = 215
- vert (herbe) = 1
- rouge (canyon) = 87
- blanc (neige) = 3
*/
void GLWidget::UpdateBiome(QString imgname)
{
    biome_data.clear();

    QImage img = QImage(imgname);
    img = img.convertToFormat(QImage::Format_Grayscale8);

    

    Image_biome_size = QVector2D(img.width(), img.height());

    biome = new QOpenGLTexture(img);

   
}

int GLWidget::getResolution()
{
    return object.getResolution();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    shift_press = false;
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);

    if (e->key() == Qt::Key_W)
    {
        wireframe = !wireframe;
    }
    if (e->key() == Qt::Key_Z)
    {
        camera->setPosition(camera->getPosition() + camera->getMvtSpeed() * camera->getFront());
    }
    if (e->key() == Qt::Key_S)
    {
        camera->setPosition(camera->getPosition() - camera->getMvtSpeed() * camera->getFront());
    }

    if (e->key() == Qt::Key_Q)
    {
        if (shift_press)
        {
            QVector3D cross = (QVector3D::crossProduct(camera->getFront(), camera->getUp()));
            cross.normalize();
            camera->setPosition(camera->getPosition() - cross * camera->getMvtSpeed());
        }
        else
        {
            // positionOrbit -= right * camera->getMvtSpeed();
        }
    }
    if (e->key() == Qt::Key_D)
    {
        if (shift_press)
        {
            QVector3D cross = (QVector3D::crossProduct(camera->getFront(), camera->getUp()));
            cross.normalize();
            camera->setPosition(camera->getPosition() + cross * camera->getMvtSpeed());
        }
        else
        {
            // positionOrbit += right * camera->getMvtSpeed();
        }
    }
    if (e->key() == Qt::Key_Shift)
    {
        shift_press = true;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift)
    {
        shift_press = false;
    }
}

void GLWidget::DrawCircle()
{
    tool_active = true;
}

void GLWidget::Hand_Tool()
{
    tool_active = false;
}

glm::mat4 GLWidget::convertQMatrixToGLM(const QMatrix4x4 &qMatrix)
{
    glm::mat4 glmMatrix;

    // Les indices de ligne et de colonne des éléments peuvent nécessiter des ajustements
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            // Assigner chaque élément individuel de QMatrix à glmMatrix
            glmMatrix[col][row] = qMatrix(row, col); // L'ordre peut nécessiter des ajustements
        }
    }

    return glmMatrix;
}

QVector3D GLWidget::GetWorldPosition(QPointF pt)
{
    // std::cout << "Pas Global : " << pt.x() << " - " << pt.y() << std::endl;
    // QPoint globalPos = this->mapToGlobal(pt);
    // std::cout << "Global : " << globalPos.x() << " - " << globalPos.y() << std::endl;
    // QSize viewportSize = size();

    // float x = ((2.0f * static_cast<float>(globalPos.x())) / (width())) - 1.0f;
    // float y = 1.0f - ((2.0f * static_cast<float>(globalPos.y())) / height());
    // float z = 1.0f;

    QMatrix4x4 viewportMatrix;
    float w2 = width() / 2.0f;
    float h2 = height() / 2.0f;

    viewportMatrix.setToIdentity();
    viewportMatrix.setColumn(0, QVector4D(w2, 0.0f, 0.0f, 0.0f));
    viewportMatrix.setColumn(1, QVector4D(0.0f, h2, 0.0f, 0.0f));
    viewportMatrix.setColumn(2, QVector4D(0.0f, 0.0f, 1.0f, 0.0f));
    viewportMatrix.setColumn(3, QVector4D(w2, h2, 0.0f, 1.0f));

    QMatrix4x4 viewMatrix = m_view;
    QMatrix4x4 modelViewMatrix = viewMatrix * m_model;
    QMatrix4x4 modelViewProject = m_projection * modelViewMatrix;
    QMatrix4x4 inverted = viewportMatrix * modelViewProject;

    inverted = inverted.inverted();

    float posZ;
    // float posY = height() - pt.y() - 1.0f;

    glReadPixels(static_cast<int>(pt.x()), static_cast<int>(pt.y()), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &posZ);

    QVector4D clickedPointOnScreen(static_cast<float>(pt.x()), static_cast<float>(pt.y()), 2.0f * posZ - 1.0f, 1.0f);
    QVector4D clickedPointIn3DOrgn = inverted * clickedPointOnScreen;

    // return clickedPointIn3DOrgn.toVector3D();

    // Inversion of the viewport transformation
    // QMatrix4x4 invertedViewport;
    // invertedViewport.setToIdentity();
    // invertedViewport.scale(1.0 / width(), 1.0 / height(), 1.0);

    // QVector4D screenCoordinates = invertedViewport * normalized;

    // QMatrix4x4 invertedProjection = projection.inverted();

    // QMatrix4x4 invertedModelView = modelView.inverted();

    // QVector4D modelCoordinates = invertedModelView * invertedProjection * screenCoordinates;

    // return modelCoordinates.toVector3D();

    float x = (2.0f * pt.x()) / width() - 1.0f;
    float y = 1.0f - (2.0f * pt.y()) / height();

    QVector4D rayClip(x, y, -1.0f, 1.0f);

    QMatrix4x4 invertedProjection = m_projection.inverted();
    QVector4D rayEye = invertedProjection * rayClip;
    rayEye.setZ(-1.0f);
    rayEye.setW(0.0f);

    QMatrix4x4 invertedView = m_view.inverted();
    QVector4D rayWorld = invertedView * rayEye;
    rayWorld.normalize();

    QVector3D intersectionPoint = camera->getPosition() - (cam_position.z() / rayWorld.z()) * rayWorld.toVector3D();
    qDebug() << "Intersection Point: " << intersectionPoint;

    return intersectionPoint;

}

float GLWidget::getAmplitudeMAX()
{
    return amplitude_max;
}

float GLWidget::getAmplitudeMIN()
{
    return amplitude_min;
}

void GLWidget::setAmplitudeMAX(float ampl)
{
    amplitude_max = ampl;
}

void GLWidget::setAmplitudeMIN(float ampl)
{
    amplitude_min = ampl;
}