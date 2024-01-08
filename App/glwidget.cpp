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
#include <QThread>
#include <random>

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

    QImage blackImage(300, 300, QImage::Format_Grayscale8);
    blackImage.fill(Qt::black);
    biome_img = blackImage;
    biome = new QOpenGLTexture(blackImage);

    hmap = new QOpenGLTexture(blackImage);
    heightMAP_generated = QImage(256, 256, QImage::Format_Grayscale8);
    heightMAP_generated.fill(0);
    heightMAP = blackImage;
    QImage whiteImage(300, 300, QImage::Format_RGB888);
    whiteImage.fill(Qt::white);
    water = new QOpenGLTexture(whiteImage);
    m_program = new QOpenGLShaderProgram;
    vege_shader = new QOpenGLShaderProgram;

    // Compile vertex shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    if (!vege_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader_vege.glsl"))
        close();
    // Compile fragment shader
    if (!vege_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader_vege.glsl"))
        close();

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("texture_coordonnees", 2);

    // Link shader pipeline
    if (!m_program->link())
        close();
    if (!m_program->bind())
        close();

    if (!vege_shader->link())
        close();
    if (!vege_shader->bind())
        close();

    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");

    vao_.create();
    // QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);
    // QOpenGLVertexArrayObject::Binder vaoBinder(&Vegevao_);

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

    // hauteursBuffer.create();
    // hauteursBuffer.bind();
    // hauteursBuffer.allocate(heightMapDATA.constData(), heightMapDATA.size() * sizeof(GL_FLOAT));
    // hauteursBuffer.release();

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

    // hauteursBuffer.bind();
    // glEnableVertexAttribArray(3);
    // glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), nullptr);

    vao_.release();

    // Vegevao_.bind();
    // Vegevao_.release();

    m_view.setToIdentity();
    m_model.setToIdentity();
    m_projection.setToIdentity();

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

    QImage image_water = QImage("waterText.png");
    waterText = new QOpenGLTexture(image_water);

    m_program->release();

    // -----------------VEGETATION--------------------
    Vegevao_.create();
    Vegevao_.bind();
    vege_shader->bind();
    // VegeVertexBuffer_.create();
    // VegeVertexBuffer_.bind();
    // VegeVertexBuffer_.allocate(vege.getVertices().constData(), vege.getVertices().size() * sizeof(Vertex));
    // VegeVertexBuffer_.release();

    // VegeIndicesBuffer_.create();
    // VegeIndicesBuffer_.bind();
    // VegeIndicesBuffer_.allocate(vege.getIndices().constData(), vege.getIndices().size() * sizeof(unsigned int));
    // VegeIndicesBuffer_.release();
    // VegeUvsBuffer_.create();
    // VegeUvsBuffer_.bind();
    // VegeUvsBuffer_.allocate(vege.getUVs().constData(), sizeof(QVector2D) * vege.getUVs().size());
    // VegeUvsBuffer_.release();
    // vege_shader->bind();

    // VegeVertexBuffer_.bind();
    // VegeIndicesBuffer_.bind();
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    canyon_model = new AssimpModel();
    canyon_model->LoadModel("./rock_canyon.obj");

    snow_model = new AssimpModel();
    snow_model->LoadModel("./sapin.obj");

    desert_model = new AssimpModel();
    desert_model->LoadModel("./rock_desert.obj");

    mountain_model = new AssimpModel();
    mountain_model->LoadModel("./tree.obj");

    // vegetation.append(mountain_model);

    vege_shader->release();
    Vegevao_.release();
    vege_shader->bindAttributeLocation("texture_coordonnees", 3);

    // m_texturebufferForVege.create();
    // m_texturebufferForVege.bind();
    // m_texturebufferForVege.allocate(object.uvs.constData(), sizeof(QVector2D) * object.uvs.size());
    // glEnableVertexAttribArray(3);
    // glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    // QImage image_vege = QImage("text_vege.jpg");
    // vegeText = new QOpenGLTexture(image_vege);

    // VegeUvsBuffer_.bind();
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    // worldPosition = QVector3D(0, 0, 0);
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
    m_view.rotate(180.0f, 0.0, 0.0, 1.0);
    m_projection.setToIdentity();
    // m_model.setToIdentity();

    m_model.setToIdentity();
    // m_model.rotate(-180.0f , 0, 0, 1);
    m_model.rotate(180 - (m_xRot / 16.0f), 1, 0, 0);
    m_model.rotate(m_yRot / 16.0f, 0, 1, 0);

    // m_model.rotate(m_zRot / 16.0f, 0, 0, 1);
    m_view.lookAt(camera->getPosition(), camera->getPosition() + camera->getFront(), camera->getUp());

    m_projection.perspective(camera->getFov(), width() / height(), 0.1f, 100.0f);

    // m_model.rotate(45.0f, 1, 1, 0);
    // m_model.rotate(90, 1.0f, 0.0f, 0.0f);

    QMatrix3x3 normal_matrix = m_model.normalMatrix();

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_program->bind();

    if (hm_active)
    {
        glActiveTexture(GL_TEXTURE0);

        if (tool_active)
        {
            hmap_tool->bind();
            m_program->setUniformValue("heightmap_tool", 0);
        }
        else if (height_tool)
        {
            NEW_hmap_tool->bind();
            m_program->setUniformValue("NEW_hmap_tool", 0);
        }
        else /* if(tree_active) */
        {
            hmap->bind();
            m_program->setUniformValue("heightmap", 0);
        }
        m_program->setUniformValue(m_program->uniformLocation("amplitudeMAX"), amplitude_max);
        m_program->setUniformValue(m_program->uniformLocation("amplitudeMIN"), amplitude_min);
    }

    glActiveTexture(GL_TEXTURE1);
    biome->bind();
    m_program->setUniformValue("biome", 1);

    glActiveTexture(GL_TEXTURE2);
    water->bind();
    m_program->setUniformValue("water", 2);

    glActiveTexture(GL_TEXTURE3);
    desertB->bind();
    m_program->setUniformValue("desertB", 3);

    glActiveTexture(GL_TEXTURE4);
    desertM->bind();
    m_program->setUniformValue("desertM", 4);

    glActiveTexture(GL_TEXTURE5);
    desertT->bind();
    m_program->setUniformValue("desertT", 5);

    glActiveTexture(GL_TEXTURE6);
    canyonB->bind();
    m_program->setUniformValue("canyonB", 6);

    glActiveTexture(GL_TEXTURE7);
    canyonM->bind();
    m_program->setUniformValue("canyonM", 7);

    glActiveTexture(GL_TEXTURE8);
    canyonT->bind();
    m_program->setUniformValue("canyonT", 8);

    glActiveTexture(GL_TEXTURE9);
    montagneB->bind();
    m_program->setUniformValue("montagneB", 9);

    glActiveTexture(GL_TEXTURE10);
    montagneM->bind();
    m_program->setUniformValue("montagneM", 10);

    glActiveTexture(GL_TEXTURE11);
    montagneT->bind();
    m_program->setUniformValue("montagneT", 11);

    glActiveTexture(GL_TEXTURE12);
    glacialB->bind();
    m_program->setUniformValue("glacialB", 12);

    glActiveTexture(GL_TEXTURE13);
    glacialM->bind();
    m_program->setUniformValue("glacialM", 13);

    glActiveTexture(GL_TEXTURE14);
    glacialT->bind();
    m_program->setUniformValue("glacialT", 14);

    glActiveTexture(GL_TEXTURE15);
    waterText->bind();
    m_program->setUniformValue("waterText", 15);

    m_program->setUniformValue(m_program->uniformLocation("tool_active"), tool_active);
    m_program->setUniformValue(m_program->uniformLocation("height_tool"), height_tool);
    m_program->setUniformValue(m_program->uniformLocation("tree_active"), tree_active);

    if (tool_active || height_tool || tree_active)
    {
        // qDebug()<<"chlab"<<worldPosition;
        m_program->setUniformValue(m_program->uniformLocation("center"), worldPosition);
        m_program->setUniformValue(m_program->uniformLocation("radius"), radius_sphere_selection);
    }

    m_program->setUniformValue(m_light_pos_loc, m_model * QVector3D(0, -3, 0));
    m_program->setUniformValue(m_program->uniformLocation("textureSize"), Image_biome_size);

    m_program->bind();
    m_program->setUniformValue(m_program->uniformLocation("model"), m_model);

    m_program->setUniformValue(m_program->uniformLocation("view"), m_view);
    m_program->setUniformValue(m_program->uniformLocation("projection"), m_projection);

    m_program->setUniformValue(m_program->uniformLocation("lightColor"), QVector3D(1.0f, 1.0f, 1.0f));
    m_program->setUniformValue(m_program->uniformLocation("viewPos"), m_model * QVector3D(0, -2, 0));
    // m_program->setUniformValue(m_program->uniformLocation("cam_pos"), camera->getPosition());

    if (!mouseRightPressed && !mouseLeftPressed && !mouseMiddlePressed)
        setMouseTracking(true);
    else
        setMouseTracking(false);

    update();

    vao_.bind();

    m_program->bind();
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

    m_texturebuffer.bind();
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    vao_.release();

    m_program->release();

    if (hm_active)
    {
        hmap->release();
        if (tool_active)
        {
            hmap_tool->release();
        }

        if (height_tool)
        {
            NEW_hmap_tool->release();
        }
    }

    biome->release();

    vege_shader->bind();
    Vegevao_.bind();

    // glActiveTexture(GL_TEXTURE0);
    // vegeText->bind();
    // vege_shader->setUniformValue("text_tree", 0);

    // VegeUvsBuffer_.bind();
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // glDrawElements(GL_TRIANGLES, vege.getIndices().size(), GL_UNSIGNED_SHORT, (void *)0);
    // QMatrix4x4 m;
    // m.setToIdentity();
    // m.scale(0.1);
    // m.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    // m.rotate(m_yRot / 16.0f, 0, 1, 0);
    // vege.setModelMatrix(m);
    m_program->setUniformValue(m_program->uniformLocation("light_position"), m_model * QVector3D(0, -3, 0));
    m_program->setUniformValue(m_program->uniformLocation("lightColor"), QVector3D(1.0f, 1.0f, 1.0f));
    std::mt19937 gen(vegetation.size()); // La graine est ici 42, vous pouvez la modifier
    std::uniform_int_distribution<> distribution(0, 360);

    for (size_t i = 0; i < vegetation.size(); i++)
    {
        bool haveToBeDraw = true;
        if (hm_active)
        {
            QVector2D my_uv = object.uvs[vegetation[i]->getIndiceReference()];
            int my_height = qGray(heightMAP.pixel(my_uv[0] * (heightMAP.width() - 1), my_uv[1] * (heightMAP.height() - 1)));
            my_height = (my_height * (amplitude_max - amplitude_min)) + amplitude_min;
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    if (vegetation[i]->getIndiceReference() + k + l * object.getResolution() > 0 && vegetation[i]->getIndiceReference() + k + l * object.getResolution() < object.uvs.size())
                    {
                        QVector2D my_voisin = object.uvs[vegetation[i]->getIndiceReference() + k + l * object.getResolution()] /* + QVector2D((1 / heightMAP.width()), 0) */;
                        int my_voisin_height = qGray(heightMAP.pixel(my_voisin[0] * (heightMAP.width() - 1), my_voisin[1] * (heightMAP.height() - 1)));
                        my_voisin_height = (my_voisin_height * (amplitude_max - amplitude_min)) + amplitude_min;
                        if (abs(my_height - my_voisin_height) > degre_de_pente_tolere)
                        {
                            haveToBeDraw = false;
                        }
                    }
                }
            }
            if (!haveToBeDraw)
            {
                continue;
            }
        }
        if (tree_active_delete)
        {
            vege_shader->setUniformValue(vege_shader->uniformLocation("selected"), (i == selectedVege));
        }
        else
        {
            vege_shader->setUniformValue(vege_shader->uniformLocation("selected"), false);
        }
        QMatrix4x4 m = vegetation[i]->getModel_Matrix();

        m.setToIdentity();
        // qDebug()<<object.normals[vegetation[i]->getIndiceReference()];
        m.rotate(-180.0f - (m_xRot / 16.0f), 1, 0, 0);
        m.rotate(m_yRot / 16.0f, 0, 1, 0);
        QVector3D v = object.vertices[vegetation[i]->getIndiceReference()];
        m.translate(v);

        m.rotate(distribution(gen), .0f, 1.0f, 0.0f);
        vegetation[i]->setModel_Matrix(m);
        vege_shader->setUniformValue(vege_shader->uniformLocation("model"), vegetation[i]->getModel_Matrix());
        vege_shader->setUniformValue(vege_shader->uniformLocation("view"), m_view);
        vege_shader->setUniformValue(vege_shader->uniformLocation("projection"), m_projection);

        if (hm_active)
        {
            glActiveTexture(GL_TEXTURE1);

            if (tool_active)
            {
                hmap_tool->bind();
                vege_shader->setUniformValue("heightmap_tool", 1);
            }
            else
            {
                hmap->bind();
                vege_shader->setUniformValue("heightmap", 1);
            }
            vege_shader->setUniformValue(vege_shader->uniformLocation("amplitudeMIN"), amplitude_min);
            vege_shader->setUniformValue(vege_shader->uniformLocation("amplitudeMAX"), amplitude_max);
        }

        vege_shader->setUniformValue(vege_shader->uniformLocation("tool_active"), tool_active);
        vege_shader->setUniformValue(vege_shader->uniformLocation("uvLocalPlan"), object.uvs[vegetation[i]->getIndiceReference()]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        // m_texturebufferForVege.bind();
        // glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
        QVector<AssimpModel::MeshEntry> m_Entries = vegetation[i]->getM_entries();
        QVector<QOpenGLTexture *> m_Textures = vegetation[i]->getTextures();
        for (unsigned int i = 0; i < m_Entries.size(); i++)
        {
            // glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VegeVertexBuffer_);
            m_Entries[i].VegeVertexBuffer_.bind();
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)12);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)20);

            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].VegeIndicesBuffer_);
            m_Entries[i].VegeIndicesBuffer_.bind();

            const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

            if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex])
            {
                glActiveTexture(GL_TEXTURE0);
                m_Textures[MaterialIndex]->bind();
            }
            glDrawElements(GL_TRIANGLES, m_Entries[i].getNumIndices(), GL_UNSIGNED_INT, 0);
        }
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
    vege_shader->release();
    Vegevao_.release();

    emit UpdateFPS(last_count);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());

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
            // heightMAP = QImage("./newHeight.png");
            object.SmoothMoyenneur(radius_sphere_selection, worldPosition, amplitude_min, amplitude_max, heightMAP_smooth);
            hmap_tool = new QOpenGLTexture(heightMAP_smooth);
        }
        if (height_tool)
        {
            // NEW_hmap_tool = new QOpenGLTexture(heightMAP);
            object.GenerateHeight(radius_sphere_selection, worldPosition, heightMAP_generated, shift_press);
            NEW_hmap_tool = new QOpenGLTexture(heightMAP_generated);
            // hmap = new QOpenGLTexture(heightMAP);
            // hmap_tool = new QOpenGLTexture(heightMAP);

            amplitude_min = 0.1f;
            amplitude_max = 2.0f;
            hm_active = true;
        }
        if (tree_active)
        {
            int densite = 10;
            QVector3D target = worldPosition;
            float distMin = target.distanceToPoint(object.vertices[0]);

            for (size_t i = 1; i < object.vertices.size(); i++)
            {
                float dist = target.distanceToPoint(object.vertices[i]);
                if (dist < radius_sphere_selection && rand() % densite == 8)
                {
                    vegetation_map.setPixel(object.uvs[i].x() * (vegetation_map.width() - 1), object.uvs[i].y() * (vegetation_map.height() - 1), qRgb(0, 255, 0));
                }
            }
            vegetation_map.save("./newVege.png");
            UpdateVegetation(vegetation_map);
        }

        // if(height_tool && shift_press)
        // {
        //     object.GenerateHeight(radius_sphere_selection, worldPosition, heightMAP_generated, shift_press);
        //     NEW_hmap_tool = new QOpenGLTexture(heightMAP_generated);
        //     amplitude_min = 0.1f;
        //     amplitude_max = 2.0f;
        //     hm_active = true;
        // }
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

    mousePos = event->localPos();

    if (mouseRightPressed && tool_active)
    {
        worldPosition = GetWorldPosition(mousePos);
    }
    else if (mouseRightPressed && height_tool)
    {
        worldPosition = GetWorldPosition(mousePos);
    }

    else if (mouseMiddlePressed && shift_press)
    {
        int deltaX = mouseX - lastX;
        int deltaY = mouseY - lastY;

        camera->setPosition(camera->getPosition() + QVector3D(-deltaX * 0.001, -deltaY * 0.001, 0));

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

        // std::cout << "x : " << m_xRot << std::endl;

        setXRotation(m_xRot + 2.5 * deltaY);
        setYRotation(m_yRot + 2.5 * deltaX);

        update();
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

        normalsBuffer.bind();
        normalsBuffer.allocate(object.normals.constData(), sizeof(QVector3D) * object.normals.size());
        normalsBuffer.release();

        m_texturebuffer.bind();
        m_texturebuffer.allocate(object.uvs.constData(), object.uvs.size() * sizeof(QVector2D));
        m_texturebuffer.release();
        if (!vegetation_map.isNull())
            UpdateVegetation(vegetation_map);
    }
}

void GLWidget::UpdateTerrain(QString imgname)
{
    heightMAP = QImage(imgname);
    hmap = new QOpenGLTexture(heightMAP);
    hmap_tool = new QOpenGLTexture(heightMAP);
    NEW_hmap_tool = new QOpenGLTexture(heightMAP);
    heightMAP_generated = heightMAP;
    heightMAP_smooth = heightMAP;

    heightMapDATA.clear();

    amplitude_min = 0.1f;
    amplitude_max = 2.0f;

    m_program->bind();
    // hauteursBuffer.bind();
    // hauteursBuffer.allocate(heightMapDATA.constData(), heightMapDATA.size() * sizeof(float));
    // hauteursBuffer.release();
    hm_active = true;
}

/**
- jaune (desert) = 215
- vert (herbe) = 127
- rouge (canyon) = 87
- blanc (neige) = 255
*/
void GLWidget::UpdateBiome(QString imgname)
{
    std::cout << qGray(biome_img.pixel(0, 0)) << std::endl;
    biome_img = QImage(imgname);
    std::cout << qGray(biome_img.pixel(0, 0)) << std::endl;

    // biome_img = biome_img.convertToFormat(QImage::Format_Grayscale8);

    Image_biome_size = QVector2D(biome_img.width(), biome_img.height());

    biome = new QOpenGLTexture(biome_img.convertToFormat(QImage::Format_Grayscale8));
}

void GLWidget::UpdateWater(QString imgname)
{
    QImage img = QImage(imgname);
    img = img.convertToFormat(QImage::Format_Grayscale8);
    water = new QOpenGLTexture(img);
}

void GLWidget::UpdateVegetation(QImage imgname)
{
    vegetation_map = imgname;
    selectedVege = 0;
    vegetation.clear();
    QThread *thread = new QThread();
    QObject::connect(thread, &QThread::started, [this, imgname]()
                     { initVegeThread(imgname); });
    thread->start();
}

void GLWidget::initVegeThread(QImage img)
{
    int size_vertices = object.vertices.size();
    for (size_t i = 0; i < size_vertices; i++)
    {
        int green = qGreen(img.pixel(object.uvs[i].x() * (img.width() - 1), object.uvs[i].y() * (img.height() - 1)));
        int blue = qBlue(img.pixel(object.uvs[i].x() * (img.width() - 1), object.uvs[i].y() * (img.height() - 1)));
        int red = qRed(img.pixel(object.uvs[i].x() * (img.width() - 1), object.uvs[i].y() * (img.height() - 1)));
        if (green == 255 && red == 0 && blue == 0)
        {
            AssimpModel *A = new AssimpModel();
            int biomeVal = qGray(biome_img.pixel(object.uvs[i].x() * (biome_img.width() - 1), object.uvs[i].y() * (biome_img.height() - 1)));
            if (biomeVal == 87)
            {
                A->setVertices(canyon_model->getVertices());
                A->setIndices(canyon_model->getIndices());
                A->setTextures(canyon_model->getTextures());
                A->setM_entries(canyon_model->getM_entries());
            }
            else if (biomeVal == 215)
            {
                A->setVertices(desert_model->getVertices());
                A->setIndices(desert_model->getIndices());
                A->setTextures(desert_model->getTextures());
                A->setM_entries(desert_model->getM_entries());
            }
            else if (biomeVal == 127)
            {
                A->setVertices(mountain_model->getVertices());
                A->setIndices(mountain_model->getIndices());
                A->setTextures(mountain_model->getTextures());
                A->setM_entries(mountain_model->getM_entries());
            }
            else if (biomeVal == 255)
            {
                A->setVertices(snow_model->getVertices());
                A->setIndices(snow_model->getIndices());
                A->setTextures(snow_model->getTextures());
                A->setM_entries(snow_model->getM_entries());
            }

            A->setIndiceReference(i);
            vegetation.append(A);
            // qDebug() << object.uvs[i];
        }
        // qDebug() << i << "/" << object.vertices.size();
    }
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
    if (e->key() == Qt::Key_I && (height_tool || tool_active || tree_active))
    {
        worldPosition.setZ(worldPosition.z() - 0.05);
    }
    else if (e->key() == Qt::Key_K && (height_tool || tool_active || tree_active))
    {
        worldPosition.setZ(worldPosition.z() + 0.05);
    }
    if (e->key() == Qt::Key_J && (height_tool || tool_active || tree_active))
    {
        worldPosition.setX(worldPosition.x() - 0.05);
    }
    else if (e->key() == Qt::Key_L && (height_tool || tool_active || tree_active))
    {
        worldPosition.setX(worldPosition.x() + 0.05);
    }

    // qDebug() << worldPosition;
    if (e->key() == Qt::Key_N)
    {
        selectedVege = (selectedVege + 1);
        if (selectedVege >= vegetation.size())
        {
            selectedVege = vegetation.size() - 1;
        }
    }
    else if (e->key() == Qt::Key_B)
    {
        selectedVege = (selectedVege - 1);
        if (selectedVege < 0)
        {
            selectedVege = 0;
        }
    }

    if (e->key() == Qt::Key_Delete)
    {
        if (vegetation.size() > 0)
        {
            vegetation_map.setPixel(object.uvs[vegetation[selectedVege]->getIndiceReference()].x() * (vegetation_map.width() - 1), object.uvs[vegetation[selectedVege]->getIndiceReference()].y() * (vegetation_map.height() - 1), qRgb(255, 255, 255));

            vegetation.remove(selectedVege);
            if (selectedVege >= vegetation.size())
            {
                selectedVege = vegetation.size() - 1;
            }
            if (selectedVege < 0)
            {
                selectedVege = 0;
            }
        }
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
    if (height_tool)
    {
        QMessageBox msgBox;
        msgBox.setText("Voulez-vous sauvegarder vos changements ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        // Affichage de la boîte de dialogue et attente de la réponse de l'utilisateur
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
        {
            if (height_tool)
            {
                heightMAP = heightMAP_generated;
                heightMAP_smooth = heightMAP_generated;
                hmap_tool = new QOpenGLTexture(heightMAP_smooth);
                hmap = new QOpenGLTexture(heightMAP);
            }
            if (tool_active)
            {
                heightMAP = heightMAP_smooth;
                heightMAP_generated = heightMAP_smooth;
                hmap = new QOpenGLTexture(heightMAP);
                NEW_hmap_tool = new QOpenGLTexture(heightMAP_smooth);
            }
        }
    }

    tool_active = true;
    height_tool = false;
    tree_active = false;
}

void GLWidget::Hand_Tool()
{
    if (height_tool || tool_active)
    {
        QMessageBox msgBox;
        msgBox.setText("Voulez-vous sauvegarder vos changements ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        // Affichage de la boîte de dialogue et attente de la réponse de l'utilisateur
        int ret = msgBox.exec();

        // Traitement de la réponse de l'utilisateur
        if (ret == QMessageBox::Yes)
        {
            // Action si l'utilisateur clique sur "Oui"
            // Remplacez cette partie avec votre action souhaitée
            if (height_tool)
            {
                heightMAP = heightMAP_generated;
                heightMAP_smooth = heightMAP_generated;
                hmap_tool = new QOpenGLTexture(heightMAP_smooth);
                hmap = new QOpenGLTexture(heightMAP);
            }
            if (tool_active)
            {
                heightMAP = heightMAP_smooth;
                heightMAP_generated = heightMAP_smooth;
                hmap = new QOpenGLTexture(heightMAP);
                NEW_hmap_tool = new QOpenGLTexture(heightMAP_smooth);
            }
        }
    }

    tool_active = false;
    height_tool = false;
    tree_active = false;
    tree_active_delete = false;
}

void GLWidget::HeightTool()
{
    if (tool_active)
    {

        QMessageBox msgBox;
        msgBox.setText("Voulez-vous sauvegarder vos changements ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        // Affichage de la boîte de dialogue et attente de la réponse de l'utilisateur
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {
            // Action si l'utilisateur clique sur "Oui"
            // Remplacez cette partie avec votre action souhaitée
            if (tool_active)
            {
                heightMAP = heightMAP_smooth;
                heightMAP_generated = heightMAP_smooth;
                hmap = new QOpenGLTexture(heightMAP);
                NEW_hmap_tool = new QOpenGLTexture(heightMAP_smooth);
            }
        }
    }

    height_tool = true;
    tool_active = false;
    tree_active = false;
    tree_active_delete = false;
}

void GLWidget::Tree_Tool()
{
    if (height_tool || tool_active)
    {

        QMessageBox msgBox;
        msgBox.setText("Voulez-vous sauvegarder vos changements ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        // Affichage de la boîte de dialogue et attente de la réponse de l'utilisateur
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {
            if (height_tool)
            {
                heightMAP = heightMAP_generated;
                heightMAP_smooth = heightMAP_generated;
                hmap_tool = new QOpenGLTexture(heightMAP_smooth);
                hmap = new QOpenGLTexture(heightMAP);
            }
            if (tool_active)
            {
                heightMAP = heightMAP_smooth;
                heightMAP_generated = heightMAP_smooth;
                hmap = new QOpenGLTexture(heightMAP);
                NEW_hmap_tool = new QOpenGLTexture(heightMAP_smooth);
            }
        }
    }

    height_tool = false;
    tool_active = false;
    tree_active_delete = false;
    tree_active = true;
}

void GLWidget::Tree_Tool_Delete()
{
    if (height_tool || tool_active)
    {
        QMessageBox msgBox;
        msgBox.setText("Voulez-vous sauvegarder vos changements ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        // Affichage de la boîte de dialogue et attente de la réponse de l'utilisateur
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {
            // Action si l'utilisateur clique sur "Oui"
            // Remplacez cette partie avec votre action souhaitée
            if (height_tool)
            {
                heightMAP = heightMAP_generated;
                heightMAP_smooth = heightMAP_generated;
                hmap_tool = new QOpenGLTexture(heightMAP_smooth);
                hmap = new QOpenGLTexture(heightMAP);
            }
            if (tool_active)
            {
                heightMAP = heightMAP_smooth;
                heightMAP_generated = heightMAP_smooth;
                hmap = new QOpenGLTexture(heightMAP);
                NEW_hmap_tool = new QOpenGLTexture(heightMAP_smooth);
            }
        }
    }

    height_tool = false;
    tool_active = false;
    tree_active_delete = true;
    tree_active = false;
}

QVector3D GLWidget::GetWorldPosition(QPointF pt)
{
    float x_n = (pt.x() - 0.0f) / width();
    float y_n = (pt.y() - 0.0f) / height();
    float x = (2.0f * x_n) - 1.0f;
    float y = 1.0f - (2.0f * y_n);

    QMatrix4x4 invertedViewProjection = (m_projection * m_view).inverted();

    QVector4D rayClip(x, 0, y, 1.0f);
    // QMatrix4x4 inverted = m_view.inverted() * m_projection.inverted();
    // QVector4D rayEye = m_projection.inverted() * rayClip;
    // rayEye.setZ(-1.0f);
    // rayEye.setW(0.0f);

    QVector3D intersectionPoint(0, 0, 0);

    QMatrix4x4 proj_inv = m_projection.inverted();
    QMatrix4x4 view_inv = m_view.inverted();
    QMatrix4x4 model_inv = m_model.inverted();
    QMatrix4x4 inverse = model_inv * view_inv * m_projection;
    QVector4D point = invertedViewProjection * rayClip;

    // intersectionPoint = camera->getPosition() - (camera->getPosition().z() / world.z()) * world;
    return (rayClip.toVector3D() * m_model);
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

void GLWidget::setDegrePente(float deg)
{
    degre_de_pente_tolere = deg;
}
