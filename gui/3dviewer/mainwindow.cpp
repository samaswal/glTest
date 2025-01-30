#include "mainwindow.h"
#include "ui_mainwindow.h"

std::atomic<int> Translatable::next_id(1);

void GLViewScreen::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
    initShaders();
    placeObjects();
    for(int i = 0; i < 20; i++) {
        for(int j = 0; j < 20; j++) {
            cubes.append(Cube(QVector3D((GLfloat)i, wHeight, (GLfloat)j)));
        }
    }
    placeObjects();
    //initSquare();
    //initCube(1);
    setMouseTracking(true);
    projection.setToIdentity();
    selectMode = false;
    player = new Player(QVector3D(0.0f, 1.5f, 3.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f));

    keys.fill(false);
    lastTime = QTime::currentTime().msecsSinceStartOfDay();
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
    QMatrix4x4 view = player->GetViewMatrix();
    m_program.bind();
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);
    for(int i = 0; i < cubes.size(); i++) {
        cubes[i].draw(&m_program, false);
    }
    for(int i = 0; i < xarta.size(); i++)
        xarta[i].draw(&m_program, false);
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

void GLViewScreen::shoot() {
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    selectMode = true;
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
        cubes[i].draw(&m_selectProgram, false);
    }
    GLubyte pix[4];
    int x = width() / 2;
    int y = height() / 2;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
        GLuint b = (pix[2]);
        GLuint g = pix[1];
        GLuint r = pix[0];

        int id = (r << 16) | (g << 8) | b;
        for(auto i = cubes.begin(); i != cubes.end(); i++) {
            if(i->getId() == id && i->isActive()) {
                i->kill();}
            qDebug() << i->isActive() << ' '<< i->getId() << ' ' << pix[0] << Qt::endl;

    }
    //qDebug() << pix[0] << Qt::endl;

    selectMode = false;
    glEnable(GL_DEPTH_TEST);
    //update();
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
