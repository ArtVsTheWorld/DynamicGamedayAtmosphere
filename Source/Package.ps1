param([Parameter(Mandatory=$true)][string]$MmcRoot,[string]$ReleaseName='',[string]$BuildName='',[switch]$Install,[switch]$DeferSettings)
$ErrorActionPreference='Stop'
$product=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'Product.json') -Raw | ConvertFrom-Json
$crowdRoot=Split-Path -Parent $PSScriptRoot
if(-not $BuildName){$BuildName='Release'+$product.version.Replace('.','')}
if($BuildName -notmatch '^[A-Za-z0-9-]+$'){throw 'Invalid build folder name.'}
if($ReleaseName -and $ReleaseName -notmatch '^[A-Za-z0-9][A-Za-z0-9 .-]*$'){throw 'Use a simple release name such as Update 3.'}
$crowdBuild=Join-Path $crowdRoot ('Build\'+$BuildName)
$crowdSuffix=if($ReleaseName){$ReleaseName.Replace(' ','')}else{$product.version}
$crowdPackage=Join-Path $crowdRoot ('Release\'+$product.name+'-'+$crowdSuffix)
foreach($crowdRequired in @('FrostyCore.dll','FrostySdk.dll')){if(-not(Test-Path -LiteralPath (Join-Path $MmcRoot $crowdRequired))){throw 'MmcRoot must point to your MMC installation containing FrostyCore.dll and FrostySdk.dll.'}}
if(-not(Test-Path -LiteralPath (Join-Path $crowdBuild 'BuildInfo.cs'))){throw 'Build the project first with Source\Build.ps1 -Automatic.'}
$crowdPlugins=Join-Path $crowdPackage 'Plugins'
New-Item -ItemType Directory -Path $crowdPlugins -Force | Out-Null
$crowdPlugin=Join-Path $crowdPlugins ($product.name+'.dll')
$native=$product.name+'Native-'+$product.version+'.dll'
$csc=Join-Path $env:WINDIR 'Microsoft.NET\Framework64\v4.0.30319\csc.exe'
& $csc /nologo /target:library /platform:x64 "/out:$crowdPlugin" "/reference:$(Join-Path $MmcRoot 'FrostyCore.dll')" "/reference:$(Join-Path $MmcRoot 'FrostySdk.dll')" "/resource:$(Join-Path $crowdBuild $native),Attendance.$native" "/resource:$(Join-Path $crowdBuild 'LoadDynamicGamedayAtmosphere.exe'),Attendance.LoadDynamicGamedayAtmosphere.exe" "/resource:$(Join-Path $PSScriptRoot 'DynamicGamedayAtmosphere.ini'),Attendance.DynamicGamedayAtmosphere.ini" "/resource:$(Join-Path $crowdBuild 'DgaDiagnostics.exe'),Attendance.DgaDiagnostics.exe" (Join-Path $PSScriptRoot 'MmcPlugin.cs') (Join-Path $PSScriptRoot 'LaunchDiagnostics.cs') (Join-Path $PSScriptRoot 'PluginSettings.cs') (Join-Path $PSScriptRoot 'PluginStorage.cs') (Join-Path $crowdBuild 'BuildInfo.cs')
if($LASTEXITCODE -ne 0){throw 'MMC adapter compilation failed'}
Copy-Item -LiteralPath (Join-Path $crowdRoot 'README.md') -Destination $crowdPackage
Copy-Item -LiteralPath (Join-Path $crowdRoot 'FEATURES.md') -Destination $crowdPackage
if($Install){
    & (Join-Path $PSScriptRoot 'Install.ps1') -MmcRoot $MmcRoot -PackageRoot $crowdPackage
    $crowdInstalled=Join-Path $MmcRoot ('Plugins\'+$product.name+'\Runtime-'+$product.version)
    New-Item -ItemType Directory -Path $crowdInstalled -Force | Out-Null
    foreach($name in @($native,'LoadDynamicGamedayAtmosphere.exe')){Copy-Item -LiteralPath (Join-Path $crowdBuild $name) -Destination (Join-Path $crowdInstalled $name) -Force}
    if(-not $DeferSettings){
    & $csc /nologo ('/out:'+(Join-Path $crowdBuild 'MigrateSettings.exe')) (Join-Path $PSScriptRoot 'MigrateSettings.cs') (Join-Path $PSScriptRoot 'PluginSettings.cs') (Join-Path $PSScriptRoot 'PluginStorage.cs')
    if($LASTEXITCODE -ne 0){throw 'Settings migration build failed'}
    & (Join-Path $crowdBuild 'MigrateSettings.exe') (Join-Path $MmcRoot 'Plugins') (Join-Path $PSScriptRoot 'DynamicGamedayAtmosphere.ini')
    if($LASTEXITCODE -ne 0){throw 'Settings migration failed'}
    }
}
Compress-Archive -Path $crowdPlugins,(Join-Path $crowdPackage 'README.md'),(Join-Path $crowdPackage 'FEATURES.md') -DestinationPath (Join-Path $crowdRoot ('Release\'+$product.name+'-'+$crowdSuffix+'.zip')) -Force
Write-Output ('Packaged '+$crowdPackage)
