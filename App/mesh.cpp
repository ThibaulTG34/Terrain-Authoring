

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

int Mesh::getResolution(){
    return resolution;
}

void Mesh::setResolution(int r){
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

bool Mesh::loadOFF(const std::string &filename)
{
    this->vertices.clear();
    this->indices.clear();
    bool convertToTriangles = true;
    bool randomize = false;

    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << filename << " cannot be opened" << std::endl;
        return false;
    }

    std::string magic_s;

    myfile >> magic_s;

    if (magic_s != "OFF")
    {
        std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
        myfile.close();
        return false;
    }

    int n_vertices, n_indices, dummy_int;
    myfile >> n_vertices >> n_indices >> dummy_int;

    vertices.resize(n_vertices);

    for (int v = 0; v < n_vertices; ++v)
    {
        QVector3D vertex;
        myfile >> vertex[0] >> vertex[1] >> vertex[2];

        vertices[v] = vertex;
    }

    for (int f = 0; f < n_indices; ++f)
    {
        int n_vertices_on_face;
        myfile >> n_vertices_on_face;
        if (n_vertices_on_face == 3)
        {
            unsigned short _v1, _v2, _v3;
            std::vector<unsigned short> _v;
            myfile >> _v1 >> _v2 >> _v3;
            //            _v.push_back( _v1 );
            //            _v.push_back( _v2 );
            //            _v.push_back( _v3 );
            //            indices.push_back( _v );
            indices.push_back(_v1);
            indices.push_back(_v2);
            indices.push_back(_v3);
        }
        else if (n_vertices_on_face > 3)
        {
            std::vector<unsigned short> vhandles;
            vhandles.resize(n_vertices_on_face);
            for (int i = 0; i < n_vertices_on_face; ++i)
                myfile >> vhandles[i];

            if (convertToTriangles)
            {
                unsigned short k = (randomize) ? (rand() % vhandles.size()) : 0;
                for (unsigned short i = 0; i < vhandles.size() - 2; ++i)
                {
                    std::vector<unsigned short> tri(3);
                    tri[0] = vhandles[(k + 0) % vhandles.size()];
                    tri[1] = vhandles[(k + i + 1) % vhandles.size()];
                    tri[2] = vhandles[(k + i + 2) % vhandles.size()];
                    // indices.push_back(tri);
                    indices.push_back(tri[0]);
                    indices.push_back(tri[1]);
                    indices.push_back(tri[2]);
                }
            }
            else
            {
                // indices.push_back(vhandles);
                for (int i = 0; i < vhandles.size(); ++i)
                    indices.push_back(vhandles[i]);
            }
        }
        else
        {
            std::cout << "OFFIO::open error : Face number " << f << " has " << n_vertices_on_face << " vertices" << std::endl;
            myfile.close();
            return false;
        }
    }

    myfile.close();

    return true;
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
            float x = (float)(taille * i) / (float)(resolution - 1) - taille/2.0;
            float y = 0;
            float z = (float)(taille * j) / (float)(resolution - 1) - taille/2.0;

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


