[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)][string]$Version,
  [string]$RuntimeDirectory
)

. (Join-Path $PSScriptRoot 'tools.ps1')
. (Join-Path $PSScriptRoot 'source_state.ps1')
if ($Version -notmatch '^[0-9A-Za-z][0-9A-Za-z._+-]*$') { throw 'Invalid release version.' }
$projectRoot = Split-Path $PSScriptRoot -Parent
$distribution = Join-Path $projectRoot 'Artifacts/Distribution'
$stage = Join-Path $projectRoot ('Artifacts/Packages/' + [guid]::NewGuid().ToString('N'))
$binaryRoot = Join-Path $projectRoot 'Artifacts/Bin/x64/Release'
$program = Join-Path $binaryRoot 'BubbleFighter_CN0261_Server.exe'
if (-not (Test-Path -LiteralPath $program)) { throw 'Build Release before packaging.' }
$verification = Join-Path $projectRoot 'Artifacts/Verification/Release/build_inputs.json'
if (-not (Test-Path -LiteralPath $verification)) { throw 'Run build/build.ps1 -Configuration Release before packaging.' }
$buildInputs = Get-Content -Raw -LiteralPath $verification | ConvertFrom-Json -AsHashtable
Assert-SourceState -Expected $buildInputs.sources -Actual (Get-SourceState -ProjectRoot $projectRoot)
if ((Get-FileHash -LiteralPath $program -Algorithm SHA256).Hash -cne $buildInputs.serverProgramHash) {
  throw 'Release executable changed after build verification.'
}
if ([string]::IsNullOrWhiteSpace($RuntimeDirectory)) {
  $redist = Join-Path (Find-VisualStudio) 'VC/Redist/MSVC'
  $runtime = Get-ChildItem -LiteralPath $redist -Directory | Sort-Object Name -Descending |
    ForEach-Object { Join-Path $_.FullName 'x64/Microsoft.VC143.CRT' } |
    Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
  if (-not $runtime) { throw 'MSVC v143 x64 CRT not found; use -RuntimeDirectory.' }
  $RuntimeDirectory = $runtime
}
if (-not (Test-Path -LiteralPath (Join-Path $RuntimeDirectory 'msvcp140.dll'))) {
  throw 'RuntimeDirectory must contain the release x64 MSVC CRT.'
}
New-Item -ItemType Directory -Force -Path $distribution, $stage | Out-Null
Copy-Item -LiteralPath $program -Destination $stage
Get-ChildItem -LiteralPath $binaryRoot -Filter '*.dll' -File | Copy-Item -Destination $stage
Get-ChildItem -LiteralPath $RuntimeDirectory -Filter '*.dll' -File | Copy-Item -Destination $stage
foreach ($directory in @('config', 'database/migrations', 'scripts', 'licenses')) {
  New-Item -ItemType Directory -Force -Path (Join-Path $stage $directory) | Out-Null
}
foreach ($name in @('server', 'launcher', 'game_maps', 'game_weapons', 'game_defense', 'game_characters', 'game_respawn', 'progression')) {
  Copy-Item -LiteralPath (Join-Path $projectRoot "config/$name.json") -Destination (Join-Path $stage 'config')
}
Get-ChildItem -LiteralPath (Join-Path $projectRoot 'database/migrations') -Filter '*.sql' -File |
  Copy-Item -Destination (Join-Path $stage 'database/migrations')
foreach ($name in @('launcher.py', 'start_client.py', 'prepare_client_resources.py', 'prepare_client_resources.ps1',
                    'create_client_runtime.ps1', 'create_certificate.py', 'requirements.txt')) {
  Copy-Item -LiteralPath (Join-Path $projectRoot "scripts/$name") -Destination (Join-Path $stage 'scripts')
}
$share = Join-Path $projectRoot 'Artifacts/Dependencies/x64-windows/share'
foreach ($license in (Get-ChildItem -LiteralPath $share -Filter copyright -File -Recurse)) {
  $package = Split-Path $license.DirectoryName -Leaf
  Copy-Item -LiteralPath $license.FullName -Destination (Join-Path $stage "licenses/$package.txt")
}
if (-not (Test-Path -LiteralPath (Join-Path $stage 'licenses/openssl.txt'))) { throw 'Dependency licenses are missing.' }
Copy-Item -LiteralPath (Join-Path $projectRoot 'LICENSE') -Destination $stage
$sourceCommit = & git -C $projectRoot rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Cannot identify source revision.' }
@"
BubbleFighter CN0261 Server $Version (Windows x64)
Source: https://github.com/K1roSama/BubbleFighter_CN0261_Server
Commit: $sourceCommit

Run PowerShell in this extracted directory.
1. Prepare MySQL 8.x and apply the 14 SQL files in database/migrations in ascending schema_migrations version order.
2. Copy config/server.json to Local/server.json and configure your database/TLS.
3. Set BF_DATABASE_PASSWORD using your database account password.
4. Run ./BubbleFighter_CN0261_Server.exe --config Local/server.json --check-config
5. Run ./BubbleFighter_CN0261_Server.exe --config Local/server.json

--check-config does not connect to the database.
Game login requires a separately supplied matching CN0261 client, a local game
account, and a configured TLS authentication endpoint or login-ticket integration.
See docs/build.md and docs/usage.md in the source repository for the complete guide.
Python 3.12+ and scripts/requirements.txt are needed only for client helper scripts.
No database passwords, certificates, private keys or clients are included.

Project license: LICENSE. Dependency notices: licenses/.
MSVC runtime DLLs are redistributed with the application under the Visual Studio license.
"@ | Set-Content -LiteralPath (Join-Path $stage 'README.txt') -Encoding utf8NoBOM
$archive = Join-Path $distribution "BubbleFighter_CN0261_Server-$Version-Windows-x64.zip"
if (Test-Path -LiteralPath $archive) { throw "Archive already exists: $archive" }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $archive -CompressionLevel Optimal
$hash = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
[IO.File]::WriteAllText("$archive.sha256", "$hash  $([IO.Path]::GetFileName($archive))`n", [Text.UTF8Encoding]::new($false))
Write-Output $archive
