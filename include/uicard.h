#ifndef UICARD_H
#define UICARD_H

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QPoint>

class UiCard : public QFrame
{
    Q_OBJECT

public:
    UiCard(QWidget* parent);
    ~UiCard();

private:
    int     m_radius;
    int     m_blur;
    QPoint  m_offset;
    QColor  m_shadowColor;
};

#endif // UICARD_H
