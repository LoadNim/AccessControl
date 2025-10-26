#include "remainbtn.h"

RemainBtn::RemainBtn(QWidget* parent)
    : QWidget(parent)
{
    m_remainSec = 60;
    m_btn = new QPushButton(tr("남은 시간 60s"), this);
    m_timer = new QTimer(this);

    m_btn->setEnabled(false);
    m_btn->setFocusPolicy(Qt::NoFocus);
    m_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_btn->setFixedHeight(40);
    m_btn->setObjectName("remainBtn");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_btn, 0, Qt::AlignCenter);

    m_timer->setInterval(1000);

    connect(m_timer, &QTimer::timeout, this, [=]{
        --m_remainSec;
        QString btnStr = "남은 시간 " + QString::number(m_remainSec) + "s";
        m_btn->setText(btnStr);

        if(!m_remainSec)
        {
            m_timer->stop();
            emit finished();
        }
    });
}

void RemainBtn::startTimer()
{
    m_timer->start();
}

void RemainBtn::stopTimer()
{
    m_remainSec = 60;
    m_btn->setText(tr("남은 시간 60s"));
    m_timer->stop();
}

RemainBtn::~RemainBtn() {}
