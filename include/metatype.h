#ifndef METATYPE_H
#define METATYPE_H

#include <QString>
#include <QMetaType>
#include <variant>
#include <opencv2/opencv.hpp>

// 위젯 식별자
enum class PageId {Home, QR, RegistInfo, RegistCam};

// 전송 데이터 타입
struct QRInfo
{
    QString purpose;
    QString phone;
};
Q_DECLARE_METATYPE(QRInfo)

struct RegisterInfo
{
    QString dong;
    QString ho;
    QString phone;
};
Q_DECLARE_METATYPE(RegisterInfo)

using PageData =  std::variant<
                    std::monostate,
                    QRInfo,
                    RegisterInfo>;
Q_DECLARE_METATYPE(PageData)

//위젯 전환 요청
struct PageRequest
{
    PageId id;
    PageData data = std::monostate{};
};
Q_DECLARE_METATYPE(PageRequest)

#endif // METATYPE_H
