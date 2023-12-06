#ifndef CARTE_H
#define CARTE_H

#include <QWidget>
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
#include <QLabel>
#include <basics.h>
#include <window.h>
class Window;

class Cartes : public QWidget
{
    Q_OBJECT
public:
    explicit Cartes(QWidget *parent = nullptr);
    QSize taille_image;
    QLabel *gradient;
    QLabel *texture;
    QLabel *vegetation;
    QLabel *rivieres;
    QVector<char> hauteurs;
    QVector<char> biomes;
    QVector<char> vege;
    QVector<char> water;
    Window *w;
    QSlider *amplitudeMAX;
    QSlider *amplitudeMIN;

signals:
    void UpdateSliderMax(float v);
    void UpdateSliderMin(float v);


public slots:
    void initMap();
    void initMapBiome();
    void initMapVege();
    void initMapWater();

private:
    QSlider *createSlider();

};

#endif // CARTE_H
