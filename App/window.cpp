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
#include <QDebug>
#include <QMenu>

using namespace std;

Window::Window(MainWindow *mw)
    : mainWindow(mw)
{
    mainWindow = mw;

    QWidget *w = new QWidget;
    QHBoxLayout *container = new QHBoxLayout;
    // container->addWidget(alt_min);
    w->setLayout(container);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // ToolSide->addWidget(import);
    mainLayout->addWidget(w);

    glWidget = new GLWidget;
    QObject::connect(glWidget, &GLWidget::UpdateFPS, this, &Window::ChangeFPS);
    QObject::connect(glWidget, &GLWidget::UpdateReso, this, &Window::ChangeReso);

    // QWidget *minimap= new QWidget();
    // minimap->setStyleSheet("background-color: rgba(1,1,,0.2)");

    container->setSpacing(20);

    QVBoxLayout *ToolSide = new QVBoxLayout;
    QWidget *ToolSideContainer = new QWidget;
    ToolSideContainer->setLayout(ToolSide);
    Aide = new QPushButton("HELP");
    Aide->setStyleSheet("font-size: 16pt;font-weight: bold;");
    connect(Aide, &QPushButton::clicked, [=]()
            {
            QMessageBox msgBox;
            msgBox.setTextFormat(Qt::RichText); 

            msgBox.setText("<h3>Indications pour l'application <em>Terrain Authoring</em></h3>\
            <p>Ici vous trouverez toutes les indications nécessaire pour une bonne expérience sur notre application.</p></br>\
            <p>Touches utiles :</p>\
            <p><b>Z,Q,S et D :</b> déplacement du cercle de sélection</p>\
            <p><b>+/- :</b> augmenter/réduire la résolution du terrain</p>\
            <p><b>w :</b> appliquer le mode Wireframe</p>\
            <p><b>clique molette + déplacement de la souris :</b> rotation du terrain</p>\
            <p><b>clique gauche (lorsqu'un outil est activé) :</b> applique l'outil dans la zone de sélection</p>\
            <p><b>shift + clique gauche (seulement si outil des hauteurs activé):</b> réduire les hauteurs dans la zone</p>\
            <p><b>shift + clique gauche (seulement si outil des eaux activé):</b> enlève l'eau dans la zone</p>\
            <p><b>shift + molette :</b> augmenter/réduire le rayon du cercle</p>\
            <p><b>bouton réglages transitions :</b> permet de régler les valeurs des transitions etre chaque texture selon la hauteur pour un biome</p>");

            msgBox.exec(); 
            });

    Aide->setFixedSize(80, 50);
    ToolSide->addWidget(Aide);
    ToolSide->setAlignment(Aide, Qt::AlignTop | Qt::AlignHCenter);

    FPS = new QLabel();
    FPS->setText("FPS: --");
    FPS->setFixedSize(100, 40);
    FPS->setStyleSheet("font-size: 16pt;");

    ToolSide->addWidget(FPS);
    ToolSide->setAlignment(FPS, Qt::AlignTop);
    ToolSideContainer->setStyleSheet("padding: 0 ; margin: 0;");
    ToolSideContainer->setFixedSize(140, 910);

    Reso = new QLabel();

    Reso->setText("Résolution: --");
    Reso->setStyleSheet("font-size: 11pt;");

    ToolSide->addWidget(Reso);
    ToolSide->setAlignment(Reso, Qt::AlignTop);

    QWidget *BackgroundTools = new QWidget();
    BackgroundTools->setFixedSize(QSize(100, 750));
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
    tool1->setStyleSheet("background-color:rgb(180,180,180)");
    QIcon icon("./hand.png");
    tool1->setIcon(icon);
    tool1->setIconSize(QSize(30, 30));
    connect(tool1, &QPushButton::clicked, glWidget, &GLWidget::Hand_Tool);
    connect(tool1, &QPushButton::clicked, [=]()
            { this->setTool_to_Active(1); });

    tool2 = new QPushButton();
    QIcon iconSmooth("./smooth_icon.png");
    tool2->setIcon(iconSmooth);
    tool2->setIconSize(QSize(30, 30));
    tool2->setFixedSize(QSize(40, 40));
    tool2->setStyleSheet("background-color:grey");
    connect(tool2, &QPushButton::clicked, glWidget, &GLWidget::DrawCircle);
    connect(tool2, &QPushButton::clicked, [=]()
            { this->setTool_to_Active(2); });

    tool3 = new QPushButton();
    QIcon iconHeight("./height_icon.png");
    tool3->setIcon(iconHeight);
    tool3->setIconSize(QSize(30, 30));
    tool3->setFixedSize(QSize(40, 40));
    tool3->setStyleSheet("background-color:grey");
    QMenu *subMenuHeight = new QMenu("Sous-menu");
    QAction *action1_h = subMenuHeight->addAction("Gaussienne");
    QAction *action2_h = subMenuHeight->addAction("fonction linéaire");

    QObject::connect(tool3, &QPushButton::clicked, [this, subMenuHeight]()
                     { subMenuHeight->exec(tool3->mapToGlobal(QPoint(0, tool3->height()))); });

    connect(action1_h, &QAction::triggered, this, [this]()
            {
            this->setTool_to_Active(3);
            glWidget->HeightTool(1); });
    connect(action2_h, &QAction::triggered, this, [this]()
            { this->setTool_to_Active(3);
            glWidget->HeightTool(2); });
    // connect(tool3, &QPushButton::clicked, glWidget, &GLWidget::HeightTool);
    // connect(tool3, &QPushButton::clicked, [=]()
    //         {
    //     tool1->setStyleSheet("background-color:grey");
    //     tool2->setStyleSheet("background-color:grey");
    //     tool4->setStyleSheet("background-color:grey");
    //     tool3->setStyleSheet("background-color:rgb(180,180,180)");
    //     tool6->setStyleSheet("background-color:rgb(180,180,180)");
    //     tool5->setStyleSheet("background-color:grey"); });

    tool4 = new QPushButton();
    QIcon iconTree("./tree_icon.png");
    tool4->setIcon(iconTree);
    tool4->setIconSize(QSize(30, 30));
    tool4->setFixedSize(QSize(40, 40));
    tool4->setStyleSheet("background-color:grey");
    connect(tool4, &QPushButton::clicked, glWidget, &GLWidget::Tree_Tool);
    connect(tool4, &QPushButton::clicked, [=]()
            { if (!vegetation_is_set)
                {
                    QMessageBox::critical(this, "Avertissement Vegetation", "Une carte de vegetation est nécéssaire pour utiliser cet outil");
                }
                else
                {this->setTool_to_Active(4);} });

    tool5 = new QPushButton();
    QIcon iconTreeDelete("./tree_icon_delete.png");
    tool5->setIcon(iconTreeDelete);
    tool5->setIconSize(QSize(30, 30));
    tool5->setFixedSize(QSize(40, 40));
    tool5->setStyleSheet("background-color:grey");
    connect(tool5, &QPushButton::clicked, glWidget, &GLWidget::Tree_Tool_Delete);
    connect(tool5, &QPushButton::clicked, [=]()
            {  if (!vegetation_is_set)
                {
                    QMessageBox msgBox(QMessageBox::Critical, "Avertissement Vegetation", "Une carte de vegetation est nécessaire pour utiliser cet outil", QMessageBox::Ok, this);
                    msgBox.exec();
                }
                else
                {
                    this->setTool_to_Active(5);
                } });

    tool6 = new QPushButton();
    QIcon iconEditBiome("./biome_icon.png");
    tool6->setIcon(iconEditBiome);

    // Création du sous-menu
    QMenu *subMenu = new QMenu("Sous-menu");
    QAction *action1 = subMenu->addAction("Biome Désert");
    QAction *action2 = subMenu->addAction("Biome Canyon");
    QAction *action3 = subMenu->addAction("Biome Montagne");
    QAction *action4 = subMenu->addAction("Biome Neige");

    connect(action1, &QAction::triggered, this, [this]()
            { this->setTool_to_Active(6);
            glWidget->setBiomeRef(215);
            glWidget->Biome_Tool(); });
    connect(action2, &QAction::triggered, this, [this]()
            { this->setTool_to_Active(6);
            glWidget->setBiomeRef(87);
            glWidget->Biome_Tool(); });
    connect(action3, &QAction::triggered, this, [this]()
            { this->setTool_to_Active(6);
            glWidget->setBiomeRef(127);
            glWidget->Biome_Tool(); });
    connect(action4, &QAction::triggered, this, [this]()
            { this->setTool_to_Active(6);
            glWidget->setBiomeRef(255);
            glWidget->Biome_Tool(); });

    // Associer le sous-menu au bouton
    // tool6->setMenu(subMenu);

    // Connecter le signal clicked du bouton au slot pour afficher le sous-menu
    QObject::connect(tool6, &QPushButton::clicked, [this, subMenu]()
                     {
        if (maps->biomeIsSet)
        {
            subMenu->exec(tool6->mapToGlobal(QPoint(0, tool6->height())));
        }
        else
        {
            QMessageBox::critical(this, "Avertissement Biome", "Une carte biome est nécéssaire pour les modifier");
        } });

    tool6->setIconSize(QSize(30, 30));
    tool6->setFixedSize(QSize(40, 40));
    tool6->setStyleSheet("background-color:grey");
    // connect(tool6, &QPushButton::clicked, glWidget, &GLWidget::Biome_Tool);

    tool7 = new QPushButton();
    QIcon iconWater("./water_logo.png");
    tool7->setIcon(iconWater);
    tool7->setIconSize(QSize(30, 30));
    tool7->setFixedSize(QSize(40, 40));
    tool7->setStyleSheet("background-color:grey");
    connect(tool7, &QPushButton::clicked, glWidget, [=]()
            {
        if (!water_is_set)
        {
            QMessageBox::critical(this, "Avertissement Eau", "Une carte d'eau est nécéssaire pour utiliser cet outil");
        }
        else
        {
            this->setTool_to_Active(7);
            glWidget->Water_Tool();
        } });

    QVBoxLayout *ToolsLayout = new QVBoxLayout;
    BackgroundTools->setLayout(ToolsLayout);

    ToolsLayout->addWidget(tool1);
    ToolsLayout->addWidget(tool2);
    ToolsLayout->addWidget(tool3);
    ToolsLayout->addWidget(tool4);
    ToolsLayout->addWidget(tool5);
    ToolsLayout->addWidget(tool6);
    ToolsLayout->addWidget(tool7);

    ToolsLayout->setAlignment(tool1, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool2, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool3, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool4, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool5, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool6, Qt::AlignHCenter);
    ToolsLayout->setAlignment(tool7, Qt::AlignHCenter);

    QPushButton *SaveChanges = new QPushButton();
    connect(SaveChanges, &QPushButton::clicked, glWidget, &GLWidget::SaveChanges);

    SaveChanges->setFixedSize(200, 60);
    SaveChanges->setText("Exporter mes modifications");
    mainLayout->addWidget(SaveChanges);
    mainLayout->setAlignment(SaveChanges, Qt::AlignHCenter);

    setLayout(mainLayout);

    setWindowTitle(tr("Qt OpenGL"));

}
void Window::setTool_to_Active(int t)
{
    tool1->setStyleSheet("background-color:grey");
    tool3->setStyleSheet("background-color:grey");
    tool2->setStyleSheet("background-color:grey");
    tool4->setStyleSheet("background-color:grey");
    tool5->setStyleSheet("background-color:grey");
    tool6->setStyleSheet("background-color:grey");
    tool7->setStyleSheet("background-color:grey");

    switch (t)
    {
    case 1:
        tool1->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    case 2:
        tool2->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    case 3:
        tool3->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    case 4:
        tool4->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    case 5:
        tool5->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    case 6:
        tool6->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    case 7:
        tool7->setStyleSheet("background-color:rgb(180,180,180)");
        break;
    default:
        break;
    }
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Plus)
    {
        glWidget->UpdateResolution(glWidget->getResolution() + 1);
    }

    if (e->key() == Qt::Key_Minus)
    {
        glWidget->UpdateResolution(glWidget->getResolution() - 1);
    }

    if (e->key() == Qt::Key_P)
    {
        glWidget->mode_pres = !glWidget->mode_pres;
        glWidget->angle_speed = 0.15;
    }

    if (e->key() == Qt::Key_G)
    {
        glWidget->angle_speed += 0.02;
    }

    if (e->key() == Qt::Key_B)
    {
        glWidget->angle_speed -= 0.02;
    }

    glWidget->keyPressEvent(e);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    glWidget->keyReleaseEvent(event);
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

void Window::WaterModif(QString imgname)
{
    water_is_set = true;
    glWidget->UpdateWater(imgname);
}

void Window::VegeModif(QString imgname)
{
    vegetation_is_set = true;
    glWidget->UpdateVegetation(QImage(imgname));
}

void Window::ChangeFPS(int fps)
{
    FPS->setText(QString("FPS:%1").arg(fps));
}

void Window::ChangeReso(int reso)
{
    Reso->setText(QString("Résolution: %1").arg(reso));
}

void Window::UpdateAmplitudeMax(int v)
{
    // emit AmplitudeMAX(v);
    glWidget->setAmplitudeMAX(float(v) / 10.f);
}

void Window::UpdateAmplitudeMin(int v)
{
    // emit AmplitudeMIN(v);
    glWidget->setAmplitudeMIN(float(v) / 10.f);
}

void Window::UpdateAltMax(float v)
{
    glWidget->setAltMAX(v);
}

void Window::UpdateAltMin(float v)
{
    glWidget->setAltMIN(v);
}

void Window::UpdateDegrePente(int v)
{
    glWidget->setDegrePente(v);
}

void Window::UpdateDensite(int v)
{
    std::cout << "densite : " << v << std::endl;
    glWidget->setDensite(80 - v + 6);
}
