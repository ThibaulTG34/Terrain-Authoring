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
#include "window.h"
#include "mainwindow.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QImage>
#include <iostream>
#include <QFileDialog>
#include <QCheckBox>
#include <QGraphicsOpacityEffect>

using namespace std;

Window::Window(MainWindow *mw)
    : mainWindow(mw)
{
    mainWindow = mw;
    mainWindow->setFixedSize(1000, 1000);

    QWidget *w = new QWidget;
    QHBoxLayout *container = new QHBoxLayout;
    w->setLayout(container);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // ToolSide->addWidget(import);
    mainLayout->addWidget(w);

    glWidget = new GLWidget;
    QObject::connect(glWidget, &GLWidget::UpdateFPS, this, &Window::ChangeFPS);

    container->setSpacing(20);

    QVBoxLayout *ToolSide = new QVBoxLayout;
    QWidget *ToolSideContainer = new QWidget;
    ToolSideContainer->setLayout(ToolSide);

    FPS = new QLabel();
    FPS->setText("FPS: --");
    FPS->setFixedSize(100, 40);
    FPS->setStyleSheet("font-size: 16pt;");

    ToolSide->addWidget(FPS);
    ToolSide->setAlignment(FPS, Qt::AlignTop);
    ToolSideContainer->setStyleSheet("padding: 0 ; margin: 0;");
    ToolSideContainer->setFixedSize(120, 600);

    QWidget *BackgroundTools = new QWidget();
    BackgroundTools->setFixedSize(QSize(100, 500));
    BackgroundTools->setStyleSheet("background-color: lightgrey;border-radius:20px;");
    ToolSide->addWidget(BackgroundTools);
    ToolSide->setAlignment(BackgroundTools, Qt::AlignTop);
    ToolSide->setSpacing(0);

    maps = new Cartes(this);

    container->addWidget(ToolSideContainer);
    container->addWidget(glWidget);
    container->addWidget(maps);
    container->setAlignment(ToolSideContainer, Qt::AlignVCenter);
    container->setAlignment(maps, Qt::AlignVCenter);

    tool1 = new QPushButton();
    tool1->setFixedSize(QSize(40, 40));
    tool1->setStyleSheet("background-color:grey");
    QIcon icon("./hand.png");
    tool1->setIcon(icon);
    tool1->setIconSize(QSize(30, 30));
    connect(tool1, &QPushButton::clicked, glWidget, &GLWidget::Hand_Tool);
    

    tool2 = new QPushButton();
    tool2->setFixedSize(QSize(40, 40));
    tool2->setStyleSheet("background-color:grey");
    connect(tool2, &QPushButton::clicked, glWidget, &GLWidget::DrawCircle);
  
    tool3 = new QPushButton();
    tool3->setFixedSize(QSize(40, 40));
    tool3->setStyleSheet("background-color:grey");

    tool4 = new QPushButton();
    tool4->setFixedSize(QSize(40, 40));
    tool4->setStyleSheet("background-color:grey");

    QVBoxLayout *ToolsLayout = new QVBoxLayout;
    BackgroundTools->setLayout(ToolsLayout);

    ToolsLayout->addWidget(tool1);
    ToolsLayout->addWidget(tool2);
    ToolsLayout->addWidget(tool3);
    ToolsLayout->addWidget(tool4);

    ToolsLayout->setAlignment(tool1, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool2, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool3, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool4, Qt::AlignHCenter);

    setLayout(mainLayout);

    setWindowTitle(tr("Qt OpenGL"));
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);

    if (e->key() == Qt::Key_W)
    {
        glWidget->wireframe = !glWidget->wireframe;
    }

    if (e->key() == Qt::Key_Plus)
    {
        glWidget->UpdateResolution(glWidget->getResolution() + 1);
    }

    if (e->key() == Qt::Key_Minus)
    {
        glWidget->UpdateResolution(glWidget->getResolution() - 1);
    }

    if(e->key() == Qt::Key_P)
    {
        glWidget->mode_pres = !glWidget->mode_pres;
        glWidget->angle_speed = 0.15;
    }

    if(e->key() == Qt::Key_G)
    {
        glWidget->angle_speed += 0.02;
    }

    if(e->key() == Qt::Key_B)
    {
        glWidget->angle_speed -= 0.02;
    }


}

void Window::initTexture(QPixmap p)
{
    texture->setStyleSheet("background-color: transparent;");
    QPixmap scaledPixmap = p.scaled(texture->size(), Qt::KeepAspectRatio);
    texture->setPixmap(scaledPixmap);
    texture->setAlignment(Qt::AlignCenter);
}

void Window::initRidge(QPixmap p)
{
    cretes->setStyleSheet("background-color: transparent;");
    QPixmap scaledPixmap = p.scaled(cretes->size(), Qt::KeepAspectRatio);
    cretes->setPixmap(scaledPixmap);
    cretes->setAlignment(Qt::AlignCenter);
}

void Window::initRiver(QPixmap p)
{
    rivieres->setStyleSheet("background-color: transparent;");
    QPixmap scaledPixmap = p.scaled(rivieres->size(), Qt::KeepAspectRatio);
    rivieres->setPixmap(scaledPixmap);
    rivieres->setAlignment(Qt::AlignCenter);
}

void Window::TerrainModif(QString imgname)
{
    glWidget->UpdateTerrain(imgname);
}

void Window::BiomeModif(QString imgname)
{
    glWidget->UpdateBiome(imgname);
}

void Window::ChangeFPS(int fps)
{
    FPS->setText(QString("FPS:%1").arg(fps));
}