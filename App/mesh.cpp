#include "mesh.h"
#include <qopengl.h>
#include <QOpenGLWidget>
#include <fstream>
#include <iostream>
#include <random>

Mesh::Mesh(int mode )
{
    if (mode == 0)
    {
        resolution = 16;
        CreateFlatTerrain(1, 16);
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
}

void Mesh::SmoothMoyenneur(float r, QVector3D c, QVector<float> height, float amplMAX, float amplMIN)
{
    QVector<QVector<unsigned int>> o_one_ring;
    collect_one_ring(o_one_ring);
    for (size_t i = 0; i < vertices.size(); i++)
    {

        vertices[i].setY(static_cast<float>((height[i] * (amplMAX - amplMIN)) + amplMIN));
        std::cout << (c - vertices[i]).length() << std::endl;

        if ((c.distanceToPoint(vertices[i]) <= r))
        {
            float sumWeights = 0.0f;
            float weightedHeightSum = 0.0f;
            for (int v = 0; v < o_one_ring[i].size(); ++v)
            {
                float distance=(c - vertices[i]).length();
                float weight = exp(-(distance * distance) / (2 * r * r));
                weightedHeightSum += (weight*height[i]);
                sumWeights += weight;
            }

            float newHeight = weightedHeightSum / sumWeights;
            std::cout << newHeight << std::endl;
            vertices[i].setY(newHeight);
        }   
    }
}

void Mesh::createCube()
{

    /*
     *
     * 3_______2
     * |       |
     * |       |
     * 0______1
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

int Mesh::Max(QVector<char> data)
{
    int max = data[0];
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] > max)
            max = data[i];
    }

    return max;
}

int Mesh::Min(QVector<char> data)
{
    int min = data[0];
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] < min)
            min = data[i];
    }

    return min;
}

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