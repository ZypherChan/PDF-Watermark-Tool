<h4 align="right">
  <strong>简体中文</strong> | <a href="README_EN.md">English</a>
</h4>

# PdfTool - PDF水印处理工具

![Windows](https://img.shields.io/badge/Windows%2064--bit-10%2F11-blue?logo=windows)
![Qt](https://img.shields.io/badge/Qt-5.9.7%2B-green?logo=qt)
![PDFium](https://img.shields.io/badge/PDFium-Google-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

![PdfTool Screenshot](./screenshots/main_window.png)

## 📝 项目简介

PdfTool 是一个基于 Qt5 和 PDFium 的 PDF 水印处理工具，提供了直观的图形界面，支持 PDF 文件的浏览、水印添加和保存功能。该工具使用现代化的界面设计，支持中文本地化，并集成了字体子集化技术以优化文件大小。

## ✨ 主要功能

### 📖 PDF 浏览
- **多页浏览**: 支持多页 PDF 文件的完整浏览
- **缩略图导航**: 左侧缩略图面板，快速跳转到指定页面
- **缩放适配**: 自动适配屏幕 DPI，支持高 DPI 显示器
- **页面切换**: 上一页/下一页按钮导航

### 💧 水印功能
- **文字水印**: 支持自定义文字内容的水印添加
- **字体设置**: 内置 Noto Sans CJK SC 字体，支持中文显示
- **样式自定义**:
  - 字体大小可调
  - 透明度设置 (0-1)
  - 旋转角度调整
- **布局控制**:
  - 重复行数设置
  - 重复列数设置
  - 自动间距计算
- **实时预览**: 添加水印前可预览效果

### 💾 文件操作
- **打开 PDF**: 支持标准 PDF 文件格式
- **保存文件**: 直接保存修改到原文件
- **另存为**: 将修改后的文件保存到新位置
- **字体子集化**: 自动优化嵌入字体，减小文件大小

## 🏗️ 技术架构

### 核心技术栈
- **Qt 5**: 跨平台 GUI 框架
- **PDFium**: Google 开源 PDF 渲染引擎
- **Sfntly**: Google 字体处理库，用于字体子集化
- **CMake**: 跨平台构建系统

## 🚀 快速开始

### 环境要求
- **操作系统**: Windows 10/11 (x64)
- **编译器**: Visual Studio 2019 或更高版本
- **Qt 版本**: Qt 5.9.7 或更高版本
- **CMake**: 3.10 或更高版本

### 编译步骤

1. **克隆项目**
```bash
git clone https://github.com/ZypherChan/PdfTool.git
cd PdfTool
```

2. **设置 Qt 环境**
确保设置 `QTDIR` 环境变量指向你的 Qt 安装目录：
```bash
set QTDIR=C:\Qt\5.15.2\msvc2019_64
```

3. **创建构建目录**
```bash
mkdir build
cd build
```

4. **生成项目文件**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

5. **编译项目**
```bash
cmake --build . 
```

### 部署说明

项目使用 `windeployqt` 自动部署 Qt 依赖项，编译完成后会自动：
- 拷贝 Qt 核心库（Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll）
- 部署平台插件（platforms 目录）
- 拷贝字体文件到输出目录
- 排除不必要的组件（翻译、图标引擎、图像格式插件等）

## 📸 界面预览

### 主界面
![Main Window](./screenshots/main_window.png)

### 水印设置对话框
![Watermark Settings](./screenshots/watermark_settings.png)

### 水印效果预览
![Watermark Preview](./screenshots/watermark_preview.png)

### 实时预览
![Real-time Preview](./screenshots/real_time_preview.gif)

## ⚙️ 配置选项

### 运行时配置

程序会自动加载以下配置：
- **样式表**: `:/qss/silvery.css` - 银色主题界面
- **中文字体**: `fonts/NotoSansCJKsc-Bold.ttf` - 支持中文水印
- **翻译文件**: `:/PdfTool_zh_CN.qm` - 中文界面本地化

## 🎯 待实现功能

- [ ] **水印颜色**: 选择水印颜色（默认灰色）
- [ ] **水印位置**: 精确控制水印位置（左上、居中、右下等）
- [ ] **图片水印**: 支持添加图片作为水印
- [ ] **字体设置**: 支持用户选择水印字体

---
💡 **欢迎贡献**: 如果你有想实现的功能，欢迎提交 Issue 或 Pull Request！
💬 **功能建议**: 有好的功能想法？请在 Issue 中分享你的建议！

## 🐛 常见问题

### Q: 程序无法打开 PDF 文件？
**A**: 确保 pdfium.dll 在可执行文件同目录下，且为 x64 版本。

### Q: 水印中文显示为方框？
**A**: 检查 `fonts/NotoSansCJKsc-Bold.ttf` 文件是否存在，且字体文件完整。

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

### 提交规范
- 使用清晰的提交信息
- 添加适当的测试用例
- 更新相关文档

### 开发环境设置
1. Fork 项目到你的仓库
2. 创建功能分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- [PDFium](https://pdfium.googlesource.com/pdfium/) - Google 开源 PDF 引擎
- [Qt Framework](https://www.qt.io/) - 跨平台应用开发框架
- [Sfntly](https://github.com/googlefonts/sfntly) - Google 字体处理库
- [Noto Fonts](https://fonts.google.com/noto) - Google 开源字体项目

⭐ 如果这个项目对你有帮助，请给个 Star 支持一下！