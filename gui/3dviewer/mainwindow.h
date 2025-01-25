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

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Player {
public:
    Player();
    Player(QVector3D pos, QVector3D front, QVector3D up) : cameraPos(pos), cameraFront(front), worldUp(up){updateVectors();}
    QVector3D cameraFront;
    QVector3D cameraUp;
    QVector3D cameraPos;
    QVector3D worldUp;
    QVector3D right;
    GLint mouseX = 400;
    GLint mouseY = 300;
    GLfloat pitch = 0.0f;
    GLfloat yaw = -90.0f;
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
    float rate = 0.5f;
    void roll(float dir);
    QVector3D *cameraFront;
    QVector3D *cameraPos;
    QVector3D *cameraUp;
    QVector3D direction;
    std::array<bool, 1024> keys;
    void move();
    GLint mouseX = 400;
    GLint mouseY = 300;
    bool firstMouse;

protected:
    void drawCube();
    QOpenGLTexture *m_tex;
    QOpenGLTexture *m_tex1;
    QMatrix4x4 modelTransMatrix;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_program2;
    QOpenGLBuffer VBO;
    QOpenGLBuffer m_pixBuffer;
    QOpenGLBuffer IBO;
    QOpenGLBuffer VBO1;
    QOpenGLBuffer VBO2;
    QVector<QVector3D> cubePositions;
    void initShaders();
    void initCube(float width);
    void initTriangle();
    void initSquare();
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
    void leaveEvent(QEvent *e) override;
    Ui::MainWindow *ui;
};

struct VertexData {
    VertexData() {};
    VertexData(QVector3D p, QVector3D n) : position(p), normal(n){}
    QVector3D position;
    QVector3D normal;
};

#endif // MAINWINDOW_H
