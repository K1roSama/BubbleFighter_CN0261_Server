# 开发约定

采用 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)，与 KR1304 项目的文档和 LLVM `22.1.8` 工具配置保持一致。

## 源码与命名

- 手写源码使用 UTF-8、LF、两空格缩进，普通 `.cpp` / `.h` 扩展名，目录和文件名使用 `snake_case`。
- 类型、函数和方法使用 `CamelCase`；变量、参数、公开字段和命名空间使用 `snake_case`；类私有成员加尾随下划线；命名空间常量、静态常量与枚举值使用 `kCamelCase`，局部 `const` 变量使用 `snake_case`。
- 内部 include 从 `src/` 开始写完整路径，例如 `"protocol/packet_buffer.h"`。
- 头文件采用路径唯一的 `BUBBLEFIGHTER_SERVER_..._` 保护宏，必须能独立编译。
- 不给未知协议字段添加超出证据的语义，不为了命名规则改动消息编号、报文布局或配置键。

| 目录 | 职责 |
| --- | --- |
| `src/application` | 程序入口与账号管理命令 |
| `src/features` | 认证、角色、库存、商店和升级 |
| `src/game` | 游戏会话、房间、广场、战斗与结算 |
| `src/protocol`、`src/network`、`src/relay` | 协议编码、连接和中继 |
| `src/persistence`、`src/configuration`、`src/logging` | 数据库、配置和日志 |
| `build`、`tests` | MSBuild 项目、验证、发布工具与离线回归 |
| `scripts`、`config`、`database/migrations` | 客户端辅助脚本、配置示例和历史迁移 |

## 项目例外与兼容边界

保留异常及现有错误传播模型。标准入口 `main` / `wmain` 保留工具链要求的名称。Windows、Boost、OpenSSL 等外部 API 的命名不改；JSON 字段、SQL、环境变量、CLI、日志文本、协议常量与线上数据不因风格重构改变。解决方案名、程序名及 `Artifacts/Bin/x64/<配置>/` 输出位置保留兼容。

源码目录和配置示例文件已改为小写下划线；旧配置中的 `Config/GameMaps.json` 等资源路径需更新为对应 `config/game_maps.json`，运行命令迁移到 `build/` 和 `scripts/`。迁移文件使用 `initialize_schema.sql` 等描述性名称，按 SQL 内的迁移版本确定执行顺序；SQL 内容、数据库表与迁移记录名称保持原样。

格式器不拆分字符串字面量，便于核查 SQL、错误文本与协议文本。Windows 基础头文件的 include 优先级由 `.clang-format` 固定。规范检查只覆盖可机械验证的规则，不能据此宣称满足 Google 全部规则。

具体配置以 [.clang-format](../.clang-format)、[.clang-tidy](../.clang-tidy)、[.editorconfig](../.editorconfig) 和 [.gitattributes](../.gitattributes) 为准。MSBuild 工程、解决方案和 PowerShell 文件使用 CRLF；其他受检查文本使用 LF，不带 UTF-8 BOM，保留文件末尾换行。

## 检查与构建

要求 Python 3.12+、LLVM `22.1.8`，将 LLVM 放入 PATH 或安装到 `C:\Program Files\LLVM`；完整解析还需要 MSVC/Windows SDK 和已安装的 vcpkg 头文件。

```powershell
python build/verify.py
python build/verify.py --format
python build/verify.py --headers --naming
```

默认检查工程项、filters、内部 include、头文件保护宏、文档链接及冻结兼容清单。`--format` 检查所有手写 C++ 文件，`--fix-format` 在同一范围应用格式；`--headers` 独立编译每个头文件，`--naming` 对实现运行 clang-tidy。完整输出写入被忽略的 `Artifacts/checks/`，控制台只显示有限错误。

新增源码后刷新工程和 filters：

```powershell
python build/update_project_files.py
./build/build.ps1 -Configuration Debug
./Artifacts/Tests/x64/Debug/BubbleFighter.Regression.exe
python tests/runtime_smoke_test.py --configuration Debug
```

Release 使用相应配置与目录。回归测试使用固定的报文预期，不连接数据库；启动检查覆盖帮助、有效/无效配置及缺失凭据的失败路径。协议、数据库和真实客户端变更仍须执行相应集成验证。

提交前还应运行验证脚本与诊断报告的单元测试，并按[工作流说明](ci.md#本地复现)复现两种构建配置：

```powershell
python -m unittest discover -s tests -p '*_test.py'
```
