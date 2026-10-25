param([string]$MmcRoot='C:\Apps\CFB 27\MMC\MMC_ModManager_v1.1.0.6',[switch]$Install)
$ErrorActionPreference='Stop'
$product=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'Product.json') -Raw | ConvertFrom-Json
$crowdRoot=Split-Path -Parent $PSScriptRoot
$crowdBuild=Join-Path $crowdRoot ('Build\Release'+$product.version.Replace('.',''))
$crowdPackage=Join-Path $crowdRoot ('Release\'+$product.name+'-'+$product.version)
$crowdPlugins=Join-Path $crowdPackage 'Plugins'
New-Item -ItemType Directory -Path $crowdPlugins -Force | Out-Null
$crowdPlugin=Join-Path $crowdPlugins ($product.name+'.dll')
$native=$product.name+'Native-'+$product.version+'.dll'
$csc='C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe'
& $csc /nologo /target:library /platform:x64 "/out:$crowdPlugin" "/reference:$(Join-Path $MmcRoot 'FrostyCore.dll')" "/reference:$(Join-Path $MmcRoot 'FrostySdk.dll')" "/resource:$(Join-Path $crowdBuild $native),Attendance.$native" "/resource:$(Join-Path $crowdBuild 'LoadDynamicGamedayAtmosphere.exe'),Attendance.LoadDynamicGamedayAtmosphere.exe" "/resource:$(Join-Path $PSScriptRoot 'DynamicGamedayAtmosphere.ini'),Attendance.DynamicGamedayAtmosphere.ini" (Join-Path $PSScriptRoot 'MmcPlugin.cs') (Join-Path $PSScriptRoot 'PluginSettings.cs') (Join-Path $crowdBuild 'BuildInfo.cs')
if($LASTEXITCODE -ne 0){throw 'MMC adapter compilation failed'}
Copy-Item -LiteralPath (Join-Path $crowdRoot 'README.md') -Destination $crowdPackage
Copy-Item -LiteralPath (Join-Path $crowdRoot 'FEATURES.md') -Destination $crowdPackage
if($Install){
    & (Join-Path $PSScriptRoot 'Install.ps1') -MmcRoot $MmcRoot -PackageRoot $crowdPackage
    $crowdInstalled=Join-Path $MmcRoot ('Plugins\'+$product.name+'\Runtime-'+$product.version)
    New-Item -ItemType Directory -Path $crowdInstalled -Force | Out-Null
    foreach($name in @($native,'LoadDynamicGamedayAtmosphere.exe')){Copy-Item -LiteralPath (Join-Path $crowdBuild $name) -Destination (Join-Path $crowdInstalled $name) -Force}
    & $csc /nologo ('/out:'+(Join-Path $crowdBuild 'MigrateSettings.exe')) (Join-Path $PSScriptRoot 'MigrateSettings.cs') (Join-Path $PSScriptRoot 'PluginSettings.cs')
    if($LASTEXITCODE -ne 0){throw 'Settings migration build failed'}
    & (Join-Path $crowdBuild 'MigrateSettings.exe') (Join-Path $MmcRoot 'Plugins\DynamicGamedayAtmosphere.ini') (Join-Path $MmcRoot 'Plugins\DynamicAttendance.ini') (Join-Path $PSScriptRoot 'DynamicGamedayAtmosphere.ini')
    if($LASTEXITCODE -ne 0){throw 'Settings migration failed'}
}
Compress-Archive -Path $crowdPlugins,(Join-Path $crowdPackage 'README.md'),(Join-Path $crowdPackage 'FEATURES.md') -DestinationPath (Join-Path $crowdRoot ('Release\'+$product.name+'-'+$product.version+'.zip')) -Force
Write-Output ('Packaged '+$crowdPackage)
