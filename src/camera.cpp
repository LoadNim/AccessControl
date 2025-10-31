#include "camera.h"

Camera::Camera(QObject* parent)
    : QObject(parent)
{
    //harrcascade 모델 로드
    m_faceModel.load("haarcascade_frontalface_default.xml");
}

void Camera::setCamMode(bool mode)
{
    m_camMode = mode;
}

void Camera::setCamTimer(bool mode)
{
    if(!m_frameTimer)
    {
        m_frameTimer = new QTimer(this);
        m_frameTimer->setInterval(50);

        connect(m_frameTimer, &QTimer::timeout, this, [this]{
            if(m_enableDrop && m_framePending)
            {
                m_cam.grab();
                ++m_dropCount;
                return;
            }

            if(!m_cam.isOpened())
            {
                return;
            }

            m_cam >> m_originImg;
            if(m_originImg.empty())
            {
                return;
            }

            m_rgbImg = m_originImg.clone();
            cv::cvtColor(m_rgbImg, m_rgbImg, cv::COLOR_BGR2RGB);

            if(m_camMode)
            {
                if(findFace())
                {
                    emit findEntryFace(m_originImg.clone());
                }
            }
            else
            {
                if(findFace())
                {
                    if(!m_registSampler.isValid() || m_registSampler.elapsed() >= 250)
                    {
                        emit findRegistFace(m_originImg.clone());
                        m_registSampler.restart();
                    }
                }
            }

            emit updateFrame(QImage(m_rgbImg.data, m_rgbImg.cols, m_rgbImg.rows,
                                    m_rgbImg.step, QImage::Format_RGB888).copy());
            m_framePending = true;
        });
    }

    if(mode)
    {
        if(!m_timerRunning)
        {
            if(!m_frameTimer)
            {
                return;
            }
            if(!m_cam.isOpened())
            {
                m_cam.open(1);

                if(m_cam.isOpened())
                {
                    m_cam.set(cv::CAP_PROP_FRAME_WIDTH, 960);
                    m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, 540);
                }
                else
                {
                    return;
                }
            }
            m_registSampler.invalidate();
            m_frameTimer->start();
            m_timerRunning = true;
        }
    }
    else
    {
        if(m_timerRunning)
        {
            if(m_frameTimer)
            {
                m_frameTimer->stop();
            }
            m_timerRunning = false;
            m_framePending = false;
        }
    }
}

bool Camera::findFace()
{
    cv::Mat gray;
    cv::cvtColor(m_rgbImg, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);

    std::vector<cv::Rect> faces;
    m_faceModel.detectMultiScale
        (gray, faces, 1.1, 4, 0, cv::Size(200, 200));

    for(const auto rect : faces)
    {
        cv::rectangle(m_rgbImg, rect, cv::Scalar(0, 255, 0), 2);
    }

    return !faces.empty();
}

void Camera::onFrameConsumed()
{
    m_framePending = false;
}

Camera::~Camera() {}
