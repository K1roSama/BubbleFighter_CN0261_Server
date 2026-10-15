param(
  [Parameter(Mandatory = $true)][string]$ClientDirectory,
  [Parameter(Mandatory = $true)][string]$RuntimeDirectory
)

$ErrorActionPreference = 'Stop'
$clientRoot = (Resolve-Path -LiteralPath $ClientDirectory).Path
$runtimeRoot = [IO.Path]::GetFullPath($RuntimeDirectory)
if (Test-Path -LiteralPath $runtimeRoot) {
  throw 'The client runtime directory must be new.'
}
$clientImage = Join-Path $clientRoot 'BubbleFighter.exe'
if ((Get-FileHash -LiteralPath $clientImage -Algorithm SHA256).Hash -ne '4c16c49e4dcf90b3fb8422918dce7698acbe7f1c1e318eccddd71c444284ff4b') {
  throw 'The selected client image does not match CN0261.'
}
New-Item -ItemType Directory -Path $runtimeRoot | Out-Null
Copy-Item -LiteralPath $clientImage -Destination $runtimeRoot
Get-ChildItem -LiteralPath $clientRoot -File | Where-Object Extension -in @('.dll', '.dat', '.bin', '.ini') |
  Copy-Item -Destination $runtimeRoot
if (Test-Path -LiteralPath (Join-Path $clientRoot 'Config')) {
  Copy-Item -LiteralPath (Join-Path $clientRoot 'Config') -Destination $runtimeRoot -Recurse
}
foreach ($directory in @('Data', 'Dds', 'Effects', 'HShield', 'Images', 'Mapbound', 'Maps', 'NpcMode', 'Promotion', 'Scenes', 'ServerShared', 'Shader', 'Sounds', 'StringTable', 'Ui', 'SDO', 'sdologin', 'dengluweb', 'SDSkin')) {
  $source = Join-Path $clientRoot $directory
  if (Test-Path -LiteralPath $source -PathType Container) {
    New-Item -ItemType Junction -Path (Join-Path $runtimeRoot $directory) -Target $source | Out-Null
  }
}
& (Join-Path $PSScriptRoot 'prepare_client_resources.ps1') -ClientDirectory $clientRoot -RuntimeDirectory $runtimeRoot
New-Item -ItemType Directory -Path (Join-Path $runtimeRoot 'Dump') | Out-Null
