#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QImage>
#include <opencv2/opencv.hpp>

class Camera : public QObject
{
    Q_OBJECT

public:
    Camera(QObject* parent = nullptr);
    ~Camera();
    void setCamMode(bool mode);
    void setCamTimer(bool mode);
    bool findFace();

signals:
    void updateFrame(const QImage& frame);
    void findEntryFace(const cv::Mat);
    void findRegistFace(const cv::Mat);

public slots:
    void onFrameConsumed();

private:
    cv::VideoCapture        m_cam;
    cv::Mat                 m_originImg;
    cv::Mat                 m_rgbImg;
    cv::CascadeClassifier   m_faceModel;

    QTimer*                 m_frameTimer = nullptr;
    bool                    m_camMode;
    bool                    m_timerRunning = false;

    bool                    m_framePending = false;
    bool                    m_enableDrop = true;
    QElapsedTimer           m_registSampler;
    qint64                  m_dropCount = 0;
};

#endif // CAMERA_H
