#include "registinfo.h"

RegistInfo::RegistInfo(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("registInfoPage");

    // 상단 타이틀/버튼
    m_title = new QLabel(tr("세대주 등록하기"), this);
    m_title->setObjectName("registInfoTitle");

    m_btnBack = new QPushButton(tr("이전 화면"), this);
    m_btnBack->setObjectName("btnBack");
    m_btnBack->setProperty("kind", "ghost");

    m_btnNext = new QPushButton(tr("다음"), this);
    m_btnNext->setObjectName("btnNext");
    m_btnNext->setProperty("kind", "primary");
    m_btnNext->setAutoDefault(false);
    m_btnNext->setDefault(false);
    m_btnNext->setFocusPolicy(Qt::NoFocus);

    // 중앙 카드
    m_uiCard = new UiCard(this);               // objectName = "uiCard" (uicard 내부)
    m_uiCard->setMinimumWidth(960);
    m_uiCard->setMaximumWidth(1280);

    // 카드 내부 입력 필드
    m_editDong = new QLineEdit(m_uiCard);
    m_editDong->setObjectName("editDong");
    m_editDong->setPlaceholderText(tr("동을 입력하세요"));
    m_editDong->installEventFilter(this);
    m_editDong->setReadOnly(true);

    m_editHo = new QLineEdit(m_uiCard);
    m_editHo->setObjectName("editHo");
    m_editHo->setPlaceholderText(tr("호수를 입력하세요"));
    m_editHo->installEventFilter(this);
    m_editHo->setReadOnly(true);

    m_editPhone = new QLineEdit(m_uiCard);
    m_editPhone->setObjectName("editPhone");
    m_editPhone->setPlaceholderText(tr("전화번호를 입력하세요"));
    m_editPhone->installEventFilter(this);
    m_editPhone->setReadOnly(true);

    m_keyPad = new KeyPad(m_uiCard);
    m_keyPad->setObjectName("registInfoKeyPad");

    // 카드 레이아웃
    QVBoxLayout* cardLayout = new QVBoxLayout(m_uiCard);
    cardLayout->setContentsMargins(26, 22, 26, 22);
    cardLayout->setSpacing(24);
    cardLayout->addWidget(m_editDong);
    cardLayout->addWidget(m_editHo);
    cardLayout->addWidget(m_editPhone);
    cardLayout->addWidget(m_keyPad, 0, Qt::AlignHCenter);

    // 우하단 잔여시간 배지
    m_remainBtn = new RemainBtn(this);
    QHBoxLayout* footer = new QHBoxLayout();
    footer->setContentsMargins(0, 8, 0, 0);
    footer->addStretch(1);
    footer->addWidget(m_remainBtn, 0, Qt::AlignRight | Qt::AlignBottom);

    // 상단 바 (좌:뒤로 / 가운데:제목 / 우:자리맞춤)
    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 6, 20, 0);
    topLayout->setSpacing(0);

    topLayout->addWidget(m_btnBack, 0, Qt::AlignLeft);
    topLayout->addStretch(1);
    topLayout->addWidget(m_title, 0, Qt::AlignCenter);
    topLayout->addStretch(1);
    topLayout->addWidget(m_btnNext, 0, Qt::AlignRight);

    // 루트 레이아웃
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 8, 24, 18);
    root->setSpacing(10);
    root->addWidget(topBar);
    root->addSpacing(10);
    root->addWidget(m_uiCard, 0, Qt::AlignHCenter);
    root->addStretch(1);
    root->addLayout(footer);

    m_toast = new Toast(this);
    setActiveEdit(m_editDong);

    connect(m_btnBack, &QPushButton::clicked, this, [=]{
        clearPage();
        emit request({PageId::Home});
    });

    connect(m_btnNext, &QPushButton::clicked, this, [=]{
        static const QRegularExpression reDong("^[1-9]\\d{2,3}$");
        static const QRegularExpression reHo("^[1-9]\\d{2,3}$");

        if(!reDong.match(m_editDong->text()).hasMatch())
        {
            m_toast->showText(tr("유효한 동을 입력하세요"), 1800);
            return;
        }

        if(!reHo.match(m_editHo->text()).hasMatch())
        {
            m_toast->showText(tr("유효한 호수를 입력하세요"), 1800);
            return;
        }

        if(!isValidKrMobile(m_editPhone->text()))
        {
            m_toast->showText(tr("유효한 전화번호를 입력하세요"), 1800);
            return;
        }

        RegisterInfo info{m_editDong->text(), m_editHo->text(), m_editPhone->text()};
        connect(m_toast, &Toast::finish, this, [=]{
            clearPage();
            emit request(PageRequest{PageId::RegistCam, PageData{info}});
        }, Qt::SingleShotConnection);
    });

    connect(m_keyPad, &KeyPad::keyClicked, this, [=](const QString& key){
        QString currentString = m_activeEdit->text();
        if(key == "←")
        {
            currentString.chop(1);
        }
        else
        {
            currentString.append(key);
        }
        m_activeEdit->setText(currentString);
    });

    connect(m_remainBtn, &RemainBtn::finished, this, [=]{
        m_toast->showText(tr("초기 화면으로 돌아갑니다."), 1800);
        QTimer::singleShot(1800, this, [=](){
            emit request(PageRequest{PageId::Home});
        });
    });
}

void RegistInfo::clearPage()
{
    m_editDong->clear();
    m_editHo->clear();
    m_editPhone->clear();
}

void RegistInfo::setActiveEdit(QLineEdit* edit)
{
    if(m_activeEdit == edit)
    {
        return;
    }

    auto mark = [](QLineEdit* le, bool on)
    {
        if(!le)
        {
            return;
        }

        le->setProperty("active", on);
        le->style()->unpolish(le);
        le->style()->polish(le);
        le->update();
    };

    mark(m_editDong, false);
    mark(m_editHo, false);
    mark(m_editPhone, false);

    m_activeEdit = edit;
    mark(m_activeEdit, true);
}

void RegistInfo::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    if(m_remainBtn)
    {
        m_remainBtn->startTimer();
    }
}

void RegistInfo::hideEvent(QHideEvent* e)
{
    QWidget::hideEvent(e);
    if(m_remainBtn)
    {
        m_remainBtn->stopTimer();
    }
}

bool RegistInfo::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::FocusIn)
    {
        if(watched == m_editDong)
        {
            setActiveEdit(m_editDong);
        }
        else if(watched == m_editHo)
        {
            setActiveEdit(m_editHo);
        }
        else if(watched == m_editPhone)
        {
            setActiveEdit(m_editPhone);
        }
    }

    return QWidget::eventFilter(watched, event);
}

RegistInfo::~RegistInfo() {}
