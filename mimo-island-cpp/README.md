# MiMo Island C++ 版本

将 Python PySide6 版本的 MiMo Token 监控应用重写为 C++ Qt 版本，提升性能并保留所有功能。

## 项目概述

MiMo Island 是一个动态岛风格的桌面小部件，用于监控小米 MiMo 平台的 token 使用情况。C++ 版本在保持所有功能的同时，提供了更好的性能和更低的内存占用。

## 主要特性

- **动态岛风格界面**：类似 iPhone Dynamic Island 的桌面小部件
- **多账号管理**：支持多个 Cookie 配置，方便切换
- **实时数据监控**：每 5 分钟自动获取 token 使用数据
- **贴边模式**：窗口可以贴边显示，鼠标悬停时展开
- **动画效果**：流畅的弹簧动画和辉光效果
- **错误处理**：401 错误时显示红色边框闪烁提示

## 技术栈

- **编程语言**：C++17
- **GUI 框架**：Qt 6.x（Qt Widgets 模块）
- **构建系统**：CMake
- **目标平台**：Windows 10/11

## 项目结构

```
mimo-island-cpp/
├── CMakeLists.txt          # CMake 构建配置
├── app.rc                  # Windows 资源文件
├── BUILD.md                # 构建说明
├── src/
│   ├── main.cpp            # 主程序入口
│   ├── DynamicIsland.h/cpp # 主窗口模块
│   ├── FetchThread.h/cpp   # 网络请求模块
│   ├── ProfileManager.h/cpp # 配置管理模块
│   └── SpringValue.h/cpp   # 动画模块
├── assets/
│   └── icon.ico            # 应用图标
└── build/
    └── Release/
        ├── mimo-island.exe # 可执行文件
        ├── Qt6Core.dll     # Qt 核心库
        ├── Qt6Gui.dll      # Qt GUI 库
        ├── Qt6Widgets.dll  # Qt Widgets 库
        ├── Qt6Network.dll  # Qt 网络库
        └── platforms/
            └── qwindows.dll # Windows 平台插件
```

## 性能对比

| 指标 | Python 版本 | C++ 版本 | 提升 |
|------|------------|----------|------|
| 启动时间 | ~2 秒 | <0.5 秒 | 4x |
| 内存占用 | ~300 MB | ~200 MB | 1.5x |
| CPU 占用 | 较高 | 较低 | 显著 |
| 文件大小 | ~50 MB | ~200 KB + Qt dll | 更小 |

## 快速开始

### 1. 环境准备

- 安装 Visual Studio 2019 或更高版本
- 安装 Qt 6.x（需要 MSVC 2019 64-bit 组件）
- 安装 CMake 3.16 或更高版本

### 2. 构建项目

```bash
# 克隆项目
git clone <repository-url>
cd mimo-island

# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/msvc2019_64"

# 构建项目
cmake --build . --config Release

# 收集 Qt 依赖
cd Release
windeployqt mimo-island.exe
```

### 3. 运行程序

双击 `mimo-island.exe` 即可运行。

## 使用说明

### 基本操作

- **左键点击**：切换紧凑/展开模式
- **右键点击**：显示上下文菜单
- **左键拖拽**：移动窗口位置
- **贴边检测**：拖到屏幕边缘自动贴边

### 菜单功能

- **切换账号**：切换不同的 Cookie 配置
- **添加 Cookie**：添加新的 Cookie 配置
- **重命名当前**：重命名当前配置
- **删除当前**：删除当前配置
- **刷新数据**：立即获取最新数据
- **置顶窗口**：切换窗口置顶状态
- **退出程序**：退出应用

### Cookie 配置

1. 打开小米 MiMo 平台
2. 按 F12 打开开发者工具
3. 切换到 Network 选项卡
4. 右键点击任意请求，选择 "Copy as fetch"
5. 在应用中粘贴 Cookie

## 开发说明

### 模块设计

1. **DynamicIsland**：主窗口模块，负责 UI 渲染和用户交互
2. **FetchThread**：网络请求模块，负责 API 数据获取
3. **ProfileManager**：配置管理模块，负责 Cookie 配置管理
4. **SpringValue**：动画模块，负责弹簧动画效果

### 添加新功能

1. 在相应的模块中添加新功能
2. 更新头文件和实现文件
3. 重新构建项目
4. 测试新功能

### 调试技巧

1. 使用 Visual Studio 的调试器
2. 在代码中添加断点
3. 查看输出窗口的日志信息
4. 使用 Qt 的调试输出函数

## 故障排除

### 编译错误

**问题**：找不到 Qt 库
**解决**：检查 CMAKE_PREFIX_PATH 是否正确指向 Qt 安装目录

**问题**：找不到 Visual Studio
**解决**：确保已安装 Visual Studio 2019 或更高版本

### 运行时错误

**问题**：程序启动后立即退出
**解决**：检查是否缺少 Qt dll 文件，使用 windeployqt 工具收集依赖

**问题**：无法连接到服务器
**解决**：检查网络连接，确保 Cookie 配置正确

## 版本历史

### v1.0.0 (2026-06-06)

- 初始版本
- 实现基本功能
- 支持多账号管理
- 实现动态岛风格界面
- 添加贴边模式
- 实现动画效果

## 许可证

本项目采用 MIT 许可证。

## 联系方式

如有问题或建议，请提交 Issue 或 Pull Request。
