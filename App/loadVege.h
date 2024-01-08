#ifndef VEGE_H
#define VEGE_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QVector2D>
#include <QOpenGLShaderProgram>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)


class loadVege : protected QOpenGLFunctions
{
public:
    loadVege(){}
    void loadModel(const QString &filePath)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(filePath.toStdString().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            qDebug() << "Erreur lors du chargement du modèle:" << importer.GetErrorString();
            return;
        }

        processNode(scene->mRootNode, scene);
    }


    void processNode(aiNode *node, const aiScene *scene)
    {
        qDebug() << node->mNumMeshes;
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    void processMesh(aiMesh *mesh, const aiScene *scene)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            aiVector3D vertex = mesh->mVertices[i];
            aiVector3D normal = mesh->mNormals[i];
            aiVector3D uv = mesh->mTextureCoords[0][i];
            vertices.append(QVector3D(vertex.x, vertex.y, vertex.z));
            normals.append(QVector3D(normal.x, normal.y, normal.z));
            // qDebug() << (uv.x+1)/2 << " " << (uv.y+1)/2 ;
            uvs.append(QVector2D(uv.x, uv.y));
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            assert(face.mNumIndices == 3);

            // for (unsigned int j = 0; j < face.mNumIndices; ++j)
            // {
            //     unsigned short vertexIndex = face.mIndices[j];
                indices.append(face.mIndices[0]);
                indices.append(face.mIndices[1]);
                indices.append(face.mIndices[2]);
            // }
        }
    }

    QVector<QVector3D> getVertices(){ 
        return vertices;
    }

    void setVertices(QVector<QVector3D> v){ 
        vertices = v;
    }

    QVector<QVector3D> getNormals(){ return normals;}
    QVector<QVector2D> getUVs(){ return uvs;}
    QVector<unsigned short> getIndices(){ return indices;}

    QOpenGLBuffer getVertexBuffer() {return VegeVertexBuffer_;}
    QOpenGLBuffer getIndexBuffer() {return VegeIndicesBuffer_;}

    QMatrix4x4 getModelMatrix(){return vege_model;}
    void setModelMatrix(QMatrix4x4 m){vege_model = m;}

private:
    QVector<QVector3D> vertices; 
    QVector<unsigned short> indices; 
    QVector<QVector3D> normals; 
    QVector<QVector2D> uvs; 
    QOpenGLBuffer VegeVertexBuffer_ = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer VegeIndicesBuffer_ = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    QOpenGLVertexArrayObject Vegevao_;
    QMatrix4x4 vege_model;

};

#endif
