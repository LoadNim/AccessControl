#include "homepage.h"

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    // 상단 카메라 영역
    m_cameraView = new QLabel(this);
    m_cameraView->setObjectName("cameraView");
    m_cameraView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 안내 문구
    m_guideLabel = new QLabel("화면을 바라보세요", this);
    m_guideLabel->setObjectName("guideLabel");
    m_guideLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_guideLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_guideLabel->setMinimumHeight(42);

    // QR 버튼
    m_btnQR = new QPushButton("방문자 QR 코드 발급", this);
    m_btnQR->setObjectName("btnQR");

    // 등록 버튼
    m_btnRegist = new QPushButton("세대주 등록하기", this);
    m_btnRegist->setObjectName("btnRegist");

    // 버튼 레이아웃 배치
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_btnQR);
    btnLayout->addWidget(m_btnRegist);
    btnLayout->setSpacing(14);
    btnLayout->setContentsMargins(100, 12, 100, 60);

    // 메인 레이아웃 배치
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_cameraView, 7);
    mainLayout->addWidget(m_guideLabel);
    mainLayout->addLayout(btnLayout, 3);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 시그널 연결
    connect(m_btnQR, &QPushButton::clicked, this, [=]{
        emit request({PageId::QR});
    });

    connect(m_btnRegist, &QPushButton::clicked, this, [=]{
        emit request({PageId::RegistInfo});
    });
}

HomePage::~HomePage() {}
