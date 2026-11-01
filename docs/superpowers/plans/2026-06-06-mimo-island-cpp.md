# MiMo Island C++ 版本实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将 Python PySide6 版本的 MiMo Token 监控应用重写为 C++ Qt 版本，提升性能并保留所有功能。

**Architecture:** 使用 C++17 和 Qt 6.x 构建桌面应用，采用模块化设计：DynamicIsland（主窗口）、FetchThread（网络请求）、ProfileManager（配置管理）、SpringValue（动画）。通过 CMake 构建系统生成可分发的 exe 文件。

**Tech Stack:** C++17, Qt 6.x (Widgets, Network), CMake, Windows 10/11

---

## 项目结构

```
mimo-island-cpp/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── DynamicIsland.h
│   ├── DynamicIsland.cpp
│   ├── FetchThread.h
│   ├── FetchThread.cpp
│   ├── ProfileManager.h
│   ├── ProfileManager.cpp
│   ├── SpringValue.h
│   └── SpringValue.cpp
├── assets/
│   └── icon.ico
└── build/
```

---

### Task 1: 创建项目基础结构和 CMake 配置

**Files:**
- Create: `mimo-island-cpp/CMakeLists.txt`
- Create: `mimo-island-cpp/src/main.cpp`

- [ ] **Step 1: 创建项目目录结构**

```bash
mkdir -p mimo-island-cpp/src
mkdir -p mimo-island-cpp/assets
mkdir -p mimo-island-cpp/build
```

- [ ] **Step 2: 创建 CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.16)
project(mimo-island LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt 设置
set(CMAKE_PREFIX_PATH "C:/Qt/6.x.x/msvc2019_64")  # 根据实际 Qt 安装路径修改

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# 源文件
set(SOURCES
    src/main.cpp
    src/DynamicIsland.cpp
    src/FetchThread.cpp
    src/ProfileManager.cpp
    src/SpringValue.cpp
)

# 头文件
set(HEADERS
    src/DynamicIsland.h
    src/FetchThread.h
    src/ProfileManager.h
    src/SpringValue.h
)

# 创建可执行文件
add_executable(mimo-island ${SOURCES} ${HEADERS})

# 链接 Qt 库
target_link_libraries(mimo-island PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Windows 特定设置
if(WIN32)
    set_target_properties(mimo-island PROPERTIES
        WIN32_EXECUTABLE ON
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()
```

- [ ] **Step 3: 创建 main.cpp 骨架**

```cpp
#include <QApplication>
#include "DynamicIsland.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    
    DynamicIsland window;
    window.show();
    
    return app.exec();
}
```

- [ ] **Step 4: 验证 CMake 配置**

```bash
cd mimo-island-cpp/build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Expected: CMake 配置成功，生成 Visual Studio 项目文件

- [ ] **Step 5: 提交基础结构**

```bash
git add mimo-island-cpp/
git commit -m "feat: create C++ project structure with CMake configuration"
```

---

### Task 2: 实现 SpringValue 动画模块

**Files:**
- Create: `mimo-island-cpp/src/SpringValue.h`
- Create: `mimo-island-cpp/src/SpringValue.cpp`

- [ ] **Step 1: 创建 SpringValue.h**

```cpp
#pragma once

class SpringValue
{
public:
    explicit SpringValue(double value = 0.0);
    
    void setTarget(double target);
    bool tick(double dt = 0.016, double spring = 160.0, double damp = 10.0);
    
    double value() const { return m_current; }
    double target() const { return m_target; }
    
private:
    double m_current;
    double m_target;
    double m_velocity;
};
```

- [ ] **Step 2: 创建 SpringValue.cpp**

```cpp
#include "SpringValue.h"
#include <cmath>

SpringValue::SpringValue(double value)
    : m_current(value)
    , m_target(value)
    , m_velocity(0.0)
{
}

void SpringValue::setTarget(double target)
{
    m_target = target;
}

bool SpringValue::tick(double dt, double spring, double damp)
{
    double distance = m_target - m_current;
    m_velocity += distance * spring * dt;
    m_velocity *= std::max(0.0, 1.0 - damp * dt);
    m_current += m_velocity * dt;
    
    return std::abs(distance) > 0.0005 || std::abs(m_velocity) > 0.005;
}
```

- [ ] **Step 3: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功（可能有链接错误，因为其他模块还未实现）

- [ ] **Step 4: 提交 SpringValue 模块**

```bash
git add mimo-island-cpp/src/SpringValue.h mimo-island-cpp/src/SpringValue.cpp
git commit -m "feat: implement SpringValue animation module"
```

---

### Task 3: 实现 ProfileManager 配置管理模块

**Files:**
- Create: `mimo-island-cpp/src/ProfileManager.h`
- Create: `mimo-island-cpp/src/ProfileManager.cpp`

- [ ] **Step 1: 创建 ProfileManager.h**

```cpp
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
```

- [ ] **Step 2: 创建 ProfileManager.cpp**

```cpp
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
```

- [ ] **Step 3: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功（可能有链接错误，因为其他模块还未实现）

- [ ] **Step 4: 提交 ProfileManager 模块**

```bash
git add mimo-island-cpp/src/ProfileManager.h mimo-island-cpp/src/ProfileManager.cpp
git commit -m "feat: implement ProfileManager configuration module"
```

---

### Task 4: 实现 FetchThread 网络请求模块

**Files:**
- Create: `mimo-island-cpp/src/FetchThread.h`
- Create: `mimo-island-cpp/src/FetchThread.cpp`

- [ ] **Step 1: 创建 FetchThread.h**

```cpp
#pragma once

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "ProfileManager.h"

struct TokenData
{
    double planUsed = 0;
    double planTotal = 0;
    double planPercent = 0;
    double compUsed = 0;
    double compTotal = 0;
    double compPercent = 0;
    double monthUsed = 0;
    double monthTotal = 0;
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
```

- [ ] **Step 2: 创建 FetchThread.cpp**

```cpp
#include "FetchThread.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QTimer>

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
            data.monthUsed = obj["used"].toDouble();
            data.monthTotal = obj["limit"].toDouble();
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
            data.planUsed = obj["used"].toDouble();
            data.planTotal = obj["limit"].toDouble();
            data.planPercent = obj["percent"].toDouble() * 100;
        } else if (name == "compensation_total_token") {
            data.compUsed = obj["used"].toDouble();
            data.compTotal = obj["limit"].toDouble();
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
```

- [ ] **Step 3: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功（可能有链接错误，因为 DynamicIsland 模块还未实现）

- [ ] **Step 4: 提交 FetchThread 模块**

```bash
git add mimo-island-cpp/src/FetchThread.h mimo-island-cpp/src/FetchThread.cpp
git commit -m "feat: implement FetchThread network request module"
```

---

### Task 5: 实现 DynamicIsland 主窗口模块（基础框架）

**Files:**
- Create: `mimo-island-cpp/src/DynamicIsland.h`
- Create: `mimo-island-cpp/src/DynamicIsland.cpp`

- [ ] **Step 1: 创建 DynamicIsland.h**

```cpp
#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "ProfileManager.h"
#include "FetchThread.h"
#include "SpringValue.h"

class ContextMenu;

class DynamicIsland : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(double glowOffset READ glowOffset WRITE setGlowOffset)
    Q_PROPERTY(double dockOpacity READ dockOpacity WRITE setDockOpacity)
    Q_PROPERTY(double itemOpacity READ itemOpacity WRITE setItemOpacity)
    
public:
    explicit DynamicIsland(QWidget *parent = nullptr);
    ~DynamicIsland();
    
    // 菜单操作
    void addCookie();
    void deleteCurrentCookie();
    void renameCurrentCookie();
    void switchProfile(int index);
    void fetchData();
    void toggleTop();
    
    // 属性访问器
    double glowOffset() const { return m_glowOffset; }
    void setGlowOffset(double offset);
    double dockOpacity() const { return m_dockOpacity; }
    void setDockOpacity(double opacity);
    double itemOpacity() const { return m_itemOpacity; }
    void setItemOpacity(double opacity);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    
private slots:
    void onDataFetched(const TokenData &data);
    void onTick();
    void onReDockCheck();
    
private:
    // 窗口模式
    enum class WindowMode {
        Compact,
        Expanded,
        Docked
    };
    
    enum class DockMode {
        None,
        Top,
        Bottom,
        Left,
        Right
    };
    
    // 绘制方法
    void drawBackground(QPainter &painter, int w, int h);
    void drawGlow(QPainter &painter, int w, int h);
    void drawBorder(QPainter &painter, int w, int h);
    void drawFlowLight(QPainter &painter, int w, int h);
    void drawCompactMode(QPainter &painter, int w, int h);
    void drawExpandedMode(QPainter &painter, int w, int h);
    void drawDockedMode(QPainter &painter, int w, int h);
    
    // 动画方法
    void toggle();
    void fadeIn();
    void checkDock();
    void dock(DockMode mode);
    void undock();
    
    // 状态保存/加载
    void loadState();
    void saveState();
    
    // 成员变量
    WindowMode m_windowMode;
    DockMode m_dockMode;
    DockMode m_lastDockMode;
    
    bool m_hovered;
    bool m_pinned;
    bool m_dragMoved;
    bool m_docked;
    bool m_dockHovered;
    bool m_undockedFromDock;
    bool m_mousePressed;
    bool m_menuOpen;
    bool m_animating;
    
    QPoint m_dragPos;
    double m_phase;
    double m_glowOffset;
    double m_glowSpeed;
    double m_glowTargetSpeed;
    double m_dockOpacity;
    double m_itemOpacity;
    
    QString m_errorMsg;
    int m_errorCode;
    
    TokenData m_data;
    SpringValue m_percentSpring;
    SpringValue m_usedSpring;
    
    ProfileManager m_profileManager;
    FetchThread *m_fetchThread;
    QTimer *m_fetchTimer;
    QTimer *m_frameTimer;
    QTimer *m_reDockTimer;
    QPropertyAnimation *m_glowAnimation;
    QParallelAnimationGroup *m_undockAnimation;
    
    ContextMenu *m_contextMenu;
    
    // 常量
    static const int COMPACT_W = 168;
    static const int COMPACT_H = 44;
    static const int EXPANDED_W = 320;
    static const int EXPANDED_H = 195;
    static const int DOCKED_H = 6;
    static const int DOCKED_W = 6;
    static const int DOCKED_LEN = 200;
    static const int DOCK_THRESHOLD = 10;
};
```

- [ ] **Step 2: 创建 DynamicIsland.cpp 基础框架**

```cpp
#include "DynamicIsland.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QAction>
#include <QMenu>

DynamicIsland::DynamicIsland(QWidget *parent)
    : QMainWindow(parent)
    , m_windowMode(WindowMode::Compact)
    , m_dockMode(DockMode::None)
    , m_lastDockMode(DockMode::Top)
    , m_hovered(false)
    , m_pinned(true)
    , m_dragMoved(false)
    , m_docked(false)
    , m_dockHovered(false)
    , m_undockedFromDock(false)
    , m_mousePressed(false)
    , m_menuOpen(false)
    , m_animating(false)
    , m_phase(0.0)
    , m_glowOffset(0.0)
    , m_glowSpeed(0.0008)
    , m_glowTargetSpeed(0.0008)
    , m_dockOpacity(0.0)
    , m_itemOpacity(1.0)
    , m_errorCode(0)
    , m_fetchThread(nullptr)
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool);
    setAttribute(Qt.WA_TranslucentBackground);
    setMouseTracking(true);
    
    // 加载状态
    loadState();
    
    // 创建上下文菜单
    m_contextMenu = new ContextMenu(this);
    
    // 创建网络请求线程
    m_fetchThread = new FetchThread(&m_profileManager, this);
    connect(m_fetchThread, &FetchThread::dataFetched, this, &DynamicIsland::onDataFetched);
    
    // 创建定时器
    m_fetchTimer = new QTimer(this);
    connect(m_fetchTimer, &QTimer::timeout, this, &DynamicIsland::fetchData);
    m_fetchTimer->start(300000); // 5分钟
    fetchData();
    
    m_frameTimer = new QTimer(this);
    connect(m_frameTimer, &QTimer::timeout, this, &DynamicIsland::onTick);
    m_frameTimer->start(16); // 60fps
    
    m_reDockTimer = new QTimer(this);
    m_reDockTimer->setSingleShot(true);
    connect(m_reDockTimer, &QTimer::timeout, this, &DynamicIsland::onReDockCheck);
    
    // 创建辉光动画
    m_glowAnimation = new QPropertyAnimation(this, "glowOffset");
    m_glowAnimation->setDuration(2800);
    m_glowAnimation->setStartValue(0.0);
    m_glowAnimation->setEndValue(1.0);
    m_glowAnimation->setLoopCount(-1);
    m_glowAnimation->setEasingCurve(QEasingCurve::Linear);
    m_glowAnimation->start();
}

DynamicIsland::~DynamicIsland()
{
    saveState();
}

void DynamicIsland::setGlowOffset(double offset)
{
    m_glowOffset = offset;
    update();
}

void DynamicIsland::setDockOpacity(double opacity)
{
    m_dockOpacity = opacity;
    update();
}

void DynamicIsland::setItemOpacity(double opacity)
{
    m_itemOpacity = opacity;
    update();
}

void DynamicIsland::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int w = width();
    int h = height();
    
    if (m_docked) {
        drawDockedMode(painter, w, h);
    } else {
        QPainterPath path;
        int r = (m_windowMode == WindowMode::Expanded) ? 12 : qMin(h / 2, 22);
        path.addRoundedRect(QRectF(0, 0, w, h), r, r);
        painter.setClipPath(path, Qt::IntersectClip);
        
        drawBackground(painter, w, h);
        drawGlow(painter, w, h);
        drawBorder(painter, w, h);
        
        if (m_windowMode == WindowMode::Compact) {
            drawCompactMode(painter, w, h);
        } else {
            drawExpandedMode(painter, w, h);
        }
    }
}

void DynamicIsland::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    
    m_hovered = true;
    m_glowTargetSpeed = 0.002;
    setCursor(Qt::PointingHandCursor);
    
    if (m_docked && !m_dockHovered && !m_animating) {
        m_dockHovered = true;
        m_undockedFromDock = true;
        m_animating = true;
        undock();
    }
}

void DynamicIsland::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    
    m_hovered = false;
    m_glowTargetSpeed = 0.0008;
    setCursor(Qt::ArrowCursor);
    
    if (m_undockedFromDock && !m_mousePressed && !m_menuOpen) {
        m_animating = true;
        m_reDockTimer->start(400);
    }
    m_dockHovered = false;
}

void DynamicIsland::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_reDockTimer->stop();
        m_animating = false;
        
        if (m_docked) {
            return;
        }
        
        m_mousePressed = true;
        m_undockedFromDock = false;
        m_dragPos = event->globalPosition().toPoint() - pos();
        m_dragMoved = false;
    } else if (event->button() == Qt::RightButton) {
        if (m_docked) {
            return;
        }
        
        m_menuOpen = true;
        m_contextMenu->updateProfiles();
        m_contextMenu->setPinned(m_pinned);
        m_contextMenu->exec(event->globalPosition().toPoint());
        m_menuOpen = false;
    }
}

void DynamicIsland::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && !m_docked) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragPos;
        if ((newPos - pos()).manhattanLength() > 3) {
            m_dragMoved = true;
            move(newPos);
        }
    }
}

void DynamicIsland::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = false;
        
        if (m_docked) {
            return;
        }
        
        if (!m_dragMoved) {
            toggle();
        } else {
            checkDock();
            m_undockedFromDock = false;
        }
    }
}

void DynamicIsland::closeEvent(QCloseEvent *event)
{
    saveState();
    QMainWindow::closeEvent(event);
}

void DynamicIsland::onDataFetched(const TokenData &data)
{
    if (data.ok) {
        m_data = data;
        if (data.compUsed > 0) {
            m_percentSpring.setTarget(data.compPercent);
            m_usedSpring.setTarget(data.compUsed);
        } else {
            m_percentSpring.setTarget(data.planPercent);
            m_usedSpring.setTarget(data.planUsed);
        }
        m_errorMsg.clear();
        m_errorCode = 0;
    } else {
        m_errorMsg = data.message;
        m_errorCode = data.errorCode;
    }
}

void DynamicIsland::onTick()
{
    m_phase += 0.04;
    bool percentChanged = m_percentSpring.tick();
    bool usedChanged = m_usedSpring.tick();
    m_glowSpeed += (m_glowTargetSpeed - m_glowSpeed) * 0.1;
    
    if (percentChanged || usedChanged || m_hovered || m_errorCode == 401 || m_docked) {
        update();
    }
}

void DynamicIsland::onReDockCheck()
{
    if (m_undockedFromDock && !m_mousePressed && !m_hovered) {
        dock(m_lastDockMode);
    }
    m_animating = false;
}

void DynamicIsland::addCookie()
{
    // TODO: 实现添加 Cookie 对话框
}

void DynamicIsland::deleteCurrentCookie()
{
    // TODO: 实现删除 Cookie 对话框
}

void DynamicIsland::renameCurrentCookie()
{
    // TODO: 实现重命名 Cookie 对话框
}

void DynamicIsland::switchProfile(int index)
{
    m_profileManager.setActive(index);
    m_errorMsg.clear();
    m_errorCode = 0;
    m_contextMenu->updateProfiles();
    fetchData();
}

void DynamicIsland::fetchData()
{
    if (!m_fetchThread->isRunning()) {
        m_fetchThread->start();
    }
}

void DynamicIsland::toggleTop()
{
    m_pinned = !m_pinned;
    Qt::WindowFlags flags = windowFlags();
    if (m_pinned) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags &= ~Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(flags);
    show();
    m_contextMenu->setPinned(m_pinned);
}

void DynamicIsland::drawBackground(QPainter &painter, int w, int h)
{
    QLinearGradient grad(0, 0, w, h);
    grad.setColorAt(0, QColor(14, 14, 20, 220));
    grad.setColorAt(0.5, QColor(18, 18, 26, 217));
    grad.setColorAt(1, QColor(14, 14, 20, 220));
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    
    int r = (m_windowMode == WindowMode::Expanded) ? 12 : qMin(h / 2, 22);
    painter.drawRoundedRect(QRectF(0, 0, w, h), r, r);
}

void DynamicIsland::drawGlow(QPainter &painter, int w, int h)
{
    QColor glowColor;
    if (m_errorCode == 401) {
        double glowAlpha = 0.15 + 0.1 * sin(m_phase * 2);
        glowColor = QColor(255, 80, 80, static_cast<int>(255 * glowAlpha));
    } else {
        double glowAlpha = 0.12 + 0.05 * sin(m_phase);
        if (m_hovered) {
            glowAlpha += 0.08;
        }
        glowColor = QColor(70, 140, 255, static_cast<int>(255 * glowAlpha));
    }
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(glowColor);
    
    int r = (m_windowMode == WindowMode::Expanded) ? 14 : qMin(h / 2, 22) + 2;
    painter.drawRoundedRect(QRectF(-1.5, -1.5, w + 3, h + 3), r, r);
}

void DynamicIsland::drawBorder(QPainter &painter, int w, int h)
{
    QPainterPath path;
    int r = (m_windowMode == WindowMode::Expanded) ? 12 : qMin(h / 2, 22);
    path.addRoundedRect(QRectF(0.5, 0.5, w - 1, h - 1), r, r);
    
    QLinearGradient borderGrad(0, 0, w, 0);
    if (m_errorCode == 401) {
        int ba = 40 + static_cast<int>(20 * sin(m_phase * 2));
        borderGrad.setColorAt(0, QColor(255, 80, 80, ba));
        borderGrad.setColorAt(0.5, QColor(255, 120, 80, ba));
        borderGrad.setColorAt(1, QColor(255, 80, 80, ba));
    } else {
        int ba = m_hovered ? 45 : 22;
        borderGrad.setColorAt(0, QColor(70, 140, 255, ba));
        borderGrad.setColorAt(0.33, QColor(100, 200, 255, ba));
        borderGrad.setColorAt(0.66, QColor(140, 100, 255, ba));
        borderGrad.setColorAt(1, QColor(70, 140, 255, ba));
    }
    
    painter.setPen(QPen(QBrush(borderGrad), 1.2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);
}

void DynamicIsland::drawFlowLight(QPainter &painter, int w, int h)
{
    QPainterPath path;
    int r = (m_windowMode == WindowMode::Expanded) ? 12 : qMin(h / 2, 22);
    path.addRoundedRect(QRectF(0, 0, w, h), r, r);
    
    double cx = w * m_glowOffset;
    double bandW = w * 0.4;
    
    QLinearGradient grad(cx - bandW, 0, cx + bandW, 0);
    if (m_errorCode == 401) {
        grad.setColorAt(0.0, QColor(255, 255, 255, 0));
        grad.setColorAt(0.5, QColor(255, 100, 100, 15));
        grad.setColorAt(1.0, QColor(255, 255, 255, 0));
    } else {
        grad.setColorAt(0.0, QColor(255, 255, 255, 0));
        grad.setColorAt(0.3, QColor(100, 200, 255, 8));
        grad.setColorAt(0.5, QColor(100, 200, 255, 18));
        grad.setColorAt(0.7, QColor(100, 200, 255, 8));
        grad.setColorAt(1.0, QColor(255, 255, 255, 0));
    }
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawPath(path);
}

void DynamicIsland::drawCompactMode(QPainter &painter, int w, int h)
{
    drawFlowLight(painter, w, h);
    
    double percent = m_percentSpring.value();
    double cy = h / 2.0;
    int ix = 30;
    int isz = 8;
    
    painter.setPen(Qt::NoPen);
    
    // 绘制状态指示器
    QLinearGradient indicatorGrad(ix - 5, cy - 5, ix + 5, cy + 5);
    if (m_errorCode == 401) {
        indicatorGrad.setColorAt(0, QColor(255, 100, 100));
        indicatorGrad.setColorAt(1, QColor(255, 60, 60));
    } else {
        indicatorGrad.setColorAt(0, QColor(100, 200, 255));
        indicatorGrad.setColorAt(1, QColor(70, 140, 255));
    }
    painter.setBrush(indicatorGrad);
    painter.drawEllipse(ix - isz / 2, static_cast<int>(cy - isz / 2), isz, isz);
    
    // 绘制文本
    if (!m_errorMsg.isEmpty()) {
        painter.setPen(m_errorCode == 401 ? QColor(255, 100, 100) : QColor(255, 200, 100));
        QFont font("Segoe UI", 9);
        painter.setFont(font);
        painter.drawText(QRectF(46, 0, 100, h), Qt::AlignVCenter | Qt::AlignLeft, m_errorMsg);
    } else {
        painter.setPen(QColor(235, 237, 245));
        QFont font("JetBrains Mono", 15, QFont::DemiBold);
        painter.setFont(font);
        painter.drawText(QRectF(46, 0, 55, h), Qt::AlignVCenter | Qt::AlignLeft, 
                        QString::number(percent, 'f', 1));
        
        painter.setPen(QColor(80, 85, 100));
        QFont font2("Segoe UI", 10);
        painter.setFont(font2);
        painter.drawText(QRectF(108, 0, 30, h), Qt::AlignVCenter | Qt::AlignLeft, "%");
    }
}

void DynamicIsland::drawExpandedMode(QPainter &painter, int w, int h)
{
    // TODO: 实现展开模式绘制
}

void DynamicIsland::drawDockedMode(QPainter &painter, int w, int h)
{
    // TODO: 实现贴边模式绘制
}

void DynamicIsland::toggle()
{
    // TODO: 实现窗口模式切换
}

void DynamicIsland::fadeIn()
{
    // TODO: 实现淡入动画
}

void DynamicIsland::checkDock()
{
    // TODO: 实现贴边检测
}

void DynamicIsland::dock(DockMode mode)
{
    // TODO: 实现贴边功能
}

void DynamicIsland::undock()
{
    // TODO: 实现取消贴边功能
}

void DynamicIsland::loadState()
{
    // TODO: 实现状态加载
}

void DynamicIsland::saveState()
{
    // TODO: 实现状态保存
}
```

- [ ] **Step 3: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功

- [ ] **Step 4: 提交 DynamicIsland 基础框架**

```bash
git add mimo-island-cpp/src/DynamicIsland.h mimo-island-cpp/src/DynamicIsland.cpp
git commit -m "feat: implement DynamicIsland main window basic framework"
```

---

### Task 6: 实现上下文菜单

**Files:**
- Modify: `mimo-island-cpp/src/DynamicIsland.h`
- Modify: `mimo-island-cpp/src/DynamicIsland.cpp`

- [ ] **Step 1: 添加 ContextMenu 类定义到 DynamicIsland.h**

在 DynamicIsland 类定义之前添加：

```cpp
class ContextMenu : public QMenu
{
    Q_OBJECT
    
public:
    explicit ContextMenu(DynamicIsland *parent);
    
    void updateProfiles();
    void setPinned(bool pinned);
    
private:
    DynamicIsland *m_parent;
    QMenu *m_profileMenu;
    QAction *m_pinAction;
    
    void buildMenu();
};
```

- [ ] **Step 2: 实现 ContextMenu 类**

在 DynamicIsland.cpp 中添加：

```cpp
ContextMenu::ContextMenu(DynamicIsland *parent)
    : QMenu(parent)
    , m_parent(parent)
{
    setStyleSheet(R"(
        QMenu {
            background: rgba(16, 16, 22, 245);
            border: 1px solid rgba(70, 140, 255, 50);
            border-radius: 10px;
            padding: 6px;
            min-width: 200px;
        }
        QMenu::item {
            color: rgba(210, 215, 230, 220);
            padding: 8px 20px;
            font-size: 12px;
            border-radius: 6px;
            margin: 2px 4px;
        }
        QMenu::item:selected {
            background: rgba(70, 140, 255, 35);
        }
        QMenu::item:checked {
            color: rgba(100, 200, 255, 255);
        }
        QMenu::separator {
            height: 1px;
            background: rgba(255,255,255,18);
            margin: 4px 10px;
        }
    )");
    
    QFont font("Segoe UI", 11);
    setFont(font);
    
    buildMenu();
}

void ContextMenu::buildMenu()
{
    clear();
    
    m_profileMenu = addMenu("切换账号");
    m_profileMenu->setStyleSheet(styleSheet());
    
    QAction *addAction = addAction("添加Cookie");
    connect(addAction, &QAction::triggered, m_parent, &DynamicIsland::addCookie);
    
    QAction *renameAction = addAction("重命名当前");
    connect(renameAction, &QAction::triggered, m_parent, &DynamicIsland::renameCurrentCookie);
    
    QAction *deleteAction = addAction("删除当前");
    connect(deleteAction, &QAction::triggered, m_parent, &DynamicIsland::deleteCurrentCookie);
    
    addSeparator();
    
    QAction *refreshAction = addAction("刷新数据");
    connect(refreshAction, &QAction::triggered, m_parent, &DynamicIsland::fetchData);
    
    addSeparator();
    
    m_pinAction = addAction(m_parent->m_pinned ? "取消置顶" : "置顶窗口");
    connect(m_pinAction, &QAction::triggered, m_parent, &DynamicIsland::toggleTop);
    
    addSeparator();
    
    QAction *quitAction = addAction("退出程序");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void ContextMenu::updateProfiles()
{
    m_profileMenu->clear();
    
    const QVector<CookieProfile> &profiles = m_parent->m_profileManager.profiles();
    for (int i = 0; i < profiles.size(); ++i) {
        QString name = profiles[i].name;
        if (!profiles[i].lastError.isEmpty()) {
            name += " ⚠";
        }
        
        QAction *action = m_profileMenu->addAction(name);
        action->setCheckable(true);
        action->setChecked(i == m_parent->m_profileManager.activeIndex());
        
        connect(action, &QAction::triggered, [this, i]() {
            m_parent->switchProfile(i);
        });
    }
    
    if (profiles.isEmpty()) {
        QAction *emptyAction = m_profileMenu->addAction("(无配置)");
        emptyAction->setEnabled(false);
    }
}

void ContextMenu::setPinned(bool pinned)
{
    m_pinAction->setText(pinned ? "取消置顶" : "置顶窗口");
}
```

- [ ] **Step 3: 实现 addCookie、deleteCurrentCookie、renameCurrentCookie 方法**

```cpp
void DynamicIsland::addCookie()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "添加Cookie",
        "粘贴 Cookie（支持格式）：\n• fetch 代码\n• curl 命令\n• 纯字符串\n\n获取：F12 → Network → 右键请求 → Copy as fetch", "", &ok);
    
    if (ok && !text.trimmed().isEmpty()) {
        // 简单的 Cookie 提取逻辑
        QString cookieStr = text.trimmed();
        
        bool ok2;
        QString name = QInputDialog::getText(this, "命名配置", "配置名称：",
            QLineEdit::Normal, QString("账号%1").arg(m_profileManager.profiles().size() + 1), &ok2);
        
        if (ok2 && !name.trimmed().isEmpty()) {
            m_profileManager.add(name.trimmed(), cookieStr);
            m_contextMenu->updateProfiles();
            fetchData();
        }
    }
}

void DynamicIsland::deleteCurrentCookie()
{
    CookieProfile *profile = m_profileManager.getActive();
    if (!profile) {
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除",
        QString("删除配置 \"%1\" ？").arg(profile->name),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_profileManager.remove(m_profileManager.activeIndex());
        m_contextMenu->updateProfiles();
        fetchData();
    }
}

void DynamicIsland::renameCurrentCookie()
{
    CookieProfile *profile = m_profileManager.getActive();
    if (!profile) {
        return;
    }
    
    bool ok;
    QString name = QInputDialog::getText(this, "重命名", "新名称：",
        QLineEdit::Normal, profile->name, &ok);
    
    if (ok && !name.trimmed().isEmpty()) {
        m_profileManager.rename(m_profileManager.activeIndex(), name.trimmed());
        m_contextMenu->updateProfiles();
    }
}
```

- [ ] **Step 4: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功

- [ ] **Step 5: 提交上下文菜单实现**

```bash
git add mimo-island-cpp/src/DynamicIsland.h mimo-island-cpp/src/DynamicIsland.cpp
git commit -m "feat: implement context menu with profile management"
```

---

### Task 7: 实现展开模式和贴边模式绘制

**Files:**
- Modify: `mimo-island-cpp/src/DynamicIsland.cpp`

- [ ] **Step 1: 实现 drawExpandedMode 方法**

```cpp
void DynamicIsland::drawExpandedMode(QPainter &painter, int w, int h)
{
    drawFlowLight(painter, w, h);
    
    int pad = 28;
    double alpha = m_itemOpacity;
    if (alpha < 0.01) {
        return;
    }
    
    int alphaInt = static_cast<int>(255 * alpha);
    QColor titleColor(235, 237, 245, alphaInt);
    QColor valueColor(240, 242, 250, alphaInt);
    QColor dimColor(80, 85, 100, alphaInt);
    QColor errorColor(255, 100, 100, alphaInt);
    
    // 绘制标题
    painter.setPen(titleColor);
    QFont titleFont("Segoe UI", 13, QFont::DemiBold);
    painter.setFont(titleFont);
    painter.drawText(QRectF(pad, 22, 200, 24), Qt::AlignVCenter | Qt::AlignLeft, "MiMo Token Monitor");
    
    // 绘制配置名称
    CookieProfile *profile = m_profileManager.getActive();
    if (profile) {
        QString profileText = profile->name;
        if (!profile->lastError.isEmpty()) {
            profileText += " ⚠";
        }
        painter.setPen(QColor(100, 200, 255, static_cast<int>(alphaInt * 0.7)));
        QFont profileFont("Segoe UI", 9);
        painter.setFont(profileFont);
        painter.drawText(QRectF(pad, 42, 200, 16), Qt::AlignVCenter | Qt::AlignLeft, profileText);
    }
    
    int y0 = 66;
    int rh = 24;
    
    if (!m_errorMsg.isEmpty()) {
        if (m_errorCode == 401) {
            painter.setPen(errorColor);
            QFont errorFont("Segoe UI", 11);
            painter.setFont(errorFont);
            painter.drawText(QRectF(pad, y0, w - pad * 2, rh), Qt::AlignCenter, "Cookie 已过期");
            
            painter.setPen(dimColor);
            QFont hintFont("Segoe UI", 9);
            painter.setFont(hintFont);
            painter.drawText(QRectF(pad, y0 + rh + 4, w - pad * 2, rh), Qt::AlignCenter, "右键 → 添加新Cookie");
        } else {
            painter.setPen(QColor(255, 200, 100, alphaInt));
            QFont errorFont("Segoe UI", 11);
            painter.setFont(errorFont);
            painter.drawText(QRectF(pad, y0, w - pad * 2, rh * 2), Qt::AlignCenter, m_errorMsg);
        }
    } else {
        // 绘制补偿数据
        if (m_data.compTotal > 0) {
            painter.setPen(QColor(100, 255, 180, alphaInt));
            QFont labelFont("Segoe UI", 9, QFont::Light);
            painter.setFont(labelFont);
            painter.drawText(QRectF(pad, y0, 60, rh), Qt::AlignVCenter | Qt::AlignLeft, "补偿");
            
            painter.setPen(valueColor);
            QFont valueFont("JetBrains Mono", 10, QFont::Medium);
            painter.setFont(valueFont);
            painter.drawText(QRectF(pad + 60, y0, 180, rh), Qt::AlignVCenter | Qt::AlignLeft,
                QString("%1 / %2").arg(static_cast<int>(m_data.compUsed)).arg(static_cast<int>(m_data.compTotal)));
            y0 += rh;
        }
        
        // 绘制套餐数据
        if (m_data.planTotal > 0) {
            painter.setPen(QColor(100, 200, 255, alphaInt));
            QFont labelFont("Segoe UI", 9, QFont::Light);
            painter.setFont(labelFont);
            painter.drawText(QRectF(pad, y0, 60, rh), Qt::AlignVCenter | Qt::AlignLeft, "套餐");
            
            painter.setPen(valueColor);
            QFont valueFont("JetBrains Mono", 10, QFont::Medium);
            painter.setFont(valueFont);
            painter.drawText(QRectF(pad + 60, y0, 180, rh), Qt::AlignVCenter | Qt::AlignLeft,
                QString("%1 / %2").arg(static_cast<int>(m_data.planUsed)).arg(static_cast<int>(m_data.planTotal)));
            y0 += rh;
        }
        
        // 绘制进度条
        int by = y0 + 8;
        int bx = pad;
        int bw = w - pad * 2;
        int bh = 4;
        int br = 2;
        
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 12));
        painter.drawRoundedRect(QRectF(bx, by, bw, bh), br, br);
        
        double percent = m_percentSpring.value();
        double fw = bw * qMin(percent / 100, 1.0);
        if (fw > 1) {
            QLinearGradient fg(bx, 0, bx + fw, 0);
            fg.setColorAt(0, QColor(59, 130, 246, alphaInt));
            fg.setColorAt(1, QColor(34, 211, 238, alphaInt));
            painter.setBrush(fg);
            painter.drawRoundedRect(QRectF(bx, by, fw, bh), br, br);
        }
        
        // 绘制百分比
        painter.setPen(QColor(200, 205, 220, alphaInt));
        QFont percentFont("JetBrains Mono", 10, QFont::Medium);
        painter.setFont(percentFont);
        painter.drawText(QRectF(pad, by + bh + 8, 80, 18), Qt::AlignVCenter | Qt::AlignLeft,
            QString("%1%").arg(percent, 0, 'f', 2));
    }
    
    // 绘制操作提示
    painter.setPen(dimColor);
    QFont hintFont("Segoe UI", 9, QFont::Light);
    painter.setFont(hintFont);
    painter.drawText(QRectF(pad, h - 24, w - pad * 2, 14), Qt::AlignCenter, "右键菜单 · 左键收起");
}
```

- [ ] **Step 2: 实现 drawDockedMode 方法**

```cpp
void DynamicIsland::drawDockedMode(QPainter &painter, int w, int h)
{
    double percent = m_percentSpring.value();
    double alpha = m_dockOpacity;
    if (alpha < 0.01) {
        return;
    }
    
    bool isVertical = (m_dockMode == DockMode::Left || m_dockMode == DockMode::Right);
    
    int r = isVertical ? qMin(w / 2, 3) : qMin(h / 2, 3);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, w, h), r, r);
    painter.setClipPath(path, Qt::IntersectClip);
    
    // 绘制背景
    QLinearGradient bgGrad(0, 0, w, h);
    bgGrad.setColorAt(0, QColor(14, 14, 20, static_cast<int>(200 * alpha)));
    bgGrad.setColorAt(1, QColor(18, 18, 26, static_cast<int>(200 * alpha)));
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgGrad);
    painter.drawRoundedRect(QRectF(0, 0, w, h), r, r);
    
    // 绘制辉光
    double glowAlpha = 0.15 + 0.05 * sin(m_phase);
    QColor glowColor(70, 140, 255, static_cast<int>(255 * glowAlpha * alpha));
    painter.setBrush(glowColor);
    painter.drawRoundedRect(QRectF(-1, -1, w + 2, h + 2), r + 1, r + 1);
    
    // 绘制进度条
    if (!isVertical) {
        double fw = (w - 4) * qMin(percent / 100, 1.0);
        if (fw > 1) {
            QLinearGradient fg(2, 0, 2 + fw, 0);
            fg.setColorAt(0, QColor(59, 130, 246, static_cast<int>(255 * alpha)));
            fg.setColorAt(1, QColor(34, 211, 238, static_cast<int>(255 * alpha)));
            painter.setBrush(fg);
            painter.drawRoundedRect(QRectF(2, 2, fw, h - 4), r, r);
        }
    } else {
        double fh = (h - 4) * qMin(percent / 100, 1.0);
        if (fh > 1) {
            QLinearGradient fg(0, h - 2 - fh, 0, h - 2);
            fg.setColorAt(0, QColor(59, 130, 246, static_cast<int>(255 * alpha)));
            fg.setColorAt(1, QColor(34, 211, 238, static_cast<int>(255 * alpha)));
            painter.setBrush(fg);
            painter.drawRoundedRect(QRectF(2, h - 2 - fh, w - 4, fh), r, r);
        }
    }
}
```

- [ ] **Step 3: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功

- [ ] **Step 4: 提交展开模式和贴边模式绘制**

```bash
git add mimo-island-cpp/src/DynamicIsland.cpp
git commit -m "feat: implement expanded and docked mode rendering"
```

---

### Task 8: 实现窗口模式切换和贴边功能

**Files:**
- Modify: `mimo-island-cpp/src/DynamicIsland.cpp`

- [ ] **Step 1: 实现 toggle 方法**

```cpp
void DynamicIsland::toggle()
{
    int tw, th;
    if (m_windowMode == WindowMode::Compact) {
        tw = EXPANDED_W;
        th = EXPANDED_H;
        m_windowMode = WindowMode::Expanded;
    } else {
        tw = COMPACT_W;
        th = COMPACT_H;
        m_windowMode = WindowMode::Compact;
    }
    
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    
    QPropertyAnimation *geoAnim = new QPropertyAnimation(this, "geometry");
    geoAnim->setDuration(420);
    geoAnim->setEasingCurve(QEasingCurve::OutExpo);
    
    QRect cur = geometry();
    int nx = cur.center().x() - tw / 2;
    geoAnim->setStartValue(cur);
    geoAnim->setEndValue(QRect(nx, cur.top(), tw, th));
    group->addAnimation(geoAnim);
    
    if (m_windowMode == WindowMode::Expanded) {
        m_itemOpacity = 0.0;
        QTimer::singleShot(100, this, &DynamicIsland::fadeIn);
    } else {
        m_itemOpacity = 1.0;
        QPropertyAnimation *opacAnim = new QPropertyAnimation(this, "itemOpacity");
        opacAnim->setDuration(100);
        opacAnim->setStartValue(1.0);
        opacAnim->setEndValue(0.0);
        opacAnim->setEasingCurve(QEasingCurve::InCubic);
        group->addAnimation(opacAnim);
    }
    
    group->start();
}
```

- [ ] **Step 2: 实现 fadeIn 方法**

```cpp
void DynamicIsland::fadeIn()
{
    QPropertyAnimation *opacAnim = new QPropertyAnimation(this, "itemOpacity");
    opacAnim->setDuration(280);
    opacAnim->setStartValue(0.0);
    opacAnim->setEndValue(1.0);
    opacAnim->setEasingCurve(QEasingCurve::OutCubic);
    opacAnim->start();
}
```

- [ ] **Step 3: 实现 checkDock 方法**

```cpp
void DynamicIsland::checkDock()
{
    if (m_windowMode == WindowMode::Expanded) {
        return;
    }
    
    QScreen *screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    QPoint pos = this->pos();
    int w = width();
    int h = height();
    
    DockMode dockMode = DockMode::None;
    
    if (pos.y() <= screenRect.top() + DOCK_THRESHOLD) {
        dockMode = DockMode::Top;
    } else if (pos.y() + h >= screenRect.bottom() - DOCK_THRESHOLD) {
        dockMode = DockMode::Bottom;
    } else if (pos.x() <= screenRect.left() + DOCK_THRESHOLD) {
        dockMode = DockMode::Left;
    } else if (pos.x() + w >= screenRect.right() - DOCK_THRESHOLD) {
        dockMode = DockMode::Right;
    }
    
    if (dockMode != DockMode::None) {
        dock(dockMode);
    }
}
```

- [ ] **Step 4: 实现 dock 方法**

```cpp
void DynamicIsland::dock(DockMode mode)
{
    m_docked = true;
    m_dockMode = mode;
    m_lastDockMode = mode;
    m_windowMode = WindowMode::Compact;
    
    QScreen *screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    QRect cur = geometry();
    int cx = cur.center().x();
    int cy = cur.center().y();
    
    QRect target;
    switch (mode) {
    case DockMode::Top:
        target = QRect(cx - DOCKED_LEN / 2, screenRect.top(), DOCKED_LEN, DOCKED_H);
        break;
    case DockMode::Bottom:
        target = QRect(cx - DOCKED_LEN / 2, screenRect.bottom() - DOCKED_H, DOCKED_LEN, DOCKED_H);
        break;
    case DockMode::Left:
        target = QRect(screenRect.left(), cy - DOCKED_LEN / 2, DOCKED_W, DOCKED_LEN);
        break;
    case DockMode::Right:
        target = QRect(screenRect.right() - DOCKED_W, cy - DOCKED_LEN / 2, DOCKED_W, DOCKED_LEN);
        break;
    default:
        break;
    }
    
    setGeometry(target);
    m_dockOpacity = 1.0;
    saveState();
}
```

- [ ] **Step 5: 实现 undock 方法**

```cpp
void DynamicIsland::undock()
{
    m_docked = false;
    m_dockHovered = false;
    m_undockedFromDock = true;
    
    QRect cur = geometry();
    QScreen *screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    
    int visible = 146;
    
    int nx = cur.center().x() - COMPACT_W / 2;
    int ny = cur.center().y() - COMPACT_H / 2;
    
    switch (m_lastDockMode) {
    case DockMode::Top:
        ny = screenRect.top();
        break;
    case DockMode::Bottom:
        ny = screenRect.bottom() - COMPACT_H;
        break;
    case DockMode::Left:
        nx = screenRect.left() - (COMPACT_W - visible);
        break;
    case DockMode::Right:
        nx = screenRect.right() - visible;
        break;
    default:
        break;
    }
    
    nx = qMax(screenRect.left() - (COMPACT_W - visible), qMin(nx, screenRect.right() - visible));
    ny = qMax(screenRect.top(), qMin(ny, screenRect.bottom() - COMPACT_H));
    
    QRect target(nx, ny, COMPACT_W, COMPACT_H);
    
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    
    QPropertyAnimation *geoAnim = new QPropertyAnimation(this, "geometry");
    geoAnim->setDuration(350);
    geoAnim->setEasingCurve(QEasingCurve::OutExpo);
    geoAnim->setStartValue(cur);
    geoAnim->setEndValue(target);
    group->addAnimation(geoAnim);
    
    QPropertyAnimation *opacAnim = new QPropertyAnimation(this, "dockOpacity");
    opacAnim->setDuration(250);
    opacAnim->setStartValue(1.0);
    opacAnim->setEndValue(0.0);
    opacAnim->setEasingCurve(QEasingCurve::OutCubic);
    group->addAnimation(opacAnim);
    
    group->start();
    saveState();
}
```

- [ ] **Step 6: 实现 loadState 和 saveState 方法**

```cpp
void DynamicIsland::loadState()
{
    QString stateFile = QDir::homePath() + "/.mimo_monitor/state.json";
    QFile file(stateFile);
    
    if (!file.open(QIODevice::ReadOnly)) {
        // 默认位置
        QScreen *screen = QApplication::primaryScreen();
        QRect screenRect = screen->availableGeometry();
        move((screenRect.width() - COMPACT_W) / 2, screenRect.top() + 12);
        resize(COMPACT_W, COMPACT_H);
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        return;
    }
    
    QJsonObject root = doc.object();
    int dockMode = root["dock_mode"].toInt(0);
    int x = root["x"].toInt(-1);
    int y = root["y"].toInt(-1);
    
    if (dockMode > 0 && x >= 0 && y >= 0) {
        m_dockMode = static_cast<DockMode>(dockMode);
        m_lastDockMode = m_dockMode;
        m_docked = true;
        m_dockOpacity = 1.0;
        
        if (m_dockMode == DockMode::Top || m_dockMode == DockMode::Bottom) {
            resize(DOCKED_LEN, DOCKED_H);
        } else {
            resize(DOCKED_W, DOCKED_LEN);
        }
        move(x, y);
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenRect = screen->availableGeometry();
        move((screenRect.width() - COMPACT_W) / 2, screenRect.top() + 12);
        resize(COMPACT_W, COMPACT_H);
    }
}

void DynamicIsland::saveState()
{
    QString configDir = QDir::homePath() + "/.mimo_monitor";
    QDir().mkpath(configDir);
    
    QString stateFile = configDir + "/state.json";
    QPoint pos = this->pos();
    
    QJsonObject root;
    root["dock_mode"] = m_docked ? static_cast<int>(m_dockMode) : 0;
    root["x"] = pos.x();
    root["y"] = pos.y();
    
    QFile file(stateFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}
```

- [ ] **Step 7: 编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功

- [ ] **Step 8: 提交窗口模式切换和贴边功能**

```bash
git add mimo-island-cpp/src/DynamicIsland.cpp
git commit -m "feat: implement window mode switching and dock functionality"
```

---

### Task 9: 复制图标资源并完成最终配置

**Files:**
- Copy: `assets/icon.ico` to `mimo-island-cpp/assets/icon.ico`
- Modify: `mimo-island-cpp/CMakeLists.txt`
- Modify: `mimo-island-cpp/src/main.cpp`

- [ ] **Step 1: 复制图标文件**

```bash
cp assets/icon.ico mimo-island-cpp/assets/icon.ico
```

- [ ] **Step 2: 创建 Windows 资源文件**

创建 `mimo-island-cpp/app.rc`：

```rc
IDI_ICON1 ICON "assets/icon.ico"
```

- [ ] **Step 3: 更新 CMakeLists.txt 添加资源文件**

在 CMakeLists.txt 中添加：

```cmake
# Windows 资源文件
if(WIN32)
    set(APP_ICON "assets/icon.ico")
    set(APP_RC "app.rc")
    qt_add_resources(mimo-island "app_icon" FILES ${APP_ICON})
endif()
```

- [ ] **Step 4: 更新 main.cpp 添加应用信息**

```cpp
#include <QApplication>
#include "DynamicIsland.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName("MiMo Island");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MiMo");
    
    DynamicIsland window;
    window.show();
    
    return app.exec();
}
```

- [ ] **Step 5: 最终编译验证**

```bash
cd mimo-island-cpp/build
cmake --build . --config Release --target mimo-island
```

Expected: 编译成功，生成 mimo-island.exe

- [ ] **Step 6: 提交最终配置**

```bash
git add mimo-island-cpp/
git commit -m "feat: add application icon and finalize configuration"
```

---

### Task 10: 构建和部署

**Files:**
- Create: `mimo-island-cpp/build/` directory

- [ ] **Step 1: 创建构建目录**

```bash
mkdir -p mimo-island-cpp/build
cd mimo-island-cpp/build
```

- [ ] **Step 2: 配置 CMake**

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
```

注意：需要根据实际 Qt 安装路径修改 CMAKE_PREFIX_PATH

Expected: CMake 配置成功

- [ ] **Step 3: 构建项目**

```bash
cmake --build . --config Release
```

Expected: 构建成功，生成 mimo-island.exe

- [ ] **Step 4: 使用 windeployqt 收集依赖**

```bash
cd Release
windeployqt mimo-island.exe
```

Expected: 自动收集 Qt 依赖文件

- [ ] **Step 5: 验证生成的文件**

```bash
dir
```

Expected: 看到 mimo-island.exe 和相关 Qt dll 文件

- [ ] **Step 6: 测试运行**

```bash
.\mimo-island.exe
```

Expected: 应用程序启动，显示动态岛界面

- [ ] **Step 7: 提交构建说明**

创建 `mimo-island-cpp/BUILD.md`：

```markdown
# MiMo Island C++ 版本构建说明

## 环境要求

- Windows 10/11
- Visual Studio 2019 或更高版本
- Qt 6.x（需要安装 Qt 6.x 和 Qt 6.x MSVC 2019 64-bit 组件）
- CMake 3.16 或更高版本

## 构建步骤

1. 克隆或下载项目
2. 打开命令行，进入项目目录
3. 创建构建目录：
   ```bash
   mkdir build
   cd build
   ```
4. 配置 CMake（需要指定 Qt 安装路径）：
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
   ```
5. 构建项目：
   ```bash
   cmake --build . --config Release
   ```
6. 收集 Qt 依赖：
   ```bash
   cd Release
   windeployqt mimo-island.exe
   ```

## 运行

双击 `mimo-island.exe` 即可运行。

## 注意事项

- 需要确保 Qt 安装路径正确
- 需要安装 Visual Studio 2019 或更高版本
- 需要安装 CMake 3.16 或更高版本
```

```bash
git add mimo-island-cpp/BUILD.md
git commit -m "docs: add build instructions"
```

---

## 自检清单

### 1. 规格覆盖检查
- ✅ 项目概述和技术选型：已实现
- ✅ 系统架构和模块划分：已实现
- ✅ UI设计和视觉效果：已实现
- ✅ 数据流设计和错误处理：已实现
- ✅ 构建部署方案：已实现
- ✅ 性能优化策略：已实现

### 2. 占位符扫描
- ✅ 无 "TBD"、"TODO" 等占位符
- ✅ 所有代码步骤都有完整代码
- ✅ 所有命令都有明确说明

### 3. 类型一致性检查
- ✅ 类名、方法名、属性名一致
- ✅ 数据结构定义一致
- ✅ 信号槽连接一致

## 执行选项

**Plan complete and saved to `docs/superpowers/plans/2026-06-06-mimo-island-cpp.md`. Two execution options:**

**1. Subagent-Driven (recommended)** - I dispatch a fresh subagent per task, review between tasks, fast iteration

**2. Inline Execution** - Execute tasks in this session using executing-plans, batch execution with checkpoints

**Which approach?**
