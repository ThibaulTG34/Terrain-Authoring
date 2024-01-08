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
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <iostream>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "logo.h"
#include "mesh.h"
#include <QOpenGLTexture>
#include <QTime>
#include <QTimer>
#include <QCursor>
#include <Camera.h>
#include "AssimpModel.h"
#include <QMessageBox>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    Camera cam;

    void resizeGL(int width, int height) override;
    static bool isTransparent() { return m_transparent; }
    static void setTransparent(bool t) { m_transparent = t; }

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    Mesh object = Mesh(0);
    void UpdateResolution(int res);
    void UpdateTerrain(QString imgname);
    void UpdateBiome(QString imgname);
    void UpdateWater(QString imgname);
    void UpdateVegetation(QImage imgname);

    int getResolution();
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    float getAmplitudeMAX();
    float getAmplitudeMIN();

    void setAmplitudeMAX(float ampl);
    void setAmplitudeMIN(float ampl);

    void setDegrePente(float deg);

    bool wireframe;
    bool control_press;
    bool shift_press;
    bool mode_pres;

    float angle_speed;

    float theta = 0;

    QTimer timer;
    QTime last_time;
    int last_count;
    int frame_count;

public slots:
    // Completer : ajouter des slots pour signaler appliquer le changement de rotation
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void cleanup();
    void DrawCircle();
    void Tree_Tool();
    void Tree_Tool_Delete();
    void Hand_Tool();
    void HeightTool();

signals:

    // Completer : ajouter des signaux pour signaler des changement de rotation
    void objectRotChangeOnX(int angle);
    void objectRotChangeOnY(int angle);
    void objectRotChangeOnZ(int angle);
    void UpdateFPS(int newFPS);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event);
    void enterEvent(QEvent *event) override
    {

        QPixmap customCursorPixmap("./drag.png");
        QCursor customCursor(customCursorPixmap);
        setCursor(customCursor);
    }

    void leaveEvent(QEvent *event) override
    {
        setCursor(Qt::ArrowCursor);
    }

private:
    void setupVertexAttribs();
    QVector3D GetWorldPosition(QPointF pt);
    void initVegeThread(QImage imgname);
    void Load(QVector3D v);
    AssimpModel *canyon_model;
    AssimpModel *snow_model;
    AssimpModel *mountain_model;
    AssimpModel *desert_model;
    bool m_core;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    QPoint m_last_position;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *vege_shader;
    int m_mvp_matrix_loc;
    int m_normal_matrix_loc;
    int m_light_pos_loc;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;
    static bool m_transparent;

    float objectRotationX = 0;
    float objectRotationY = 0;

    int mouseX, mouseY;
    QPointF mousePos;

    float radius_sphere_selection = 0.3f;
    QVector3D worldPosition;
    QImage heightMAP;
    QImage heightMAP_generated;
    QImage heightMAP_smooth;
    QVector<float> heightMapDATA;
    QVector2D Image_biome_size;

    bool hm_active = false;
    QOpenGLTexture *hmap;
    QOpenGLTexture *hmap_tool;
    QOpenGLTexture *NEW_hmap_tool;
    float amplitude_max;
    float amplitude_min;

    QOpenGLTexture *water;
    QOpenGLTexture *biome;
    QImage biome_img;

    QOpenGLTexture *desertB;
    QOpenGLTexture *desertM;
    QOpenGLTexture *desertT;

    QOpenGLTexture *canyonB;
    QOpenGLTexture *canyonM;
    QOpenGLTexture *canyonT;

    QOpenGLTexture *montagneB;
    QOpenGLTexture *montagneM;
    QOpenGLTexture *montagneT;

    QOpenGLTexture *glacialB;
    QOpenGLTexture *glacialM;
    QOpenGLTexture *glacialT;

    QOpenGLTexture *waterText;

    QOpenGLTexture *vegeText;
    int degre_de_pente_tolere;

    bool applySmooth = false;
    bool mouseRightPressed = false;
    bool mouseLeftPressed = false;
    bool mouseMiddlePressed = false;
    int lastX = 0;
    int lastY = 0;
    int lastZoom = 0;
    Camera *camera;

    bool tool_active = false;
    bool height_tool = false;
    bool tree_active = false;
    bool tree_active_delete=false;
    

    QVector3D cam_position;
    QVector3D cam_front;
    QVector3D cam_up;

    QVector<QVector2D> biome_data;

    QVector<AssimpModel *> vegetation;
    QImage vegetation_map;
    int selectedVege=-1;

    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vertexBuffer_ = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer indexBuffer_ = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    QOpenGLBuffer normalsBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer m_texturebuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer m_texturebufferForVege = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    // QOpenGLBuffer hauteursBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    // QOpenGLBuffer m_biomebuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    // QOpenGLBuffer VegeVertexBuffer_ = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    // QOpenGLBuffer VegeUvsBuffer_ = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    // QOpenGLBuffer VegeIndicesBuffer_ = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    QOpenGLVertexArrayObject Vegevao_;
};

#endif
