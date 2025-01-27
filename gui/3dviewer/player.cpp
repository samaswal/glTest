#include "mainwindow.h"

void Player::move(std::array<bool, 1024> keys, GLfloat speed)
{
    if(keys[Qt::Key_W]) {
        cameraPos += QVector3D(std::cos(qDegreesToRadians(yaw)), 0.0f, std::sin(qDegreesToRadians(yaw))).normalized() * speed;
    }
    if(keys[Qt::Key_S]) {
        cameraPos -= QVector3D(std::cos(qDegreesToRadians(yaw)), 0.0f, std::sin(qDegreesToRadians(yaw))).normalized() * speed;
    }
    if(keys[Qt::Key_D]) {
        cameraPos += right * speed;
    }
    if(keys[Qt::Key_A]) {
        cameraPos -= right * speed;
    }
    // if(keys[Qt::Key_Space]) {
    //     cameraPos += worldUp * speed;
    // }
    // if(keys[Qt::Key_C]) {
    //     cameraPos -= worldUp * speed;
    // }
    updateVectors();
}

void Player::mouseMove(GLint xOffset, GLint yOffset)
{
    yaw += xOffset;
    pitch += yOffset;
    if(pitch > 89) pitch = 89;
    if(pitch < -89) pitch = -89;
    updateVectors();
}

QMatrix4x4 Player::GetViewMatrix()
{
    QMatrix4x4 view;
    view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    return view;
}

void Player::updateVectors() {
    QVector3D front;
    front.setX(std::cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.setY(std::sin(qDegreesToRadians(pitch)));
    front.setZ(std::sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    cameraFront = front.normalized();
    right = QVector3D::crossProduct(cameraFront, worldUp);
    right.normalize();
    cameraUp = QVector3D::crossProduct(right, cameraFront);
    cameraUp.normalize();
}
