#include "mesh.h"
#include <qopengl.h>
#include <QOpenGLWidget>
#include <fstream>
#include <iostream>
#include <random>
#include <QDebug>

Mesh::Mesh(int mode)
{
    if (mode == 0)
    {
        resolution = 64;
        CreateFlatTerrain(1, resolution);
    }
}

Mesh::Mesh(int mode, int res)
{
    if (mode == 0)
    {
        resolution = res;
        CreateFlatTerrain(1, 16);
    }
}

Mesh::~Mesh()
{
}

int Mesh::getResolution()
{
    return resolution;
}

void Mesh::setResolution(int r)
{
    resolution = r;
    // std::cout << resolution << std::endl;
}

float gaussian(float x, float sigma)
{
    return exp(-0.5 * (x * x) / (sigma * sigma));
}

void Mesh::SmoothMoyenneur(float r, QVector3D c, float min, float max, QImage &hmap)
{
    int width = hmap.width() - 1;
    int height = hmap.height() - 1;
    float dist = 0;

    QVector<Pixel> neighbors;

    float smoothedHeightSum = 0.f;
    float weightSum = 0.0;

    for (int i = 0; i < this->vertices.size(); i++)
    {
        dist = norm(c, vertices[i]);

        if (dist <= r)
        {
            QRgb pixelColor_current = hmap.pixel(uvs[i].x() * (hmap.width() - 1), uvs[i].y() * (hmap.height() - 1));
            QRgb pixelColor_after = hmap.pixel(uvs[i + 1].x() * (hmap.width() - 1), uvs[i + 1].y() * (hmap.height() - 1));
            QRgb pixelColor_before = hmap.pixel(uvs[i - 1].x() * (hmap.width() - 1), uvs[i - 1].y() * (hmap.height() - 1));
            int grayValue_curr = qGray(pixelColor_current);
            int grayValue_aft = qGray(pixelColor_after);
            int grayValue_bef = qGray(pixelColor_before);

            float L = 0.5 * (grayValue_aft - grayValue_curr) + 0.5 * (grayValue_bef - grayValue_curr);

            float res = grayValue_curr + 0.5 * L;

            int v = static_cast<int>(res);

            // Pixel p;
            // p.coord = QVector2D(uvs[i].x(), uvs[i].y());
            // p.distance = dist;
            // neighbors.append(p);
            hmap.setPixel(uvs[i].x() * (hmap.width() - 1), uvs[i].y() * (hmap.height() - 1), qRgb(v, v, v));
        }
    }

    // int value = 0;
    // float sumWeights = 0.0f;
    // int max_ = 0;
    // int min_ = hmap.pixel(0, 0);
    // QVector2D coord_min;
    // QVector2D coord_max;
    // for (Pixel neighbor : neighbors)
    // {
    //     int nx = static_cast<int>(neighbor.coord.x() * (width));
    //     int ny = static_cast<int>(neighbor.coord.y() * (height));

    // float weight = exp(-(neighbor.distance * neighbor.distance) / (r * r));
    // QRgb pixelColor = hmap.pixel(nx, ny);
    // int grayValue = qGray(pixelColor);

    // if (max_ < grayValue)
    // {
    //     max_ = grayValue;
    //     coord_max = QVector2D(nx, ny);
    // }
    // if (min_ > grayValue)
    // {
    //     min_ = grayValue;
    //     coord_min = QVector2D(nx, ny);
    // }

    // int v = grayValue * weight;
    // v = sin(v);
    // v *= 255;
    // v = static_cast<int>(v);
    // std::cout << "v : " << v << std::endl;
    // if (v > 255)
    //     v = 255;
    // else if (v < 0)
    //     v = 0;
    // }

    // int diff = max_ - min_;
    // max_ = qGray(hmap.pixel(coord_max.x(), coord_max.y())) - diff;
    // min_ = qGray(hmap.pixel(coord_min.x(), coord_min.y())) + diff;
    // hmap.setPixel(coord_max.x(), coord_max.y(), qRgb(max_, max_, max_));
    // hmap.setPixel(coord_min.x(), coord_min.y(), qRgb(min_, min_, min_));

    hmap.save("./modif.png");
}

void Mesh::GenerateHeight(float r, QVector3D c, QImage &hm, bool shift)
{
    for (size_t i = 0; i < vertices.size(); i++)
    {
        float dist = norm(c, vertices[i]);

        // std::cout << dist << std::endl;

        if (dist <= r)
        {
            float falloff = 1.0 - (dist / r);

            float heightChange = 0.5 * falloff;
            QRgb pixelColor = hm.pixel(uvs[i].x() * (hm.width() - 1), uvs[i].y() * (hm.height() - 1));
            int v = qGray(pixelColor);
            // int v = static_cast<int>(heightChange * 255);
            int hauteur = 20 * (1 - dist / r);

            if (!shift)
                v += hauteur;
            else
                v -= hauteur;

            v = static_cast<int>(v);

            if (v > 255)
                v = 255;
            else if (v < 0)
                v = 0;

            hm.setPixel(uvs[i].x() * (hm.width() - 1), uvs[i].y() * (hm.height() - 1), qRgb(v, v, v));
        }
    }
    hm.save("./newHeight.png");
}

float Mesh::norm(QVector3D v1, QVector3D v2)
{
    return qSqrt(qPow(v1.x() - v2.x(), 2) + qPow(v1.y() - v2.y(), 2) + qPow(v1.z() - v2.z(), 2));
}

void Mesh::averageNeighbors(QVector2D texCoord, QImage &image, float r)
{
    int width = image.width() - 1;
    int height = image.height() - 1;

    // Coordonnées normalisées
    float u = texCoord.x();
    float v = texCoord.y();

    // Convertir les coordonnées normalisées en coordonnées d'image
    int x = static_cast<int>(u * (width));
    int y = static_cast<int>(v * (height));

    // Valeur du pixel d'origine
    float centerValueR = qRed(image.pixel(x, y)) / 255.0f;
    float centerValueG = qGreen(image.pixel(x, y)) / 255.0f;
    float centerValueB = qBlue(image.pixel(x, y)) / 255.0f;

    // Coordonnées des voisins
    QVector<QVector2D> neighbors;
    neighbors.append(QVector2D(u - 1.0f / width, v - 1.0f / height));
    neighbors.append(QVector2D(u, v - 1.0f / height));
    neighbors.append(QVector2D(u + 1.0f / width, v - 1.0f / height));
    neighbors.append(QVector2D(u - 1.0f / width, v));
    neighbors.append(QVector2D(u + 1.0f / width, v));
    neighbors.append(QVector2D(u - 1.0f / width, v + 1.0f / height));
    neighbors.append(QVector2D(u, v + 1.0f / height));
    neighbors.append(QVector2D(u + 1.0f / width, v + 1.0f / height));

    // Moyenne pondérée des valeurs des voisins dans le cercle de rayon r
    float sumWeights = 0.0f;
    float weightedSumR = centerValueR;
    float weightedSumG = centerValueG;
    float weightedSumB = centerValueB;

    for (const QVector2D &neighborCoord : neighbors)
    {
        int nx = static_cast<int>(neighborCoord.x() * (width));
        int ny = static_cast<int>(neighborCoord.y() * (height));

        // Calculer la distance entre le pixel d'origine et le voisin
        float distance = (QVector2D(u - neighborCoord.x(), v - neighborCoord.y()).length());

        if (distance <= r)
        {
            // Si le voisin est dans le cercle de rayon r, ajouter à la moyenne pondérée
            float neighborValueR = qRed(image.pixel(nx, ny)) / 255.0f;
            float neighborValueG = qGreen(image.pixel(nx, ny)) / 255.0f;
            float neighborValueB = qBlue(image.pixel(nx, ny)) / 255.0f;

            // Ajustez la pondération selon vos besoins
            float weight = exp(-(distance * distance) / (2 * r * r));
            sumWeights += weight;
            weightedSumR += weight * neighborValueR;
            weightedSumG += weight * neighborValueG;
            weightedSumB += weight * neighborValueB;
        }
    }

    for (const QVector2D &neighborCoord : neighbors)
    {
        // Convertir les coordonnées normalisées des voisins en coordonnées d'image
        int nx = static_cast<int>(neighborCoord.x() * (width));
        int ny = static_cast<int>(neighborCoord.y() * (height));
        image.setPixel(nx, ny, qRgb(weightedSumR, weightedSumG, weightedSumB));
    }
}

void Mesh::createCube()
{

    /*
     *
     * 3_______2
     * |       |
     * |       |
     * 0_______1
     *
     * */

    /*
     *
     * 7_______6
     * |       |
     * |       |
     * 4_______5
     *
     * */

    this->vertices = {
        // Front face
        QVector3D(-1.0f, -1.0f, 1.0f), // 0
        QVector3D(1.0f, -1.0f, 1.0f),  // 1
        QVector3D(1.0f, 1.0f, 1.0f),   // 2
        QVector3D(-1.0f, 1.0f, 1.0f),  // 3

        // Back face
        QVector3D(-1.0f, -1.0f, -1.0f), // 4
        QVector3D(1.0f, -1.0f, -1.0f),  // 5
        QVector3D(1.0f, 1.0f, -1.0f),   // 6
        QVector3D(-1.0f, 1.0f, -1.0f),  // 7
    };

    this->indices = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        1, 5, 2,
        6, 5, 2,
        4, 0, 7,
        7, 3, 0,
        7, 6, 3,
        6, 2, 3,
        4, 5, 1,
        4, 1, 0};
}

void Mesh::CreateFlatTerrain(int taille, int resolution)
{
    vertices.clear();
    indices.clear();
    normals.clear();
    uvs.clear();

    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            float x = (float)(taille * i) / (float)(resolution - 1) - taille / 2.0;
            float y = 0;
            float z = (float)(taille * j) / (float)(resolution - 1) - taille / 2.0;

            this->vertices.append(QVector3D(x, y, z));

            this->uvs.append(QVector2D(1.0 * ((float)i / (float)resolution), 1.0 - (float)(1.0 * j) / (float)resolution));
        }
    }

    for (int row = 0; row < resolution - 1; row++)
    {
        for (int col = 0; col < resolution - 1; col++)
        {
            short topLeftIndexNum = (short)(row * resolution + col);
            short topRightIndexNum = (short)(row * resolution + col + 1);
            short bottomLeftIndexNum = (short)((row + 1) * resolution + col);
            short bottomRightIndexNum = (short)((row + 1) * resolution + col + 1);

            this->indices.append(topLeftIndexNum);
            this->indices.append(bottomLeftIndexNum);
            this->indices.append(topRightIndexNum);

            this->indices.append(topRightIndexNum);
            this->indices.append(bottomLeftIndexNum);
            this->indices.append(bottomRightIndexNum);
        }
    }

    computeNormals();

    // Initialise();
}

// QVector2D Mesh::Max(QVector<Pixel> data)
// {
//     float max = data[0].value;
//     QVector2D coord_max;
//     for (int i = 0; i < data.size(); i++)
//     {
//         if (data[i].value > max)
//             coord_max = data[i].coord;
//     }
//     return coord_max;
// }

// QVector2D Mesh::Min(QVector<Pixel> data)
// {
//     float min = data[0].value;
//     QVector2D coord_min;
//     for (int i = 0; i < data.size(); i++)
//     {
//         if (data[i].value < min)
//             coord_min = data[i].coord;
//     }
//     return coord_min;
// }

void Mesh::ModifyTerrain(QVector<int> data)
{
    for (int i = 0; i < this->vertices.size(); i++)
    {
        std::cout << data[i] / 255.0f << std::endl;
        this->vertices[i].setY(data[i] / 255.0f);
    }
}

void Mesh::AddRandomNoise(float amplitude)
{
    for (auto &point : this->vertices)
    {

        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<double> distribution(-amplitude, amplitude);

        point.setY(distribution(generator));
    }
}

bool Mesh::contain(QVector<unsigned int> const &i_vector, unsigned int element)
{
    for (unsigned int i = 0; i < i_vector.size(); i++)
    {
        if (i_vector[i] == element)
            return true;
    }
    return false;
}

void Mesh::collect_one_ring(QVector<QVector<unsigned int>> &o_one_ring)
{
    o_one_ring.clear();
    o_one_ring.resize(vertices.size());
    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (j != k)
                {
                    if (!contain(o_one_ring[indices[i + j]], indices[i + k]))
                    {
                        o_one_ring[indices[i + j]].append(indices[i + k]);
                    }
                }
            }
        }
    }
}

void Mesh::computeNormals()
{
    normals.clear();
    normals.resize(vertices.size());

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        QVector3D v0 = vertices[indices[i]];
        QVector3D v1 = vertices[indices[i + 1]];
        QVector3D v2 = vertices[indices[i + 2]];

        QVector3D faceNormal = QVector3D::crossProduct(v1 - v0, v2 - v0);

        triangles_normals.append(faceNormal);

        normals[indices[i]] += faceNormal;
        normals[indices[i + 1]] += faceNormal;
        normals[indices[i + 2]] += faceNormal;
    }

    for (size_t i = 0; i < normals.size(); ++i)
    {
        QVector3D n = normals[i];
        n.normalize();
        normals[i] = n;
    }
}