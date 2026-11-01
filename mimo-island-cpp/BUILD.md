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

## 项目结构

```
mimo-island-cpp/
├── CMakeLists.txt          # CMake 构建配置
├── app.rc                  # Windows 资源文件
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

## 功能特性

- **动态岛风格界面**：类似 iPhone Dynamic Island 的桌面小部件
- **多账号管理**：支持多个 Cookie 配置，方便切换
- **实时数据监控**：每 5 分钟自动获取 token 使用数据
- **贴边模式**：窗口可以贴边显示，鼠标悬停时展开
- **动画效果**：流畅的弹簧动画和辉光效果
- **错误处理**：401 错误时显示红色边框闪烁提示

## 注意事项

- 需要确保 Qt 安装路径正确
- 需要安装 Visual Studio 2019 或更高版本
- 需要安装 CMake 3.16 或更高版本
- 生成的 exe 文件需要与 Qt dll 文件在同一目录下运行
- 首次运行需要添加 Cookie 配置

## 性能优化

- 使用 C++17 标准，提升性能
- 采用模块化设计，便于维护和扩展
- 使用 Qt 的信号槽机制，实现高效的事件处理
- 采用异步网络请求，避免阻塞 UI 线程

## 故障排除

### 1. 编译错误

**问题**：找不到 Qt 库
**解决**：检查 CMAKE_PREFIX_PATH 是否正确指向 Qt 安装目录

**问题**：找不到 Visual Studio
**解决**：确保已安装 Visual Studio 2019 或更高版本，并选择了 C++ 开发工作负载

### 2. 运行时错误

**问题**：程序启动后立即退出
**解决**：检查是否缺少 Qt dll 文件，使用 windeployqt 工具收集依赖

**问题**：无法连接到服务器
**解决**：检查网络连接，确保 Cookie 配置正确

### 3. 界面问题

**问题**：界面显示异常
**解决**：检查是否缺少必要的字体文件，或尝试更新显卡驱动

## 开发说明

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

### 代码规范

1. 使用 C++17 标准
2. 遵循 Qt 的编码规范
3. 使用有意义的变量名和函数名
4. 添加必要的注释

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
