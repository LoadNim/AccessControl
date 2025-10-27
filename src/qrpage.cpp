#include "qrpage.h"

QRPage::QRPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("qrPage"); // QSS 스코프 분리

    // 멤버 위젯(요청한 변수명 유지)
    m_btnBack = new QPushButton(tr("이전 화면"), this);
    m_btnBack->setObjectName("btnBack");
    m_btnBack->setProperty("kind", "ghost");

    m_title = new QLabel(tr("방문자 QR 코드 발급"), this);
    m_title->setObjectName("qrTitle");

    m_purposeBtnGroup = new QButtonGroup(this);
    m_purposeBtnGroup->setExclusive(true);

    m_phoneLineEdit = new QLineEdit(this);
    m_phoneLineEdit->setObjectName("phoneLineEdit");
    m_phoneLineEdit->setPlaceholderText(tr("전화번호를 입력하세요"));
    m_phoneLineEdit->setReadOnly(true);

    m_btnSend = new QPushButton(tr("전송"), this);
    m_btnSend->setObjectName("btnSend");
    m_btnSend->setProperty("kind", "primary");
    m_btnSend->setAutoDefault(false);
    m_btnSend->setDefault(false);
    m_btnSend->setFocusPolicy(Qt::NoFocus);

    m_pKeyPad = new KeyPad(this);
    m_pKeyPad->setObjectName("qrKeyPad");

    // 상단 바 (타이틀 완전 중앙)
    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 6, 20, 0);
    topLayout->setSpacing(0);

    // 좌/우 더미 스페이서로 타이틀을 정확히 중앙 정렬
    QWidget* leftSpacer = new QWidget(topBar);
    QWidget* rightSpacer = new QWidget(topBar);
    int backWidth = m_btnBack->sizeHint().width();
    leftSpacer->setFixedWidth(backWidth);
    rightSpacer->setFixedWidth(backWidth);

    topLayout->addWidget(m_btnBack, 0, Qt::AlignLeft);
    topLayout->addStretch(1);
    topLayout->addWidget(m_title, 0, Qt::AlignCenter);
    topLayout->addStretch(1);
    topLayout->addWidget(rightSpacer, 0, Qt::AlignRight);

    // 카드 컨테이너 (흰 배경 + 라운드 + 그림자)
    QFrame* card = new QFrame(this);
    card->setObjectName("qrCard");

    QGraphicsDropShadowEffect* cardShadow = new QGraphicsDropShadowEffect(card);
    cardShadow->setBlurRadius(18.0);
    cardShadow->setOffset(0.0, 3.0);
    cardShadow->setColor(QColor(0, 0, 0, 64));
    card->setGraphicsEffect(cardShadow);

    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(26, 22, 26, 22);
    cardLayout->setSpacing(24);

    // 좌측 패널 (방문 목적)
    QWidget* leftPanel = new QWidget(card);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setMaximumWidth(480);

    QVBoxLayout* left = new QVBoxLayout(leftPanel);
    left->setContentsMargins(0, 0, 0, 0);
    left->setSpacing(14);

    m_purposeTitle = new QLabel(tr("방문 목적"), leftPanel);
    m_purposeTitle->setObjectName("purposeTitle");
    left->addWidget(m_purposeTitle, 0, Qt::AlignHCenter);

    QGridLayout* purposeGrid = new QGridLayout();
    purposeGrid->setHorizontalSpacing(12);
    purposeGrid->setVerticalSpacing(14);

    QStringList purposes;
    purposes << tr("택배") << tr("배달") << tr("지인") << tr("친척")
             << tr("과외") << tr("소독") << tr("수리") << tr("그 외");
    for (int i = 0; i < purposes.size(); ++i) {
        QPushButton* btn = new QPushButton(purposes.at(i), leftPanel);
        btn->setCheckable(true);
        btn->setProperty("purpose", true);     // QSS 선택자
        btn->setFixedSize(100, 50);            // 시안 일치 (QSS도 동일 min 규칙)
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_purposeBtnGroup->addButton(btn, i);
        int r = i / 2;
        int c = i % 2;
        purposeGrid->addWidget(btn, r, c);
        m_purposeBtns.append(btn);
    }
    left->addLayout(purposeGrid);
    left->addStretch(1);

    // 세로 구분선(상/하 8px 끊김)
    QFrame* divider = new QFrame(card);
    divider->setObjectName("divider");
    divider->setFrameShape(QFrame::VLine);
    divider->setLineWidth(2);

    QWidget* divWrap = new QWidget(card);
    QVBoxLayout* divLay = new QVBoxLayout(divWrap);
    divLay->setContentsMargins(0, 8, 0, 8);
    divLay->addWidget(divider);

    // 우측 패널 (전화 입력 + 키패드)
    QWidget* rightPanel = new QWidget(card);
    rightPanel->setObjectName("rightPanel");

    QVBoxLayout* right = new QVBoxLayout(rightPanel);
    right->setContentsMargins(0, 0, 0, 0);
    right->setSpacing(12);

    QFrame* phoneGroup = new QFrame(rightPanel);
    phoneGroup->setObjectName("phoneGroup");

    QHBoxLayout* phoneGroupLayout = new QHBoxLayout(phoneGroup);
    phoneGroupLayout->setContentsMargins(0, 0, 0, 0);
    phoneGroupLayout->setSpacing(0);
    m_phoneLineEdit->setFixedHeight(44);
    m_btnSend->setFixedHeight(44);

    phoneGroupLayout->addWidget(m_phoneLineEdit, 1);
    phoneGroupLayout->addWidget(m_btnSend, 0);

    right->addWidget(phoneGroup, 0, Qt::AlignTop);
    right->addSpacing(10);
    right->addWidget(m_pKeyPad, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    right->addStretch(1);

    // 카드 내부 구성
    cardLayout->addWidget(leftPanel);
    cardLayout->addWidget(divWrap);
    cardLayout->addWidget(rightPanel);
    cardLayout->setStretch(0, 4);
    cardLayout->setStretch(2, 6);

    card->setMinimumWidth(960);
    card->setMaximumWidth(1280);

    // 남은 시간 안내용
    m_remainBtn = new RemainBtn(this);
    QHBoxLayout* footer = new QHBoxLayout();
    footer->setContentsMargins(0, 8, 0, 0);
    footer->addStretch(1);
    footer->addWidget(m_remainBtn, 0, Qt::AlignRight | Qt::AlignBottom);

    // 루트 레이아웃
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 8, 24, 18);
    root->setSpacing(10);
    root->addWidget(topBar);
    root->addSpacing(10);
    root->addWidget(card, 0, Qt::AlignHCenter);
    root->addStretch(1);
    root->addLayout(footer);

    // 토스트 메시지
    m_toast = new Toast(this);

    connect(m_btnBack, &QPushButton::clicked, this, [=]{
        clearPage();
        emit request({PageId::Home});
    });

    connect(m_btnSend, &QPushButton::clicked, this, [=]{
        QPushButton* p = qobject_cast<QPushButton*>(m_purposeBtnGroup->checkedButton());
        if(!p)
        {
            m_toast->showText(tr("방문 목적을 선택하세요."), 1800);
            return;
        }

        if(!isValidKrMobile(m_phoneLineEdit->text()))
        {
            m_toast->showText(tr("유효한 전화번호를 입력하세요."), 1800);
            return;
        }

        QRInfo info{p->text(), m_phoneLineEdit->text()};
        connect(m_toast, &Toast::finish, this, [=]{
            emit request(PageRequest{PageId::Home, PageData{info}});
        }, Qt::SingleShotConnection);

        m_toast->showText(tr("QR 코드가 발급되었습니다. 휴대폰을 확인하세요."), 1800);
    });

    connect(m_pKeyPad, &KeyPad::keyClicked, this, [=](const QString& key){
        QString currentPhoneNumber = m_phoneLineEdit->text();
        if(key == "←")
        {
            currentPhoneNumber.chop(1);
        }
        else
        {
            currentPhoneNumber.append(key);
        }
        m_phoneLineEdit->setText(currentPhoneNumber);
    });
}

void QRPage::clearPage()
{
    if(m_purposeBtnGroup)
    {
        m_purposeBtnGroup->setExclusive(false);
        for(QPushButton* p : m_purposeBtns)
        {
            if(p)
            {
                p->setChecked(false);
            }
        }
        m_purposeBtnGroup->setExclusive(true);
    }

    if(m_phoneLineEdit)
    {
        m_phoneLineEdit->clear();
        m_phoneLineEdit->setCursorPosition(0);
    }
}

void QRPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if(m_remainBtn)
    {
        m_remainBtn->startTimer();
    }
}

void QRPage::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);

    if(m_remainBtn)
    {
        m_remainBtn->stopTimer();
    }
}

QRPage::~QRPage() {}
