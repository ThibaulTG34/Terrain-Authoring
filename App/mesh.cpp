#include "mesh.h"
#include <qopengl.h>
#include <QOpenGLWidget>
#include <fstream>
#include <iostream>
#include <random>

Mesh::Mesh()
{
    resolution = 16;
    CreateFlatTerrain(1, 16);
    
   
}

Mesh::Mesh(int res)
{
    resolution = res;
    CreateFlatTerrain(1, 16);
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
    o_one_ring.resize(vertices.size()); // one-ring of each vertex, i.e. a list of vertices with which it shares an edge
    // Parcourir les triangles et ajouter les voisins dans le 1-voisinage
    // Attention verifier que l'indice n'est pas deja present
    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        // Tous les points opposés dans le indices sont reliés
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

QVector<QVector3D> Mesh::LaplaceBeltrami_operator(bool smooth)
{
    QVector<QVector<unsigned int>> o_one_ring;

    QVector<QVector3D> LBv;
    LBv.resize(vertices.size());

    collect_one_ring(o_one_ring);

    for (int i = 0; i < o_one_ring.size(); i++)
    {
        unsigned int v_index = i;

        QVector3D sum(0, 0, 0);
        float w_sum = 0.f;

        for (int j = 0; j < o_one_ring[i].size(); j++)
        {

            unsigned int v_neighbor = o_one_ring[i][j];
            QVector<std::pair<QVector<short>, unsigned int>> trianglesNeighbors;
            trianglesNeighbors = GetNeighborsTriangles(v_index, v_neighbor);

            if (trianglesNeighbors.size() == 2)

            {
                QVector3D edge1_triangle_1 = vertices[v_index] - vertices[trianglesNeighbors[0].second];
                QVector3D edge2_triangle_1 = vertices[v_neighbor] - vertices[trianglesNeighbors[0].second];

                std::cout << "i = " << trianglesNeighbors[0].second << std::endl;

                QVector3D edge1_triangle_2 = vertices[v_index] - vertices[trianglesNeighbors[1].second];

                QVector3D edge2_triangle_2 = vertices[v_index] - vertices[trianglesNeighbors[1].second];

                QVector3D common_edge = vertices[v_index] - vertices[v_neighbor];

                float alpha = QVector3D::dotProduct(edge1_triangle_1, edge2_triangle_1);
                float beta = QVector3D::dotProduct(edge1_triangle_2, edge2_triangle_2);

                float cot_alpha = sin(alpha) / cos(alpha);
                float cot_beta = sin(beta) / cos(beta);

                float w = 0.5f * (alpha + beta);

                sum += w * (vertices[o_one_ring[i][j]] - vertices[i]);

                if (smooth)
                    w_sum += w;
            }
        }

        LBv[i] = smooth ? sum / w_sum : sum;
    }

    return LBv;
}

QVector<std::pair<QVector<short>, unsigned int>> Mesh::GetNeighborsTriangles(unsigned int id1, unsigned int id2)
{
    QVector<std::pair<QVector<short>, unsigned int>> map_;

    for (int i = 0; i < indices.size(); i += 3)
    {
        std::cout << " id : " << id1 << " - " << id2 << std::endl;
        std::cout << " indices : " << indices[i] << " - " << indices[i + 1] << " - " << indices[i + 2] << std::endl;

        std::pair<QVector<short>, unsigned int> pair;
        if ((indices[i] == id1 && indices[i + 1] == id2 || indices[i] == id2 && indices[i + 1] == id1))
        {
            std::cout << " ici : " << indices[i + 2] << std::endl;
            map_.append(std::pair{indices, indices[i + 2]});
        }
        else if ((indices[i] == id1 && indices[i + 2] == id2 || indices[i] == id2 && indices[i + 2] == id1))
        {
            std::cout << " la : " << indices[i + 2] << std::endl;
            map_.append(std::pair{indices, indices[i + 1]});
        }
        else if ((indices[i + 1] == id1 && indices[i + 2] == id2 || indices[i + 1] == id2 && indices[i + 2] == id1))
        {
            std::cout << " bas : " << indices[i + 2] << std::endl;
            map_.append(std::pair{indices, indices[i]});
        }
    }

    return map_;
}

void Mesh::Smooth_LaplaceBeltrami(int _iters)
{
    QVector<QVector3D> vertices_smooth;
    QVector<QVector3D> LBv;
    for (int i = 0; i < _iters; i++)
    {
        vertices_smooth.clear();
        LBv.clear();

        LBv = LaplaceBeltrami_operator(true);

        for (int j = 0; j < LBv.size(); j++)
        {
            vertices_smooth.append(vertices[j] + (0.5f * LBv[j]));
        }
        vertices = vertices_smooth;
    }
}


void Mesh::taubinSmooth(int _iters, float lambda, float mu)
{
    QVector<QVector<unsigned int>> o_one_ring;
    collect_one_ring(o_one_ring);
    QVector<QVector3D> Luv;
    QVector<QVector3D> vertices_smooth;

    QVector<QVector3D> Luv_lambda;
    QVector<QVector3D> vertices_lambda;
    for (int k = 0; k < _iters; k++)
    {
        vertices_smooth.clear();
        Luv.clear();
        Luv_lambda.clear();
        vertices_lambda.clear();

        for (int i = 0; i < vertices.size(); i++)
        {
            QVector3D mean_neighbors(0, 0, 0);
            for (int j = 0; j < o_one_ring[i].size(); j++)
            {
                mean_neighbors += vertices[o_one_ring[i][j]];
            }

            mean_neighbors /= o_one_ring[i].size();
            mean_neighbors -= vertices[i];
            Luv.append(mean_neighbors);
        }

        for (int i = 0; i < Luv.size(); i++)
        {
            vertices_lambda.append(vertices[i] + (lambda * Luv[i]));
        }

        for (int i = 0; i < vertices_lambda.size(); i++)
        {
            QVector3D mean_neighbors(0, 0, 0);
            for (int j = 0; j < o_one_ring[i].size(); j++)
            {
                mean_neighbors += vertices_lambda[o_one_ring[i][j]];
            }

            mean_neighbors /= o_one_ring[i].size();
            mean_neighbors -= vertices_lambda[i];
            Luv_lambda.append(mean_neighbors);
        }

        for (int i = 0; i < Luv_lambda.size(); i++)
        {
            vertices_smooth.append(vertices_lambda[i] + (mu * Luv_lambda[i]));
        }

        vertices = vertices_smooth;
    }
}


QVector3D Mesh::MeanSmooth(float radius, QVector3D center, QVector<int> data)
{
    int count = 0;
    QVector3D mean(0,0,0);

    // std::cout << "/* message */" << std::endl;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].setY(data[i]);

        QVector2D c(center.x(), center.z());
        QVector2D v(vertices[i].x(), vertices[i].z());

        float distance = c.distanceToPoint(v);

        if (distance <= radius)
        {
            mean += vertices[i];
            count ++;
        }
    }

    mean /= float(count);
    // std::cout << "mean : " << mean.x() << " - "<< mean.y() << " - "<< mean.z() << std::endl;
    return mean;
    
}