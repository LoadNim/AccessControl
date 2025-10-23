#include "keypad.h"

KeyPad::KeyPad(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("keyPad"); // QSS 스코프 분리

    m_pBtnGroup = new QButtonGroup(this);

    QGridLayout* keypadLayout = new QGridLayout(this);
    keypadLayout->setSpacing(12);
    keypadLayout->setVerticalSpacing(12);
    keypadLayout->setContentsMargins(8, 12, 8, 12);

    QStringList keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "←"};

    for(int i = 0; i < keys.size(); ++i)
    {
        QPushButton* btn = new QPushButton(keys[i], this);

        btn->setProperty("keypad", true);
        btn->setProperty("kind", (keys[i] == "←") ? "back" : "digit");
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setAutoDefault(false);
        btn->setDefault(false);

        btn->setFixedSize(80, 60);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        m_btns.append(btn);
        m_pBtnGroup->addButton(btn);

        if(keys[i] == "0")
        {
            keypadLayout->addWidget(btn, 3, 1);
        }
        else if(keys[i] == "←")
        {
            keypadLayout->addWidget(btn, 3, 2);
        }
        else
        {
            keypadLayout->addWidget(btn, (i / 3), (i % 3));
        }
    }
    setLayout(keypadLayout);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setMinimumSize(8 + 3*80 + 2*12 + 8,
                         12 + 4*60  + 3*12 + 12);

    connect(m_pBtnGroup, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn){
        emit keyClicked(btn->text());
    });
}

KeyPad::~KeyPad() {}
