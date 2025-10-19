#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile f(":/qss/app.qss");
    qDebug() << "QSS exists?" << f.exists();
    if (f.open(QFile::ReadOnly)) {
        const auto qss = QString::fromUtf8(f.readAll());
        qApp->setStyleSheet(qss);
        qDebug() << "QSS length:" << qss.size();
    } else {
        qDebug() << "QSS open failed";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
