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

    import = new QPushButton();
    import->setFixedSize(100, 100);
    QIcon icon("./import.png");
    import->setIcon(icon);
    import->setIconSize(QSize(40, 40));

    connect(import, &QPushButton::clicked, this, &Window::getPicture);

    // ToolSide->addWidget(import);
    mainLayout->addWidget(import);
    mainLayout->addWidget(w);

    glWidget = new GLWidget;
    container->setSpacing(20);

    QVBoxLayout *mapSide = new QVBoxLayout;
    QWidget *mapSideContainer = new QWidget;
    mapSideContainer->setLayout(mapSide);
    mapSideContainer->setFixedSize(120, 1000);

    QVBoxLayout *ToolSide = new QVBoxLayout;
    QWidget *ToolSideContainer = new QWidget;
    ToolSideContainer->setLayout(ToolSide);
    ToolSideContainer->setFixedSize(120, 1000);

    container->addWidget(ToolSideContainer);
    container->addWidget(glWidget);
    container->addWidget(mapSideContainer);
    container->setAlignment(ToolSideContainer, Qt::AlignVCenter);
    container->setAlignment(mapSideContainer, Qt::AlignVCenter);

    QWidget *BackgroundTools = new QWidget();
    BackgroundTools->setFixedSize(QSize(100, 840));
    BackgroundTools->setStyleSheet("background-color: lightgrey;border-radius:20px;");
    ToolSide->addWidget(BackgroundTools);
    ToolSide->setAlignment(BackgroundTools, Qt::AlignTop);

    tool1 = new QPushButton();
    tool1->setFixedSize(QSize(40, 40));
    tool1->setStyleSheet("background-color:grey");
    tool2 = new QPushButton();
    tool2->setFixedSize(QSize(40, 40));
    tool2->setStyleSheet("background-color:grey");
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

    gradient = new QLabel(mainWindow);
    texture = new QLabel(mainWindow);
    cretes = new QLabel(mainWindow);
    rivieres = new QLabel(mainWindow);

    gradient->setStyleSheet("background-color: black;");
    gradient->setFixedSize(QSize(120, 80));

    texture->setStyleSheet("background-color: red;");
    texture->setFixedSize(QSize(120, 80));

    cretes->setStyleSheet("background-color: green;");
    cretes->setFixedSize(QSize(120, 80));

    rivieres->setStyleSheet("background-color: blue;");
    rivieres->setFixedSize(QSize(120, 80));

    mapSide->addWidget(gradient);
    mapSide->addWidget(texture);
    mapSide->addWidget(cretes);
    mapSide->addWidget(rivieres);
    mapSide->setAlignment(gradient, Qt::AlignTop);
    mapSide->setAlignment(texture, Qt::AlignTop);
    mapSide->setAlignment(cretes, Qt::AlignTop);
    mapSide->setAlignment(rivieres, Qt::AlignTop);

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

}

void Window::getPicture()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une image", "", "Images (*.png *.jpg *.bmp)");

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        QDialog dialog;
     dialog.setWindowTitle("Saisie d'informations");

        // Layout pour organiser les widgets
        QVBoxLayout layout(&dialog);
        QRadioButton *button1 = new QRadioButton("Gradient", &dialog);
        QRadioButton *button2 = new QRadioButton("Texture", &dialog);
        QRadioButton *button3 = new QRadioButton("Crêtes", &dialog);
        QRadioButton *button4 = new QRadioButton("Rivières", &dialog);
        layout.addWidget(button1);
        layout.addWidget(button2);
        layout.addWidget(button3);
        layout.addWidget(button4);

        QPushButton buttonOK("OK");
        layout.addWidget(&buttonOK);
        // Gestionnaire de signaux pour le bouton "OK"
        QObject::connect(&buttonOK, &QPushButton::clicked, [&]()
                         {
                             if (button1->isChecked())
                             {
                                 initGradient(pixmap);
                             }
                             else if (button2->isChecked())
                             {
                                 initTexture(pixmap);
                             }
                             else if (button3->isChecked())
                             {
                                 initRidge(pixmap);
                             }
                             else if (button4->isChecked())
                             {
                                 initRiver(pixmap);
                             }  
                             dialog.accept(); });

        dialog.exec();
    }
}

void Window::initGradient(QPixmap p)
{
    gradient->setStyleSheet("background-color: transparent;");
    QPixmap scaledPixmap = p.scaled(gradient->size(), Qt::KeepAspectRatio);
    gradient->setPixmap(scaledPixmap);
    gradient->setAlignment(Qt::AlignCenter);
    QImage img = p.toImage().convertToFormat(QImage::Format_Grayscale8);
    QSize size_img = p.size();
    taille_image=size_img;
    glWidget->UpdateResolution(size_img.width());
    // glWidget->object.CreateFlatTerrain(size_img.width());

    for (size_t i = 0; i < size_img.width(); i++)
    {
        for (size_t j = 0; j < size_img.height(); j++)
        {
            gradient_data.append(qGray(img.pixel(i, j)));
        }
    }

    glWidget->UpdateTerrain(gradient_data);
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