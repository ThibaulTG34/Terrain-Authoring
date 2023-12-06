#include "Camera.h"
#include <cmath>
#include <iostream>

Camera::Camera()
{

    position = QVector3D(0.0f, 0.0f, -3.0f);
    front = QVector3D(0.0f, 0.0f, 1.0f);
    up = QVector3D(0.0f, 1.0f, 0.0f);
    yaw = (-90.0f);
    pitch = (0.0f);
    movementSpeed = (0.05f);
    sensitivity = (0.1f);
    fov = (45.0f);
    target = QVector3D(0, 0, 0);
    QVector3D direction = position - target;
    direction.normalize();
    QVector3D cross = QVector3D::crossProduct(up, direction);
    // cross.normalize();
    upOrbit = QVector3D::crossProduct(direction, cross);
}


float Camera::getFov()
{
    return fov;
}

void Camera::setFov(float fov_)
{
    fov = fov_;
}

QVector3D Camera::getPosition()
{
    return position;
}

void Camera::setPosition(QVector3D position_)
{
    position = position_;
}

QVector3D Camera::getFront()
{
    return front;
}

void Camera::setFront(QVector3D front_)
{
    front = front_;
}

QVector3D Camera::getUp()
{
    return up;
}

void Camera::setUp(QVector3D up_)
{
    up = up_;
}

QVector3D Camera::getUpOrbit()
{
    return upOrbit;
}

void Camera::setUpOrbit(QVector3D up_)
{
    upOrbit = up_;
}

float Camera::getMvtSpeed()
{
    return movementSpeed;
}

void Camera::setMvtSpeed(float ms_)
{
    movementSpeed=ms_;
}

QVector3D Camera::getTarget()
{
    return target;
}

void Camera::setTarget(QVector3D target_)
{
    target = target_;
}

float Camera::getYaw()
{
    return yaw;
}

void Camera::setYaw(float new_yaw)
{
    yaw=new_yaw;
}

float Camera::getPitch()
{
    return pitch;
}

void Camera::setPitch(float new_pitch)
{
    pitch=new_pitch;
}

