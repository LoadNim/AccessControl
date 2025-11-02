#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>
#include <QTimer>
#include <QThread>
#include <QDebug>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class NetWork : public QObject
{
    Q_OBJECT
public:
    explicit NetWork(QObject* parent = nullptr);
    ~NetWork();

public slots:
    // 워커 초기화/정리 (스레드 소속에서 호출됨)
    void threadInit();                    // QNetworkAccessManager 생성
    void shutdown();                      // 진행 중 요청 abort

    // === API: JSON만 보내는 엔드포인트 ===
    void postQrGenerate(const QString& phoneNumber,
                        const QString& purpose,
                        const QString& deviceId,
                        int validTime = 3);

    void postInfer(const QByteArray& webpBase64,
                   const QString& deviceId);

    // === API: 멀티파트(이미 WebP 바이트가 준비된 버전) ===
    void postAccessEvent(const QJsonObject& metadata,
                         const QByteArray& webpImage);

    void postRegistration(const QJsonObject& metadata,
                          const QList<QByteArray>& webpImages);

    // === API: Mat을 직접 받아 인코딩까지 워커 스레드에서 처리하는 버전 ===
    void postInferMat(const cv::Mat& bgrFrame, const QString& deviceId,
                      int webpQuality = 80);
    void postAccessEventMat(const QJsonObject& metadata, const cv::Mat& bgrFrame,
                            int webpQuality = 80);
    void postRegistrationMat(const QJsonObject& metadata, const QList<cv::Mat>& bgrFrames,
                             int webpQuality = 80);

signals:
    // QR
    void qrSucceeded(QByteArray body, int httpStatus);
    void qrFailed(QString error, int httpStatus);

    // 유사도 판정
    void inferSucceeded(double similarity, QString bestId, int httpStatus);
    void inferFailed(QString error, int httpStatus);

    // 출입 로깅
    void accessLogged(QByteArray body, int httpStatus);
    void accessFailed(QString error, int httpStatus);

    // 등록
    void registrationDone(QByteArray body, int httpStatus);
    void registrationFailed(QString error, int httpStatus);

private:
    // 공통 빌더/유틸
    QNetworkRequest makeJsonRequest(const QUrl& url) const;
    QNetworkRequest makeMultipartRequest(const QUrl& url) const;
    void attachTimeout(QNetworkRequest& req) const;        // Qt 6.5+ 네이티브 타임아웃
    void armReplyTimeout(QNetworkReply* reply) const;      // 모든 버전에서 동작하는 fallback

    static QHttpPart makeJsonPart(const QByteArray& jsonBytes,
                                  const QByteArray& name = "metadata");
    static QHttpPart makeWebPPart(const QByteArray& webpBytes,
                                  const QByteArray& name,
                                  const QByteArray& filename);

    void connectFinished(QNetworkReply* reply,
                         std::function<void(int, QByteArray)> onOk,
                         std::function<void(int, QString)> onErr);

    // 인코딩 유틸(워커 스레드에서 호출)
    static QByteArray encodeWebP(const cv::Mat& bgr, int quality);
    static QByteArray toBase64(const QByteArray& bytes);

private:
    QNetworkAccessManager* m_net = nullptr;

    // 서버 베이스 URL
    const QString m_baseQr  = QStringLiteral("http://qr.pssuai.com");
    const QString m_baseApi = QStringLiteral("http://api.pssuai.com");
    const QString m_baseAi  = QStringLiteral("http://ai.pssuai.com"); // 프록시(HTTPS 사용 권장)

    // 네트워크 정책
    int m_timeoutMs = 15000; // 요청 타임아웃(ms)
};

#endif // NETWORK_H
