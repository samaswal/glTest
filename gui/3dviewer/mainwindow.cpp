#include "mainwindow.h"
#include "ui_mainwindow.h"

void GLViewScreen::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    initShaders();
    initCube(1);
    view.setToIdentity();
    projection.setToIdentity();
    cameraPos = new QVector3D(0.0f, 0.0f, 3.0f);
    cameraFront = new QVector3D(0.0f, 0.0f, -1.0f);
    cameraUp = new QVector3D(0.0f, 1.0f, 0.0f);
    //keys.fill(false);
    lastTime = QTime::currentTime().msecsSinceStartOfDay();
    direction.setX(0);
    setMouseTracking(true);

}

void GLViewScreen::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    //view.translate(QVector3D(0.0f, 0.0f, 0.0f));
    projection.perspective(80.0f, static_cast<GLfloat>((w) / (h)), 0.1f, 100.0f);
}

void GLViewScreen::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    move();
    QMatrix4x4 vi;
    vi.lookAt(*cameraPos, *cameraPos + *cameraFront, *cameraUp);
    int vertLoc = 0;
    int colorLoc = 1;
    int texLoc = m_program.attributeLocation("texCoord");
        //m_program.attributeLocation("qt_Vertex");
    int offset = 0;
    m_tex->bind(4);
    m_tex1->bind(5);
    VBO.bind();
    //IBO.bind();
    m_program.bind();
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));

    m_program.enableAttributeArray(texLoc);
    m_program.setAttributeBuffer(texLoc, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    //m_program.setUniformValue("trans", modelTransMatrix);
    m_program.setUniformValue("tex", 4);
    m_program.setUniformValue("tex1", 5);
    m_program.setUniformValue("rate", rate);
    m_program.setUniformValue("view", vi);
    m_program.setUniformValue("projection", projection);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for(int i = 0; i < 10; i++) {
        QMatrix4x4 model;
        model.setToIdentity();
        model.translate(cubePositions[i]);
        GLfloat angle = 1.0f;
        if(i % 3 == 0) {
            angle = (grade / 10);

        }
        else angle = i * 20;
        grade++;
        model.rotate(angle, QVector3D(0.3f, 0.6f, 1.0f));
        m_program.setUniformValue("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void GLViewScreen::initTriangle() {
    GLfloat vertices[] = {
                          -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
                          0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
                          0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f
    };
    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));
    VBO.release();
}

void GLViewScreen::initSquare() {
    GLfloat vertices[] = {
        0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   0.8f, 0.8f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.8f, 0.2f,
        -0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,   0.2f, 0.2f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.2f, 0.8f
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));
    VBO.release();

    IBO.create();
    IBO.bind();
    IBO.allocate(indices, sizeof(indices));
    IBO.release();

    m_tex = new QOpenGLTexture(QImage("../../ground.jpg").mirrored());
    m_tex->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    m_tex->setMagnificationFilter(QOpenGLTexture::Linear);
    m_tex->setWrapMode(QOpenGLTexture::Repeat);

    m_tex1 = new QOpenGLTexture(QImage("../../buddhist.png").mirrored());
    m_tex1->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    m_tex1->setMagnificationFilter(QOpenGLTexture::Linear);
    m_tex1->setWrapMode(QOpenGLTexture::Repeat);

}


void GLViewScreen::initShaders() {

    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "../../w_shader.vsh")) {
        qDebug() << "vertex add fail" << Qt::endl;
        close();
    }

    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "../../fshader.fsh")) { close(); }

    if(!m_program.link()) {
        close();
    }
}

void GLViewScreen::initCube(float width) {
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    cubePositions.append(QVector3D(0.0f,  0.0f,  0.0f));
    cubePositions.append(QVector3D(0.4f,  4.0f,  -9.0f));
    cubePositions.append(QVector3D(-3.0f,  2.5f,  -7.0f));
    cubePositions.append(QVector3D(-4.0f,  -3.0f,  -12.0f));
    cubePositions.append(QVector3D(-2.0f,  5.0f,  8.5f));
    cubePositions.append(QVector3D(3.0f,  2.0f,  -4.0f));
    cubePositions.append(QVector3D(3.0f,  -1.0f,  -7.5f));
    cubePositions.append(QVector3D(-1.4f,  -3.0f,  -19.0f));
    cubePositions.append(QVector3D(-1.0f,  -6.0f,  -20.0f));
    cubePositions.append(QVector3D(2.0f,  -2.0f,  -6.0f));

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));
    VBO.release();

    m_tex = new QOpenGLTexture(QImage("../../ground.jpg").mirrored());
    m_tex->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    m_tex->setMagnificationFilter(QOpenGLTexture::Linear);
    m_tex->setWrapMode(QOpenGLTexture::Repeat);

    m_tex1 = new QOpenGLTexture(QImage("../../buddhist.png").mirrored());
    m_tex1->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    m_tex1->setMagnificationFilter(QOpenGLTexture::Linear);
    m_tex1->setWrapMode(QOpenGLTexture::Repeat);
}

void GLViewScreen::roll(float dir) {
     grade += dir;
     modelTransMatrix.setToIdentity();
     //modelTransMatrix.rotate(-55.0f, QVector3D(1.0f, 0.0f, 0.0f));
     modelTransMatrix.rotate(grade, QVector3D(-1.0f, 0.5f, 0.0f));
}

void GLViewScreen::move() {
    GLfloat speed = 0.01f;
    curTime = QTime::currentTime().msecsSinceStartOfDay();
    speed *= (curTime - lastTime);
    lastTime = curTime;
    if(keys[Qt::Key_W]) {
        *cameraPos += *cameraFront * speed;
    }
    if(keys[Qt::Key_S]) {
        *cameraPos -= *cameraFront * speed;
    }
    if(keys[Qt::Key_D]) {
        *cameraPos += QVector3D::crossProduct(*cameraFront, *cameraUp).normalized() * speed;
    }
    if(keys[Qt::Key_A]) {
        *cameraPos -= QVector3D::crossProduct(*cameraFront, *cameraUp).normalized() * speed;
    }
    //keys.fill(false);

}

/*
void GLViewScreen::initializeGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);

    initShaders();
    initCube(1.0f);

}

 void GLViewScreen::resizeGL(int w, int h) {
     float aspect = w / (float) h;
     m_projectionMatrix.setToIdentity();
     m_projectionMatrix.perspective(45, aspect, 0.1f, 10.0f);
}

void GLViewScreen::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 m_modelView;
    m_modelView.setToIdentity();
    m_modelView.translate(0.0, 0.0, -5.0);

    m_program.bind();

    m_program.setUniformValue("qt_ModelViewProjectionMatrix;", m_projectionMatrix * m_modelView);
    m_program.setUniformValue("qt_Texture0", 0);

    m_arrayBuffer.bind();

    int offset = 0; // Белый цвет линий

    int vertLoc = m_program.attributeLocation("qt_Vertex");
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texLoc = m_program.attributeLocation("qt_MultiTexCoord0");

    f->glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
}

void GLViewScreen::initShaders() {

    if(!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                      "#version 330 core\n"
                                      "layout(location = 0) in vec3 vertexPosition;\n"
                                      "uniform mat4 modelViewProjection;\n"
                                      "void main() {\n"
                                      "   gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);\n"
                                      "}"
                                           )) close();

    if(!m_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                      "#version 330 core\n"
                                      "out vec4 FragColor;\n"
                                      "void main() {\n"
                                      "   FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n" // Белый цвет линий
                                      "}"
                                           )) close();

    if(!m_program.link())
        close();
}

void GLViewScreen::initCube(float width) {
    float width_div_2 = width / 2.0f;
    QVector<VertexData> verteces;
    verteces.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector3D(0.0, 0.0, 1.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector3D(0.0, 0.0, 1.0)));
    verteces.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector3D(0.0, 0.0, 1.0)));
    verteces.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector3D(0.0, 0.0, 1.0)));

    verteces.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector3D(1.0, 0.0, 0.0)));
    verteces.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector3D(1.0, 0.0, 0.0)));
    verteces.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector3D(1.0, 0.0, 0.0)));
    verteces.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector3D(1.0, 0.0, 0.0)));

    verteces.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector3D(0.0, 1.0, 0.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector3D(0.0, 1.0, 0.0)));
    verteces.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector3D(0.0, 1.0, 0.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector3D(0.0, 1.0, 0.0)));

    verteces.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector3D(0.0, 0.0, -1.0)));
    verteces.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector3D(0.0, 0.0, -1.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector3D(0.0, 0.0, -1.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector3D(0.0, 0.0, -1.0)));

    verteces.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector3D(-1.0, 0.0, 0.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector3D(-1.0, 0.0, 0.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector3D(-1.0, 0.0, 0.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector3D(-1.0, 0.0, 0.0)));

    verteces.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector3D(0.0, -1.0, 0.0)));
    verteces.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector3D(0.0, -1.0, 0.0)));
    verteces.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector3D(0.0, -1.0, 0.0)));
    verteces.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector3D(0.0, -1.0, 0.0)));

    QVector<GLuint> indexes;
    for(int i = 0; i < 24; i += 4) {
        indexes.append(i + 0);
        indexes.append(i + 1);
        indexes.append(i + 2);
        indexes.append(i + 2);
        indexes.append(i + 1);
        indexes.append(i + 3);
    }
    m_arrayBuffer.create();
    m_arrayBuffer.bind();
    m_arrayBuffer.allocate(verteces.constData(), verteces.size() * sizeof(VertexData));
    m_arrayBuffer.release();

    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indexes.constData(), indexes.size() * sizeof(GLuint));
    m_indexBuffer.release();
}

*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    screen = new GLViewScreen(this);
    setCentralWidget(screen);
    timerId = startTimer(1);
    setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e) {
    screen->update();
}

void MainWindow::timerEvent(QTimerEvent *e) {
    repaint();
    //qDebug() << std::sin(QTime::currentTime().msecsSinceStartOfDay() / 1000.0f) / 2 + 0.5f << Qt::endl;
    screen->roll(1.0f);
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    float speed = 0.1f;
    switch(e->key()) {
        case Qt::Key_Up:
            if(screen->rate < 1.0f) screen->rate += 0.1f;
            break;
        case Qt::Key_Down:
            if(screen->rate > 0.0f) screen->rate -= 0.1f;
            break;
        case Qt::Key_Left:
            screen->roll(1.0f);
            break;
        case Qt::Key_Right:
            screen->roll(-1.0f);
            break;
        case Qt::Key_W:
            screen->keys[Qt::Key_W] = true;
            break;
        case Qt::Key_S:
            screen->keys[Qt::Key_S] = true;
            break;
        case Qt::Key_D:
            screen->keys[Qt::Key_D] = true;
            break;
        case Qt::Key_A:
            screen->keys[Qt::Key_A] = true;
            break;
        case Qt::Key_Escape:
            close();
            break;
        default:
            break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    switch(e->key()) {
    case Qt::Key_W:
        screen->keys[Qt::Key_W] = false;
        break;
    case Qt::Key_S:
        screen->keys[Qt::Key_S] = false;
        break;
    case Qt::Key_D:
        screen->keys[Qt::Key_D] = false;
        break;
    case Qt::Key_A:
        screen->keys[Qt::Key_A] = false;
        break;
    default:
        break;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    if(screen->firstMouse) {
        screen->mouseX = e->pos().x();
        screen->mouseY = e->pos().y();
        screen->firstMouse = false;
    }
    GLint xOffset = (e->pos().x() - screen->mouseX);
    GLint yOffset = (screen->mouseY - e->pos().y());
    screen->mouseX = e->pos().x();
    screen->mouseY = e->pos().y();
    GLfloat sensitivity = 0.5f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    screen->yaw += xOffset;
    screen->pitch += yOffset;
    if(screen->pitch > 89.0f) screen->pitch = 89.0f;
    if(screen->pitch < -89.0f) screen->pitch = -89.0f;
    QVector3D front;
    front.setX(std::cos(qDegreesToRadians(screen->pitch)) * cos(qDegreesToRadians(screen->yaw)));
    front.setY(std::sin(qDegreesToRadians(screen->pitch)));
    front.setZ(std::cos(qDegreesToRadians(screen->pitch)) * sin(qDegreesToRadians(screen->yaw)));
    *screen->cameraFront = front.normalized();
    QCursor::setPos(1920 / 2, 1280 / 2);
}
