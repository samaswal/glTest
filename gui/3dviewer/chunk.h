#ifndef CHUNK_H
#define CHUNK_H
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>

static const GLfloat cubeVertices[6][6][5] = {
    {
        {-0.5f, -0.5f, -0.5f,  0.0f, 0.0f},
        {0.5f, -0.5f, -0.5f,  1.0f, 0.0f},
        {0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        {0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f, 1.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f, 0.0f}
    },
    {
        {-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        {0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
        {0.5f,  0.5f,  0.5f,  1.0f, 1.0f},
        {0.5f,  0.5f,  0.5f,  1.0f, 1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f, 1.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, 0.0f}
    },
    {
        {-0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        {-0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        {-0.5f,  0.5f,  0.5f,  1.0f, 0.0f}
    },
    {
        {0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        {0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        {0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        {0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        {0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        {0.5f,  0.5f,  0.5f,  1.0f, 0.0f}
    },
    {
        {-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        {0.5f, -0.5f, -0.5f,  1.0f, 1.0f},
        {0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
        {0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f, 1.0f}
    },
    {
        {-0.5f,  0.5f, -0.5f,  0.0f, 1.0f},
        {0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        {0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        {0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f, 0.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f, 1.0f}
    }
};

struct Block {
    Block(QVector3D npos = QVector3D(), bool nactive = false) : posGlobal(npos), active(nactive){};
    QVector3D posGlobal;

    bool active = false;
};

class Chunk
{
private:
    QVector2D pos;
    QMatrix4x4 model;
    QOpenGLBuffer vbo;
    QOpenGLTexture *tex;
    QVector<GLfloat> chunkVertices;
    void removeVertex(QVector3D pos);
    int cnt = 0;
public:
    Block blocks[16][256][16];
    void buildVertexArray();
    void addCubeVertices(int x, int y, int z, int index);
    Chunk(const QVector2D &npos = QVector2D(0.0f, 0.0f));
    void draw(QOpenGLShaderProgram *m_program);
    const QVector2D &coords() {return pos;}
    void destroyBlock(QVector3D pos);
    void createBlock(QVector3D pos);
};

#endif // CHUNK_H
