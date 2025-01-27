#include "mainwindow.h"
#include "ui_mainwindow.h"

void GLViewScreen::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
    initShaders();
    placeObjects();
    setMouseTracking(true);
    projection.setToIdentity();
    selectMode = false;
    player = new Player(QVector3D(0.0f, 1.5f, 3.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f));

    keys.fill(false);
    lastTime = QTime::currentTime().msecsSinceStartOfDay();
    // direction.setX(0);
    firstMouse = true;
}

void GLViewScreen::drawMap() {
    if(selectMode) return;
    initSquare();
    int vertLoc = 0;
    int colorLoc = 1;
    int texLoc = m_program.attributeLocation("texCoord");
    int offset = 0;
    m_tex->bind(4);
    m_tex1->bind(5);
    VBO.bind();
    IBO.bind();
    m_program.bind();
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    m_program.enableAttributeArray(texLoc);
    m_program.setAttributeBuffer(texLoc, GL_FLOAT, 6 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
    //m_program.setUniformValue("trans", modelTransMatrix);
    for(int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            QMatrix4x4 model;
            model.setToIdentity();
            model.translate(QVector3D((GLfloat)(i), 0.0f, j));

            model.rotate(-90, QVector3D(1.0f, 0.0f, 0.0f));
            m_program.setUniformValue("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }
    IBO.release();
    VBO.release();
    if(m_tex->isCreated()) m_tex->release();
    if(m_tex1->isCreated()) m_tex1->release();
}

void GLViewScreen::drawObjects() {
    initCube(1);
    VBO.bind();
    int vertLoc = 0;
    if(!selectMode) {
        int colorLoc = 1;
        int texLoc = m_program.attributeLocation("texCoord");
        m_tex->bind(4);
        m_tex1->bind(5);
        m_program.enableAttributeArray(vertLoc);
        m_program.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
        m_program.enableAttributeArray(texLoc);
        m_program.setAttributeBuffer(texLoc, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
        for(int i = 0; i < cubePositions.size(); i++) {
            if(cubePositions[i].isActive) {
                    QMatrix4x4 model;
                    model.setToIdentity();
                    model.translate(cubePositions[i].pos);
                    m_program.setUniformValue("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
    else {
        QMatrix4x4 view = player->GetViewMatrix();
        m_selectProgram.bind();
        m_selectProgram.setUniformValue("view", view);
        m_selectProgram.setUniformValue("projection", projection);
        m_selectProgram.enableAttributeArray(vertLoc);
        m_selectProgram.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
        for(int i = 0; i < cubePositions.size(); i++) {
            if(cubePositions[i].isActive) {
                QMatrix4x4 model;
                model.setToIdentity();
                model.translate(cubePositions[i].pos);
                m_selectProgram.setUniformValue("model", model);
                GLfloat normalized = 1.0f - (cubePositions[i].id / 255.0f);
                m_selectProgram.setUniformValue("id", normalized);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
    VBO.release();
    if(m_selectProgram.isLinked()) m_selectProgram.release();
    if(m_tex->isCreated()) m_tex->release();
    if(m_tex1->isCreated()) m_tex1->release();
}

void GLViewScreen::resizeGL(int w, int h) {
//    glViewport(0, 0, w, h);
    //view.translate(QVector3D(0.0f, 0.0f, 0.0f));
    projection.perspective(80.0f, ((GLfloat)(w) / (GLfloat)(h)), 0.1f, 100.0f);
}

void GLViewScreen::setMouse() {
    QPoint npos = mapFromGlobal(QCursor::pos());
    GLint xOffset = (npos.x() - width() / 2);
    GLint yOffset = (height() / 2 - npos.y());
    if(xOffset != 0 || yOffset != 0) {
        GLfloat sensitivity = 0.5f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;
        player->mouseMove(xOffset, yOffset);
    }
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));

}

void GLViewScreen::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    move();
    setMouse();
    QMatrix4x4 view = player->GetViewMatrix();
    if(!selectMode) {
        m_program.bind();
        m_program.setUniformValue("view", view);
        m_program.setUniformValue("tex", 4);
        m_program.setUniformValue("tex1", 5);
        m_program.setUniformValue("rate", 0.01f);
        m_program.setUniformValue("projection", projection);
        drawMap();
    }
    drawObjects();
    if(m_program.isLinked()) m_program.release();
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
        0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        -0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f
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



    if(!m_selectProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "../../select.vsh")) {
        qDebug() << "select vertex add fail" << Qt::endl;
        close();
    }

    if(!m_selectProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "../../select.fsh")) { close(); }

    if(!m_selectProgram.link()) {
        close();
    }
}

void GLViewScreen::placeObjects() {
    int id = 0;
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            int n = rand() % 10;
            if(n == 1) {
                cubePositions.append(Entity(QVector3D((GLfloat)i,  0.5f,  (GLfloat)j), id++));
            }
        }
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

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));
    VBO.release();

    m_tex = new QOpenGLTexture(QImage("../../gold.webp").mirrored());
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
    GLfloat speed = 0.005f;
    curTime = QTime::currentTime().msecsSinceStartOfDay();
    speed *= (curTime - lastTime);
    lastTime = curTime;
    player->move(keys, speed);
}

void GLViewScreen::shoot() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    selectMode = true;
    glDisable(GL_DEPTH_TEST);
    drawObjects();
    GLubyte pix[4];
    int x = 400;
    int y = 300;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pix);
    if(pix[0] > 0) {
        GLint id = (255 - pix[0]);
        //GLint nid = (GLint)id;
        cubePositions[id].isActive = false;
    }
    qDebug() << pix[0] << Qt::endl;

    selectMode = false;
    glEnable(GL_DEPTH_TEST);
}


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
    screen->roll(1.0f);
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
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
        case Qt::Key_Space:
            screen->keys[Qt::Key_Space] = true;
            break;
        case Qt::Key_C:
            screen->keys[Qt::Key_C] = true;
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
    case Qt::Key_Space:
        screen->keys[Qt::Key_Space] = false;
        break;
    case Qt::Key_C:
        screen->keys[Qt::Key_C] = false;
        break;
    default:
        break;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    // QPoint npos = mapFromGlobal(QCursor::pos());
    // GLint xOffset = (npos.x() - screen->mouseX);
    // GLint yOffset = (screen->mouseY - npos.y());
    // screen->mouseX = npos.x();
    // screen->mouseY = npos.y();
    // GLfloat sensitivity = 0.5f;
    // xOffset *= sensitivity;
    // yOffset *= sensitivity;
    // screen->player->mouseMove(xOffset, yOffset);
    // qDebug() << screen->mouseX << ' ' << screen->mouseY << Qt::endl;
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    screen->shoot();
    //screen->selectMode = !screen->selectMode;
}

void MainWindow::enterEvent(QEnterEvent *e)
{
    //screen->mouseX = e->position().x();
    //screen->mouseY = e->position().y();
    //screen->firstMouse = false;
}

// void MainWindow::leaveEvent(QEvent *e)
// {
//     //screen->firstMouse = false;
// }
