#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>

class Mesh : protected QOpenGLFunctions
{
public:
    Mesh();
    ~Mesh();
    QVector<QVector3D> vertices;
    QVector<short> indices;
    bool loadOFF(const std::string &filename);

private:
    void createCube();
    void CreateFlatTerrain(int nb);
    void changeResolution();
};

#endif // MESH_H
