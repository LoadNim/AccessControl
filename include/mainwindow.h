#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QThread>

#include "metatype.h"
#include "camera.h"

#include "homepage.h"
#include "qrpage.h"
#include "registinfo.h"
#include "registcam.h"

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
};
#endif // MAINWINDOW_H
