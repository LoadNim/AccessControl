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

struct ResisterInfo
{
    QString dong;
    QString ho;
    QString phone;
};
Q_DECLARE_METATYPE(ResisterInfo)

using PageData =  std::variant<
                    std::monostate,
                    QRInfo,
                    ResisterInfo>;
Q_DECLARE_METATYPE(PageData)

//위젯 전환 요청
struct PageRequest
{
    PageId id;
    PageData data = std::monostate{};
};

#endif // METATYPE_H
