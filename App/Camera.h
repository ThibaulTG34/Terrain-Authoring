#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
    Camera();

    void moveRight();
    void moveLeft();

    // Getters and setters for camera properties...
    float getFov();
    void setFov(float fov_);

    QVector3D getPosition();
    void setPosition(QVector3D position_);

    QVector3D getFront();
    void setFront(QVector3D front_);

    // void setDirection(QVector3D direction_);
    // QVector3D getDirection();

    QVector3D getUp();
    void setUp(QVector3D up_);

    QVector3D getUpOrbit();
    void setUpOrbit(QVector3D up_);

    float getMvtSpeed();
    void setMvtSpeed(float up_);

    QVector3D getTarget();
    void setTarget(QVector3D target_);

    float getYaw();
    void setYaw(float new_yaw);

    float getPitch();
    void setPitch(float new_pitch);

private:
    QVector3D position;
    QVector3D front;
    QVector3D up;
    QVector3D upOrbit;
    QVector3D target;

    float yaw;
    float pitch;

    float movementSpeed;
    float sensitivity;
    float fov;
};

#endif // CAMERA_H
