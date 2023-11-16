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

class Cartes : public QWidget
{
    Q_OBJECT
public:
    explicit Cartes(QWidget *parent = nullptr);
    QSize taille_image;
    QLabel *gradient;
    QLabel *texture;
    QLabel *cretes;
    QLabel *rivieres;
    QVector<char> map_data;

signals:



public slots:
    void initMap();
};

#endif // CARTE_H
