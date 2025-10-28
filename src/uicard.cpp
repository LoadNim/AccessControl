#include "uicard.h"

UiCard::UiCard(QWidget* parent)
{
    m_radius = 12;
    m_blur = 18;
    m_offset = QPoint(0, 3);
    m_shadowColor = QColor(0, 0, 0, 64);

    setObjectName("uiCard");
    setContentsMargins(0, 0, 0, 0);
    setFocusPolicy(Qt::NoFocus);

    if(graphicsEffect())
    {
        delete graphicsEffect();
    }

    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect(this);
    eff->setBlurRadius(m_blur);
    eff->setOffset(m_offset);
    eff->setColor(m_shadowColor);
    setGraphicsEffect(eff);
}

UiCard::~UiCard() {}
