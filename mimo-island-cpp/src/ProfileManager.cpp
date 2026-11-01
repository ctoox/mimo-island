#include "ProfileManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>

QJsonObject CookieProfile::toJson() const
{
    QJsonObject obj;
    obj["name"] = name;
    obj["cookie_str"] = cookieStr;
    obj["created_at"] = createdAt.toString(Qt::ISODate);
    obj["last_used"] = lastUsed.toString(Qt::ISODate);
    obj["last_error"] = lastError;
    return obj;
}

CookieProfile CookieProfile::fromJson(const QJsonObject &json)
{
    CookieProfile profile;
    profile.name = json["name"].toString();
    profile.cookieStr = json["cookie_str"].toString();
    profile.createdAt = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    profile.lastUsed = QDateTime::fromString(json["last_used"].toString(), Qt::ISODate);
    profile.lastError = json["last_error"].toString();
    return profile;
}

ProfileManager::ProfileManager()
    : m_activeIndex(0)
{
    m_configDir = QDir::homePath() + "/.mimo_monitor";
    m_profilesFile = m_configDir + "/profiles.json";
    m_stateFile = m_configDir + "/state.json";
    load();
}

void ProfileManager::load()
{
    QFile file(m_profilesFile);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray profilesArray = root["profiles"].toArray();
    
    m_profiles.clear();
    for (const QJsonValue &value : profilesArray) {
        m_profiles.append(CookieProfile::fromJson(value.toObject()));
    }
    
    m_activeIndex = root["active_index"].toInt(0);
    if (m_activeIndex >= m_profiles.size()) {
        m_activeIndex = qMax(0, m_profiles.size() - 1);
    }
}

void ProfileManager::save()
{
    QDir dir;
    dir.mkpath(m_configDir);
    
    QJsonObject root;
    QJsonArray profilesArray;
    
    for (const CookieProfile &profile : m_profiles) {
        profilesArray.append(profile.toJson());
    }
    
    root["profiles"] = profilesArray;
    root["active_index"] = m_activeIndex;
    
    QFile file(m_profilesFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

int ProfileManager::add(const QString &name, const QString &cookieStr)
{
    CookieProfile profile;
    profile.name = name;
    profile.cookieStr = cookieStr;
    profile.createdAt = QDateTime::currentDateTime();
    
    m_profiles.append(profile);
    m_activeIndex = m_profiles.size() - 1;
    save();
    
    return m_activeIndex;
}

void ProfileManager::remove(int index)
{
    if (index < 0 || index >= m_profiles.size()) {
        return;
    }
    
    m_profiles.removeAt(index);
    if (m_activeIndex >= m_profiles.size()) {
        m_activeIndex = qMax(0, m_profiles.size() - 1);
    }
    save();
}

void ProfileManager::rename(int index, const QString &newName)
{
    if (index < 0 || index >= m_profiles.size()) {
        return;
    }
    
    m_profiles[index].name = newName;
    save();
}

CookieProfile *ProfileManager::getActive()
{
    if (m_profiles.isEmpty() || m_activeIndex < 0 || m_activeIndex >= m_profiles.size()) {
        return nullptr;
    }
    return &m_profiles[m_activeIndex];
}

void ProfileManager::setActive(int index)
{
    if (index >= 0 && index < m_profiles.size()) {
        m_activeIndex = index;
        save();
    }
}

void ProfileManager::markError(const QString &errorMsg)
{
    CookieProfile *profile = getActive();
    if (profile) {
        profile->lastError = errorMsg;
        save();
    }
}

void ProfileManager::markSuccess()
{
    CookieProfile *profile = getActive();
    if (profile) {
        profile->lastUsed = QDateTime::currentDateTime();
        profile->lastError.clear();
        save();
    }
}
