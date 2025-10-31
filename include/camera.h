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
    bool findFace();
    bool findQR();

signals:
    void updateFrame(QImage);
    void findEntryFace(cv::Mat);
    void findEntryQR(std::string);
    void findRegistFace(cv::Mat);

private:
    cv::VideoCapture        m_cam;
    cv::Mat                 m_originImg;
    cv::Mat                 m_rgbImg;
    cv::CascadeClassifier   m_faceModel;

    QTimer*                 m_frameTimer;
    bool                    m_camMode;
};

#endif // CAMERA_H
