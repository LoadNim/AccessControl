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
    for (QWidget *p : m_pPages)
    {
        m_pStackedWidget->addWidget(p);
    }

    // Signal - Slot 연결
    for (int i = 0; i < 4; ++i)
    {
        connect(m_pPages[i], SIGNAL(request(PageRequest)),
                this, SLOT(changePage(PageRequest)));
    }
}

void MainWindow::changePage(const PageRequest& req)
{
    m_pStackedWidget->setCurrentWidget(m_pPages[static_cast<int>(req.id)]);
}

MainWindow::~MainWindow() {}
