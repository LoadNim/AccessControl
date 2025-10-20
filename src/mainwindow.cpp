#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 윈도우 크기 정의
    resize(1280, 720);

    // 카메라 초기화
    m_pCamera = new Camera(this);

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
}

void MainWindow::changePage(const PageRequest& req)
{
    m_pStackedWidget->setCurrentWidget(m_pPages[static_cast<int>(req.id)]);

    // 데이터가 있으면 네트워크행
    if (!std::holds_alternative<std::monostate>(req.data))
    {
        // 추후 작성
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
        // 추후 작성
    }
}

MainWindow::~MainWindow() {}
