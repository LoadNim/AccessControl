#include "registcam.h"

RegistCam::RegistCam(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("registCamPage");

    m_title = new QLabel(tr("세대주 등록하기"), this);
    m_title->setObjectName("registCamTitle");

    m_cameraView = new QLabel(this);
    m_cameraView->setObjectName("cameraView");
    m_cameraView->setFixedSize(960, 540);
    m_cameraView->setAlignment(Qt::AlignCenter);
    m_cameraView->setScaledContents(false);
    m_cameraView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_guideLabel = new QLabel(tr("화면을 바라보세요"), this);
    m_guideLabel->setObjectName("guideLabel");
    m_guideLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_guideLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_guideLabel->setMinimumHeight(42);

    m_btnBack = new QPushButton(tr("이전 화면"), this);
    m_btnBack->setObjectName("btnBack");
    m_btnBack->setProperty("kind", "ghost");
    m_btnBack->setAutoDefault(false);
    m_btnBack->setDefault(false);
    m_btnBack->setFocusPolicy(Qt::NoFocus);

    m_btnSend = new QPushButton(tr("등록 완료"), this);
    m_btnSend->setObjectName("btnSend");
    m_btnSend->setProperty("kind", "primary");
    m_btnSend->setAutoDefault(false);
    m_btnSend->setDefault(false);
    m_btnSend->setFocusPolicy(Qt::NoFocus);

    m_remainBtn = new RemainBtn(this);
    m_toast = new Toast(this);

    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 6, 20, 0);
    topLayout->setSpacing(0);

    topLayout->addWidget(m_btnBack, 0, Qt::AlignLeft);
    topLayout->addStretch(1);
    topLayout->addWidget(m_title, 0, Qt::AlignCenter);
    topLayout->addStretch(1);
    topLayout->addWidget(m_btnSend, 0, Qt::AlignRight);

    QVBoxLayout* contentLayout = new QVBoxLayout;
    contentLayout->addWidget(m_cameraView, 0, Qt::AlignHCenter);
    contentLayout->addWidget(m_guideLabel, 0, Qt::AlignHCenter);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* footer = new QHBoxLayout();
    footer->setContentsMargins(0, 8, 0, 0);
    footer->addStretch(1);
    footer->addWidget(m_remainBtn, 0, Qt::AlignRight | Qt::AlignBottom);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 8, 24, 18);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(topBar);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(footer);

    connect(m_btnBack, &QPushButton::clicked, this, [=]{
        emit request({PageId::RegistInfo});
    });

    connect(m_btnSend, &QPushButton::clicked, this, [=]{
        bool trigger;
        emit(isAllowSend(trigger));

        if(trigger)
        {
            SendTrigger info{true};
            connect(m_toast, &Toast::finish, this, [=]{
                emit request(PageRequest{PageId::Home, PageData{info}});
            }, Qt::SingleShotConnection);

            m_toast->showText(tr("등록 요청이 전송되었습니다."));
        }
        else
        {
            m_toast->showText(tr("얼굴 사진 20장이 인식되지 않았습니다."), 1800);
            return;
        }
    });

    connect(m_remainBtn, &RemainBtn::finished, this, [=]{
        m_toast->showText(tr("초기 화면으로 돌아갑니다."), 1800);
        QTimer::singleShot(1800, this, [=](){
            emit request(PageRequest{PageId::Home});
        });
    });
}

void RegistCam::updateLabel(const QImage& img)
{
    QPixmap pixMap = QPixmap::fromImage(img);
    m_cameraView->setPixmap(
        pixMap.scaled(m_cameraView->size(),
                      Qt::KeepAspectRatio,
                      Qt::SmoothTransformation));
}

void RegistCam::updateGuide(const QString& str)
{
    m_guideLabel->setText(str);
}

void RegistCam::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    if(m_remainBtn)
    {
        m_remainBtn->startTimer();
    }
    emit setCamMode(false);
}

void RegistCam::hideEvent(QHideEvent* e)
{
    QWidget::hideEvent(e);
    if(m_remainBtn)
    {
        QString str = tr("화면을 바라보세요.");
        updateGuide(str);
        m_remainBtn->stopTimer();
    }
}

RegistCam::~RegistCam() {}
