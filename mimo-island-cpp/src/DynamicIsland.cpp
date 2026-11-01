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
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QRegularExpression>
#include <cmath>

// Format number with comma separators
// e.g.: 1234567 -> "1,234,567"
static QString formatNumber(qint64 num)
{
    if (num == 0) return "0";
    
    QString str = QString::number(num);
    QString result;
    int count = 0;
    
    for (int i = str.length() - 1; i >= 0; --i) {
        if (count > 0 && count % 3 == 0) {
            result.prepend(',');
        }
        result.prepend(str[i]);
        ++count;
    }
    
    return result;
}

// CookieInputDialog implementation
CookieInputDialog::CookieInputDialog(QWidget *parent, const QString &title, bool isReplace)
    : QDialog(parent)
{
    setWindowTitle(title);
    setMinimumWidth(500);
    setMinimumHeight(420);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Main container
    QWidget *container = new QWidget(this);
    container->setObjectName("container");
    
    // Dark theme stylesheet
    setStyleSheet(R"(
        QWidget#container {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(12, 12, 18, 245),
                stop:0.5 rgba(16, 16, 24, 240),
                stop:1 rgba(12, 12, 18, 245));
            border: 1.5px solid rgba(70, 140, 255, 35);
            border-radius: 18px;
        }
        QLabel {
            color: rgba(220, 225, 240, 240);
            font-size: 12px;
        }
        QLabel#title {
            color: rgba(245, 247, 255, 255);
            font-size: 16px;
            font-weight: bold;
        }
        QLabel#hint {
            color: rgba(160, 165, 185, 220);
            font-size: 12px;
        }
        QLabel#preview {
            color: rgba(130, 210, 255, 230);
            font-size: 11px;
            padding: 10px 14px;
            background: rgba(10, 14, 22, 180);
            border: 1px solid rgba(70, 140, 255, 25);
            border-radius: 10px;
        }
        QLabel#separator {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(70, 140, 255, 0),
                stop:0.2 rgba(70, 140, 255, 30),
                stop:0.8 rgba(70, 140, 255, 30),
                stop:1 rgba(70, 140, 255, 0));
        }
        QTextEdit {
            background: rgba(8, 8, 14, 220);
            color: rgba(240, 242, 255, 245);
            border: 1.5px solid rgba(70, 140, 255, 25);
            border-radius: 12px;
            padding: 16px;
            font-family: "Consolas", "Microsoft YaHei UI", monospace;
            font-size: 13px;
            selection-background-color: rgba(70, 140, 255, 100);
        }
        QTextEdit:focus {
            border: 1.5px solid rgba(70, 140, 255, 50);
            background: rgba(8, 8, 14, 235);
        }
        QPushButton {
            background: rgba(25, 25, 38, 220);
            color: rgba(210, 215, 235, 230);
            border: 1px solid rgba(70, 140, 255, 20);
            border-radius: 10px;
            padding: 10px 24px;
            font-size: 13px;
            min-width: 90px;
        }
        QPushButton:hover {
            background: rgba(35, 35, 50, 240);
            border: 1px solid rgba(70, 140, 255, 45);
        }
        QPushButton#close {
            background: transparent;
            border: none;
            border-radius: 14px;
            padding: 8px;
            min-width: 28px;
            max-width: 28px;
            max-height: 28px;
            color: rgba(140, 145, 165, 180);
            font-size: 15px;
        }
        QPushButton#close:hover {
            background: rgba(255, 70, 70, 50);
            color: rgba(255, 100, 100, 230);
        }
        QPushButton#primary {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(59, 130, 246, 230),
                stop:1 rgba(34, 211, 238, 230));
            color: rgba(255, 255, 255, 255);
            border: none;
            font-weight: bold;
        }
        QPushButton#primary:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(80, 155, 255, 245),
                stop:1 rgba(55, 230, 255, 245));
        }
        QPushButton#primary:disabled {
            background: rgba(35, 35, 50, 160);
            color: rgba(110, 115, 135, 160);
        }
    )");
    
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->addWidget(container);
    
    // Container layout
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setSpacing(16);
    layout->setContentsMargins(28, 24, 28, 28);
    
    // Top: title + close button
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(14);
    
    // Decorative dots
    QHBoxLayout *dotsLayout = new QHBoxLayout();
    dotsLayout->setSpacing(7);
    QList<QColor> dotColors = {QColor(255, 75, 75, 220), QColor(255, 200, 50, 220), QColor(65, 220, 95, 220)};
    for (const QColor &color : dotColors) {
        QLabel *dot = new QLabel(QString::fromUtf8("\xe2\x97\x8f"));  // ●
        dot->setStyleSheet(QString("color: %1; font-size: 11px;").arg(color.name()));
        dotsLayout->addWidget(dot);
    }
    topLayout->addLayout(dotsLayout);
    
    topLayout->addSpacing(10);
    
    // Title
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("title");
    topLayout->addWidget(titleLabel);
    
    topLayout->addStretch();
    
    // Close button (embedded)
    QPushButton *closeBtn = new QPushButton(QString::fromUtf8("\xe2\x9c\x95"));  // ✕
    closeBtn->setObjectName("close");
    closeBtn->setCursor(QCursor(Qt::PointingHandCursor));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    topLayout->addWidget(closeBtn);
    
    layout->addLayout(topLayout);
    
    // Separator
    QLabel *separator = new QLabel();
    separator->setObjectName("separator");
    separator->setFixedHeight(1);
    layout->addWidget(separator);
    
    // Hint text
    QLabel *hintLabel = new QLabel(isReplace ? 
        QString::fromUtf8("\xe2\x9a\xa0  Cookie expired, please paste a new one") : 
        "Paste Cookie, supports fetch / curl / plain string format");
    hintLabel->setObjectName("hint");
    layout->addWidget(hintLabel);
    
    // Input field
    m_textEdit = new QTextEdit();
    m_textEdit->setPlaceholderText("Paste Cookie here...\n\nExample: document.cookie = 'session_id=abc123; ...'");
    connect(m_textEdit, &QTextEdit::textChanged, this, &CookieInputDialog::onTextChanged);
    layout->addWidget(m_textEdit);
    
    // Preview area
    m_previewLabel = new QLabel("");
    m_previewLabel->setObjectName("preview");
    m_previewLabel->setWordWrap(true);
    m_previewLabel->hide();
    layout->addWidget(m_previewLabel);
    
    // Bottom buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();
    
    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setCursor(QCursor(Qt::PointingHandCursor));
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);
    
    m_okBtn = new QPushButton("OK");
    m_okBtn->setObjectName("primary");
    m_okBtn->setCursor(QCursor(Qt::PointingHandCursor));
    m_okBtn->setEnabled(false);
    connect(m_okBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_okBtn);
    
    layout->addLayout(buttonLayout);
    
    // Auto focus input
    m_textEdit->setFocus();
}

void CookieInputDialog::onTextChanged()
{
    QString text = m_textEdit->toPlainText().trimmed();
    bool hasText = !text.isEmpty();
    m_okBtn->setEnabled(hasText);
    
    // Show preview
    if (hasText) {
        QString cookie = text;
        if (cookie.contains("'cookie'") || cookie.contains("\"cookie\"")) {
            // Extract from fetch code
            QRegularExpression re("['\"]cookie['\"]\\s*:\\s*['\"](.+?)['\"]");
            QRegularExpressionMatch match = re.match(cookie);
            if (match.hasMatch()) {
                cookie = match.captured(1);
            }
        }
        
        if (!cookie.isEmpty() && cookie != text) {
            QString display = cookie.length() > 60 ? cookie.left(60) + "..." : cookie;
            m_previewLabel->setText(QString::fromUtf8("\xe2\x9c\x93 Parsed: ") + display);
            m_previewLabel->show();
        } else {
            m_previewLabel->hide();
        }
    } else {
        m_previewLabel->hide();
    }
}

QString CookieInputDialog::getText() const
{
    return m_textEdit->toPlainText().trimmed();
}

QString CookieInputDialog::getCookie() const
{
    QString text = getText();
    if (text.isEmpty()) return "";
    
    // Simple cookie extraction
    if (text.contains("'cookie'") || text.contains("\"cookie\"")) {
        QRegularExpression re("['\"]cookie['\"]\\s*:\\s*['\"](.+?)['\"]");
        QRegularExpressionMatch match = re.match(text);
        if (match.hasMatch()) {
            return match.captured(1);
        }
    }
    
    return text;
}

// DynamicIsland implementation
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
    // Set window flags
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    
    // Load state
    loadState();
    
    // Create context menu
    m_contextMenu = new ContextMenu(this);
    
    // Create network thread
    m_fetchThread = new FetchThread(&m_profileManager, this);
    connect(m_fetchThread, &FetchThread::dataFetched, this, &DynamicIsland::onDataFetched);
    
    // Create timers
    m_fetchTimer = new QTimer(this);
    connect(m_fetchTimer, &QTimer::timeout, this, &DynamicIsland::fetchData);
    m_fetchTimer->start(300000); // 5 minutes
    fetchData();
    
    m_frameTimer = new QTimer(this);
    connect(m_frameTimer, &QTimer::timeout, this, &DynamicIsland::onTick);
    m_frameTimer->start(16); // 60fps
    
    m_reDockTimer = new QTimer(this);
    m_reDockTimer->setSingleShot(true);
    connect(m_reDockTimer, &QTimer::timeout, this, &DynamicIsland::onReDockCheck);
    
    // Create glow animation
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

void DynamicIsland::enterEvent(QEnterEvent *event)
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
            // If 401 error and expanded, click to replace cookie
            if (m_windowMode == WindowMode::Expanded && m_errorCode == 401) {
                replaceCookie();
                return;
            }
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
    CookieInputDialog dialog(this, "Add Cookie");
    if (dialog.exec() == QDialog::Accepted) {
        QString cookieStr = dialog.getCookie();
        if (!cookieStr.isEmpty()) {
            bool ok;
            QString name = QInputDialog::getText(this, "Profile Name", "Name:",
                QLineEdit::Normal, QString("Account %1").arg(m_profileManager.profiles().size() + 1), &ok);
            
            if (ok && !name.trimmed().isEmpty()) {
                m_profileManager.add(name.trimmed(), cookieStr);
                m_contextMenu->updateProfiles();
                fetchData();
            }
        }
    }
}

void DynamicIsland::replaceCookie()
{
    CookieProfile *profile = m_profileManager.getActive();
    if (!profile) return;
    
    CookieInputDialog dialog(this, QString("Replace Cookie - %1").arg(profile->name), true);
    if (dialog.exec() == QDialog::Accepted) {
        QString cookieStr = dialog.getCookie();
        if (!cookieStr.isEmpty()) {
            // Update current profile's cookie
            profile->cookieStr = cookieStr;
            profile->lastError.clear();
            m_profileManager.save();
            m_errorMsg.clear();
            m_errorCode = 0;
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
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        QString("Delete profile \"%1\" ?").arg(profile->name),
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
    QString name = QInputDialog::getText(this, "Rename", "New name:",
        QLineEdit::Normal, profile->name, &ok);
    
    if (ok && !name.trimmed().isEmpty()) {
        m_profileManager.rename(m_profileManager.activeIndex(), name.trimmed());
        m_contextMenu->updateProfiles();
    }
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
    
    // Status indicator
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
    
    // Text
    if (!m_errorMsg.isEmpty()) {
        painter.setPen(m_errorCode == 401 ? QColor(255, 100, 100) : QColor(255, 200, 100));
        QFont font("Microsoft YaHei UI");
        font.setPixelSize(9);
        painter.setFont(font);
        painter.drawText(QRectF(46, 0, 100, h), Qt::AlignVCenter | Qt::AlignLeft, m_errorMsg);
    } else {
        painter.setPen(QColor(235, 237, 245));
        QFont font("Consolas");
        font.setPixelSize(15);
        font.setWeight(QFont::DemiBold);
        painter.setFont(font);
        painter.drawText(QRectF(46, 0, 55, h), Qt::AlignVCenter | Qt::AlignLeft, 
                        QString::number(percent, 'f', 1));
        
        painter.setPen(QColor(80, 85, 100));
        QFont font2("Microsoft YaHei UI");
        font2.setPixelSize(10);
        painter.setFont(font2);
        painter.drawText(QRectF(108, 0, 30, h), Qt::AlignVCenter | Qt::AlignLeft, "%");
    }
}

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
    
    // Title
    painter.setPen(titleColor);
    QFont titleFont("Microsoft YaHei UI");
    titleFont.setPixelSize(13);
    titleFont.setWeight(QFont::DemiBold);
    painter.setFont(titleFont);
    painter.drawText(QRectF(pad, 22, 200, 24), Qt::AlignVCenter | Qt::AlignLeft, "MiMo Token Monitor");
    
    // Profile name
    CookieProfile *profile = m_profileManager.getActive();
    if (profile) {
        QString profileText = profile->name;
        if (!profile->lastError.isEmpty()) {
            profileText += " !";
        }
        painter.setPen(QColor(100, 200, 255, static_cast<int>(alphaInt * 0.7)));
        QFont profileFont("Microsoft YaHei UI");
        profileFont.setPixelSize(9);
        painter.setFont(profileFont);
        painter.drawText(QRectF(pad, 42, 200, 16), Qt::AlignVCenter | Qt::AlignLeft, profileText);
    }
    
    int y0 = 66;
    int rh = 24;
    
    if (!m_errorMsg.isEmpty()) {
        if (m_errorCode == 401) {
            painter.setPen(errorColor);
            QFont errorFont("Microsoft YaHei UI");
            errorFont.setPixelSize(11);
            painter.setFont(errorFont);
            painter.drawText(QRectF(pad, y0, w - pad * 2, rh), Qt::AlignCenter, "Cookie Expired");
            
            painter.setPen(dimColor);
            QFont hintFont("Microsoft YaHei UI");
            hintFont.setPixelSize(9);
            painter.setFont(hintFont);
            painter.drawText(QRectF(pad, y0 + rh + 4, w - pad * 2, rh), Qt::AlignCenter, "Click here to replace Cookie");
        } else {
            painter.setPen(QColor(255, 200, 100, alphaInt));
            QFont errorFont("Microsoft YaHei UI");
            errorFont.setPixelSize(11);
            painter.setFont(errorFont);
            painter.drawText(QRectF(pad, y0, w - pad * 2, rh * 2), Qt::AlignCenter, m_errorMsg);
        }
    } else {
        // Compensation data
        if (m_data.compTotal > 0) {
            painter.setPen(QColor(100, 255, 180, alphaInt));
            QFont labelFont("Microsoft YaHei UI");
            labelFont.setPixelSize(9);
            labelFont.setWeight(QFont::Light);
            painter.setFont(labelFont);
            painter.drawText(QRectF(pad, y0, 60, rh), Qt::AlignVCenter | Qt::AlignLeft, "Bonus");
            
            painter.setPen(valueColor);
            QFont valueFont("Consolas");
            valueFont.setPixelSize(10);
            valueFont.setWeight(QFont::Medium);
            painter.setFont(valueFont);
            painter.drawText(QRectF(pad + 60, y0, w - pad - 60, rh), Qt::AlignVCenter | Qt::AlignLeft,
                QString("%1 / %2").arg(formatNumber(m_data.compUsed), formatNumber(m_data.compTotal)));
            y0 += rh;
        }
        
        // Plan data
        if (m_data.planTotal > 0) {
            painter.setPen(QColor(100, 200, 255, alphaInt));
            QFont labelFont("Microsoft YaHei UI");
            labelFont.setPixelSize(9);
            labelFont.setWeight(QFont::Light);
            painter.setFont(labelFont);
            painter.drawText(QRectF(pad, y0, 60, rh), Qt::AlignVCenter | Qt::AlignLeft, "Plan");
            
            painter.setPen(valueColor);
            QFont valueFont("Consolas");
            valueFont.setPixelSize(10);
            valueFont.setWeight(QFont::Medium);
            painter.setFont(valueFont);
            painter.drawText(QRectF(pad + 60, y0, w - pad - 60, rh), Qt::AlignVCenter | Qt::AlignLeft,
                QString("%1 / %2").arg(formatNumber(m_data.planUsed), formatNumber(m_data.planTotal)));
            y0 += rh;
        }
        
        // Progress bar
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
        
        // Percentage
        painter.setPen(QColor(200, 205, 220, alphaInt));
        QFont percentFont("Consolas");
        percentFont.setPixelSize(10);
        percentFont.setWeight(QFont::Medium);
        painter.setFont(percentFont);
        painter.drawText(QRectF(pad, by + bh + 8, 80, 18), Qt::AlignVCenter | Qt::AlignLeft,
            QString("%1%").arg(percent, 0, 'f', 2));
    }
    
    // Hint
    painter.setPen(dimColor);
    QFont hintFont("Microsoft YaHei UI");
    hintFont.setPixelSize(9);
    hintFont.setWeight(QFont::Light);
    painter.setFont(hintFont);
    painter.drawText(QRectF(pad, h - 24, w - pad * 2, 14), Qt::AlignCenter, "Right-click menu \u00b7 Left-click collapse");
}

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
    
    // Background
    QLinearGradient bgGrad(0, 0, w, h);
    bgGrad.setColorAt(0, QColor(14, 14, 20, static_cast<int>(200 * alpha)));
    bgGrad.setColorAt(1, QColor(18, 18, 26, static_cast<int>(200 * alpha)));
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgGrad);
    painter.drawRoundedRect(QRectF(0, 0, w, h), r, r);
    
    // Glow
    double glowAlpha = 0.15 + 0.05 * sin(m_phase);
    QColor glowColor(70, 140, 255, static_cast<int>(255 * glowAlpha * alpha));
    painter.setBrush(glowColor);
    painter.drawRoundedRect(QRectF(-1, -1, w + 2, h + 2), r + 1, r + 1);
    
    // Progress bar
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

void DynamicIsland::fadeIn()
{
    QPropertyAnimation *opacAnim = new QPropertyAnimation(this, "itemOpacity");
    opacAnim->setDuration(280);
    opacAnim->setStartValue(0.0);
    opacAnim->setEndValue(1.0);
    opacAnim->setEasingCurve(QEasingCurve::OutCubic);
    opacAnim->start();
}

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

void DynamicIsland::loadState()
{
    QString stateFile = QDir::homePath() + "/.mimo_monitor/state.json";
    QFile file(stateFile);
    
    if (!file.open(QIODevice::ReadOnly)) {
        // Default position
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

// ContextMenu implementation
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
    
    QFont font("Microsoft YaHei UI");
    font.setPixelSize(11);
    setFont(font);
    
    buildMenu();
}

void ContextMenu::buildMenu()
{
    clear();
    
    m_profileMenu = addMenu("Switch Account");
    m_profileMenu->setStyleSheet(styleSheet());
    
    QAction *addCookieAction = addAction("Add Cookie");
    connect(addCookieAction, &QAction::triggered, m_parent, &DynamicIsland::addCookie);
    
    QAction *renameAction = addAction("Rename Current");
    connect(renameAction, &QAction::triggered, m_parent, &DynamicIsland::renameCurrentCookie);
    
    QAction *deleteAction = addAction("Delete Current");
    connect(deleteAction, &QAction::triggered, m_parent, &DynamicIsland::deleteCurrentCookie);
    
    addSeparator();
    
    QAction *refreshAction = addAction("Refresh Data");
    connect(refreshAction, &QAction::triggered, m_parent, &DynamicIsland::fetchData);
    
    addSeparator();
    
    m_pinAction = addAction(m_parent->m_pinned ? "Unpin Window" : "Pin Window");
    connect(m_pinAction, &QAction::triggered, m_parent, &DynamicIsland::toggleTop);
    
    addSeparator();
    
    QAction *quitAction = addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void ContextMenu::updateProfiles()
{
    m_profileMenu->clear();
    
    const QVector<CookieProfile> &profiles = m_parent->m_profileManager.profiles();
    for (int i = 0; i < profiles.size(); ++i) {
        QString name = profiles[i].name;
        if (!profiles[i].lastError.isEmpty()) {
            name += " !";
        }
        
        QAction *action = m_profileMenu->addAction(name);
        action->setCheckable(true);
        action->setChecked(i == m_parent->m_profileManager.activeIndex());
        
        connect(action, &QAction::triggered, [this, i]() {
            m_parent->switchProfile(i);
        });
    }
    
    if (profiles.isEmpty()) {
        QAction *emptyAction = m_profileMenu->addAction("(No profiles)");
        emptyAction->setEnabled(false);
    }
}

void ContextMenu::setPinned(bool pinned)
{
    m_pinAction->setText(pinned ? "Unpin Window" : "Pin Window");
}
