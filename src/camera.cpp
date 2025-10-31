#include "camera.h"

Camera::Camera(QObject* parent)
    : QObject(parent)
{
    // 카메라 초기 설정
    m_cam.open(0);
    m_cam.set(cv::CAP_PROP_FRAME_WIDTH, 960);
    m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, 540);

    //harrcascade 모델 로드
    m_faceModel.load("haarcascade_frontalface_default.xml");

    // 프레임 갱신 타이머 설정
    m_frameTimer = new QTimer(this);
    m_frameTimer->setInterval(33);

    connect(m_frameTimer, &QTimer::timeout, this, [=]{
        m_cam >> m_originImg;
        m_rgbImg = m_originImg.clone();


        cv::cvtColor(m_rgbImg, m_rgbImg, cv::COLOR_BGR2RGB);
        emit updateFrame(QImage(m_rgbImg.data, m_rgbImg.cols, m_rgbImg.rows,
                                m_rgbImg.step, QImage::Format_RGB888).copy());
    });
    m_frameTimer->start();
}

void Camera::setCamMode(bool mode)
{
    m_camMode = mode;
}

bool Camera::findFace()
{
    return true;
}

Camera::~Camera() {}
