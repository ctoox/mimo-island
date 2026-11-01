#pragma once

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QJsonObject>

struct CookieProfile
{
    QString name;
    QString cookieStr;
    QDateTime createdAt;
    QDateTime lastUsed;
    QString lastError;
    
    QJsonObject toJson() const;
    static CookieProfile fromJson(const QJsonObject &json);
};

class ProfileManager
{
public:
    ProfileManager();
    
    void load();
    void save();
    
    int add(const QString &name, const QString &cookieStr);
    void remove(int index);
    void rename(int index, const QString &newName);
    
    CookieProfile *getActive();
    int activeIndex() const { return m_activeIndex; }
    void setActive(int index);
    
    const QVector<CookieProfile> &profiles() const { return m_profiles; }
    
    void markError(const QString &errorMsg);
    void markSuccess();
    
private:
    QVector<CookieProfile> m_profiles;
    int m_activeIndex;
    QString m_configDir;
    QString m_profilesFile;
    QString m_stateFile;
};
