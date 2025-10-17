#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "metatype.h"
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

private:
    // 서브 위젯 포인터
    HomePage*       m_pHomePage;
    QRPage*         m_pQRPage;
    RegistInfo*     m_pRegistInfo;
    RegistCam*      m_pRegistCam;

    QWidget*        m_pPages[4];       // StackedWidget 초기화용
    QStackedWidget* m_pStackedWidget;  // 위젯이 표시될 영역
};
#endif // MAINWINDOW_H
