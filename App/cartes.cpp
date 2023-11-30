#include "cartes.h"
#include <QVBoxLayout>
#include <basics.h>

Cartes::Cartes(QWidget *parent)
    : QWidget{parent}
{
    w=(Window*) parent;  
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
    gradientSide->addWidget(amplitudeMIN);
    connect(amplitudeMIN, SIGNAL(valueChanged(int)), w, SLOT(UpdateAmplitudeMin(int)));


    QLabel *amax = new QLabel(this);
    amax->setText("Amplitude maximale :");
    gradientSide->addWidget(amax); 
    amplitudeMAX = createSlider();
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
    QIcon icon2("./import.png");
    import2->setIcon(icon2);
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


    cretes = new QLabel(this);
    cretes->setStyleSheet("background-color: green;");
    cretes->setFixedSize(QSize(122, 80));
    mapSide->addWidget(cretes);
    mapSide->setAlignment(cretes, Qt::AlignTop | Qt::AlignHCenter);
    mapSide->setContentsMargins(0, 0, 0, 0);

    rivieres = new QLabel(this);
    rivieres->setStyleSheet("background-color: blue;");
    rivieres->setFixedSize(QSize(122, 80));
    mapSide->addWidget(rivieres);
    mapSide->setAlignment(rivieres, Qt::AlignTop | Qt::AlignHCenter);
    mapSide->setContentsMargins(0, 0, 0, 0);

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
                // localisation_constraint.append(qRed(img.pixel(i, j)));
                // values_constraint.append(qGreen(img.pixel(i, j)));
                // laplacien.append(qBlue(img.pixel(i, j)));
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


