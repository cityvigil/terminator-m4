# Terminator M4（SQL*Plus 专用）

这是一个独立构建的 ARM64 Terminator，供 SQL*Plus 宽行查看使用。上游仓库和 Kaku 仓库均未修改。

- 应用位置：`dist/Terminator-M4.app`
- 应用标识：`org.jessies.TerminatorM4`
- 独立配置目录：`~/.terminator-m4`

依赖：本机 ARM64 Oracle JDK 21；运行 SQL*Plus 时还需要本机 Oracle Instant Client。

## 使用

在 Finder 中打开 `dist/Terminator-M4.app`，在终端窗口运行：

```text
sqlplus /nolog
```

登录测试库后，执行只读查询，并用 `set linesize 300` 或 `set linesize 500` 调整 SQL*Plus 输出宽度。连接口令不要写入工程或测试脚本。

应用使用单独的 `~/.terminator-m4` 配置和 terminfo 目录，不读取或覆盖旧 Terminator 的设置。

## 上游项目和 Fork 来源

本项目基于 [`software-jessies-org/jessies`](https://github.com/software-jessies-org/jessies) 仓库中的 Java Terminator 项目。

使用的上游基线是提交 [`fa4fbaff1e51af5f075b0d4aef0299774c004a1a`](https://github.com/software-jessies-org/jessies/commit/fa4fbaff1e51af5f075b0d4aef0299774c004a1a)。这里的“固定”是指本项目从这个确切的 Git 代码快照开始修改；它不是软件版本号，也不会自动包含上游之后的新提交。保留这个提交号可以让其他人查到本项目修改前的准确源码状态。

本项目在该基线上增加 Apple Silicon/macOS 的启动和打包兼容修改。上游作者的版权与许可证声明继续保留，详见仓库中的 `COPYING` 和依赖库许可证。

### 与 `nelk114/terminator` 的区别

[`nelk114/terminator`](https://github.com/nelk114/terminator) 虽然也叫 Terminator，但它是一个独立的 C/X11 会话终止工具，仓库只有 `Imakefile` 和 `terminator.c`。它通过连接 X Server 并设置 `_ENH_TERMINATOR` 窗口属性来请求或等待会话结束；它不是终端模拟器，也没有参与本项目的源码、依赖或构建。该仓库的说明称其归档自 `jessies.org/~enh/software/x11/`，这只是它的归档来源，不代表它是本项目的代码基线。

本项目使用的是上面所述 Jessies 仓库中的 Java 终端模拟器。其界面使用 Swing 水平滚动条，终端模型会记录实际输出的最大行宽，因此 SQL*Plus 的宽行可以横向查看。

## 构建

构建机需为 Apple Silicon，并安装 ARM64 JDK 21、Command Line Tools、Ruby 和 Make。

上游源码包没有附带 `salma-hayek/lib/jars/` 中的两个 Java 兼容库。本项目保留了它们及随附许可证，干净检出后可直接构建：

- `AppleJavaExtensions.jar` 与 `AppleJavaExtensions-LICENSE.txt`
- `swing-worker-1.2.jar` 与 `swing-worker-1.2-LICENSE.txt`

在仓库根目录执行：

```sh
export JAVA_HOME=$(/usr/libexec/java_home -v 21)
export PATH="$JAVA_HOME/bin:$PATH"
make build
make -C terminator terminator.app
mkdir -p dist
ditto terminator/.generated/native/universal_Darwin/terminator/Terminator.app dist/Terminator-M4.app
```

`universal_Darwin` 是旧构建脚本生成的目录名；本次实际产物是 ARM64。`package-for-distribution.rb` 和 `macos-app-launcher.c` 包含本项目的本地兼容修复：补齐 Finder 所需的 `CFBundleExecutable`，用原生 ARM64 入口调用原启动脚本，设置独立配置目录，并对本机构建做 ad-hoc 签名。上游旧包把 Bash 脚本直接作为应用入口，当前 macOS 会将其判为不完整。

## 本次验证

- 根目录 `make build` 成功；`libpty.dylib`、`libposix.dylib`、启动器均为 ARM64。
- 新包的 `Info.plist` 和代码签名检查通过。修复后从 Finder 打开，系统中出现运行中的 Terminator-M4 进程。
- SQL*Plus `/nolog` 无数据库测试成功输出 314 字符和 514 字符的行，两个行尾标记均完整；当前版本已由用户确认满足其水平滚动需求。

数据库连接使用需由用户在本机手动测试。已知运行限制是需要本机 Java 21 和 SQL*Plus 客户端。
