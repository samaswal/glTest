#include "mainwindow.h"
#include "ui_mainwindow.h"

std::atomic<int> Translatable::next_id(1);

void GLViewScreen::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
    initShaders();
   // placeObjects();
    // for(int k = 0; k < 20; k++) {
    // for(int i = 0; i < 16; i++) {
    //     for(int j = 0; j < 16; j++) {
            //cubes.append(Cube(QVector3D(0.0f, 0.0f, 0.0f)));
            //cubes.append(Cube(QVector3D(1.0f, 0.0f, 0.0f)));
            //cubes[1].rotate(45.0f, QVector3D(0.0f, 0.0f, 1.0f));

    //     }
    // }
    // }
    //placeObjects();
    //initSquare();
    //initCube(1);
    //chunk = new Chunk(QVector2D(0.0f, 0.0f));
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            chunks.append(QVector2D((GLfloat)i, (GLfloat)j));
        }
    }
    setMouseTracking(true);
    projection.setToIdentity();
    selectMode = false;
    GLfloat PLAYER_POS_Y = 100.0f;
    GLfloat PLAYER_HEIGHT = 1.5f;
    GLfloat BLOCK_HALF = 0.5f;
    player = new Player(QVector3D(0.0f, PLAYER_POS_Y + PLAYER_HEIGHT, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f));

    keys.fill(false);
    lastTime = QTime::currentTime().msecsSinceStartOfDay();
    //lastFrame = QTime::currentTime().second();
    // direction.setX(0);
    firstMouse = true;
}

void GLViewScreen::clean() {
    for(auto i = cubes.begin(); i != cubes.end(); i++) {
        if(!i->isActive()) cubes.erase(i);
    }
}

void GLViewScreen::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    move();
    setMouse();
    //intersectWithAABB(chunk->blocks[0][100][0]);
    //qDebug() << chunk->blocks[0][100][0].posGlobal << Qt::endl;
    //       blockIdentify();

    //mouseToWorldCoords();
    QMatrix4x4 view = player->GetViewMatrix();
    m_program.bind();
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // for(int i = 0; i < cubes.size(); i++) {
    //     cubes[i].draw(&m_program, false);
    // }
    // for(int i = 0; i < xarta.size(); i++)
    //     xarta[i].draw(&m_program, false);
    //chunk->draw(&m_program);
    for(int i = 0; i < 9; i++) {

        chunks[i].draw(&m_program);
    }
    m_program.release();
}

void GLViewScreen::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
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
                cubes.append(Cube(QVector3D((GLfloat)i, wHeight + 1.0f, (GLfloat)j)));
            }
        }
    }
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

bool GLViewScreen::stare() {
    //QVector3D N(0.0f, 1.0f, 0.0f);
    QVector3D res;
    for(float i = 0; i < 10.0f; i += 0.1f) {
        res = player->cameraPos + player->cameraFront * i;
            if(cubes[0].getPos() == res) {
                return true;
        }
    }
    return false;
}

GLfloat GLViewScreen::intersectWithAABB(Block c) {
    QVector3D min(c.posGlobal - QVector3D(0.5f, 0.5f, 0.5f));
    QVector3D max(c.posGlobal + QVector3D(0.5f, 0.5f, 0.5f));
    GLfloat tmin = (min.x() - player->cameraPos.x()) / player->cameraFront.x();
    GLfloat tmax = (max.x() - player->cameraPos.x()) / player->cameraFront.x();

    if(tmin > tmax) std::swap(tmin, tmax);
    GLfloat tymin = (min.y() - player->cameraPos.y()) / player->cameraFront.y();
    GLfloat tymax = (max.y() - player->cameraPos.y()) / player->cameraFront.y();

    if(tymin > tymax) std::swap(tymin, tymax);

    if((tymin > tmax) || (tmin > tymax)) return -1.0f;

    if(tymin > tmin) tmin = tymin;
    if(tymax < tmax) tmax = tymax;

    GLfloat tzmin = (min.z() - player->cameraPos.z()) / player->cameraFront.z();
    GLfloat tzmax = (max.z() - player->cameraPos.z()) / player->cameraFront.z();

    if(tzmin > tzmax) std::swap(tzmin, tzmax);

    if((tzmin > tmax) || (tmin > tzmax)) return -1.0f;


    if(tzmin > tmin) tmin = tzmin;
    if(tzmax < tmax) tmax = tzmax;

    if(((tmin + tmax) / 2.0f) > 5.0f) return -1.0f;

    return (tmin + tmax) / 2.0f;
}

GLfloat GLViewScreen::intersectWithChunk(Chunk c) {
    QVector3D posGlobal = QVector3D(c.coords().x() * 16 + 8.0f, 128.0f, c.coords().y() * 16 + 8.0f);
    QVector3D min(posGlobal - QVector3D(8.0f, 128.0f, 8.0f));
    QVector3D max(posGlobal + QVector3D(8.0f, 128.0f, 8.0f));
    GLfloat tmin = (min.x() - player->cameraPos.x()) / player->cameraFront.x();
    GLfloat tmax = (max.x() - player->cameraPos.x()) / player->cameraFront.x();

    if(tmin > tmax) std::swap(tmin, tmax);
    GLfloat tymin = (min.y() - player->cameraPos.y()) / player->cameraFront.y();
    GLfloat tymax = (max.y() - player->cameraPos.y()) / player->cameraFront.y();

    if(tymin > tymax) std::swap(tymin, tymax);

    if((tymin > tmax) || (tmin > tymax)) return -999.0f;

    if(tymin > tmin) tmin = tymin;
    if(tymax < tmax) tmax = tymax;

    GLfloat tzmin = (min.z() - player->cameraPos.z()) / player->cameraFront.z();
    GLfloat tzmax = (max.z() - player->cameraPos.z()) / player->cameraFront.z();

    if(tzmin > tzmax) std::swap(tzmin, tzmax);

    if((tzmin > tmax) || (tmin > tzmax)) return -999.0f;


    if(tzmin > tmin) tmin = tzmin;
    if(tzmax < tmax) tmax = tzmax;

    //if(((tmin + tmax) / 2.0f) > 5.0f) return -999.0f;

    qDebug() << (tmin + tmax) / 2.0f << Qt::endl;

    return (tmin + tmax) / 2.0f;
}

void GLViewScreen::blockIdentify() {
    int xPos = player->cameraPos.x();
    int zPos = player->cameraPos.z();
    xPos -= xPos % 16;
    xPos = xPos / 16;
    zPos -= zPos % 16;
    zPos = zPos / 16;
    int id = 0;
    for(; id < chunks.size(); id++) {
        if((chunks[id].coords().x() == xPos) && (chunks[id].coords().y() == zPos)) break;
    }
    //QMap<GLfloat, QVector3D> intersectChunk;
    // QMap<GLfloat, int> chunkTrace;
    // for(int id = 0; id < chunks.size(); id++) {
    //     GLfloat res = intersectWithChunk(chunks[id]);
    //     if(res != -999.0f)
    //         chunkTrace.insert(res, id);
    // }
    // int id = chunkTrace.begin().value();
    QMap<GLfloat, QVector3D> intersect;
        for(int y = 0; y < 256; y++) {
            for(int x = 0; x < 16; x++) {
                for(int z = 0; z < 16; z++) {
                    if(chunks[id].blocks[x][y][z].active) {
                        GLfloat res = intersectWithAABB(chunks[id].blocks[x][y][z]);
                        if(res > 0)
                            intersect.insert(res, QVector3D(x, y, z));
                    }
                }
            }
        }

    if(intersect.size() > 0) {
        chunks[id].destroyBlock(*intersect.begin());
    //qDebug() << intersect << Qt::endl;
    }
}

QVector3D GLViewScreen::mouseToWorldCoords() {
    QVector3D N(0.0f, 1.0f, 0.0f);
    float t = -QVector3D::dotProduct(player->cameraPos, N) / QVector3D::dotProduct(player->cameraFront, N);
    qDebug() << t << Qt::endl;
    QVector3D preresult = player->cameraPos + player->cameraFront * t;
    //qDebug() << preresult << Qt::endl;
    //qDebug() << Dir << Qt::endl;
    QVector3D result = QVector3D(round(preresult.x()), round(preresult.y()), round(preresult.z()));
    return result;
}

void GLViewScreen::create() {
    int xPos = player->cameraPos.x();
    int zPos = player->cameraPos.z();
    xPos -= xPos % 16;
    xPos = xPos / 16;
    zPos -= zPos % 16;
    zPos = zPos / 16;
    int id = 0;
    for(; id < chunks.size(); id++) {
        if((chunks[id].coords().x() == xPos) && (chunks[id].coords().y() == zPos)) break;
    }

    QMap<GLfloat, QVector3D> intersect;
    for(int y = 0; y < 256; y++) {
        for(int x = 0; x < 16; x++) {
            for(int z = 0; z < 16; z++) {
                if(chunks[id].blocks[x][y][z].active) {
                    GLfloat res = intersectWithAABB(chunks[id].blocks[x][y][z]);
                    if(res > 0)
                        intersect.insert(res, QVector3D(x, y, z));
                }
            }
        }
    }
    if(intersect.size() > 0) {
        QVector3D ps = intersect.begin().value();
        QMap<GLfloat, QVector3D> newPos;
        if(ps.y() < 255) {
            GLfloat res = intersectWithAABB(chunks[id].blocks[(int)ps.x()][(int)ps.y() + 1][(int)ps.z()]);
            if(res > 0) newPos.insert(res, QVector3D(ps.x(), ps.y() + 1, ps.z()));
        }
        if(ps.y() > 0) {
            GLfloat res = intersectWithAABB(chunks[id].blocks[(int)ps.x()][(int)ps.y() - 1][(int)ps.z()]);
            if(res > 0) newPos.insert(res, QVector3D(ps.x(), ps.y() - 1, ps.z()));
        }
        if(ps.x() < 15) {
            GLfloat res = intersectWithAABB(chunks[id].blocks[(int)ps.x() + 1][(int)ps.y()][(int)ps.z()]);
            if(res > 0) newPos.insert(res, QVector3D(ps.x() + 1, ps.y(), ps.z()));
        }
        if(ps.x() > 0) {
            GLfloat res = intersectWithAABB(chunks[id].blocks[(int)ps.x() - 1][(int)ps.y()][(int)ps.z()]);
            if(res > 0) newPos.insert(res, QVector3D(ps.x() - 1, ps.y(), ps.z()));
        }
        if(ps.z() < 15) {
            GLfloat res = intersectWithAABB(chunks[id].blocks[(int)ps.x()][(int)ps.y()][(int)ps.z() + 1]);
            if(res > 0) newPos.insert(res, QVector3D(ps.x(), ps.y(), ps.z() + 1));
        }
        if(ps.z() > 0) {
            GLfloat res = intersectWithAABB(chunks[id].blocks[(int)ps.x()][(int)ps.y()][(int)ps.z() - 1]);
            if(res > 0) newPos.insert(res, QVector3D(ps.x(), ps.y(), ps.z() - 1));
        }

        if(!newPos.empty())
        chunks[id].createBlock(*newPos.begin());

    }
}

void GLViewScreen::shoot() {
    initializeOpenGLFunctions();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
   glViewport(0, 0, width(), height());
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    QMatrix4x4 view = player->GetViewMatrix();
    m_selectProgram.bind();
    m_selectProgram.setUniformValue("view", view);
    m_selectProgram.setUniformValue("projection", projection);

    for(int i = 0; i < cubes.size(); i++) {
        GLubyte r = ((cubes[i].getId() >> 16) & 0xFF);
        GLubyte g = ((cubes[i].getId() >> 8) & 0xFF);
        GLubyte b = (cubes[i].getId() & 0xFF);
        m_selectProgram.setUniformValue("r", (GLfloat)r);
        m_selectProgram.setUniformValue("g", (GLfloat)g);
        m_selectProgram.setUniformValue("b", (GLfloat)b);
        cubes[i].draw(&m_selectProgram, true);
    }
    //glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    GLubyte pix[3];
    int x = width() / 2;
    int y = height() / 2;
    mouseToWorldCoords();
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
    GLuint b = (pix[2]);
    GLuint g = pix[1];
    GLuint r = pix[0];



    int id = (r << 16) | (g << 8) | b;
    for(auto i = cubes.begin(); i != cubes.end(); i++) {
        if(i->getId() == id && i->isActive()) {
            i->kill();
        }
    }

    m_selectProgram.release();
    //clean();
    update();
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
    if(e->button() == Qt::LeftButton)
        screen->blockIdentify();
    else if(e->button() == Qt::RightButton)
        screen->create();
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
