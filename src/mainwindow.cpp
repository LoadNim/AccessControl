#include "mainwindow.h"

static inline QString nowKstIso()
{
    return QDateTime::currentDateTimeUtc()
        .toOffsetFromUtc(9 * 3600)
        .toString(Qt::ISODate);
}

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

    connect(m_pNetwork, &NetWork::inferSucceeded, this, [this](double sim, QString bestId, int http){
        qDebug() << "[INFER OK]" << http << "sim=" << sim << "best_id=" << bestId;

        if(sim > 0.7)
        {
            m_pHomePage->setToast(tr("등록된 얼굴이 인식되었습니다\n 문이 열립니다"));
        }

        else
        {
            m_pHomePage->updateGuide(tr("등록되지 않은 얼굴입니다\n 다시 인식해주세요"));
            QTimer::singleShot(3000, this, [this]() {
                m_pHomePage->updateGuide(tr("화면을 바라보세요"));
            });
        }

        if (m_pNetwork && !m_entryImg.empty())
        {
            Info accessInfo;
            accessInfo.sendtype = 0;
            QJsonObject metadata = buildEnvelope(accessInfo);
            QJsonObject data = metadata["data"].toObject();
            data["similarity"] = sim;
            data["best_id"] = bestId;
            metadata["data"] = data;
            // qDebug() << metadata;

            cv::Mat imgCopy = m_entryImg.clone();
            QMetaObject::invokeMethod(m_pNetwork, [=](){
                m_pNetwork->postAccessEventMat(metadata, imgCopy, 80);
            }, Qt::QueuedConnection);
        }
    });

    connect(m_pNetwork, &NetWork::inferFailed, this, [this](QString err, int http){
        qDebug() << "[INFER FAIL]" << http << err;
        m_pHomePage->setToast(tr("AI 서버와의 통신에 실패했습니다\n 관리자에게 문의하세요"));
    });

    connect(m_pNetwork, &NetWork::qrSucceeded, this, [](QByteArray data, int http){
        qDebug() << "[QR OK]" << http << "data=" << data;
    });

    connect(m_pNetwork, &NetWork::qrFailed, this, [this](QString err, int http){
        qDebug() << "[QR FAIL]" << http << "err" << err;
        m_pHomePage->setToast(tr("QR 서버와의 통신에 실패했습니다\n 관리자에게 문의하세요"));
    });

    connect(m_pNetwork, &NetWork::registrationDone, this, [](QByteArray data, int http){
        qDebug() << "[REGIST DONE]" << http << "data=" << data;
    });

    connect(m_pNetwork, &NetWork::registrationFailed, this, [this](QString data, int http){
        qDebug() << "[REGIST FAIL]" << http << "data" << data;
        m_pHomePage->setToast("얼굴 등록에 실패했습니다\n 관리자에게 문의하세요");
    });

    connect(m_pNetwork, &NetWork::qrAuthenticated, this, [this](QString phone, QString purpose){
        qDebug() << "[QR AUTHENTICATED]" << "phone:" << phone << "purpose:" << purpose;
        m_pHomePage->setToast(tr("SMS 인증이 감지되었습니다\n 문이 열립니다"));
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
            m_entryImg = img;
            QMetaObject::invokeMethod(m_pNetwork, [=](){
                m_pNetwork->postInferMat(m_entryImg, deviceId);
            }, Qt::QueuedConnection);
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
        if (m_pNetwork)
        {
            Info registInfo;
            registInfo.dong = m_registerInfo.dong;
            registInfo.ho = m_registerInfo.ho;
            registInfo.phone = m_registerInfo.phone;
            registInfo.sendtype = 1;

            QJsonObject metadata = buildEnvelope(registInfo);
            QList<cv::Mat> safeImages;
            for(const auto& img : m_faceImg)
            {
                safeImages.push_back(img.clone());
            }

            QMetaObject::invokeMethod(m_pNetwork, [=](){
                m_pNetwork->postRegistrationMat(metadata, safeImages, 80);
            }, Qt::QueuedConnection);
        }
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

QJsonObject MainWindow::buildEnvelope(const Info& info) const
{
    QJsonObject client{
        { "device_id", deviceId }, // ◀ v2의 멤버 변수 사용
        { "version",   "v2-client-1.0" }
    };

    QJsonObject env{
        { "type",   "access_control" }, // (이벤트 타입)
        { "schema", 2 },
        { "client", client },
        { "data",   info.toJson() } // ◀ Info 구조체의 toJson() 호출
    };

    QJsonObject timing;
    timing["capture_time"] = nowKstIso();
    env["timing"] = timing;
    return env;
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
