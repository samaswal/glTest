#include "floor.h"

Floor::Floor(const QVector3D &npos, const QVector3D &nrot, GLfloat angle) : ibo(QOpenGLBuffer::IndexBuffer){
    this->pos = npos;
    this->rot = nrot;
    this->angle = angle;
    GLfloat vertices[] = {
        0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        -0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(vertices, sizeof(vertices));
    vbo.release();

    ibo.create();
    ibo.bind();
    ibo.allocate(indices, sizeof(indices));
    ibo.release();

    tex = new QOpenGLTexture(QImage("../../ground.jpg").mirrored());
    tex->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::Repeat);
}

void Floor::draw(QOpenGLShaderProgram *m_program, bool selectMode) {
    vbo.bind();
    ibo.bind();
    int vertLoc = 0;
    //if(!selectMode) {

        m_program->enableAttributeArray(vertLoc);
        int colorLoc = 1;
        int texLoc = m_program->attributeLocation("texCoord");
        tex->bind(4);
        m_program->enableAttributeArray(vertLoc);
        m_program->setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
        m_program->enableAttributeArray(texLoc);
        m_program->setAttributeBuffer(texLoc, GL_FLOAT, 6 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
        m_program->setUniformValue("tex", 4);
        model.setToIdentity();
        model.translate(pos);
        model.rotate(angle, rot);
        m_program->setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //}
}
