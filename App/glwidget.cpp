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

bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0)
{
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
    tool_active = false;

    timer.start(20);
    frame_count = 0;
    last_count = 0;
    last_time = QTime::currentTime();
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

void GLWidget::loadOFF(std::string file)
{
    object.loadOFF(file);
    cleanup();
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

    initializeOpenGLFunctions();
    glClearColor(0.8, 0.8, 0.8, m_transparent ? 0 : 1);

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
    m_program->bindAttributeLocation("biome", 3);

    // Link shader pipeline
    if (!m_program->link())
        close();

    // Bind shader pipeline for use
    if (!m_program->bind())
        close();

    m_mvp_matrix_loc = m_program->uniformLocation("mvp_matrix");
    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");
    m_program->setUniformValue("ambiant_color", QVector4D(0.4, 0.4, 0.4, 1.0));

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

    m_biomebuffer.create();
    m_biomebuffer.bind();
    m_biomebuffer.allocate(biome_data.constData(), sizeof(QVector2D) * biome_data.size());
    m_biomebuffer.release();

    resizeGL(16, 9);

    vao_.bind();
    vertexBuffer_.bind();
    indexBuffer_.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    m_texturebuffer.bind();
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    m_biomebuffer.bind();
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    vao_.release();

    m_view.setToIdentity();
    m_view.translate(0.2, 0.15, -3);
    m_view.rotate(35.0f, 1, 1, 0);

    // Light position is fixed.
    m_program->setUniformValue(m_light_pos_loc, QVector3D(0, 0, 10));

    m_program->release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // m_model.setToIdentity();
    // m_model.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    // m_model.rotate(m_yRot / 16.0f, 0, 1, 0);
    // m_model.rotate(m_zRot / 16.0f, 0, 0, 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);
    m_program->bind();

    // Set modelview-projection matrix
    m_program->setUniformValue(m_mvp_matrix_loc, m_projection * m_view * m_model);
    QMatrix3x3 normal_matrix = m_model.normalMatrix();

    // Set normal matrix
    m_program->setUniformValue(m_normal_matrix_loc, normal_matrix);

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

    
    // ---------- Compute FPS ---------
    ++frame_count;
    QTime new_time = QTime::currentTime();
    if (last_time.msecsTo(new_time) >= 1000)
    {
        last_count = frame_count;
        frame_count = 0;
        last_time = QTime::currentTime();
    }
    // -------------------------------

    if (mode_pres)
    {
        m_view.rotate(angle_speed, 0, 1, 0);
        // m_projection.perspective(glm::radians(45.0f), (4.0f / 3.0f), 0.01f, 100.0f);
        // m_view.lookAt(QVector3D(0, 0, -3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    }


    glm::vec3 worldPosition = GetWorldPosition();
    // if (tool_active)
    // {
    m_program->setUniformValue(m_program->uniformLocation("radius"), 0.3f);
    m_program->setUniformValue(m_program->uniformLocation("center"), QVector3D(worldPosition.x, 0, worldPosition.z));
    m_program->setUniformValue(m_program->uniformLocation("tool_active"), tool_active);
    // }

    // if(biome_active)
    // {
    //     m_program->setUniformValue(m_program->uniformLocation("biomes"), biomes);
    // }

    update();

    vao_.bind();

    glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_SHORT, (void *)0);

    vao_.release();

    m_program->release();
    if (hm_active)
    {
        hmap->release();
    }


    emit UpdateFPS(last_count);
}

void GLWidget::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_last_position = event->pos();

    if (event->button() == Qt::LeftButton)
    {
        if (!tool_active)
        {
            QPixmap customCursorPixmap("./dragClick.png");
            QCursor customCursor(customCursorPixmap);
            setCursor(customCursor);
        }
        mouseMovePressed = false;
        mouseRotatePressed = true;
        mouseZoomPressed = false;
    }
    else if (event->button() == Qt::RightButton)
    {
        if (!tool_active)
        {
            QPixmap customCursorPixmap("./deplacer.png");
            QCursor customCursor(customCursorPixmap);
            setCursor(customCursor);
        }
        lastX = event->x();
        lastY = event->y();
        mouseMovePressed = true;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    }
    else if (event->button() == Qt::MiddleButton)
    {
        if (!mouseZoomPressed)
        {
            lastZoom = event->y();
            mouseMovePressed = false;
            mouseRotatePressed = false;
            mouseZoomPressed = true;
        }
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_last_position.x();
    int dy = event->y() - m_last_position.y();

    if (!tool_active)
    {
        mouseX = event->x();
        mouseY = event->y();

        if (mouseRotatePressed)
        {
            // setXRotation(m_xRot + dx);
            // setYRotation(m_yRot + dy);
            m_view.rotate(m_xRot + 0.004 * dy, 1, 0, 0);
            m_view.rotate(m_yRot + 0.004 * dx, 0, 1, 0);
        }
        else if (mouseMovePressed)
        {
            int deltaX = mouseX - lastX;
            int deltaY = mouseY - lastY;

            m_view.translate(deltaX * 0.001, -deltaY * 0.001, 0);

            lastX = mouseX;
            lastY = mouseY;
        }
        else if (mouseZoomPressed)
        {

            int deltaZoom = mouseY - lastZoom;

            m_view.translate(0.f, 0.f, deltaZoom * 0.02f);

            lastZoom = mouseY;
            update();
        }
    }
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
    QImage img = QImage(imgname);
    hmap = new QOpenGLTexture(img);
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
std::cout << img.width()<<" "<<  img.height()<< std::endl;
    for (size_t i = 0; i < img.width(); i++)
    {
        for (size_t j = 0; j < img.height(); j++)
        {
            int v = (int)qGray(img.pixel(i, j));
            
            // if (i == 0 && j == 0)
            //     std::cout << v << std::endl;

            if (abs(215 - v) < abs(87 - v))
            {
                biome_data.append(QVector2D(0, 0));
            }
            else
            {
                biome_data.append(QVector2D(1, 0));
            }
        }
    }

    m_biomebuffer.bind();
    m_biomebuffer.allocate(biome_data.constData(), biome_data.size() * sizeof(QVector2D));
    m_biomebuffer.release();

    biome_active = true;
}

int GLWidget::getResolution()
{
    return object.getResolution();
}

void GLWidget::DrawCircle()
{
    tool_active = true;
}

void GLWidget::Hand_Tool()
{
    tool_active = false;
}

glm::vec3 GLWidget::GetWorldPosition()
{

    QPoint mousePos = QCursor::pos();

    QPoint globalPos = mapToGlobal(mousePos); 
    QSize viewportSize = size();
    
    float x = 2.0 * static_cast<float>(globalPos.x()) / viewportSize.width() - 1.0;
    float y = -2.0 * static_cast<float>(globalPos.y()) / viewportSize.height() + 1.0;
    float z = 0.0; 

    return glm::vec3(x, z, -y);
}