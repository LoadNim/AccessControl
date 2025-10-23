#ifndef QRPAGE_H
#define QRPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <QList>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QButtonGroup>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>
#include <QRegularExpression>
#include <QMessageBox>

#include "keypad.h"
#include "metatype.h"

class QRPage : public QWidget
{
    Q_OBJECT

public:
    QRPage(QWidget* parent = nullptr);
    void clearPage();
    ~QRPage();

signals:
    void request(PageRequest req);

private:
    QPushButton*        m_btnBack;
    QLabel*             m_title;
    QLabel*             m_purposeTitle;
    QButtonGroup*       m_purposeBtnGroup;
    QList<QPushButton*> m_purposeBtns;
    QLineEdit*          m_phoneLineEdit;
    QPushButton*        m_btnSend;
    KeyPad*             m_pKeyPad;
};

#endif // QRPAGE_H
