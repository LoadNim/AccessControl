#include "toast.h"

Toast::Toast(QWidget* parent)
    : QWidget(parent)
{
    m_label = new QLabel(this);
    m_timer = new QTimer(this);
    m_opacity = new QGraphicsOpacityEffect(this);
    m_fadeIn = new QPropertyAnimation(m_opacity, "opacity", this);
    m_fadeOut = new QPropertyAnimation(m_opacity, "opacity", this);

    setObjectName("toast");
    setAttribute(Qt::WA_StyledBackground, true);
    setFocusPolicy(Qt::NoFocus);
    setVisible(false);
    setGraphicsEffect(m_opacity);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 16, 24, 16);
    layout->addWidget(m_label, 0, Qt::AlignCenter);

    m_label->setObjectName("toastLabel");
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setWordWrap(true);

    m_fadeIn->setDuration(150);
    m_fadeIn->setStartValue(0.0);
    m_fadeIn->setEndValue(1.0);

    m_fadeOut->setDuration(180);
    m_fadeOut->setStartValue(1.0);
    m_fadeOut->setEndValue(0.0);

    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, [this]{
        m_fadeOut->start();
    });

    connect(m_fadeOut, &QPropertyAnimation::finished, this, [this]{
        setVisible(false);
        emit finish();
    });
}

void Toast::showText(const QString& text, int time)
{
    m_label->setText(text);
    adjustSizeAndCenter();
    setVisible(true);
    m_opacity->setOpacity(0.0);
    m_fadeIn->start();
    m_timer->start(time);
    raise();
}

void Toast::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    adjustSizeAndCenter();
}

void Toast::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    if(isVisible())
    {
        adjustSizeAndCenter();
    }
}

void Toast::adjustSizeAndCenter()
{
    if(!parentWidget())
    {
        return;
    }

    adjustSize();
    const QSize ps = parentWidget()->size();
    const QSize me = size();
    move((ps.width() - me.width()) / 2, (ps.height() - me.height()) / 2);
}

Toast::~Toast() {}
