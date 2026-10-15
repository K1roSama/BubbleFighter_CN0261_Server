[CmdletBinding()]
param()

. (Join-Path $PSScriptRoot 'tools.ps1')
$projectRoot = Split-Path $PSScriptRoot -Parent
$vcpkgRoot = Find-Vcpkg
$installed = Join-Path $projectRoot 'Artifacts/Dependencies'
$scratch = Join-Path $projectRoot 'Artifacts/VcpkgBuild'
New-Item -ItemType Directory -Force -Path $scratch | Out-Null

& (Join-Path $vcpkgRoot 'vcpkg.exe') install --triplet x64-windows --x-wait-for-lock `
  "--x-manifest-root=$projectRoot" "--x-install-root=$installed" `
  "--x-buildtrees-root=$scratch/Buildtrees" "--x-packages-root=$scratch/Packages"
if ($LASTEXITCODE -ne 0) { throw 'vcpkg dependency installation failed.' }
