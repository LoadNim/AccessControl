#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 윈도우 크기 정의
    resize(1280, 720);

    // 카메라 초기화
    m_pCamera = new Camera(nullptr);
    m_cameraThread = new QThread(this);
    m_pCamera->moveToThread(m_cameraThread);
    m_cameraThread->start();

    m_pNetwork = new NetWork(nullptr);
    m_networkThread = new QThread(this);
    m_pNetwork->moveToThread(m_networkThread);
    connect(m_networkThread, &QThread::started, m_pNetwork, &NetWork::threadInit);
    connect(m_networkThread, &QThread::finished, m_pNetwork, &QObject::deleteLater);
    m_networkThread->start();

    connect(m_pNetwork, &NetWork::inferSucceeded, this, [](double sim, QString bestId, int http){
        qDebug() << "[INFER OK]" << http << "sim=" << sim << "best_id=" << bestId;
    });

    connect(m_pNetwork, &NetWork::inferFailed, this, [](QString err, int http){
        qDebug() << "[INFER FAIL]" << http << err;
    });

    // 위젯 포인터 초기화
    m_pHomePage = new HomePage(this);
    m_pQRPage = new QRPage(this);
    m_pRegistInfo = new RegistInfo(this);
    m_pRegistCam = new RegistCam(this);

    // 위젯 매핑
    m_pPages[static_cast<int>(PageId::Home)]        = m_pHomePage;
    m_pPages[static_cast<int>(PageId::QR)]          = m_pQRPage;
    m_pPages[static_cast<int>(PageId::RegistInfo)]  = m_pRegistInfo;
    m_pPages[static_cast<int>(PageId::RegistCam)]   = m_pRegistCam;

    // StackedWidget 등록
    m_pStackedWidget = new QStackedWidget(this);
    for (QWidget *p : m_pPages)
    {
        m_pStackedWidget->addWidget(p);
    }
    setCentralWidget(m_pStackedWidget);

    // Signal - Slot 연결
    for (int i = 0; i < 4; ++i)
    {
        connect(m_pPages[i], SIGNAL(request(PageRequest)),
                this, SLOT(changePage(PageRequest)));
    }

    connect(m_pCamera, &Camera::updateFrame, this, &MainWindow::frameBroker);

    connect(m_pRegistCam, &RegistCam::isAllowSend, this, [=](bool& trigger){
        trigger = m_faceImg.size() >= 20;
    });

    connect(m_pCamera, &Camera::findEntryFace, this, [=](const cv::Mat img){
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - m_lastInferMs < 1500)
        {
            return;
        }
        m_lastInferMs = now;

        if(m_pNetwork)
        {
            QMetaObject::invokeMethod(m_pNetwork,[=](){
                m_pNetwork->postInferMat(img, deviceId);
            },Qt::QueuedConnection);
        }
    });

    connect(m_pCamera, &Camera::findRegistFace, this, [=](const cv::Mat img){
        if(m_faceImg.size() != 20)
        {
            m_faceImg.push_back(img);
            int count = m_faceImg.size();
            QString str = tr("현재 인식된 얼굴 사진: %1장/20장").arg(count);
            m_pRegistCam->updateGuide(str);
        }
        else
        {
            QString str = tr("등록 요청 버튼을 눌러주세요");
            m_pRegistCam->updateGuide(str);
        }
    });

    connect(m_pHomePage, &HomePage::setCamMode, m_pCamera, &Camera::setCamMode);
    connect(m_pHomePage, &HomePage::setCamTimer, m_pCamera, &Camera::setCamTimer);
    connect(m_pRegistCam, &RegistCam::setCamMode, m_pCamera, &Camera::setCamMode);
    connect(m_pRegistCam, &RegistCam::setCamTimer, m_pCamera, &Camera::setCamTimer);
    connect(m_cameraThread, &QThread::finished, m_pCamera, &QObject::deleteLater);
    connect(this, &MainWindow::frameConsumed, m_pCamera, &Camera::onFrameConsumed);
}

void MainWindow::changePage(const PageRequest& req)
{
    m_pStackedWidget->setCurrentWidget(m_pPages[static_cast<int>(req.id)]);

    if(std::holds_alternative<std::monostate>(req.data))
    {
        m_faceImg.clear();
        m_registerInfo.dong.clear();
        m_registerInfo.ho.clear();
        m_registerInfo.phone.clear();
        return;
    }
    else if(const QRInfo* info = std::get_if<QRInfo>(&req.data))
    {
        if(m_pNetwork)
        {
            const QRInfo infoCopy = *info;
            QMetaObject::invokeMethod(m_pNetwork, [=](){
                m_pNetwork->postQrGenerate(infoCopy.phone,
                                           infoCopy.purpose,
                                           deviceId);
            }, Qt::QueuedConnection);
        }
    }
    else if(const RegisterInfo* info = std::get_if<RegisterInfo>(&req.data))
    {
        m_registerInfo.dong = info->dong;
        m_registerInfo.ho = info->ho;
        m_registerInfo.phone = info->phone;
    }
    else
    {
        // 네트워크 모듈 구현 시 작성
    }
}

void MainWindow::frameBroker(const QImage& frame)
{
    QWidget* currentPage = m_pStackedWidget->currentWidget();

    if(currentPage == m_pHomePage)
    {
        m_pHomePage->updateLabel(frame);
    }

    else if(currentPage == m_pRegistCam)
    {
        m_pRegistCam->updateLabel(frame);
    }

    emit frameConsumed();
}

void MainWindow::stopCameraThread()
{

}

MainWindow::~MainWindow()
{
    if(!m_cameraThread)
    {
        return;
    }

    if(m_pCamera)
    {
        QMetaObject::invokeMethod(
            m_pCamera, "setCamTimer",
            Qt::QueuedConnection,
            Q_ARG(bool, false)
            );
    }

    m_cameraThread->quit();
    m_cameraThread->wait();

    if(m_pNetwork)
    {
        QMetaObject::invokeMethod(
            m_pNetwork, "shutdown",
            Qt::QueuedConnection);
    }
    if(m_networkThread)
    {
        m_networkThread->quit();
        m_networkThread->wait();
    }
}
