#ifndef FLOOR_H
#define FLOOR_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include "translatable.h"

class Floor : public Translatable
{
public:
    Floor(const QVector3D &npos = QVector3D(0.0f, 0.0f, 0.0f),
          const QVector3D &nrot = QVector3D(0.0f, 0.0f, 0.0f),
          GLfloat angle = 0.0f);
    ~Floor() = default;
    void translate(const QVector3D &pos) override {this->pos = pos;}
    void rotate(GLfloat angle, const QVector3D &rot) override {this->rot = rot; this->angle = angle;}
    void draw(QOpenGLShaderProgram *m_program, bool selectMode) override;
    int getId() override {return id;}
protected:
    QVector3D pos;
    QVector3D rot;
    GLfloat angle;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLTexture *tex;
    QMatrix4x4 model;
};

#endif // FLOOR_H
