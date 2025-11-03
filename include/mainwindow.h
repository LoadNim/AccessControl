#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <QList>

#include "metatype.h"
#include "camera.h"
#include "homepage.h"
#include "qrpage.h"
#include "registinfo.h"
#include "registcam.h"
#include "network.h"
#include "dto.h"
#include <QHostInfo>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void frameConsumed();

private slots:
    void changePage(const PageRequest& req);
    void frameBroker(const QImage& frame);

private:
    Camera*                 m_pCamera;
    HomePage*               m_pHomePage;
    QRPage*                 m_pQRPage;
    RegistInfo*             m_pRegistInfo;
    RegistCam*              m_pRegistCam;

    QWidget*                m_pPages[4];
    QStackedWidget*         m_pStackedWidget;

    std::vector<cv::Mat>    m_faceImg;
    RegisterInfo            m_registerInfo;

    QThread*                m_cameraThread = nullptr;
    void                    stopCameraThread();

    QThread*                m_networkThread = nullptr;
    NetWork*                m_pNetwork = nullptr;
    qint64                  m_lastInferMs = 0;
    int                     m_inferIntervalMs = 1500;
    const QString           deviceId = QStringLiteral("LoadNim_PC");

    cv::Mat                 m_entryImg;

    QJsonObject             buildEnvelope(const Info& info) const;
};
#endif // MAINWINDOW_H
