#ifndef REGISTCAM_H
#define REGISTCAM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>

#include "metatype.h"
#include "camera.h"
#include "remainbtn.h"
#include "toast.h"

class RegistCam : public QWidget
{
    Q_OBJECT

public:
    RegistCam(QWidget* parent = nullptr);
    ~RegistCam();
    void updateLabel(const QImage& img);
    void updateGuide(const QString& str);

signals:
    void request(PageRequest req);
    void isAllowSend(bool& trigger);
    void setCamMode(bool mode);
    void setCamTimer(bool mode);

protected:
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;

private:
    QLabel*         m_cameraView;
    QLabel*         m_guideLabel;
    QLabel*         m_title;
    QPushButton*    m_btnBack;
    QPushButton*    m_btnSend;

    RemainBtn*      m_remainBtn;
    Toast*          m_toast;
};

#endif // REGISTCAM_H
