# 构建与数据库准备

以下命令从仓库根目录执行。工程配置以 [common.props](../build/common.props)、[主工程](../build/BubbleFighter_CN0261_Server.vcxproj) 和 [vcpkg.json](../vcpkg.json) 为准。

## 构建要求

| 项目 | 配置 |
| --- | --- |
| 平台与标准 | Windows x64、C++20、Debug / Release |
| 工具链 | MSVC v143、Windows SDK `10.0.26100.0` |
| 构建检查 | Python 3.12+，脚本只使用标准库 |
| 依赖 | boost-asio、boost-mysql、nlohmann-json、openssl、spdlog，由 vcpkg manifest 安装 |
| 警告 | `/W4`、警告视为错误、UTF-8 |
| 输出 | `Artifacts/Bin/x64/<配置>/BubbleFighter_CN0261_Server.exe` |

安装 Visual Studio 的 C++ 桌面开发、上述 v143 工具集与 SDK。`build/build.ps1` 自动查找带 C++ 工具的 Visual Studio 安装；较新 Visual Studio 也必须安装 v143。编译使用 MSVC；开发时的格式、独立头文件和命名检查另需 LLVM `22.1.8`，安装方式见[开发约定](development.md#检查与构建)。

vcpkg 可使用 Visual Studio 组件提供的安装，也可通过 `VCPKG_ROOT` 指定；此环境变量同样适用于直接打开解决方案或调用 MSBuild，显式的 MSBuild `/p:VcpkgRoot=...` 优先级更高。其 registry baseline 已固定在 manifest 中；[overlay triplet](../build/triplets/x64-windows.cmake) 将依赖也固定到 v143，避免较新 Visual Studio 默认选用其他工具集。首次构建需要网络下载依赖。

```powershell
./build/build.ps1 -Configuration Debug
./build/build.ps1 -Configuration Release
```

编译产物及依赖位于被忽略的 `Artifacts/`；MSBuild 日志位于 `Artifacts/Logs/`。构建会将依赖 DLL 放到可执行文件目录，并记录输入散列，检查编译期间源码是否变化。

手动调用 MSBuild 时，从 Developer PowerShell 执行：

```powershell
msbuild .\BubbleFighter_CN0261_Server.sln /m:2 /p:Configuration=Release /p:Platform=x64
```

## 数据库与配置

使用支持项目 SQL 的 MySQL 8.x 服务。按各 SQL 中写入 `schema_migrations` 的 `version` 从小到大，逐个执行 [database/migrations](../database/migrations) 中的 14 份迁移。首份 `initialize_schema.sql` 创建 `bubblefighter_cn0261` 数据库和迁移记录表；后续文件依次建立账号、角色、库存、统计、商店、结算、复活装备、奖励和武器预设等结构。文件名使用描述性名称，字母排序不表示执行顺序。

迁移是历史文件，已有数据库只执行尚未应用的部分。服务端会核查 `schema_migrations` 中 1–14 的版本及名称，数量或名称不符时拒绝启动。不要通过改写旧迁移绕过校验。

为服务端创建独立运行账号，赋予项目表所需的读写权限；建库和迁移使用管理账号完成。将无真实密码的配置复制到私有目录：

```powershell
New-Item -ItemType Directory -Force .\Local | Out-Null
Copy-Item .\config\server.json .\Local\server.json
```

编辑 `Local/server.json` 的 `database`：

| 字段 | 说明 |
| --- | --- |
| `host`、`port` | MySQL 地址与端口，示例为 `127.0.0.1:3306`，支持远程主机 |
| `databaseName` | 仅接受 `bubblefighter_cn0261` 或 `bubblefighter_cn0261_tests` |
| `username` | MySQL 运行账号，示例为 `bubblefighter_server` |
| `tlsMode` | `require` 或 `disable`，示例为 `require` |
| `verifyCertificate` | 示例为 `true`；禁用 TLS 时必须为 `false` |
| `caFile` | 自建 CA 的 PEM 路径；空字符串使用 OpenSSL 默认信任路径 |
| 超时与连接池 | `connectTimeoutSeconds`、`queryTimeoutSeconds` 为 1–300；`poolSize` 为 1–128 |

密码只通过 `BF_DATABASE_PASSWORD` 环境变量传入，不写入 JSON。TLS 证书校验使用数据库主机名，证书必须覆盖配置中的地址；MySQL 的 CA 与启动器认证证书是两个独立配置。

配置与目录中的相对路径均按**进程工作目录**解析，不按 JSON 所在目录解析。以上私有配置仍引用仓库的 `config/*.json`，因此应从仓库根目录启动。发布包则从解压后的包根目录启动。

## 校验与排错

```powershell
python build/verify.py
& .\Artifacts\Bin\x64\Release\BubbleFighter_CN0261_Server.exe --config .\Local\server.json --check-config
```

`--check-config` 校验 JSON、地图、武器、防御、角色、复活和升级目录，不连接数据库，也不校验认证私钥能否加载。正常启动还会检查数据库连接、迁移版本和启用的监听器。

| 现象 | 检查方向 |
| --- | --- |
| 工具集或 SDK 缺失 | 安装 v143 与 `10.0.26100.0`，检查 MSBuild 日志 |
| vcpkg 不存在或安装失败 | 检查 `VCPKG_ROOT`、网络与依赖安装日志 |
| DLL 缺失 | 重新运行构建脚本；保留输出目录内依赖 DLL |
| 数据库认证、TLS 或 schema 错误 | 核对环境变量、账号、CA 与 14 份迁移 |
| 端口占用 | 核对 network、authentication、relay 的启用状态与端口 |

下一步见[运行与客户端接入](usage.md)。
