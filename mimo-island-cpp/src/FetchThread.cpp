#include "FetchThread.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QTimer>
#include <QEventLoop>

FetchThread::FetchThread(ProfileManager *profileManager, QObject *parent)
    : QThread(parent)
    , m_profileManager(profileManager)
    , m_networkManager(nullptr)
{
}

void FetchThread::run()
{
    CookieProfile *profile = m_profileManager->getActive();
    if (!profile) {
        emit dataFetched(createError("请添加Cookie"));
        return;
    }
    
    m_networkManager = new QNetworkAccessManager();
    
    QNetworkRequest request;
    request.setUrl(QUrl("https://platform.xiaomimimo.com/api/v1/tokenPlan/usage"));
    request.setRawHeader("accept", "*/*");
    request.setRawHeader("accept-language", "zh");
    request.setRawHeader("content-type", "application/json");
    request.setRawHeader("x-timezone", "Asia/Shanghai");
    request.setRawHeader("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/148.0.0.0 Safari/537.36");
    request.setRawHeader("referer", "https://platform.xiaomimimo.com/console/plan-manage");
    request.setRawHeader("cookie", profile->cookieStr.toUtf8());
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    // 等待响应，最多10秒
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(10000);
    
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.exec();
    
    if (timer.isActive()) {
        // 正常完成
        timer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            TokenData data = parseResponse(response);
            if (data.ok) {
                m_profileManager->markSuccess();
            }
            emit dataFetched(data);
        } else if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
            m_profileManager->markError("401");
            emit dataFetched(createError("Cookie已过期", 401));
        } else {
            QString errorMsg = reply->errorString().left(20);
            m_profileManager->markError(errorMsg);
            emit dataFetched(createError(errorMsg));
        }
    } else {
        // 超时
        reply->abort();
        m_profileManager->markError("连接超时");
        emit dataFetched(createError("连接超时"));
    }
    
    reply->deleteLater();
    m_networkManager->deleteLater();
    m_networkManager = nullptr;
}

TokenData FetchThread::parseResponse(const QByteArray &response)
{
    TokenData data;
    
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (doc.isNull()) {
        return createError("JSON解析失败");
    }
    
    QJsonObject root = doc.object();
    QJsonObject dataObj = root["data"].toObject();
    
    // 解析月度使用数据
    QJsonObject monthUsage = dataObj["monthUsage"].toObject();
    QJsonArray monthItems = monthUsage["items"].toArray();
    
    for (const QJsonValue &item : monthItems) {
        QJsonObject obj = item.toObject();
        if (obj["name"].toString() == "month_total_token") {
            data.monthUsed = obj["used"].toInteger();
            data.monthTotal = obj["limit"].toInteger();
            data.monthPercent = obj["percent"].toDouble() * 100;
            break;
        }
    }
    
    // 解析套餐和补偿数据
    QJsonObject usage = dataObj["usage"].toObject();
    QJsonArray usageItems = usage["items"].toArray();
    
    for (const QJsonValue &item : usageItems) {
        QJsonObject obj = item.toObject();
        QString name = obj["name"].toString();
        
        if (name == "plan_total_token") {
            data.planUsed = obj["used"].toInteger();
            data.planTotal = obj["limit"].toInteger();
            data.planPercent = obj["percent"].toDouble() * 100;
        } else if (name == "compensation_total_token") {
            data.compUsed = obj["used"].toInteger();
            data.compTotal = obj["limit"].toInteger();
            data.compPercent = obj["percent"].toDouble() * 100;
        }
    }
    
    data.ok = true;
    return data;
}

TokenData FetchThread::createError(const QString &message, int errorCode)
{
    TokenData data;
    data.ok = false;
    data.message = message;
    data.errorCode = errorCode;
    return data;
}
