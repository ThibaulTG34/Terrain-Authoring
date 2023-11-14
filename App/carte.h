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
class Carte : public QWidget
{
    Q_OBJECT
public:
    explicit Carte(QWidget *parent = nullptr);
    bool mapVisible=true;
    QSize taille_image;
    QLabel *map;
    QVector<char> map_data;
    QPushButton *visible_map; 

signals:



public slots:
    void initMap();
};

#endif // CARTE_H
