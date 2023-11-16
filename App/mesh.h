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
    Mesh(int res);
    ~Mesh();
    QVector<QVector3D> vertices;
    QVector<short> indices;
    bool loadOFF(const std::string &filename);
    void CreateFlatTerrain(int nb);
    void ModifyTerrain(QVector<char> data);

private:
    void createCube();
    void changeResolution();
    void AddRandomNoise(float amplitude);
    int Min(QVector<char> data);
    int Max(QVector<char> data);

};

#endif // MESH_H
