#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QVector2D>

class Mesh : protected QOpenGLFunctions
{
public:
    Mesh();
    Mesh(int res);
    ~Mesh();
    QVector<QVector3D> vertices;
    QVector<short> indices;
    QVector<QVector2D> uvs;
    bool loadOFF(const std::string &filename);
    void CreateFlatTerrain(int taille, int resolution);
    void ModifyTerrain(QVector<int> data);
    int getResolution();
    void setResolution(int r);
    void DrawCircle(float cx, float cy, float r, int num_segments);


private:
    void createCube();
    void changeResolution();
    void AddRandomNoise(float amplitude);
    int Min(QVector<char> data);
    int Max(QVector<char> data);
    int resolution;
};

#endif // MESH_H
