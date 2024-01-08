#include <AssimpModel.h>
#include <iostream>

AssimpModel::AssimpModel()
{
    model_matrix.setToIdentity();
}

AssimpModel::~AssimpModel()
{
}

AssimpModel::MeshEntry::MeshEntry()
{
    NumIndices = 0;
}

AssimpModel::MeshEntry::~MeshEntry()
{
}

unsigned int AssimpModel::MeshEntry::getNumIndices()
{
    return NumIndices;
}

void AssimpModel::MeshEntry::Init(QVector<Vertex> Vertices, QVector<unsigned int> Indices)
{
    NumIndices = Indices.size();

    VegeVertexBuffer_.create();
    VegeVertexBuffer_.bind();
    VegeVertexBuffer_.allocate(Vertices.data(), Vertices.size() * sizeof(Vertex));
    VegeVertexBuffer_.release();

    VegeIndicesBuffer_.create();
    VegeIndicesBuffer_.bind();
    VegeIndicesBuffer_.allocate(Indices.data(), NumIndices * sizeof(unsigned int));
    VegeIndicesBuffer_.release();
}

bool AssimpModel::LoadModel(QString Filename)
{
    // Clear();

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene *pScene = Importer.ReadFile(Filename.toStdString().c_str(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs );

    if (pScene)
    {
        Ret = InitFromScene(pScene, Filename);
    }
    else
    {
        printf("Error parsing '%s': '%s'\n", Filename.toStdString().c_str(), Importer.GetErrorString());
    }

    return Ret;
}

QVector<Vertex> AssimpModel::getVertices()
{
    return Vertices;
}

void AssimpModel::setVertices(QVector<Vertex> v)
{
    Vertices=v;
}

int AssimpModel::getIndiceReference()
{
    return indiceReference;
}

void AssimpModel::setIndiceReference(int I)
{
    indiceReference=I;
}

QVector<unsigned int> AssimpModel::getIndices()
{
    return Indices;
}

void AssimpModel::setIndices(QVector<unsigned int> i)
{
    Indices=i;
}

QMatrix4x4 AssimpModel::getModel_Matrix()
{
    return model_matrix;
}

void AssimpModel::setModel_Matrix(QMatrix4x4 m)
{
    model_matrix=m;
}

QVector<QOpenGLTexture*> AssimpModel::getTextures()
{
    return m_Textures;
}

void AssimpModel::setTextures(QVector<QOpenGLTexture *> t)
{
    m_Textures=t;
}

QVector<AssimpModel::MeshEntry> AssimpModel::getM_entries()
{
    return m_Entries;
}

void AssimpModel::setM_entries(QVector<MeshEntry> e)
{
    m_Entries=e;
}

bool AssimpModel::InitFromScene(const aiScene *pScene, const QString &Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++)
    {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    InitMaterials(pScene, Filename);
    return true;
}

void AssimpModel::InitMesh(unsigned int Index, const aiMesh *paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

    aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
    {
        aiVector3D pPos = (paiMesh->mVertices[i]);
        aiVector3D pNormal = (paiMesh->mNormals[i]);
        aiVector3D pTexCoord = paiMesh->HasTextureCoords(0) ? (paiMesh->mTextureCoords[0][i]) : Zero3D;

        Vertex v(QVector3D(pPos.x, pPos.y, pPos.z),
                 QVector2D(pTexCoord.x, pTexCoord.y),
                 QVector3D(pNormal.x, pNormal.y, pNormal.z));

        Vertices.append(v);
    }

    // qDebug() << Vertices.size() ;

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
    {
        const aiFace &Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.append(Face.mIndices[0]);
        Indices.append(Face.mIndices[1]);
        Indices.append(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool AssimpModel::InitMaterials(const aiScene *pScene, const QString &Filename)
{
    bool Ret = true;

    int slashIndex = Filename.lastIndexOf("/");

    QString dir;

    if (slashIndex == -1) {
        dir = ".";
    } else if (slashIndex == 0) {
        dir = "/";
    } else {
        dir = Filename.left(slashIndex);
    }

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    {
        const aiMaterial *pMaterial = pScene->mMaterials[i];
        // m_Textures[i] = NULL;
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                QString FullPath = dir + "/" + Path.data;
                // std::cout << "/* message */" << std::endl;
                // QString FullPath ="./textures/rock_canyon.png";
                
                QImage image_vege = QImage(FullPath);
                m_Textures[i] = new QOpenGLTexture(image_vege);
            }
        }
        if (!m_Textures[i])
        {
            m_Textures[i] = new QOpenGLTexture(QImage("./textures/white.png"));
            // Ret = m_Textures[i]->Load();
        }
    }

    return Ret;
}

// void AssimpModel::Render()
// {
//     glEnableVertexAttribArray(0);
//     glEnableVertexAttribArray(1);
//     glEnableVertexAttribArray(2);

//     for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
//         glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
//         glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

//         const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

//         if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
//             m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
//         }

//         glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
//     }

//     glDisableVertexAttribArray(0);
//     glDisableVertexAttribArray(1);
//     glDisableVertexAttribArray(2);
// }