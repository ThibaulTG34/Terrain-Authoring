#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QVector2D>
#include <QtMath>

enum class SMOOTH
{
    TAUBIN,
    MOYENNE
};

struct Pixel
{
    QVector2D coord;
    float distance;
};

class Mesh : protected QOpenGLFunctions
{
public:
    Mesh(int mode);
    Mesh(int mode, int res);
    ~Mesh();
    QVector<QVector3D> vertices;
    QVector<short> indices;
    QVector<QVector3D> triangles_normals;
    QVector<QVector2D> uvs;
    QVector<QVector3D> normals;
    void CreateFlatTerrain(int taille, int resolution);
    void ModifyTerrain(QVector<int> data);
    int getResolution();
    void setResolution(int r);
    void DrawCircle(float cx, float cy, float r, int num_segments);
    void SmoothMoyenneur(float r, QVector3D c, float min, float max, QImage &hmap);
    void GenerateHeight(float r, QVector3D c, QImage &hm, bool shift);

private:
    void createCube();
    void changeResolution();
    void AddRandomNoise(float amplitude);
    bool contain(QVector<unsigned int> const &i_vector, unsigned int element);
    void collect_one_ring(QVector<QVector<unsigned int>> &o_one_ring);
    void computeNormals();
    void averageNeighbors(QVector2D texCoord, QImage &image, float r);
    float norm(QVector3D v1, QVector3D v2);

    QVector2D Min(QVector<Pixel> data);
    QVector2D Max(QVector<Pixel> data);
    int resolution;
};

#endif // MESH_H
