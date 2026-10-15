param(
  [Parameter(Mandatory = $true)][string]$ClientDirectory,
  [Parameter(Mandatory = $true)][string]$RuntimeDirectory
)

$ErrorActionPreference = 'Stop'
$clientRoot = (Resolve-Path -LiteralPath $ClientDirectory).Path
$runtimeRoot = (Resolve-Path -LiteralPath $RuntimeDirectory).Path
$modelsRoot = Join-Path $runtimeRoot 'Models'
if (Test-Path -LiteralPath $modelsRoot) {
  throw 'The runtime Models directory must be new; resource junctions cannot be patched in place.'
}
New-Item -ItemType Directory -Path $modelsRoot | Out-Null
foreach ($entry in Get-ChildItem -LiteralPath (Join-Path $clientRoot 'Models')) {
  if ($entry.Name -ceq 'Characters') { continue }
  $target = Join-Path $modelsRoot $entry.Name
  if ($entry.PSIsContainer) {
    New-Item -ItemType Junction -Path $target -Target $entry.FullName | Out-Null
  }
  else {
    Copy-Item -LiteralPath $entry.FullName -Destination $target
  }
}
$charactersRoot = Join-Path $modelsRoot 'Characters'
New-Item -ItemType Directory -Path $charactersRoot | Out-Null
foreach ($entry in Get-ChildItem -LiteralPath (Join-Path $clientRoot 'Models\Characters')) {
  if ($entry.Name -ceq 'Characters.bfz') { continue }
  $target = Join-Path $charactersRoot $entry.Name
  if ($entry.PSIsContainer) {
    New-Item -ItemType Junction -Path $target -Target $entry.FullName | Out-Null
  }
  else {
    Copy-Item -LiteralPath $entry.FullName -Destination $target
  }
}
& python (Join-Path $PSScriptRoot 'prepare_client_resources.py') --client-directory $clientRoot --runtime-directory $runtimeRoot
if ($LASTEXITCODE -ne 0) { throw 'Client portrait resource preparation failed.' }
