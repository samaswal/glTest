#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qopenglwidget.h>
#include <qopenglfunctions.h>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDateTime>
#include <QTime>
#include <cmath>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include "cube.h"
#include "floor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct Entity {
    Entity(QVector3D npos, int nid) : pos(npos), id(nid), isActive(true){};
    QVector3D pos;
    GLint id;
    bool isActive;
};

class Player {
public:
    Player();
    Player(QVector3D pos, QVector3D front, QVector3D up) : pitch(0.0f), yaw(-90.0f), cameraPos(pos), cameraFront(front), worldUp(up){this->updateVectors();}
    QVector3D cameraFront;
    QVector3D cameraUp;
    QVector3D cameraPos;
    QVector3D worldUp;
    QVector3D right;
    GLint pitch = 0;
    GLint yaw = -90;
    void move(std::array<bool, 1024> keys, GLfloat speed);
    void mouseMove(GLint xOffset, GLint yOffset);
    QMatrix4x4 GetViewMatrix();
private:
    void updateVectors();
};

class GLViewScreen : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    GLViewScreen(QWidget *parent = nullptr) : QOpenGLWidget(parent), IBO(QOpenGLBuffer::IndexBuffer),
        VBO(QOpenGLBuffer::VertexBuffer), m_tex(nullptr) {};
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    Player *player;
    QVector<Cube> cubes;
    QVector<Floor> xarta;
    GLfloat wHeight = 0.0f;
    float rate = 0.5f;
    void roll(float dir);
    std::array<bool, 1024> keys;
    void move();
    GLfloat mouseX = 0;
    GLfloat mouseY = 0;
    bool firstMouse;
    QVector<GLint> cubesId;
    bool selectMode;
    void drawObjects();
    void shoot();
    void clean();

protected:
    void drawCube();
    QOpenGLTexture *m_tex;
    QOpenGLTexture *m_tex1;
    QMatrix4x4 modelTransMatrix;
    QMatrix4x4 projection;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_selectProgram;
    QOpenGLBuffer VBO;
    QOpenGLBuffer IBO;
    QOpenGLBuffer VBO1;
    QOpenGLBuffer VBO2;
    QVector<Entity> cubePositions;
    QVector<Translatable *> objects;
    void initShaders();
    void initCube(float width);
    void initTriangle();
    void initSquare();
    void drawMap();
    //void drawObjects();
    void placeObjects();
    void setMouse();
    float grade = 90.0f;

private:
    GLfloat curTime = 0.0f;
    GLfloat lastTime = 0.0f;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QVector<GLfloat> vertices;
    QVector<GLuint> edges;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    GLViewScreen *screen;
private:
    GLfloat rate = 0.5f;
    int timerId = 0;
    void timerEvent(QTimerEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    Ui::MainWindow *ui;
};

struct VertexData {
    VertexData() {};
    VertexData(QVector3D p, QVector3D n) : position(p), normal(n){}
    QVector3D position;
    QVector3D normal;
};

#endif // MAINWINDOW_H
