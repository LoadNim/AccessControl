#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>
#include <QImage>
#include <opencv2/opencv.hpp>

class Camera : public QObject
{
    Q_OBJECT

public:
    Camera(QObject* parent = nullptr);
    ~Camera();
    void setCamMode(bool mode);

signals:
    void updateFrame(QImage);

private:
    cv::VideoCapture    m_cam;
    cv::Mat             m_orginImg;
    cv::Mat             m_rgbImg;

    QTimer*             m_frameTimer;
    bool                m_camMode;
};

#endif // CAMERA_H
