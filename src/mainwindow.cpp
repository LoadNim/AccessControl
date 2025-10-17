#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 윈도우 크기 정의
    resize(1280, 720);

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
    for (QWidget *p : m_pPages) {
        m_pStackedWidget->addWidget(p);
    }
    m_pStackedWidget->setCurrentWidget(m_pPages[0]);
    setCentralWidget(m_pStackedWidget);
}

MainWindow::~MainWindow() {}
