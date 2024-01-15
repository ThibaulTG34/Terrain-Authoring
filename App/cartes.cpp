#include "cartes.h"
#include <QVBoxLayout>
#include <basics.h>
#include <QMessageBox>

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

    // QDialog pour regler les valeurs des transitions pour les hauteurs
    QPushButton *reglages = new QPushButton("réglages transitions");
    gradientSide->addWidget(reglages);
    dialog.setWindowTitle("Réglages des transitions");
    connect(reglages, &QPushButton::clicked, this, &Cartes::OuvrirReglages);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label1 = new QLabel("1er transition (min):");
    alt_min = new QSlider(Qt::Horizontal);
    alt_min->setRange(0, 20);
    alt_min->setValue(0);
    // alt_min->setPageStep(20 * 0.05);
    alt_min->setTickInterval(1);

    QLabel *label2 = new QLabel("2nd transition (max):");
    alt_max = new QSlider(Qt::Horizontal);
    alt_max->setRange(0, 20);
    alt_max->setValue(0);
    // alt_max->setPageStep(20 * 0.005);
    alt_max->setTickInterval(1);

    layout->addWidget(label1);
    layout->addWidget(alt_min);
    layout->addWidget(label2);
    layout->addWidget(alt_max);
    connect(alt_min, &QSlider::valueChanged, this, &Cartes::HandleAltMinValueChanged);
    connect(alt_max, &QSlider::valueChanged, this, &Cartes::HandleAltMinValueChanged);

    dialog.setLayout(layout);
    ///////////////////////////////////////////////////////////

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

    QHBoxLayout *encadre_info_layout = new QHBoxLayout;
    QWidget *encadre_infos = new QWidget;
    encadre_infos->setLayout(encadre_info_layout);
    vegetationSide->addWidget(encadre_infos);

    QPushButton *infoVege = new QPushButton("?");
    infoVege->setFixedSize(QSize(25, 25));
    encadre_info_layout->setAlignment(infoVege, Qt::AlignHCenter);
    encadre_info_layout->addWidget(infoVege);
    connect(infoVege, &QPushButton::clicked, this, [=]()
            { 
                QPoint boutonGlobal = infoVege->mapToGlobal(QPoint(0, 0));
                QMessageBox msgBox;
                msgBox.setFixedSize(100,100);
                msgBox.setText("Avertissement végétation");
                msgBox.setInformativeText("La végétation apparaît seulement si le terrain est plus ou moins plat.\n\nCe paramètre peut être géré avec le slider ci-dessous ou en lissant le terrain");
                msgBox.move(w->x(),w->y());
                msgBox.exec(); });

    QLabel *degrepente = new QLabel(this);
    degrepente->setText("Degré de pente :");
    encadre_info_layout->addWidget(degrepente);

    QSlider *degrePente = new QSlider(Qt::Horizontal);
    degrePente->setRange(0, 40);
    degrePente->setSingleStep(2);
    degrePente->setPageStep(2);
    degrePente->setTickInterval(2);
    degrePente->setTickPosition(QSlider::TicksRight);

    degrePente->setValue(0);
    vegetationSide->addWidget(degrePente);
    connect(degrePente, SIGNAL(valueChanged(int)), w, SLOT(UpdateDegrePente(int)));
    vegetationSide->setAlignment(degrePente, Qt::AlignTop);
    vegetationSide->setAlignment(degrepente, Qt::AlignTop | Qt::AlignHCenter);

     QLabel *densite = new QLabel(this);
    densite->setText("Densité de végétation :");
    vegetationSide->addWidget(densite);

    
    QSlider *densiteSlider = new QSlider(Qt::Horizontal);
    densiteSlider->setRange(6, 80);
    densiteSlider->setSingleStep(4);
    densiteSlider->setTickInterval(4);
    densiteSlider->setTickPosition(QSlider::TicksRight);

    densiteSlider->setValue(6);
    densiteSlider->setFixedSize(160,25);
    vegetationSide->addWidget(densiteSlider);
    connect(densiteSlider, SIGNAL(valueChanged(int)), w, SLOT(UpdateDensite(int)));
    vegetationSide->setAlignment(densiteSlider, Qt::AlignTop);
    vegetationSide->setAlignment(densiteSlider, Qt::AlignTop | Qt::AlignHCenter);

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

void Cartes::HandleAltMinValueChanged(int value)
{
    if (alt_min->value() >= alt_max->value())
    {
        if (alt_max->value() == 20)
        {
            float tmp = alt_min->value();
            alt_min->setValue(alt_max->value()-1);
            alt_max->setValue(tmp);
        }
        else
        {
            float tmp = alt_min->value();
            alt_min->setValue(alt_max->value());
            alt_max->setValue(tmp+1);
        }
    }
    w->UpdateAltMin((float)alt_min->value()/10.0f);
    w->UpdateAltMax((float)alt_max->value()/10.0f);
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
    if (biomeIsSet)
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
        w->VegeModif(fileName);
    }
    else
    {
        QMessageBox::information(this, "Avertissement végétation", "Une carte biome est nécéssaire pour modifier la végétation");
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
        w->WaterModif(fileName);
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
        biomeIsSet = true;
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

void Cartes::OuvrirReglages()
{
    dialog.exec();
    // std::cout << "/* message */" << std::endl;
}

// void Cartes::UpdateSliderMin(float v)
// {
//     w->UpdateAmplitudeMin(v);
// }

// void Cartes::UpdateSliderMax(float v)
// {
//     w->UpdateAmplitudeMax(v);
// }
