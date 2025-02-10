#include "chunk.h"
#include <QOpenGLExtraFunctions>

Chunk::Chunk(const QVector2D &npos) {
    pos = npos;
    for(int y = 0; y < 256; y++) {
        for(int x = 0; x < 16; x++) {
            for(int z = 0; z < 16; z++) {
                blocks[x][y][z] = *new Block(QVector3D(x + 16 * pos.x(), y, z + 16 * pos.y()), true);
                if(y > 100) blocks[x][y][z].active = false;
            }
        }
    }
    buildVertexArray();
    vbo.create();
    vbo.bind();
    vbo.allocate(chunkVertices.data(), chunkVertices.size() * sizeof(GLfloat));
    vbo.release();

    tex = new QOpenGLTexture(QImage("../../gold.webp").mirrored());
    tex->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::Repeat);
}

void Chunk::addCubeVertices(int chunkX, int chunkY, int chunkZ, int index) {
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            GLfloat x = cubeVertices[i][j][0] + chunkX;
            GLfloat y = cubeVertices[i][j][1] + chunkY;
            GLfloat z = cubeVertices[i][j][2] + chunkZ;
            GLfloat texX = cubeVertices[i][j][3];
            GLfloat texY = cubeVertices[i][j][4];
            chunkVertices.append(index);
            chunkVertices.append(x);
            chunkVertices.append(y);
            chunkVertices.append(z);
            chunkVertices.append(texX);
            chunkVertices.append(texY);
        }
    }
}

void Chunk::buildVertexArray() {
    for(int y = 0; y < 256; y++) {
        for(int x = 0; x < 16; x++) {
            for(int z = 0; z < 16; z++) {
                if(blocks[x][y][z].active)
                    addCubeVertices(x, y, z, cnt++);
            }
        }
    }
}

void Chunk::destroyBlock(QVector3D pos){
    if(blocks[(int)pos.x()][(int)pos.y()][(int)pos.z()].active) {
    blocks[(int)pos.x()][(int)pos.y()][(int)pos.z()].active = false;
    int index = ((int)pos.x() * 16) + ((int)pos.y() * 256) + (int)pos.z();
    int i = 0;
    for(; chunkVertices.at(i) != index; i += 6);
    chunkVertices.remove(i, 6 * 6 * 6);
    //abuildVertexArray();
    vbo.destroy();
    vbo.create();
    vbo.bind();
    vbo.allocate(chunkVertices.data(), chunkVertices.size() * sizeof(GLfloat));
    vbo.release();
    }
}

void Chunk::createBlock(QVector3D pos) {
    if(!blocks[(int)pos.x()][(int)pos.y()][(int)pos.z()].active) {
        blocks[(int)pos.x()][(int)pos.y()][(int)pos.z()].active = true;
        int index = ((int)pos.x() * 16) + ((int)pos.y() * 256) + (int)pos.z();
        addCubeVertices(pos.x(), pos.y(), pos.z(), index);
        cnt++;
        vbo.destroy();
        vbo.create();
        vbo.bind();
        vbo.allocate(chunkVertices.data(), chunkVertices.size() * sizeof(GLfloat));
        vbo.release();
    }
}

void Chunk::draw(QOpenGLShaderProgram *m_program) {
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    vbo.bind();
    int vertLoc = 0;
    int colorLoc = 1;
    int texLoc = m_program->attributeLocation("texCoord");
    tex->bind(4);
    m_program->enableAttributeArray(vertLoc);
    m_program->setAttributeBuffer(vertLoc, GL_FLOAT, sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    m_program->enableAttributeArray(texLoc);
    m_program->setAttributeBuffer(texLoc, GL_FLOAT, 4 * sizeof(GLfloat), 2, 6 * sizeof(GLfloat));
    int n = m_program->uniformLocation("tex");
    if(n != -1)m_program->setUniformValue("tex", 4);
    model.setToIdentity();
    model.translate(std::round(pos.x()) * 16, 0, std::round(pos.y()) * 16);
    m_program->setUniformValue("model", model);
    std::size_t sz = 16 * 16 * 256;
    f->glDrawArrays(GL_TRIANGLES, 0, 36 * sz);

}
