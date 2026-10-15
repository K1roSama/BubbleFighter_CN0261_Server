# 编译与发布工作流

工作流沿用 KR1304 的自动检查和手动发布结构，按 CN0261 的 vcpkg 依赖、程序名、配置、14 份迁移和客户端辅助脚本适配。

## 自动检查

[Build and Test](../.github/workflows/build.yml) 在所有分支 push、pull request 和手动触发时执行。Windows `windows-2025` runner 分别构建 Debug / Release x64，矩阵不因单个配置失败而提前结束；同一分支或 PR 的新提交取消旧检查。

流程准备 Python 3.12 和固定 baseline 的 vcpkg 依赖，编译解决方案，运行固定报文回归、生产程序 CLI 与配置检查，再检查工程引用、文档链接及源码/迁移兼容清单。使用 `contents: read`，checkout 不保留 Git 凭据。

无论前序步骤是否失败，都会尝试生成诊断摘要和上传已有日志：

- **Annotations**：保留 MSVC/MSBuild 错误码、原始说明及可定位的文件、行列。
- **Summary**：显示依赖、编译、回归、启动与配置、兼容检查的结果，以及前 12 个不同错误。
- **Artifacts**：`build-logs-<Debug或Release>-<run_id>-<run_attempt>`，保留 14 天，包含全文、错误、控制台、回归和启动日志。

摘要没有结构化错误时会提示查看原始日志，不猜测失败原因；工作流失败状态由实际构建或检查步骤决定。完整 LLVM 格式、命名和独立头文件检查按[开发约定](development.md)在本地使用 LLVM `22.1.8` 执行。

## 手动发布

[Build and Release](../.github/workflows/release.yml) 仅通过 `workflow_dispatch` 运行。将工作流提交到默认分支后，打开 **Actions → Build and Release → Run workflow**，选择分支或标签并填写：

| 输入 | 说明 |
| --- | --- |
| `tag` | 必填，例如 `v1.0.0`；已有标签必须指向选定提交 |
| `release_name` | 可选，默认 `BubbleFighter_CN0261_Server <tag>` |
| `prerelease`、`draft` | 预发布、草稿开关，默认关闭 |

发布工作流需要 `contents: write`，使用 runner 自带的 `gh` 和 `GITHUB_TOKEN` 创建 Release。checkout、setup-python、cache 和 upload-artifact 使用与参考项目相同的完整提交固定值。

发布前依次检查标签、依赖、兼容清单，构建 Debug / Release 并分别运行回归，再执行两套生产程序的离线启动检查。Release 打包后生成 SHA-256，解压并再次运行包内生产程序检查，全部成功后才创建 Release 并上传附件。

## 发布包

附件为 `BubbleFighter_CN0261_Server-<tag>-Windows-x64.zip` 与同名 `.zip.sha256`，包括：

- Release 服务端、实际部署的依赖 DLL 和 x64 MSVC v143 CRT。
- 无密码的 `config/*.json` 示例与 `database/migrations/` 中 14 份历史 SQL。
- `scripts/` 中列出的客户端辅助工具及 Python 依赖清单。
- `README.txt`、项目 `LICENSE` 与 vcpkg 依赖许可。

不包含客户端、私有 `Local/` 配置、密码、证书、私钥、日志、PDB 或测试程序。解压后从包根目录运行，并按说明自行准备数据库、TLS 与客户端。

## 本地复现

要求 MSVC v143、项目指定 SDK、Python 3.12+，打包脚本使用 PowerShell 7：

```powershell
./build/setup_dependencies.ps1
python build/verify.py
./build/build.ps1 -Configuration Debug
./Artifacts/Tests/x64/Debug/BubbleFighter.Regression.exe
./build/build.ps1 -Configuration Release
./Artifacts/Tests/x64/Release/BubbleFighter.Regression.exe
python tests/runtime_smoke_test.py
python -m unittest discover -s tests -p '*_test.py'
./build/package_release.ps1 -Version v1.0.0
```

依赖缓存位于 `Artifacts/Dependencies/`，编译日志位于 `Artifacts/Logs/`，离线启动日志位于 `Artifacts/smoke/`，发布文件位于 `Artifacts/Distribution/`。打包前会比对 `build/build.ps1` 记录的源码与程序散列，拒绝源码已变化或程序被替换的陈旧产物。打包会自动查找 Visual Studio 的 x64 `Microsoft.VC143.CRT`，也可用 `-RuntimeDirectory` 显式指定。

离线检查不会建立真实 MySQL 连接或启动游戏客户端，也不会声明已完成联机验收。可按实际构建结果手动生成诊断报告：

```powershell
$env:COMPILE_OUTCOME = 'success' # 失败时使用 failure
python build/report_build.py --configuration Release
```
