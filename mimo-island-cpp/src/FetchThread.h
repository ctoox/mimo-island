#pragma once

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "ProfileManager.h"

struct TokenData
{
    qint64 planUsed = 0;
    qint64 planTotal = 0;
    double planPercent = 0;
    qint64 compUsed = 0;
    qint64 compTotal = 0;
    double compPercent = 0;
    qint64 monthUsed = 0;
    qint64 monthTotal = 0;
    double monthPercent = 0;
    bool ok = false;
    QString message;
    int errorCode = 0;
};

class FetchThread : public QThread
{
    Q_OBJECT
    
public:
    explicit FetchThread(ProfileManager *profileManager, QObject *parent = nullptr);
    
signals:
    void dataFetched(const TokenData &data);
    
protected:
    void run() override;
    
private:
    ProfileManager *m_profileManager;
    QNetworkAccessManager *m_networkManager;
    
    TokenData parseResponse(const QByteArray &response);
    TokenData createError(const QString &message, int errorCode = 0);
};
