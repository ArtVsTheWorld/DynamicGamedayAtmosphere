param([Parameter(Mandatory=$true)][string]$MmcRoot)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot;$source=Join-Path $root 'Source'
$product=Get-Content -LiteralPath (Join-Path $source 'Product.json') -Raw | ConvertFrom-Json
$build=Join-Path $root ('Build\Release'+$product.version.Replace('.',''))
$fixture=Join-Path $root ('Build\HostCheck-'+[Guid]::NewGuid().ToString('N'));$plugins=Join-Path $fixture 'Plugins'
New-Item -ItemType Directory -Path $plugins -Force | Out-Null
$csc=Join-Path $env:WINDIR 'Microsoft.NET\Framework64\v4.0.30319\csc.exe'
$native=$product.name+'Native-'+$product.version+'.dll'
$resources=@(('/resource:'+(Join-Path $build $native)+',Attendance.'+$native),('/resource:'+(Join-Path $build 'LoadDynamicGamedayAtmosphere.exe')+',Attendance.LoadDynamicGamedayAtmosphere.exe'),('/resource:'+(Join-Path $source 'DynamicGamedayAtmosphere.ini')+',Attendance.DynamicGamedayAtmosphere.ini'),('/resource:'+(Join-Path $build 'DgaDiagnostics.exe')+',Attendance.DgaDiagnostics.exe'))
& $csc /nologo /target:library /platform:x64 ('/out:'+(Join-Path $plugins 'DynamicGamedayAtmosphere.dll')) ('/reference:'+(Join-Path $MmcRoot 'FrostyCore.dll')) ('/reference:'+(Join-Path $MmcRoot 'FrostySdk.dll')) @resources (Join-Path $source 'MmcPlugin.cs') (Join-Path $source 'LaunchDiagnostics.cs') (Join-Path $source 'PluginSettings.cs') (Join-Path $source 'PluginStorage.cs') (Join-Path $build 'BuildInfo.cs')
if($LASTEXITCODE -ne 0){throw 'Adapter compilation failed'}
& $csc /nologo ('/out:'+(Join-Path $fixture 'HostChecks.exe')) ('/reference:'+(Join-Path $MmcRoot 'FrostySdk.dll')) (Join-Path $root 'Tests\HostChecks.cs')
if($LASTEXITCODE -ne 0){throw 'Host check compilation failed'}
& (Join-Path $fixture 'HostChecks.exe') $MmcRoot $plugins
if($LASTEXITCODE -ne 0){throw 'Host ABI/resource/storage checks failed'}
if(Get-ChildItem -LiteralPath $plugins -File | Where-Object {$_.Extension -in @('.ini','.log')}){throw 'A DGA INI/log escaped its owned data folder'}
Write-Output ('PASS: isolated adapter verified at '+$plugins+'. No installed files or release archive changed.')
