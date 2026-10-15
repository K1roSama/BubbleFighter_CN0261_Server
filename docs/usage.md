# 运行与客户端接入

先完成[构建与数据库准备](build.md)。以下从仓库或发布包根目录执行，示例端口与路径均可按环境调整。命令行为以 [main.cpp](../src/application/main.cpp) 为准。

## 命令行选项

| 选项 | 行为 |
| --- | --- |
| `--help` | 显示帮助，退出码 0 |
| `--config <path>` | 配置路径，默认 `config/server.json` |
| `--check-config` | 校验配置与游戏目录，不连接数据库 |
| `--create-account <name>` | 创建本地游戏账号 |
| `--issue-ticket <name>` | 验证密码，签发一次性登录票据 |

不带参数会加载配置并启动服务端；解析或运行异常退出码为 1。账号操作只能选一个，不能与 `--check-config` 组合。监听地址、端口和数据库连接均通过 JSON 配置，登录票据由账号命令或 TLS 认证接口签发。

```powershell
$server = '.\Artifacts\Bin\x64\Release\BubbleFighter_CN0261_Server.exe'
& $server --help
& $server --check-config
```

## 账号与登录票据

先通过 `Get-Credential` 设置数据库及游戏账号密码，避免把密码写入命令历史：

```powershell
$databaseCredential = Get-Credential -UserName bubblefighter_server -Message 'MySQL 密码'
$env:BF_DATABASE_PASSWORD = $databaseCredential.GetNetworkCredential().Password
$accountCredential = Get-Credential -UserName player01 -Message '游戏账号密码'
$env:BF_ACCOUNT_PASSWORD = $accountCredential.GetNetworkCredential().Password
& $server --config .\Local\server.json --create-account player01
& $server --config .\Local\server.json --issue-ticket player01
```

成功结果是 JSON，分别包含 `accountId` / `accountName` 或 `loginTicket` / `expiresInSeconds`。账号为 3–32 个小写 ASCII 字母、数字或下划线；密码为 8–128 个 UTF-8 字节，不含 NUL。一次性票据默认有效期 **120 秒**，使用后不能重复登录。命令会把票据输出到标准输出，不要将其发布到日志或提交到仓库。

账号管理命令使用 MySQL；INI 中的官方登录信息不能替代本地账号。使用完成后清除不再需要的变量：

```powershell
Remove-Item Env:BF_ACCOUNT_PASSWORD
```

## 监听与 TLS 认证

配置示例 [server.json](../config/server.json) 默认值：

| 服务 | 默认监听 | 默认启用 |
| --- | --- | --- |
| 游戏 TCP | `127.0.0.1:26100` | 是 |
| 启动器 TLS 认证 | `127.0.0.1:26102` | 否 |
| Relay TCP | `127.0.0.1:26104` | 是 |

启用认证前，安装客户端辅助脚本的 Python 依赖并生成独立证书：

```powershell
python -m pip install -r scripts/requirements.txt
python scripts/create_certificate.py --directory Local --host localhost --host 127.0.0.1
```

证书工具拒绝覆盖已有输出，默认有效期 30 天。在 `Local/server.json` 中将 `authentication.enabled` 设为 `true`，将 `certificateFile` 和 `privateKeyFile` 分别设为 `Local/authentication_certificate.pem`、`Local/authentication_private_key.pem`。`gameHost` 是返回给客户端的游戏地址。

```powershell
& $server --config .\Local\server.json
```

开启的监听器端口必须互不相同。局域网使用本机地址或 `0.0.0.0` 监听，并将 `authentication.gameHost`、`relay.advertisedAddress` 和启动器地址配置为客户端可达的实际地址；认证证书 SAN 必须覆盖启动器使用的主机名或 IP。

## 启动客户端

仓库自带 [launcher.py](../scripts/launcher.py)，通过经证书验证的 TLS 交换凭据并获取票据，再创建隔离运行目录并启动匹配的客户端。它会调用资源准备工具，因此客户端目录与所需资源必须由使用者提供。

```powershell
Copy-Item .\config\launcher.json .\Local\launcher.json
```

编辑私有启动器配置：`authenticationHost` / `authenticationPort` 与认证监听一致，`certificateFile` 为 `Local/authentication_certificate.pem`，`clientDirectory` 指向自行准备的客户端目录，`instance` 为大写字母或数字。

```powershell
python scripts/launcher.py --config Local/launcher.json
```

启动器校验 [start_client.py](../scripts/start_client.py) 中固定的原始客户端 SHA-256、x86 架构与映像基址。校验失败表示输入不匹配，应查明版本，不能通过删除校验来声称兼容。运行目录位于 `Artifacts/ClientRuns/`，临时 `login.ini` 在启动后清理。

命令行集成也可使用 `launcher.py --authenticate-only --account player01 --config Local/launcher.json`；密码读取 `BF_ACCOUNT_PASSWORD`，输出含票据 JSON，应按凭据处理。

## 游戏配置与状态

[config](../config) 提供地图、武器、防御、角色、复活装备及 [progression.json](../config/progression.json) 示例。只使用现有解析器接受的键；修改后先运行 `--check-config`，再重启服务端。升级、奖励和商店数值均为本地策略。

数据库保存账号、角色、库存和结算；在线连接、房间和广场不跨进程恢复。CI 只检查离线回归及启动路径，真实客户端、TLS 认证和数据库联机需在自己的环境验收。
