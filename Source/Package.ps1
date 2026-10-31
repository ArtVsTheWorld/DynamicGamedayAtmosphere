param([Parameter(Mandatory=$true)][string]$MmcRoot,[string]$ReleaseName='',[string]$BuildName='',[switch]$Install,[switch]$DeferSettings)
$ErrorActionPreference='Stop'
$product=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'Product.json') -Raw | ConvertFrom-Json
$crowdRoot=Split-Path -Parent $PSScriptRoot
if(-not $BuildName){$BuildName='Release'+$product.version.Replace('.','')}
if($BuildName -notmatch '^[A-Za-z0-9-]+$'){throw 'Invalid build folder name.'}
if($ReleaseName -and $ReleaseName -notmatch '^[A-Za-z0-9][A-Za-z0-9 .-]*$'){throw 'Use a simple release name such as Update 3 Hotfix.'}
$crowdBuild=Join-Path $crowdRoot ('Build\'+$BuildName)
$crowdSuffix=if($ReleaseName){$ReleaseName.Replace(' ','')}else{$product.version}
$crowdPackage=[IO.Path]::GetFullPath((Join-Path $crowdRoot ('Release\'+$product.name+'-'+$crowdSuffix)))
$loaderRelative='DynamicGamedayAtmosphere\Runtime-'+$product.version+'\LoadDynamicGamedayAtmosphere.exe'
$wanted=@('README.md','FEATURES.md','DynamicGamedayAtmosphere.dll',$loaderRelative)
if(Test-Path -LiteralPath $crowdPackage){
    foreach($file in Get-ChildItem -LiteralPath $crowdPackage -Recurse -File){
        if($wanted -notcontains $file.FullName.Substring($crowdPackage.Length+1)){throw 'Package output contains other files. Use a fresh release name.'}
    }
}
# Ship the exact adapter that passed isolated checks, with its separate loader.
$hostOutput=@(& (Join-Path $crowdRoot 'Tools\CheckMmcHost.ps1') -MmcRoot $MmcRoot -BuildName $BuildName -ExternalLoader)
$hostOutput | ForEach-Object {Write-Output $_}
$verified=[regex]::Match(($hostOutput -join [Environment]::NewLine),'PASS: isolated adapter verified at (.*?Plugins)\. No installed files')
if(-not $verified.Success){throw 'Isolated MMC verification did not return a verified adapter.'}
$adapter=Join-Path $verified.Groups[1].Value 'DynamicGamedayAtmosphere.dll'
$loader=Join-Path $crowdBuild 'LoadDynamicGamedayAtmosphere.exe'
New-Item -ItemType Directory -Path (Split-Path -Parent (Join-Path $crowdPackage $loaderRelative)) -Force | Out-Null
Copy-Item -LiteralPath $adapter -Destination (Join-Path $crowdPackage 'DynamicGamedayAtmosphere.dll') -Force
Copy-Item -LiteralPath $loader -Destination (Join-Path $crowdPackage $loaderRelative) -Force
foreach($doc in @('README.md','FEATURES.md')){Copy-Item -LiteralPath (Join-Path $crowdRoot $doc) -Destination (Join-Path $crowdPackage $doc) -Force}
foreach($pair in @(@($adapter,'DynamicGamedayAtmosphere.dll'),@($loader,$loaderRelative))){
    if((Get-FileHash -LiteralPath $pair[0]).Hash -ne (Get-FileHash -LiteralPath (Join-Path $crowdPackage $pair[1])).Hash){throw 'A packaged runtime file failed hash verification.'}
}
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=Join-Path $crowdRoot ('Release\'+$product.name+'-'+$crowdSuffix+'.zip')
$stream=[IO.File]::Open($archive,[IO.FileMode]::Create,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
try{
    $zip=New-Object IO.Compression.ZipArchive($stream,[IO.Compression.ZipArchiveMode]::Create,$true)
    try{foreach($name in $wanted){[IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip,(Join-Path $crowdPackage $name),$name.Replace('\','/'),[IO.Compression.CompressionLevel]::Optimal) | Out-Null}}
    finally{$zip.Dispose()}
}finally{$stream.Dispose()}
& (Join-Path $crowdRoot 'Tools\VerifyRelease.ps1') -Archive $archive
if($Install){
    & (Join-Path $PSScriptRoot 'Install.ps1') -MmcRoot $MmcRoot -PackageRoot $crowdPackage
    if(-not $DeferSettings){
        $csc=Join-Path $env:WINDIR 'Microsoft.NET\Framework64\v4.0.30319\csc.exe'
        & $csc /nologo ('/out:'+(Join-Path $crowdBuild 'MigrateSettings.exe')) (Join-Path $PSScriptRoot 'MigrateSettings.cs') (Join-Path $PSScriptRoot 'PluginSettings.cs') (Join-Path $PSScriptRoot 'PluginStorage.cs')
        if($LASTEXITCODE -ne 0){throw 'Settings migration build failed'}
        & (Join-Path $crowdBuild 'MigrateSettings.exe') (Join-Path $MmcRoot 'Plugins') (Join-Path $PSScriptRoot 'DynamicGamedayAtmosphere.ini')
        if($LASTEXITCODE -ne 0){throw 'Settings migration failed'}
    }
}
Write-Output ('Packaged '+$crowdPackage+' with a separate loader. No publishing performed.')
