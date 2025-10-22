#include "keypad.h"

KeyPad::KeyPad(QWidget* parent)
    : QWidget(parent)
{
    m_pBtnGroup = new QButtonGroup(this);
    QGridLayout* keypadLayout = new QGridLayout(this);
    keypadLayout->setSpacing(8);
    keypadLayout->setContentsMargins(0, 0, 0, 0);

    QStringList keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "←"};

    for(int i = 0; i < keys.size(); ++i)
    {
        QPushButton* btn = new QPushButton(keys[i], this);

        btn->setProperty("keypad", true);
        btn->setProperty("kind", (keys[i] == "←") ? "back" : "digit");
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setAutoDefault(false);
        btn->setDefault(false);
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        btn->setMinimumSize(100, 56);

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

    connect(m_pBtnGroup, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn){
        emit keyClicked(btn->text());
    });
}

KeyPad::~KeyPad() {}
