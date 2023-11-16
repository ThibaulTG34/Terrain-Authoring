#include "cartes.h"
#include <QVBoxLayout>

Cartes::Cartes(QWidget *parent)
    : QWidget{parent}
{

    QVBoxLayout *MapContainerLayout = new QVBoxLayout;
    setLayout(MapContainerLayout);
    this->setFixedSize(QSize(120, 120));
    MapContainerLayout->setSpacing(50);


    QPushButton *import = new QPushButton();
    import->setFixedSize(QSize(100, 40));
    import->setContentsMargins(0, 0, 0, 0);
    import->setStyleSheet("padding: 0 ; margin: 0;");
    QIcon icon("./import.png");
    import->setIcon(icon);
    import->setIconSize(QSize(20, 20));
    connect(import, &QPushButton::clicked, this, &Cartes::initMap);
    MapContainerLayout->addWidget(import);


    QVBoxLayout *mapSide = new QVBoxLayout;
    QWidget *mapSideContainer = new QWidget;
    mapSideContainer->setLayout(mapSide);
    this->setFixedSize(120, 1000);

    MapContainerLayout->addWidget(mapSideContainer);

    gradient = new QLabel(this);
    gradient->setStyleSheet("background-color: black;");
    gradient->setFixedSize(QSize(122, 80));
    mapSide->addWidget(gradient);
    mapSide->setAlignment(gradient, Qt::AlignTop);
    mapSide->setContentsMargins(0, 0, 0, 0);

    texture = new QLabel(this);
    texture->setStyleSheet("background-color: red;");
    texture->setFixedSize(QSize(122, 80));
    mapSide->addWidget(texture);
    mapSide->setAlignment(texture, Qt::AlignTop);
    mapSide->setContentsMargins(0, 0, 0, 0);

    cretes = new QLabel(this);
    cretes->setStyleSheet("background-color: green;");
    cretes->setFixedSize(QSize(122, 80));
    mapSide->addWidget(cretes);
    mapSide->setAlignment(cretes, Qt::AlignTop);
    mapSide->setContentsMargins(0, 0, 0, 0);

    rivieres = new QLabel(this);
    rivieres->setStyleSheet("background-color: blue;");
    rivieres->setFixedSize(QSize(122, 80));
    mapSide->addWidget(rivieres);
    mapSide->setAlignment(rivieres, Qt::AlignTop);
    mapSide->setContentsMargins(0, 0, 0, 0);

}

void Cartes::initMap()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une image", "", "Images (*.png *.jpg *.bmp)");

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        gradient->setStyleSheet("background-color: transparent;");
        gradient->setStyleSheet("background-color: transparent;");
        gradient->setStyleSheet("background-color: transparent;");
        QPixmap scaledPixmap = pixmap.scaled(gradient->size(), Qt::KeepAspectRatio);
        gradient->setPixmap(scaledPixmap);
        gradient->setAlignment(Qt::AlignCenter);
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
        QSize size_img = pixmap.size();
        taille_image = size_img;
        for (size_t i = 0; i < size_img.width(); i++)
        {
            for (size_t j = 0; j < size_img.height(); j++)
            {
                map_data.append(qGray(img.pixel(i, j)));
            }
        }
    }
}
