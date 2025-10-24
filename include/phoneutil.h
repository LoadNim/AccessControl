#ifndef PHONEUTIL_H
#define PHONEUTIL_H

#include <QString>
#include <QRegularExpression>

inline bool isValidKrMobile(const QString& s)
{
    static const QRegularExpression re(R"(^(010\d{8}|01[16789]\d{7,8})$)");
    return re.match(s).hasMatch();
}

#endif // PHONEUTIL_H
