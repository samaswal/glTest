#ifndef TRANSLATABLE_H
#define TRANSLATABLE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <atomic>

class Translatable
{
protected:
    static std::atomic<int> next_id;
    int id;
public:
    Translatable() : id(next_id++){};
    virtual void translate(const QVector3D &pos) = 0;
    virtual void rotate(GLfloat angle, const QVector3D &rot) = 0;
    virtual void draw(QOpenGLShaderProgram *m_program, bool selectMode) = 0;
    virtual ~Translatable() = default;
    virtual int getId() = 0;
};

#endif // TRANSLATABLE_H
