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
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    // Compile vertex shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);

    // Link shader pipeline
    if (!m_program->link())
        close();

    // Bind shader pipeline for use
    if (!m_program->bind())
        close();

    m_mvp_matrix_loc = m_program->uniformLocation("mvp_matrix");
    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
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

    vao_.bind();
    vertexBuffer_.bind();
    indexBuffer_.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    vao_.release();
    // Store the vertex attribute bindings for the program.
    //    setupVertexAttribs();

    // Our camera never changes in this example.
    m_view.setToIdentity();
    m_view.translate(0,0,-10);
    // m_view.rotate(45.0f, 1, 1, 0);
    // m_view.lookAt(cam_position, cam_position + cam_front, cam_up);

    // Light position is fixed.
    m_program->setUniformValue(m_light_pos_loc, QVector3D(0, 0, 70));

    m_program->release();
}

void GLWidget::setupVertexAttribs()
{
    indexBuffer_.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    indexBuffer_.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_model.setToIdentity();
    m_model.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_model.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_model.rotate(m_zRot / 16.0f, 0, 0, 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);
    m_program->bind();

    // Set modelview-projection matrix
    m_program->setUniformValue(m_mvp_matrix_loc, m_projection * m_view * m_model);
    QMatrix3x3 normal_matrix = m_model.normalMatrix();

    // Set normal matrix
    m_program->setUniformValue(m_normal_matrix_loc, normal_matrix);

    //    glDrawArrays(GL_TRIANGLES, 0, object.vertices.size());

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    update();

    vao_.bind();
    glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_SHORT, (void *)0);

    vao_.release();

    m_program->release();
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
        mouseMovePressed = false;
        mouseRotatePressed = true;
        mouseZoomPressed = false;
    }
    else if (event->button() == Qt::RightButton)
    {
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

// void GLWidget::WheelEvent(QMouseEvent *event)
// {
//     lastZoom = event->y();
//     mouseMovePressed = false;
//     mouseRotatePressed = false;
//     mouseZoomPressed = true;
// }

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_last_position.x();
    int dy = event->y() - m_last_position.y();

    // if (event->buttons() & Qt::LeftButton)
    // {
    //     setXRotation(m_xRot + 8 * dy);
    //     setYRotation(m_yRot + 8 * dx);
    // }
    // else if (event->buttons() & Qt::RightButton)
    // {
    //     setXRotation(m_xRot + 8 * dy);
    //     setZRotation(m_zRot + 8 * dx);
    // }
    // m_last_position = event->pos();

    int x = event->x();
    int y = event->y();

    if (mouseRotatePressed)
    {
        setXRotation(m_xRot + 5 * dy);
        setYRotation(m_yRot + 5 * dx);
    }
    else if (mouseMovePressed)
    {
        int deltaX = x - lastX;
        int deltaY = y - lastY;

        m_view.translate(deltaX * 0.01, deltaY * 0.01, 0);

        lastX = x;
        lastY = y;
    }
    else if (mouseZoomPressed)
    {
        int deltaZoom = y - lastZoom;

        m_view.translate(0, 0.f, deltaZoom * 0.02f);

        lastZoom = y;
    }
}

void GLWidget::UpdateResolution(int res)
{
    object.CreateFlatTerrain(res);

    vertexBuffer_.bind();
    vertexBuffer_.allocate(object.vertices.constData(), object.vertices.size() * sizeof(QVector3D));
    vertexBuffer_.release();

    indexBuffer_.bind();
    indexBuffer_.allocate(object.indices.constData(), object.indices.size() * sizeof(short));
    indexBuffer_.release();
}

void GLWidget::UpdateTerrain(QVector<char> data)
{
    object.ModifyTerrain(data);

    vertexBuffer_.bind();
    vertexBuffer_.allocate(object.vertices.constData(), object.vertices.size() * sizeof(QVector3D));
    vertexBuffer_.release();

    indexBuffer_.bind();
    indexBuffer_.allocate(object.indices.constData(), object.indices.size() * sizeof(short));
    indexBuffer_.release();
}