#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "metatype.h"

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(QWidget* parent = nullptr);
    void updateLabel(const QImage& img);
    ~HomePage();

signals:
    void request(PageRequest req);

private:
    QLabel*         m_cameraView;
    QLabel*         m_guideLabel;
    QPushButton*    m_btnQR;
    QPushButton*    m_btnRegist;
};

#endif // HOMEPAGE_H
