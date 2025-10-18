#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

#include "metatype.h"

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(QWidget* parent = nullptr);
    ~HomePage();

signals:
    void request(PageRequest req);

private:


};

#endif // HOMEPAGE_H
