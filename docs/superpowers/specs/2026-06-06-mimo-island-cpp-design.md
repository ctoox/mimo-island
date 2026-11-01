# MiMo Island C++ 版本设计文档

## 1. 项目概述

### 1.1 项目目标
将现有的 Python PySide6 版本的 MiMo Token 监控应用重写为 C++ Qt 版本，以实现：
- 提升性能（启动速度、内存占用）
- 保留所有现有功能
- 生成可分发的 exe 文件

### 1.2 技术选型
- **编程语言**：C++17
- **GUI 框架**：Qt 6.x（Qt Widgets 模块）
- **构建系统**：CMake
- **目标平台**：Windows 10/11

### 1.3 性能目标
- 内存占用：15-25 MB
- 启动时间：< 0.5 秒
- 生成产物：mimo-island.exe + Qt dll 文件（约 25-35 MB）

## 2. 系统架构

### 2.1 模块划分

```
mimo-island-cpp/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── DynamicIsland.h/cpp
│   ├── FetchThread.h/cpp
│   ├── ProfileManager.h/cpp
│   └── SpringValue.h/cpp
├── assets/
│   └── icon.ico
└── build/
```

### 2.2 模块职责

#### 2.2.1 DynamicIsland（主窗口模块）
- **职责**：负责 UI 渲染、用户交互、窗口状态管理
- **核心功能**：
  - 三种窗口模式切换（紧凑/展开/贴边）
  - 自定义绘制（背景、辉光、边框、进度条）
  - 鼠标事件处理（点击、拖拽、悬停）
  - 动画效果管理
  - 右键上下文菜单

#### 2.2.2 FetchThread（网络请求模块）
- **职责**：负责 API 数据获取和解析
- **核心功能**：
  - 异步 HTTP GET 请求
  - JSON 响应解析
  - 错误处理和状态码判断
  - 通过信号槽传递数据

#### 2.2.3 ProfileManager（配置管理模块）
- **职责**：负责 cookie 配置的增删改查
- **核心功能**：
  - 多账号配置管理
  - JSON 文件读写
  - 配置状态跟踪（最后使用时间、错误信息）
  - Cookie 格式提取

#### 2.2.4 SpringValue（动画模块）
- **职责**：负责弹簧动画效果
- **核心功能**：
  - 弹簧物理模拟
  - 平滑的数值过渡
  - 可配置的弹簧参数

## 3. UI 设计

### 3.1 窗口模式

#### 3.1.1 紧凑模式
- **尺寸**：168 × 44 像素
- **显示内容**：
  - 圆形状态指示器（蓝色/红色）
  - 百分比数值（大字体）
  - 百分比符号（小字体）
  - 错误信息（如有）

#### 3.1.2 展开模式
- **尺寸**：320 × 195 像素
- **显示内容**：
  - 标题："MiMo Token Monitor"
  - 当前配置名称
  - 补偿 token 使用量（如有）
  - 套餐 token 使用量
  - 进度条
  - 百分比数值
  - 操作提示

#### 3.1.3 贴边模式
- **尺寸**：200 × 6 像素（水平）或 6 × 200 像素（垂直）
- **行为**：
  - 贴边时显示为细条
  - 鼠标悬停时展开为紧凑模式
  - 鼠标离开后自动收回贴边状态

### 3.2 视觉效果

#### 3.2.1 颜色方案
- **背景色**：深色主题（14, 14, 20 色调）
- **主色调**：蓝色（70, 140, 255）
- **错误色**：红色（255, 100, 100）
- **文字色**：浅灰色（235, 237, 245）
- **标签色**：中灰色（120, 125, 145）

#### 3.2.2 动画效果
- **辉光动画**：边框辉光缓慢流动
- **弹簧动画**：数值变化平滑过渡
- **展开动画**：窗口尺寸平滑变化
- **贴边动画**：贴边/展开平滑过渡

#### 3.2.3 字体
- **UI 字体**：Segoe UI Variable Display / Microsoft YaHei UI
- **数据字体**：JetBrains Mono / Cascadia Code

### 3.3 交互方式

#### 3.3.1 鼠标操作
- **左键点击**：切换紧凑/展开模式
- **右键点击**：显示上下文菜单
- **左键拖拽**：移动窗口位置
- **贴边检测**：拖到屏幕边缘自动贴边

#### 3.3.2 上下文菜单
- **切换账号**：子菜单显示所有配置
- **添加 Cookie**：弹出输入对话框
- **重命名当前**：重命名当前配置
- **删除当前**：删除当前配置（需确认）
- **刷新数据**：立即获取最新数据
- **置顶窗口**：切换窗口置顶状态
- **退出程序**：退出应用

## 4. 数据流设计

### 4.1 API 请求流程

```
定时器（5分钟） → FetchThread → HTTP GET → JSON 解析 → 信号槽 → UI 更新
```

#### 4.1.1 请求参数
- **URL**：`https://platform.xiaomimimo.com/api/v1/tokenPlan/usage`
- **Headers**：
  - accept: */*
  - accept-language: zh
  - content-type: application/json
  - x-timezone: Asia/Shanghai
  - user-agent: Mozilla/5.0 ...
  - referer: https://platform.xiaomimimo.com/console/plan-manage
  - cookie: {用户配置的 cookie}

#### 4.1.2 响应数据结构
```json
{
  "data": {
    "monthUsage": {
      "items": [
        {
          "name": "month_total_token",
          "used": 123456,
          "limit": 1000000,
          "percent": 0.123456
        }
      ]
    },
    "usage": {
      "items": [
        {
          "name": "plan_total_token",
          "used": 123456,
          "limit": 1000000,
          "percent": 0.123456
        },
        {
          "name": "compensation_total_token",
          "used": 123456,
          "limit": 1000000,
          "percent": 0.123456
        }
      ]
    }
  }
}
```

### 4.2 配置管理

#### 4.2.1 配置文件格式
```json
{
  "profiles": [
    {
      "name": "账号1",
      "cookie_str": "cookie内容",
      "created_at": "2026-06-06T12:00:00",
      "last_used": "2026-06-06T12:00:00",
      "last_error": null
    }
  ],
  "active_index": 0
}
```

#### 4.2.2 状态文件格式
```json
{
  "dock_mode": 0,
  "x": 100,
  "y": 100
}
```

### 4.3 错误处理

#### 4.3.1 HTTP 错误
- **401 错误**：显示红色边框闪烁，提示"Cookie 已过期"
- **其他 HTTP 错误**：显示错误代码（如"HTTP 500"）

#### 4.3.2 网络异常
- **连接超时**：显示"连接超时"
- **DNS 解析失败**：显示"DNS 解析失败"
- **其他异常**：显示异常信息前 20 个字符

#### 4.3.3 JSON 解析错误
- **格式错误**：显示"JSON 解析失败"
- **字段缺失**：显示"数据格式错误"

## 5. 构建和部署

### 5.1 构建系统

#### 5.1.1 CMake 配置
```cmake
cmake_minimum_required(VERSION 3.16)
project(mimo-island LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

add_executable(mimo-island
    src/main.cpp
    src/DynamicIsland.cpp
    src/FetchThread.cpp
    src/ProfileManager.cpp
    src/SpringValue.cpp
)

target_link_libraries(mimo-island PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
```

### 5.2 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Release

# 构建项目
cmake --build . --config Release
```

### 5.3 部署方式

#### 5.3.1 自动部署
```bash
# 使用 windeployqt 收集 Qt 依赖
windeployqt mimo-island.exe
```

#### 5.3.2 手动部署
需要以下文件：
- mimo-island.exe
- Qt6Core.dll
- Qt6Gui.dll
- Qt6Widgets.dll
- Qt6Network.dll
- platforms/qwindows.dll

#### 5.3.3 可选：生成安装包
使用 Inno Setup 创建安装程序。

## 6. 性能优化

### 6.1 内存优化
- 使用智能指针管理动态内存
- 避免不必要的对象拷贝
- 使用引用传递大型对象
- 及时释放不用的资源

### 6.2 渲染优化
- 使用双缓冲减少闪烁
- 只在数据变化时重绘
- 使用裁剪区域减少绘制范围
- 缓存字体和画笔对象

### 6.3 网络优化
- 使用异步请求避免阻塞 UI
- 设置合理的超时时间
- 复用网络连接（如果可能）
- 缓存最近的响应数据

## 7. 测试策略

### 7.1 单元测试
- ProfileManager 配置管理测试
- SpringValue 动画计算测试
- Cookie 格式提取测试

### 7.2 集成测试
- API 请求和响应解析测试
- 配置文件读写测试
- 错误处理流程测试

### 7.3 UI 测试
- 窗口模式切换测试
- 鼠标交互测试
- 动画效果测试
- 贴边功能测试

## 8. 风险和缓解措施

### 8.1 技术风险
- **Qt 版本兼容性**：使用 Qt 6.x，确保 API 稳定性
- **Windows 平台差异**：使用 Qt 跨平台抽象，减少平台相关代码
- **内存泄漏**：使用智能指针和 RAII 技术

### 8.2 功能风险
- **UI 效果差异**：C++ 版本可能无法完全复现 Python 版本的视觉效果
- **动画流畅度**：需要优化动画算法和渲染性能
- **网络请求稳定性**：需要完善的错误处理和重试机制

### 8.3 时间风险
- **开发周期**：预计需要 2-3 周完成核心功能
- **测试周期**：预计需要 1 周进行测试和修复
- **部署周期**：预计需要 2-3 天完成部署和打包

## 9. 后续扩展

### 9.1 功能扩展
- 支持更多 API 接口
- 添加数据历史记录
- 支持数据导出功能
- 添加系统托盘图标

### 9.2 性能扩展
- 支持 GPU 加速渲染
- 优化内存使用模式
- 添加性能监控和统计

### 9.3 平台扩展
- 支持 macOS 和 Linux 平台
- 添加多语言支持
- 支持主题自定义

## 10. 总结

本设计文档详细描述了 MiMo Island C++ 版本的整体架构、UI 设计、数据流设计、构建部署方案等。通过使用 C++ 和 Qt 6.x，我们可以实现一个高性能、低内存占用的桌面应用，同时保留所有现有功能。

设计遵循以下原则：
1. **模块化**：清晰的模块划分，便于维护和扩展
2. **高性能**：优化内存和渲染性能
3. **可维护性**：良好的代码结构和文档
4. **可扩展性**：为后续功能扩展预留接口

通过本设计，我们可以实现一个稳定、高效、易用的 MiMo Token 监控应用。
