#ifndef DTO_H
#define DTO_H

#pragma once
#include <QString>
#include <QJsonObject>

struct Info {
    QString dong;
    QString ho;
    QString phone;
    QString purpose;
    int     sendtype = 0;   // 0: 기본, 1: ... (UI의 모드 인덱스 그대로)

    // 구조체 → JSON
    QJsonObject toJson() const {
        QJsonObject j;
        if (!dong.isEmpty())    j["dong"] = dong;
        if (!ho.isEmpty())      j["ho"] = ho;
        if (!phone.isEmpty())   j["phone"] = phone;
        if (!purpose.isEmpty()) j["purpose"] = purpose;
        j["sendtype"] = sendtype;
        return j;
    }

    // (옵션) JSON → 구조체 (서버 응답 파싱용)
    static Info fromJson(const QJsonObject& j) {
        Info i;
        i.dong     = j.value("dong").toString();
        i.ho       = j.value("ho").toString();
        i.phone    = j.value("phone").toString();
        i.purpose  = j.value("purpose").toString();
        i.sendtype = j.value("sendtype").toInt();
        return i;
    }
};

#endif // DTO_H
