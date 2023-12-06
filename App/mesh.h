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
    Mesh(int mode);
    Mesh(int mode,int res);
    ~Mesh();
    QVector<QVector3D> vertices;
    QVector<short> indices;
    QVector<QVector2D> uvs;
    QVector<QVector3D> normals;
    void CreateFlatTerrain(int taille, int resolution);
    void ModifyTerrain(QVector<int> data);
    int getResolution();
    void setResolution(int r);
    void DrawCircle(float cx, float cy, float r, int num_segments);
    void SmoothMoyenneur(float r, QVector3D c, QVector<float> height, float amplMAX, float amplMIN);
private:
    void createCube();
    void changeResolution();
    void AddRandomNoise(float amplitude);
    bool contain(QVector<unsigned int> const &i_vector, unsigned int element);
    void collect_one_ring(QVector<QVector<unsigned int>> &o_one_ring);
    void computeNormals();

    int Min(QVector<char> data);
    int Max(QVector<char> data);
    int resolution;
};

#endif // MESH_H
