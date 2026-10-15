Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Find-VisualStudio {
  $vsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
  if (-not (Test-Path -LiteralPath $vsWhere)) {
    throw 'Visual Studio Installer (vswhere.exe) is required.'
  }
  $installation = & $vsWhere -latest -products '*' -requires Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
  if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($installation)) {
    throw 'Visual Studio with MSBuild, the C++ workload and MSVC v143 is required.'
  }
  return $installation.Trim()
}

function Find-Vcpkg {
  $root = $env:VCPKG_ROOT
  if ([string]::IsNullOrWhiteSpace($root)) {
    $root = Join-Path (Find-VisualStudio) 'VC/vcpkg'
  }
  if (-not (Test-Path -LiteralPath (Join-Path $root 'vcpkg.exe'))) {
    throw 'Install the Visual Studio vcpkg component or set VCPKG_ROOT.'
  }
  return (Resolve-Path -LiteralPath $root).Path
}
