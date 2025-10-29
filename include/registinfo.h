#ifndef REGISTINFO_H
#define REGISTINFO_H

#include <QWidget>
#include <QLineEdit>
#include <QEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpression>
#include <QStyle>

#include "metatype.h"
#include "uicard.h"
#include "keypad.h"
#include "remainbtn.h"
#include "toast.h"
#include "phoneutil.h"

class RegistInfo : public QWidget
{
    Q_OBJECT

public:
    RegistInfo(QWidget* parent = nullptr);
    ~RegistInfo();
    void clearPage();
    void setActiveEdit(QLineEdit* edit);

signals:
    void request(PageRequest req);

protected:
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QLabel*         m_title;
    QPushButton*    m_btnBack;
    QPushButton*    m_btnNext;
    QLineEdit*      m_editDong;
    QLineEdit*      m_editHo;
    QLineEdit*      m_editPhone;
    QLineEdit*      m_activeEdit;

    UiCard*         m_uiCard;
    KeyPad*         m_keyPad;
    RemainBtn*      m_remainBtn;
    Toast*          m_toast;
};

#endif // REGISTINFO_H
