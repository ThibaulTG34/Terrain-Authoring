#include "carte.h"

Carte::Carte(QWidget *parent)
    : QWidget{parent}
{

    QVBoxLayout *MapContainerLayout = new QVBoxLayout;
    setLayout(MapContainerLayout);
    this->setFixedSize(QSize(120, 120));

    map(this);
    map->setStyleSheet("background-color: black;");
    map->setFixedSize(QSize(122, 80));
    MapContainerLayout->addWidget(map);
    MapContainerLayout->setAlignment(map, Qt::AlignTop);
    MapContainerLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *MapOptionsContainer = new QWidget();

    QHBoxLayout *MapOptionsContainerLayout = new QHBoxLayout;
    MapOptionsContainerLayout->setContentsMargins(0, 0, 0, 0);
    MapOptionsContainerLayout->setSpacing(0);
    MapOptionsContainer->setLayout(MapOptionsContainerLayout);
    MapContainerLayout->setAlignment(MapOptionsContainer, Qt::AlignBottom | Qt::AlignLeft);

    MapContainerLayout->addWidget(MapOptionsContainer);
    QPushButton *import = new QPushButton();
    import->setFixedSize(QSize(55, 40));
    import->setContentsMargins(0, 0, 0, 0);
    import->setStyleSheet("padding: 0; margin: 0;");
    QIcon icon("./import.png");
    import->setIcon(icon);
    import->setIconSize(QSize(20, 20));
    connect(import, &QPushButton::clicked, this, &Carte::initMap);
    MapOptionsContainerLayout->addWidget(import);

    visible_map = new QPushButton();
    visible_map->setFixedSize(QSize(52, 40));
    MapOptionsContainerLayout->setAlignment(visible_map, Qt::AlignLeft);

    QIcon icon1("./eye.png");
    visible_map->setContentsMargins(0, 0, 0, 0);
    visible_map->setIcon(icon1);
    visible_map->setIconSize(QSize(30, 30));
    connect(visible_map, &QPushButton::clicked, this, [&]()
            {
        if(mapVisible){
             QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(map);
            opacityEffect->setOpacity(0.4);  // Définir l'opacité, 1.0 est complètement opaque, 0.0 est complètement transparent
            map->setGraphicsEffect(opacityEffect);
            visible_map->setIcon(QIcon("./eyebarre.png"));
            mapVisible=false;
        }else{
             QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(map);
            opacityEffect->setOpacity(1);  // Définir l'opacité, 1.0 est complètement opaque, 0.0 est complètement transparent
            map->setGraphicsEffect(opacityEffect);
            visible_map->setIcon(QIcon("./eye.png"));
            mapVisible=true;


        } });
    MapOptionsContainerLayout->addWidget(visible_map);


}



void Carte::initMap()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Sélectionner une image", "", "Images (*.png *.jpg *.bmp)");

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        map->setStyleSheet("background-color: transparent;");
        QPixmap scaledPixmap = pixmap.scaled(map->size(), Qt::KeepAspectRatio);
        map->setPixmap(scaledPixmap);
        map->setAlignment(Qt::AlignCenter);
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
