#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QVector2D>

enum class SMOOTH
{
    TAUBIN,
    MOYENNE
};


class Mesh : protected QOpenGLFunctions
{
public:
    Mesh();
    Mesh(int res);
    ~Mesh();
    QVector<QVector3D> vertices;
    QVector<short> indices;
    QVector<QVector2D> uvs;
    void CreateFlatTerrain(int taille, int resolution);
    void ModifyTerrain(QVector<int> data);
    int getResolution();
    void setResolution(int r);
    void DrawCircle(float cx, float cy, float r, int num_segments);
    void Smooth_LaplaceBeltrami(int _iters);
    void taubinSmooth(int _iters, float lambda, float mu);
    void SmoothTerrain(SMOOTH type);
    QVector3D MeanSmooth(float radius, QVector3D center, QVector<int> data);

private:
    void createCube();
    void changeResolution();
    void AddRandomNoise(float amplitude);
    bool contain(QVector<unsigned int> const &i_vector, unsigned int element);
    void collect_one_ring(QVector<QVector<unsigned int>> &o_one_ring);
    QVector<std::pair<QVector<short>, unsigned int>> GetNeighborsTriangles(unsigned int id1, unsigned int id2);
    QVector<QVector3D> LaplaceBeltrami_operator(bool smooth);

    int Min(QVector<char> data);
    int Max(QVector<char> data);
    int resolution;
};

#endif // MESH_H
