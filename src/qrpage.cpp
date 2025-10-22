#include "qrpage.h"

QRPage::QRPage(QWidget* parent)
    : QWidget(parent)
{
    // ===== 멤버 위젯(요청한 변수명 유지) =====
    m_btnBack = new QPushButton(tr("이전 화면"), this);
    m_btnBack->setObjectName("btnBack");

    QLabel* title = new QLabel(tr("방문자 QR 코드 발급"), this);
    title->setObjectName("qrTitle");

    m_purposeBtnGroup = new QButtonGroup(this);
    m_purposeBtnGroup->setExclusive(true);

    m_phoneLineEdit = new QLineEdit(this);
    m_phoneLineEdit->setObjectName("phoneLineEdit");
    m_phoneLineEdit->setPlaceholderText(tr("전화번호를 입력하세요"));

    m_btnSend = new QPushButton(tr("전송"), this);
    m_btnSend->setObjectName("btnSend");

    m_pKeyPad = new KeyPad(this);
    m_pKeyPad->setObjectName("qrKeyPad");
    m_pKeyPad->setMaximumWidth(300); // 웹 시안의 max-width 300px 감성

    // ===== 상단 바 (타이틀 완전 중앙) =====
    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 6, 20, 0);   // 살짝 타이트
    topLayout->setSpacing(0);

    // 좌/우 더미 스페이서로 타이틀을 정확히 중앙 정렬
    QWidget* leftSpacer = new QWidget(topBar);
    QWidget* rightSpacer = new QWidget(topBar);
    int backWidth = m_btnBack->sizeHint().width();
    leftSpacer->setFixedWidth(backWidth);
    rightSpacer->setFixedWidth(backWidth);

    topLayout->addWidget(m_btnBack, 0, Qt::AlignLeft);
    topLayout->addStretch(1);
    topLayout->addWidget(title, 0, Qt::AlignCenter);
    topLayout->addStretch(1);
    topLayout->addWidget(rightSpacer, 0, Qt::AlignRight);

    // ===== 카드 컨테이너 (흰 배경 + 라운드 + 그림자) =====
    QFrame* card = new QFrame(this);
    card->setObjectName("qrCard");

    QGraphicsDropShadowEffect* cardShadow = new QGraphicsDropShadowEffect(card);
    cardShadow->setBlurRadius(18.0);     // 24 → 18 (덜 뜨게)
    cardShadow->setOffset(0.0, 3.0);     // 4 → 3
    cardShadow->setColor(QColor(0, 0, 0, 64));
    card->setGraphicsEffect(cardShadow);

    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(26, 22, 26, 22);  // 28,24 → 26,22
    cardLayout->setSpacing(24);

    // ===== 좌측 패널 (방문 목적) =====
    QWidget* leftPanel = new QWidget(card);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setMaximumWidth(480); // 과확장 방지(시안 밸런스용)

    QVBoxLayout* left = new QVBoxLayout(leftPanel);
    left->setContentsMargins(0, 0, 0, 0);
    left->setSpacing(14);

    QLabel* purposeTitle = new QLabel(tr("방문 목적"), leftPanel);
    purposeTitle->setObjectName("purposeTitle");
    left->addWidget(purposeTitle, 0, Qt::AlignHCenter);

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
        btn->setMinimumSize(116, 50);          // 살짝 컴팩트
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_purposeBtnGroup->addButton(btn, i);
        int r = i / 2;
        int c = i % 2;
        purposeGrid->addWidget(btn, r, c);
    }
    left->addLayout(purposeGrid);
    left->addStretch(1);

    // ===== 세로 구분선(상/하 8px 끊김) =====
    QFrame* divider = new QFrame(card);
    divider->setObjectName("divider");
    divider->setFrameShape(QFrame::VLine);
    divider->setLineWidth(2);

    QWidget* divWrap = new QWidget(card);
    QVBoxLayout* divLay = new QVBoxLayout(divWrap);
    divLay->setContentsMargins(0, 8, 0, 8);  // 위/아래 여백으로 선을 살짝 끊어줌
    divLay->addWidget(divider);

    // ===== 우측 패널 (전화 입력 + 키패드) =====
    QWidget* rightPanel = new QWidget(card);
    rightPanel->setObjectName("rightPanel");

    QVBoxLayout* right = new QVBoxLayout(rightPanel);
    right->setContentsMargins(4, 0, 0, 0);   // 좌 4px만(시작 위치 맞춤)
    right->setSpacing(12);                   // 16 → 12

    // 입력줄 + 전송 버튼
    QWidget* phoneLine = new QWidget(rightPanel);
    QHBoxLayout* phoneLayout = new QHBoxLayout(phoneLine);
    phoneLayout->setContentsMargins(0, 0, 0, 0);
    phoneLayout->setSpacing(0);

    m_phoneLineEdit->setFixedHeight(40); // 동일 고정 높이로 정렬 맞춤
    m_btnSend->setFixedHeight(40);

    phoneLayout->addWidget(m_phoneLineEdit, 1);
    phoneLayout->addWidget(m_btnSend, 0);

    right->addWidget(phoneLine);
    right->addWidget(m_pKeyPad, 0, Qt::AlignLeft);
    right->addStretch(1);

    // ===== 카드 내부 구성 =====
    cardLayout->addWidget(leftPanel);
    cardLayout->addWidget(divWrap);    // divider 대신 래퍼를 삽입
    cardLayout->addWidget(rightPanel);
    cardLayout->setStretch(0, 4);      // 좌 : 우 = 4 : 6
    cardLayout->setStretch(2, 6);

    // 카드의 가로폭 한계(시안: 90% / max 1280 느낌)
    card->setMinimumWidth(960);
    card->setMaximumWidth(1280);

    // ===== 루트 레이아웃 =====
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 8, 24, 18);  // 전체 여백 미세 조정
    root->setSpacing(10);                     // 기본 간격 축소
    root->addWidget(topBar);
    root->addSpacing(10);                     // 제목 ↔ 카드 고정 간격
    root->addWidget(card, 0, Qt::AlignHCenter);
    root->addStretch(1);

    // ※ 시그널/슬롯 연결은 의도적으로 제외 (요청사항)
}

QRPage::~QRPage() {}
