#ifndef REMAINBTN_H
#define REMAINBTN_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QString>
#include <QHBoxLayout>

class RemainBtn : public QWidget
{
    Q_OBJECT

public:
    RemainBtn(QWidget* parent = nullptr);
    void startTimer();
    void stopTimer();
    ~RemainBtn();

signals:
    void finished();

private:
    QPushButton* m_btn;
    QTimer* m_timer;
    int m_remainSec;
};

#endif // REMAINBTN_H
