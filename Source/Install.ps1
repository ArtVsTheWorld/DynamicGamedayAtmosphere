param([Parameter(Mandatory=$true)][string]$MmcRoot,[string]$PackageRoot=$PSScriptRoot)
$ErrorActionPreference='Stop'
$pluginRoot=[IO.Path]::GetFullPath((Join-Path $MmcRoot 'Plugins'))
if(-not(Test-Path -LiteralPath (Join-Path $MmcRoot 'FrostyCore.dll'))){throw 'Choose the MMC Mod Manager folder containing FrostyCore.dll.'}
$source=Join-Path $PackageRoot 'Plugins\DynamicGamedayAtmosphere.dll'
if(-not(Test-Path -LiteralPath $source)){throw 'The package DLL is missing.'}
$backup=Join-Path $pluginRoot 'DynamicGamedayAtmosphere\Backups'
New-Item -ItemType Directory -Path $backup -Force | Out-Null
$staged=Join-Path $pluginRoot 'DynamicGamedayAtmosphere.dll.pending'
Copy-Item -LiteralPath $source -Destination $staged -Force
if((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $staged).Hash){throw 'The copied DLL did not verify.'}
$moved=@()
try {
    foreach($name in @('DynamicAttendance.dll','DynamicGamedayAtmosphere.dll')){
        $target=[IO.Path]::GetFullPath((Join-Path $pluginRoot $name))
        if(-not $target.StartsWith($pluginRoot+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Invalid plugin destination.'}
        if(Test-Path -LiteralPath $target){
            $saved=Join-Path $backup ($name+'-'+[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmssfff')+'.bak')
            Move-Item -LiteralPath $target -Destination $saved -ErrorAction Stop
            $moved+=@{Original=$target;Backup=$saved}
        }
    }
    Move-Item -LiteralPath $staged -Destination (Join-Path $pluginRoot 'DynamicGamedayAtmosphere.dll') -ErrorAction Stop
} catch {
    [array]::Reverse($moved)
    foreach($item in $moved){if(-not(Test-Path -LiteralPath $item.Original)){Move-Item -LiteralPath $item.Backup -Destination $item.Original}}
    if(Test-Path -LiteralPath $staged){Remove-Item -LiteralPath $staged}
    throw
}
Write-Output 'Installed Dynamic Gameday Atmosphere. Previous DLLs are backed up. Restart both MMC and the game. On first Update 3 startup, old DGA settings are backed up and reset to current defaults; subsequent custom settings are preserved.'
