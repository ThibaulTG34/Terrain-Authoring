#ifndef ASSIMP_MODEL_H
#define ASSIMP_MODEL_H

#include <string.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <QOpenGLTexture>

#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QMatrix4x4>

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
// #include <QOpenGLWidget>

using namespace std;

struct Vertex
{
    QVector3D m_pos;
    QVector2D m_tex;
    QVector3D m_normal;

    Vertex() {}

    Vertex(const QVector3D& pos, const QVector2D& tex, const QVector3D& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
    }

    Vertex(const QVector3D& pos, const QVector2D& tex)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = QVector3D(0.0f, 0.0f, 0.0f);
    }
};

// QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class AssimpModel
{
private:
    bool InitFromScene(const aiScene* pScene, const QString& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const QString& Filename);
    QVector<Vertex> Vertices;
    QVector<unsigned int> Indices;
    int indiceReference;

public:
    AssimpModel();
    ~AssimpModel();
    bool LoadModel(QString Filename);
    QVector<Vertex> getVertices();
    void setVertices(QVector<Vertex> v);
    int getIndiceReference();
    void setIndiceReference(int v);
    QVector<unsigned int> getIndices();
    void setIndices(QVector<unsigned int> i);
    QMatrix4x4 getModel_Matrix();
    void setModel_Matrix(QMatrix4x4 m);
    QVector<QOpenGLTexture*> getTextures();
    void setTextures(QVector<QOpenGLTexture*> t);

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(QVector<Vertex> Vertices, QVector<unsigned int> Indices);

        QOpenGLBuffer VegeVertexBuffer_ = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        QOpenGLBuffer VegeIndicesBuffer_ = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        unsigned int getNumIndices();
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };

    QVector<MeshEntry> getM_entries();
    void setM_entries(QVector<MeshEntry> e);

    QVector<MeshEntry> m_Entries;
    QVector<QOpenGLTexture*> m_Textures;
    
    private:
        QMatrix4x4 model_matrix;

};




#endif