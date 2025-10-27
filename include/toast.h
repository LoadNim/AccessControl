#ifndef TOAST_H
#define TOAST_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>

class Toast : public QWidget
{
    Q_OBJECT

public:
    Toast(QWidget* parent = nullptr);
    void showText(const QString& text, int time = 1800);
    ~Toast();

signals:
    void finish();

protected:
    void showEvent(QShowEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void adjustSizeAndCenter();

    QLabel*                 m_label;
    QTimer*                 m_timer;
    QGraphicsOpacityEffect* m_opacity;
    QPropertyAnimation*     m_fadeIn;
    QPropertyAnimation*     m_fadeOut;
};

#endif // TOAST_H
