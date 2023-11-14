

#include "mesh.h"
#include <qopengl.h>
#include <QOpenGLWidget>
#include <fstream>
#include <iostream>

Mesh::Mesh()
{
    // createCube();
    CreateFlatTerrain(4);
}

Mesh::~Mesh()
{
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

void Mesh::CreateFlatTerrain(int nb)
{
    vertices.clear();
    indices.clear();

    for (float i = 0; i < (float)nb; i++)
    {
        for (float j = 0; j < (float)nb; j++)
        {
            float x = 2 * i / (nb - 1) - 1.f;
            float y = 0;
            float z = 2 * j / (nb - 1) - 1.f;

            this->vertices.append(QVector3D(x, y, z));
        }
    }

    for (int row = 0; row < nb - 1; row++)
    {
        for (int col = 0; col < nb - 1; col++)
        {
            short topLeftIndexNum = (short)(row * nb + col);
            short topRightIndexNum = (short)(row * nb + col + 1);
            short bottomLeftIndexNum = (short)((row + 1) * nb + col);
            short bottomRightIndexNum = (short)((row + 1) * nb + col + 1);

            this->indices.append(topLeftIndexNum);
            this->indices.append(bottomLeftIndexNum);
            this->indices.append(topRightIndexNum);

            this->indices.append(topRightIndexNum);
            this->indices.append(bottomLeftIndexNum);
            this->indices.append(bottomRightIndexNum);
        }
    }
}
