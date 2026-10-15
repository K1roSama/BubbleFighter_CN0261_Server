function Get-SourceState {
  param([Parameter(Mandatory = $true)][string]$ProjectRoot)

  $resolvedRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
  $paths = @(
    Get-ChildItem -LiteralPath (Join-Path $resolvedRoot 'src'),
      (Join-Path $resolvedRoot 'config'), (Join-Path $resolvedRoot 'tests') -File -Recurse |
      Where-Object { $_.Extension -in @('.cpp', '.h', '.vcxproj', '.json') }
    Get-Item -LiteralPath (Join-Path $resolvedRoot 'BubbleFighter_CN0261_Server.sln'),
      (Join-Path $resolvedRoot 'build/BubbleFighter_CN0261_Server.vcxproj'), (Join-Path $resolvedRoot 'build/server_core.vcxproj'), (Join-Path $resolvedRoot 'build/regression.vcxproj'),
      (Join-Path $resolvedRoot 'build/common.props'), (Join-Path $resolvedRoot 'vcpkg.json'),
      (Join-Path $resolvedRoot 'vcpkg-configuration.json'), (Join-Path $resolvedRoot 'build/triplets/x64-windows.cmake')
  )
  $state = [ordered]@{}
  foreach ($path in ($paths | Sort-Object FullName)) {
    $relative = $path.FullName.Substring($resolvedRoot.Length + 1).Replace('\', '/')
    $state[$relative] = (Get-FileHash -LiteralPath $path.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
  }
  return $state
}

function Assert-SourceState {
  param(
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Expected,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Actual
  )

  $changed = @()
  foreach ($path in $Expected.Keys) {
    if ($Actual.Keys -cnotcontains $path -or $Actual[$path] -cne $Expected[$path]) {
      $changed += $path
    }
  }
  foreach ($path in $Actual.Keys) {
    if ($Expected.Keys -cnotcontains $path) {
      $changed += $path
    }
  }
  if ($changed.Count -gt 0) {
    throw ('Source files changed during verification. Inspect and rebuild: ' + ($changed -join ', '))
  }
}
