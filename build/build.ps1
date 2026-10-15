[CmdletBinding()]
param(
  [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
  [ValidateRange(1, 16)][int]$ParallelBuildCount = 4
)

. (Join-Path $PSScriptRoot 'tools.ps1')
. (Join-Path $PSScriptRoot 'source_state.ps1')
$projectRoot = Split-Path $PSScriptRoot -Parent
$installation = Find-VisualStudio
$msBuild = Join-Path $installation 'MSBuild/Current/Bin/MSBuild.exe'
$vcpkgRoot = Find-Vcpkg
$savedCompilerScratch = $env:TMP
$logs = Join-Path $projectRoot 'Artifacts/Logs'
New-Item -ItemType Directory -Force -Path $logs | Out-Null

Push-Location $projectRoot
try {
  $compilerScratch = Join-Path $projectRoot 'Artifacts/Compiler'
  New-Item -ItemType Directory -Path $compilerScratch -Force | Out-Null
  $env:TMP = $compilerScratch
  & python (Join-Path $PSScriptRoot 'verify.py')
  if ($LASTEXITCODE -ne 0) { throw 'Repository verification failed.' }
  $sourceState = Get-SourceState -ProjectRoot $projectRoot
  & $msBuild 'BubbleFighter_CN0261_Server.sln' /m:2 /nologo /verbosity:minimal `
    "/p:Configuration=$Configuration" /p:Platform=x64 /p:PlatformToolset=v143 `
    "/p:CL_MPCount=$ParallelBuildCount" "/p:VcpkgRoot=$vcpkgRoot\" `
    "/flp:logfile=$logs/build-$Configuration.log;verbosity=normal;encoding=UTF-8" `
    "/flp2:logfile=$logs/build-$Configuration.errors.log;errorsonly;encoding=UTF-8"
  if ($LASTEXITCODE -ne 0) { throw "Build failed for $Configuration; see $logs." }
  Assert-SourceState -Expected $sourceState -Actual (Get-SourceState -ProjectRoot $projectRoot)
  $verificationRoot = Join-Path $projectRoot "Artifacts/Verification/$Configuration"
  New-Item -ItemType Directory -Path $verificationRoot -Force | Out-Null
  $serverProgram = Join-Path $projectRoot "Artifacts/Bin/x64/$Configuration/BubbleFighter_CN0261_Server.exe"
  $manifest = [ordered]@{
    serverProgramHash = (Get-FileHash -LiteralPath $serverProgram -Algorithm SHA256).Hash
    sources = $sourceState
  }
  [IO.File]::WriteAllText((Join-Path $verificationRoot 'build_inputs.json'),
    (($manifest | ConvertTo-Json -Depth 5) + "`n"), [Text.UTF8Encoding]::new($false))
  Write-Host 'Build source consistency check passed.'
}
finally {
  $env:TMP = $savedCompilerScratch
  Pop-Location
}
