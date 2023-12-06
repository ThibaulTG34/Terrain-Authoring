#include "cartes.h"
#include <QVBoxLayout>
#include <basics.h>

Cartes::Cartes(QWidget *parent)
    : QWidget{parent}
{
    w = (Window *)parent;
    QVBoxLayout *MapContainerLayout = new QVBoxLayout;
    setLayout(MapContainerLayout);
    this->setFixedSize(QSize(120, 120));
    MapContainerLayout->setSpacing(50);

    QVBoxLayout *mapSide = new QVBoxLayout;
    QWidget *mapSideContainer = new QWidget;
    mapSideContainer->setLayout(mapSide);
    this->setFixedSize(200, 1000);

    MapContainerLayout->addWidget(mapSideContainer);

    /////////////////// Gradient ///////////////////////////
    QVBoxLayout *gradientSide = new QVBoxLayout;
    QWidget *gradientSideContainer = new QWidget;
    gradientSideContainer->setLayout(gradientSide);
    // this->setFixedSize(120, 1000);

    mapSide->addWidget(gradientSideContainer);

    QPushButton *import = new QPushButton();
    import->setFixedSize(QSize(100, 40));
    import->setContentsMargins(0, 0, 0, 0);
    import->setStyleSheet("padding: 0 ; margin: 0;");
    QIcon icon("./import.png");
    import->setIcon(icon);
    import->setIconSize(QSize(20, 20));
    connect(import, &QPushButton::clicked, this, &Cartes::initMap);
    gradientSide->addWidget(import);
    gradientSide->setAlignment(import, Qt::AlignHCenter);

    gradient = new QLabel(this);
    gradient->setStyleSheet("background-color: black;");
    gradient->setFixedSize(QSize(122, 80));
    gradientSide->addWidget(gradient);
    gradientSide->setAlignment(gradient, Qt::AlignTop | Qt::AlignHCenter);
    gradientSide->setContentsMargins(0, 0, 0, 0);

    QLabel *amin = new QLabel(this);
    amin->setText("Amplitude minimale :");
    gradientSide->addWidget(amin);
    amplitudeMIN = createSlider();
    amplitudeMIN->setValue(1);
    gradientSide->addWidget(amplitudeMIN);
    connect(amplitudeMIN, SIGNAL(valueChanged(int)), w, SLOT(UpdateAmplitudeMin(int)));

    QLabel *amax = new QLabel(this);
    amax->setText("Amplitude maximale :");
    gradientSide->addWidget(amax);
    amplitudeMAX = createSlider();
    amplitudeMAX->setValue(20);
    gradientSide->addWidget(amplitudeMAX);
    connect(amplitudeMAX, SIGNAL(valueChanged(int)), w, SLOT(UpdateAmplitudeMax(int)));

    //////////////////////////////////////////////////////////

    ////////////////// Texture--Biome ////////////////////////
    QVBoxLayout *textureSide = new QVBoxLayout;
    QWidget *textureSideContainer = new QWidget;
    textureSideContainer->setLayout(textureSide);
    // this->setFixedSize(120, 1000);

    mapSide->addWidget(textureSideContainer);

    QPushButton *import2 = new QPushButton();
    import2->setFixedSize(QSize(100, 40));
    import2->setContentsMargins(0, 0, 0, 0);
    import2->setStyleSheet("padding: 0 ; margin: 0;");
    import2->setIcon(icon);
    import2->setIconSize(QSize(20, 20));
    connect(import2, &QPushButton::clicked, this, &Cartes::initMapBiome);
    textureSide->addWidget(import2);
    textureSide->setAlignment(import2, Qt::AlignHCenter);

    texture = new QLabel(this);
    texture->setStyleSheet("background-color: red;");
    texture->setFixedSize(QSize(122, 80));
    textureSide->addWidget(texture);
    textureSide->setAlignment(texture, Qt::AlignTop | Qt::AlignHCenter);
    textureSide->setContentsMargins(0, 0, 0, 0);
    ////////////////////////////////////////////////////////////

    ////////////////// Vegetation ////////////////////////
    QVBoxLayout *vegetationSide = new QVBoxLayout;
    QWidget *vegetationSideContainer = new QWidget;
    vegetationSideContainer->setLayout(vegetationSide);

    mapSide->addWidget(vegetationSideContainer);

    QPushButton *import3 = new QPushButton();
    import3->setFixedSize(QSize(100, 40));
    import3->setContentsMargins(0, 0, 0, 0);
    import3->setStyleSheet("padding: 0 ; margin: 0;");
    import3->setIcon(icon);
    import3->setIconSize(QSize(20, 20));
    connect(import3, &QPushButton::clicked, this, &Cartes::initMapVege);
    vegetationSide->addWidget(import3);
    vegetationSide->setAlignment(import3, Qt::AlignHCenter);

    vegetation = new QLabel(this);
    vegetation->setStyleSheet("background-color: green;");
    vegetation->setFixedSize(QSize(122, 80));
    vegetationSide->addWidget(vegetation);
    vegetationSide->setAlignment(vegetation, Qt::AlignTop | Qt::AlignHCenter);
    vegetationSide->setContentsMargins(0, 0, 0, 0);
    ////////////////////////////////////////////////////////////

    ////////////////// Water ////////////////////////
     QVBoxLayout *waterSide = new QVBoxLayout;
    QWidget *waterSideContainer = new QWidget;
    waterSideContainer->setLayout(waterSide);

    mapSide->addWidget(waterSideContainer);

    QPushButton *import4 = new QPushButton();
    import4->setFixedSize(QSize(100, 40));
    import4->setContentsMargins(0, 0, 0, 0);
    import4->setStyleSheet("padding: 0 ; margin: 0;");
    import4->setIcon(icon);
    import4->setIconSize(QSize(20, 20));
    connect(import4, &QPushButton::clicked, this, &Cartes::initMapWater);
    waterSide->addWidget(import4);
    waterSide->setAlignment(import4, Qt::AlignHCenter);

    rivieres = new QLabel(this);
    rivieres->setStyleSheet("background-color: blue;");
    rivieres->setFixedSize(QSize(122, 80));
    waterSide->addWidget(rivieres);
    waterSide->setAlignment(rivieres, Qt::AlignTop | Qt::AlignHCenter);
    waterSide->setContentsMargins(0, 0, 0, 0);

    ////////////////////////////////////////////////////////////

}

QSlider *Cartes::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 20);
    slider->setSingleStep(1);
    slider->setPageStep(20 * 0.05);
    slider->setTickInterval(20 * 0.05);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void Cartes::initMapVege()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une carte", "", "Images (*.png *.jpg *.bmp *.ppm *.pgm)");

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        vegetation->setStyleSheet("background-color: transparent;");
        QPixmap scaledPixmap = pixmap.scaled(vegetation->size(), Qt::KeepAspectRatio);
        vegetation->setPixmap(scaledPixmap);
        vegetation->setAlignment(Qt::AlignCenter);
        QImage img = pixmap.toImage();
        img = img.convertToFormat(QImage::Format_Grayscale8);

        QSize size_img = pixmap.size();
        taille_image = size_img;
        for (size_t i = 0; i < size_img.width(); i++)
        {
            for (size_t j = 0; j < size_img.height(); j++)
            {
                vege.append((char)qGray(img.pixel(i, j)));
            }
        }
    }
}

void Cartes::initMapWater()
{
      QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une carte", "", "Images (*.png *.jpg *.bmp *.ppm *.pgm)");

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        rivieres->setStyleSheet("background-color: transparent;");
        QPixmap scaledPixmap = pixmap.scaled(rivieres->size(), Qt::KeepAspectRatio);
        rivieres->setPixmap(scaledPixmap);
        rivieres->setAlignment(Qt::AlignCenter);
        QImage img = pixmap.toImage();
        img = img.convertToFormat(QImage::Format_Grayscale8);

        QSize size_img = pixmap.size();
        taille_image = size_img;
        for (size_t i = 0; i < size_img.width(); i++)
        {
            for (size_t j = 0; j < size_img.height(); j++)
            {
                water.append((char)qGray(img.pixel(i, j)));
            }
        }
    }
}

void Cartes::initMap()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une carte", "", "Images (*.png *.jpg *.bmp *.ppm *.pgm)");

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        gradient->setStyleSheet("background-color: transparent;");
        QPixmap scaledPixmap = pixmap.scaled(gradient->size(), Qt::KeepAspectRatio);
        gradient->setPixmap(scaledPixmap);
        gradient->setAlignment(Qt::AlignCenter);
        QImage img = pixmap.toImage();
        img = img.convertToFormat(QImage::Format_Grayscale8);

        QSize size_img = pixmap.size();
        taille_image = size_img;
        for (size_t i = 0; i < size_img.width(); i++)
        {
            for (size_t j = 0; j < size_img.height(); j++)
            {
                hauteurs.append((char)qGray(img.pixel(i, j)));
            }
        }
        w->TerrainModif(fileName);
    }
}

void Cartes::initMapBiome()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une carte", "", "Images (*.png *.jpg *.bmp *.ppm *.pgm)");
    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        texture->setStyleSheet("background-color: transparent;");
        QPixmap scaledPixmap = pixmap.scaled(texture->size(), Qt::KeepAspectRatio);
        texture->setPixmap(scaledPixmap);
        texture->setAlignment(Qt::AlignCenter);
        QImage img = pixmap.toImage();
        img = img.convertToFormat(QImage::Format_Grayscale8);

        QSize size_img = pixmap.size();
        taille_image = size_img;
        for (size_t i = 0; i < size_img.width(); i++)
        {
            for (size_t j = 0; j < size_img.height(); j++)
            {
                biomes.append((char)qGray(img.pixel(i, j)));
            }
        }
        w->BiomeModif(fileName);
    }
}

// void Cartes::UpdateSliderMin(float v)
// {
//     w->UpdateAmplitudeMin(v);
// }

// void Cartes::UpdateSliderMax(float v)
// {
//     w->UpdateAmplitudeMax(v);
// }
