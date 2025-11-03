#include "homepage.h"

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("homePage"); // QSS 스코프 분리

    // 카메라 영역
    m_cameraView = new QLabel(this);
    m_cameraView->setObjectName("cameraView");
    m_cameraView->setFixedSize(960, 540);                 // 고정 해상도
    m_cameraView->setAlignment(Qt::AlignCenter);          // 중앙 정렬
    m_cameraView->setScaledContents(false);               // 비율 왜곡 방지
    m_cameraView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 안내문구 영역
    m_guideLabel = new QLabel(tr("화면을 바라보세요"), this);
    m_guideLabel->setObjectName("guideLabel");
    m_guideLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_guideLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_guideLabel->setMinimumHeight(42);

    // 버튼 초기화
    m_btnQR = new QPushButton(tr("방문자 SMS 링크 발급"), this);
    m_btnQR->setObjectName("btnQR");
    m_btnQR->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_btnQR->setMinimumHeight(48);
    m_btnQR->setAutoDefault(false);
    m_btnQR->setDefault(false);
    m_btnQR->setFocusPolicy(Qt::NoFocus);

    m_btnRegist = new QPushButton(tr("세대주 등록하기"), this);
    m_btnRegist->setObjectName("btnRegist");
    m_btnRegist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_btnRegist->setMinimumHeight(48);
    m_btnRegist->setAutoDefault(false);
    m_btnRegist->setDefault(false);
    m_btnRegist->setFocusPolicy(Qt::NoFocus);

    // 버튼 레이아웃 (두 버튼 폭 동일)
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_btnQR);
    btnLayout->addWidget(m_btnRegist);
    btnLayout->setSpacing(16);
    btnLayout->setStretch(0, 1);
    btnLayout->setStretch(1, 1);

    // 버튼 컨테이너 (카메라 폭에 맞춰 정렬)
    QWidget* btnContainer = new QWidget(this);
    btnContainer->setObjectName("btnContainer");
    btnContainer->setMaximumWidth(960);
    btnContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto* btnContainerLayout = new QHBoxLayout(btnContainer);
    btnContainerLayout->setContentsMargins(0, 0, 0, 0);
    btnContainerLayout->addLayout(btnLayout);

    // 내용 레이아웃(카메라+가이드+버튼)
    QVBoxLayout* contentLayout = new QVBoxLayout;
    contentLayout->addWidget(m_cameraView, 0, Qt::AlignHCenter);
    contentLayout->addWidget(m_guideLabel, 0, Qt::AlignHCenter);
    contentLayout->addWidget(btnContainer, 0, Qt::AlignHCenter);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // 최종 배치 (상하 대칭 여백)
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch();
    mainLayout->setContentsMargins(0, 0, 0, 60);

    // 시그널 - 슬롯 연결
    connect(m_btnQR, &QPushButton::clicked, this, [=]{
        emit request({PageId::QR});
    });

    connect(m_btnRegist, &QPushButton::clicked, this, [=]{
        emit request({PageId::RegistInfo});
    });

    m_toast = new Toast(this);
}

void HomePage::updateLabel(const QImage& img)
{
    QPixmap pixMap = QPixmap::fromImage(img);
    m_cameraView->setPixmap(
        pixMap.scaled(m_cameraView->size(),
                      Qt::KeepAspectRatio,
                      Qt::SmoothTransformation));
}

void HomePage::updateGuide(const QString& str)
{
    m_guideLabel->setText(str);
}

void HomePage::setToast(const QString& str)
{
    m_toast->showText(str);
}

void HomePage::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    emit setCamMode(true);
    emit setCamTimer(true);
}

void HomePage::hideEvent(QHideEvent* e)
{
    QWidget::hideEvent(e);
    QString str = tr("화면을 바라보세요");
    updateGuide(str);
    emit setCamTimer(false);
}

HomePage::~HomePage() {}
