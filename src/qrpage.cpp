#include "qrpage.h"

QRPage::QRPage(QWidget *parent)
    : QWidget(parent)
{
    m_btnBack = new QPushButton("이전 화면", this);
    m_btnBack->setObjectName("btnBack");
    m_title = new QLabel("방문자 QR 코드 발급", this);
    m_title->setObjectName("qrTitle");
    QWidget* spacer = new QWidget(this);
    spacer->setFixedWidth(m_btnBack->sizeHint().width());

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget(m_btnBack);
    topLayout->addWidget(m_title, 1, Qt::AlignCenter);
    topLayout->addWidget(spacer);

    m_purposeTitle = new QLabel("방문 목적", this);
    m_purposeTitle->setObjectName("purposeTitle");
    m_purposeBtnGroup = new QButtonGroup(this);
    m_purposeBtnGroup->setExclusive(false);
    QGridLayout* purposeBtnLayout = new QGridLayout;
    QStringList purposes = {"택배", "배달", "지인", "친척",
                           "과외", "소독", "수리", "그 외"};

    for(int i = 0; i < purposes.size(); ++i)
    {
        QPushButton* btn = new QPushButton(purposes[i], this);
        btn->setProperty("purpose", true);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_purposeBtnGroup->addButton(btn);
        purposeBtnLayout->addWidget(btn, i / 2, i % 2);
    }

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(m_purposeTitle, 0, Qt::AlignCenter);
    leftLayout->addLayout(purposeBtnLayout, 1);

    m_phoneLineEdit = new QLineEdit(this);
    m_phoneLineEdit->setPlaceholderText("전화번호를 입력하세요");
    m_phoneLineEdit->setReadOnly(true);
    m_phoneLineEdit->setObjectName("phoneLineEdit");
    m_btnSend = new QPushButton("전송", this);
    m_btnSend->setObjectName("btnSend");

    QHBoxLayout* phoneInputLayout = new QHBoxLayout;
    phoneInputLayout->addWidget(m_phoneLineEdit, 1);
    phoneInputLayout->addWidget(m_btnSend);
    phoneInputLayout->setSpacing(0);

    m_pKeyPad = new KeyPad(this);
    m_pKeyPad->setMaximumWidth(450);

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addLayout(phoneInputLayout);
    rightLayout->addWidget(m_pKeyPad, 1);

    QHBoxLayout* contentLayout = new QHBoxLayout;
    contentLayout->addLayout(leftLayout, 4);
    contentLayout->addLayout(rightLayout, 6);

    QHBoxLayout* horizontalWrapper = new QHBoxLayout;
    horizontalWrapper->addStretch();
    horizontalWrapper->addLayout(contentLayout);
    horizontalWrapper->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(horizontalWrapper, 1);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

QRPage::~QRPage() {}
