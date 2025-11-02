#include "network.h"
#include <QJsonArray>

NetWork::NetWork(QObject* parent)
    : QObject(parent)
{
}

NetWork::~NetWork()
{
}

void NetWork::threadInit()
{
    if (!m_net) {
        m_net = new QNetworkAccessManager(this);
        qDebug() << "[NET] QNetworkAccessManager created in thread:" << QThread::currentThread();
    }
}

void NetWork::shutdown()
{
    const auto children = this->findChildren<QNetworkReply*>();
    for (QNetworkReply* r : children) {
        if (r && r->isRunning()) r->abort();
    }
}

// ====================== 공통 빌더 ======================

void NetWork::attachTimeout(QNetworkRequest& req) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    // Qt 6.5+ 에서만 네이티브 요청 타임아웃
    req.setTransferTimeout(m_timeoutMs);
#else
    Q_UNUSED(req);
#endif
}

QNetworkRequest NetWork::makeJsonRequest(const QUrl& url) const
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    attachTimeout(req);
    return req;
}

QNetworkRequest NetWork::makeMultipartRequest(const QUrl& url) const
{
    QNetworkRequest req(url);
    attachTimeout(req);
    return req;
}

QHttpPart NetWork::makeJsonPart(const QByteArray& jsonBytes, const QByteArray& name)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
                   QString("form-data; name=\"%1\"").arg(QString::fromUtf8(name)));
    part.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    part.setBody(jsonBytes);
    return part;
}

QHttpPart NetWork::makeWebPPart(const QByteArray& webpBytes,
                                const QByteArray& name,
                                const QByteArray& filename)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
                   QString("form-data; name=\"%1\"; filename=\"%2\"")
                       .arg(QString::fromUtf8(name), QString::fromUtf8(filename)));
    part.setHeader(QNetworkRequest::ContentTypeHeader, "image/webp");
    part.setBody(webpBytes);
    return part;
}

void NetWork::armReplyTimeout(QNetworkReply* reply) const
{
    if (!reply || m_timeoutMs <= 0) return;

    // reply의 스레드에서 안전하게 abort()를 호출하도록 예약
    QTimer::singleShot(m_timeoutMs, reply, [reply](){
        if (reply->isRunning()) {
            qDebug() << "[DEBUG] Timeout reached, aborting reply for" << reply->url();
            reply->abort();
        }
    });
}

void NetWork::connectFinished(QNetworkReply* reply,
                              std::function<void(int, QByteArray)> onOk,
                              std::function<void(int, QString)> onErr)
{
    // 디버깅 보조 로그
    qDebug() << "[DEBUG] Reply started:" << reply->url().toString();
    QObject::connect(reply, &QNetworkReply::readyRead, this, [reply]{
        qDebug() << "[DEBUG] readyRead(), bytesAvailable=" << reply->bytesAvailable();
    });
    QObject::connect(reply, &QNetworkReply::errorOccurred, this,
                     [reply](QNetworkReply::NetworkError code){
                         qDebug() << "[DEBUG] errorOccurred:" << code << reply->errorString();
                     });

    QObject::connect(reply, &QNetworkReply::finished, this, [reply, onOk, onErr]{
        qDebug() << "[DEBUG] finished(), error=" << reply->error() << reply->errorString();
        const int http = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() == QNetworkReply::NoError) {
            onOk(http, reply->readAll());
        } else {
            onErr(http, reply->errorString());
        }
        reply->deleteLater();
    });
}

// ====================== 인코딩 유틸 ======================

QByteArray NetWork::encodeWebP(const cv::Mat& bgr, int quality)
{
    if (bgr.empty()) return QByteArray();
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_WEBP_QUALITY, quality };
    bool ok = cv::imencode(".webp", bgr, buf, params);
    if (!ok || buf.empty()) return QByteArray();
    return QByteArray(reinterpret_cast<const char*>(buf.data()),
                      static_cast<int>(buf.size()));
}

QByteArray NetWork::toBase64(const QByteArray& bytes)
{
    return bytes.toBase64(QByteArray::Base64Encoding);
}

// ====================== API 구현 ======================

// 1) QR 생성: POST http://qr.pssuai.com/generate-qr  (application/json)
void NetWork::postQrGenerate(const QString& phoneNumber,
                             const QString& purpose,
                             const QString& deviceId,
                             int validTime)
{
    if (!m_net) return;

    QUrl url(m_baseQr + "/generate-qr");
    QNetworkRequest req = makeJsonRequest(url);

    QJsonObject obj;
    obj.insert("phoneNumber", phoneNumber);
    obj.insert("validTime", validTime);
    obj.insert("purpose", purpose);
    obj.insert("device_id", deviceId);

    QByteArray body = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    qDebug() << "[DEBUG] POST /generate-qr"
             << "URL:" << url.toString()
             << "Thread:" << QThread::currentThread()
             << "Body size:" << body.size();

    QNetworkReply* reply = m_net->post(req, body);
    armReplyTimeout(reply);

    connectFinished(reply,
                    [this](int http, QByteArray data){ emit qrSucceeded(data, http); },
                    [this](int http, QString err){ emit qrFailed(err, http); }
                    );
}

// 2) 유사도 판정: POST https://ai.pssuai.com/infer  (application/json)
void NetWork::postInfer(const QByteArray& webpBase64, const QString& deviceId)
{
    if (!m_net) return;

    // 외부 공개는 프록시(HTTPS)로 열린 상태라고 가정
    QUrl url(m_baseAi + "/infer");
    QNetworkRequest req = makeJsonRequest(url);

    QJsonObject obj;
    obj.insert("device_id", deviceId);
    obj.insert("image", QString::fromUtf8(webpBase64)); // 순수 base64 문자열

    QByteArray body = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    qDebug() << "[DEBUG] POST /infer";
    qDebug() << "URL:" << url.toString();
    qDebug() << "Thread:" << QThread::currentThread();
    qDebug() << "Body size:" << body.size() << "bytes";

    QNetworkReply* reply = m_net->post(req, body);
    armReplyTimeout(reply);

    qDebug() << "[DEBUG] Reply created:" << reply << "parent:" << reply->parent();

    connectFinished(reply,
                    // --- onOk ---
                    [this](int http, QByteArray data)
                    {
                        qDebug() << "[DEBUG] infer finished HTTP:" << http;
                        qDebug() << "[DEBUG] infer raw body:" << QString::fromUtf8(data);

                        QJsonParseError perr{};
                        const QJsonDocument doc = QJsonDocument::fromJson(data, &perr);
                        if (perr.error != QJsonParseError::NoError || !doc.isObject())
                        {
                            emit inferFailed(QStringLiteral("Invalid JSON"), http);
                            return;
                        }

                        const QJsonObject obj = doc.object();
                        const double similarity = obj.value("similarity").toDouble(0.0);
                        const QString bestId = obj.value("best_id").toString();
                        emit inferSucceeded(similarity, bestId, http);
                    },
                    // --- onErr ---
                    [this](int http, QString err)
                    {
                        qDebug() << "[DEBUG] infer error:" << http << err;
                        emit inferFailed(err, http);
                    }
                    );
}

// 3) 출입 이벤트: POST http://api.pssuai.com/access-events  (multipart: metadata + image)
void NetWork::postAccessEvent(const QJsonObject& metadata,
                              const QByteArray& webpImage)
{
    if (!m_net) return;

    QUrl url(m_baseApi + "/access-events");
    QNetworkRequest req = makeMultipartRequest(url);

    QHttpMultiPart* mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    const QByteArray jsonBytes = QJsonDocument(metadata).toJson(QJsonDocument::Compact);

    mp->append(makeJsonPart(jsonBytes, "metadata"));
    mp->append(makeWebPPart(webpImage, "image", "frame_000.webp"));

    qDebug() << "[DEBUG] POST /access-events"
             << "URL:" << url.toString()
             << "Thread:" << QThread::currentThread()
             << "meta bytes:" << jsonBytes.size()
             << "image bytes:" << webpImage.size();

    QNetworkReply* reply = m_net->post(req, mp);
    mp->setParent(reply);
    armReplyTimeout(reply);

    connectFinished(reply,
                    [this](int http, QByteArray data){ emit accessLogged(data, http); },
                    [this](int http, QString err){ emit accessFailed(err, http); }
                    );
}

// 4) 등록: POST https://ai.pssuai.com/register  (multipart: metadata + images[])
void NetWork::postRegistration(const QJsonObject& metadata,
                               const QList<QByteArray>& webpImages)
{
    if (!m_net) return;

    QUrl url(m_baseAi + "/register");
    QNetworkRequest req = makeMultipartRequest(url);

    QHttpMultiPart* mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    const QByteArray jsonBytes = QJsonDocument(metadata).toJson(QJsonDocument::Compact);
    mp->append(makeJsonPart(jsonBytes, "metadata"));

    for (int i = 0; i < webpImages.size(); ++i) {
        const QByteArray filename = QByteArray("frame_")
        + QByteArray::number(i).rightJustified(3, '0')
            + ".webp";
        mp->append(makeWebPPart(webpImages[i], "images", filename));
    }

    qDebug() << "[DEBUG] POST /register"
             << "URL:" << url.toString()
             << "Thread:" << QThread::currentThread()
             << "meta bytes:" << jsonBytes.size()
             << "count(images):" << webpImages.size();

    QNetworkReply* reply = m_net->post(req, mp);
    mp->setParent(reply);
    armReplyTimeout(reply);

    connectFinished(reply,
                    [this](int http, QByteArray data){ emit registrationDone(data, http); },
                    [this](int http, QString err){ emit registrationFailed(err, http); }
                    );
}

// ====================== Mat 오버로드 ======================

void NetWork::postInferMat(const cv::Mat& bgrFrame, const QString& deviceId, int webpQuality)
{
    const QByteArray webp = encodeWebP(bgrFrame, webpQuality);
    if (webp.isEmpty()) { emit inferFailed(QStringLiteral("WebP encode failed"), 0); return; }
    const QByteArray b64 = toBase64(webp);
    postInfer(b64, deviceId);
}

void NetWork::postAccessEventMat(const QJsonObject& metadata, const cv::Mat& bgrFrame, int webpQuality)
{
    const QByteArray webp = encodeWebP(bgrFrame, webpQuality);
    if (webp.isEmpty()) { emit accessFailed(QStringLiteral("WebP encode failed"), 0); return; }
    postAccessEvent(metadata, webp);
}

void NetWork::postRegistrationMat(const QJsonObject& metadata, const QList<cv::Mat>& bgrFrames, int webpQuality)
{
    QList<QByteArray> webps;
    webps.reserve(bgrFrames.size());
    for (const cv::Mat& m : bgrFrames) {
        const QByteArray w = encodeWebP(m, webpQuality);
        if (w.isEmpty()) {
            emit registrationFailed(QStringLiteral("WebP encode failed"), 0);
            return;
        }
        webps.push_back(w);
    }
    postRegistration(metadata, webps);
}
