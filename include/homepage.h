#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>

#include "metatype.h"
#include "toast.h"

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(QWidget* parent = nullptr);
    ~HomePage();
    void updateLabel(const QImage& img);
    void updateGuide(const QString& str);
    void setToast(const QString& str);

signals:
    void request(PageRequest req);
    void setCamMode(bool Mode);
    void setCamTimer(bool Mode);

protected:
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;

private:
    QLabel*         m_cameraView;
    QLabel*         m_guideLabel;
    QPushButton*    m_btnQR;
    QPushButton*    m_btnRegist;
    Toast*          m_toast;
};

#endif // HOMEPAGE_H
