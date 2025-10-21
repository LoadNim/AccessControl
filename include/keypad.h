#ifndef KEYPAD_H
#define KEYPAD_H

#include <QWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QGridLayout>
#include <QStringList>

class KeyPad : public QWidget
{
    Q_OBJECT

public:
    KeyPad(QWidget* parent = nullptr);
    ~KeyPad();

signals:
    void keyClicked(const QString& key);

private:
    QButtonGroup*       m_pBtnGroup;
    QList<QPushButton*> m_btns;
};

#endif // KEYPAD_H
