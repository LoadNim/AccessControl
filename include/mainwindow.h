#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

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

private slots:
    void changePage(const PageRequest& req);
    void frameBroker(const QImage& frame);

private:
    Camera*         m_pCamera;
    HomePage*       m_pHomePage;
    QRPage*         m_pQRPage;
    RegistInfo*     m_pRegistInfo;
    RegistCam*      m_pRegistCam;

    QWidget*        m_pPages[4];       // StackedWidget 초기화용
    QStackedWidget* m_pStackedWidget;
};
#endif // MAINWINDOW_H
