#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "ProfileManager.h"
#include "FetchThread.h"
#include "SpringValue.h"

class DynamicIsland;

// Custom Cookie Input Dialog
class CookieInputDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CookieInputDialog(QWidget *parent = nullptr, const QString &title = "Add Cookie", bool isReplace = false);
    
    QString getText() const;
    QString getCookie() const;
    
private slots:
    void onTextChanged();
    
private:
    QTextEdit *m_textEdit;
    QLabel *m_previewLabel;
    QPushButton *m_okBtn;
};

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

class DynamicIsland : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(double glowOffset READ glowOffset WRITE setGlowOffset)
    Q_PROPERTY(double dockOpacity READ dockOpacity WRITE setDockOpacity)
    Q_PROPERTY(double itemOpacity READ itemOpacity WRITE setItemOpacity)
    
    friend class ContextMenu;
    
public:
    explicit DynamicIsland(QWidget *parent = nullptr);
    ~DynamicIsland();
    
    // Menu operations
    void addCookie();
    void replaceCookie();
    void deleteCurrentCookie();
    void renameCurrentCookie();
    void switchProfile(int index);
    void fetchData();
    void toggleTop();
    
    // Property accessors
    double glowOffset() const { return m_glowOffset; }
    void setGlowOffset(double offset);
    double dockOpacity() const { return m_dockOpacity; }
    void setDockOpacity(double opacity);
    double itemOpacity() const { return m_itemOpacity; }
    void setItemOpacity(double opacity);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
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
    // Window modes
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
    
    // Drawing methods
    void drawBackground(QPainter &painter, int w, int h);
    void drawGlow(QPainter &painter, int w, int h);
    void drawBorder(QPainter &painter, int w, int h);
    void drawFlowLight(QPainter &painter, int w, int h);
    void drawCompactMode(QPainter &painter, int w, int h);
    void drawExpandedMode(QPainter &painter, int w, int h);
    void drawDockedMode(QPainter &painter, int w, int h);
    
    // Animation methods
    void toggle();
    void fadeIn();
    void checkDock();
    void dock(DockMode mode);
    void undock();
    
    // State save/load
    void loadState();
    void saveState();
    
    // Member variables
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
    
    // Constants
    static const int COMPACT_W = 168;
    static const int COMPACT_H = 44;
    static const int EXPANDED_W = 400;
    static const int EXPANDED_H = 195;
    static const int DOCKED_H = 6;
    static const int DOCKED_W = 6;
    static const int DOCKED_LEN = 200;
    static const int DOCK_THRESHOLD = 10;
};
