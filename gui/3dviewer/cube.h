#ifndef CUBE_H
#define CUBE_H
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include "translatable.h"

class Cube : public Translatable
{
public:
    Cube(const QVector3D &npos = QVector3D(0.0f, 0.0f, 0.0f),
         const QVector3D &nrot = QVector3D(0.0f, 0.0f, 0.0f),
         GLfloat angle = 0.0f);
    ~Cube() = default;
    void translate(const QVector3D &pos) override {this->pos = pos;}
    void rotate(GLfloat angle, const QVector3D &rot) override {this->rot = rot; this->angle = angle;}
    void draw(QOpenGLShaderProgram *m_program, bool selectMode) override;
    bool isActive() {return active;}
    int getId() override {return id;}
    void kill() {active = false;}
    const QVector3D &getPos() {return pos;}
    const QVector3D *norms() {return normals;}

protected:
    QVector3D normals[6];
    QOpenGLBuffer vbo;
    QOpenGLTexture *tex;
    QVector3D pos;
    QVector3D rot;
    GLfloat angle;
    QMatrix4x4 model;
    bool active;

};

#endif // CUBE_H
