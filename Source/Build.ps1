param([string]$Version='', [switch]$Automatic)
$ErrorActionPreference='Stop'
$product=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'Product.json') -Raw | ConvertFrom-Json
if(-not $Version){$Version='Release'+$product.version.Replace('.','')}
if($product.version -notmatch '^\d+\.\d+\.\d+$'){throw 'Invalid product version'}
if($Version -notmatch '^[A-Za-z0-9-]+$'){throw 'Invalid build name'}
$crowdRoot=Split-Path -Parent $PSScriptRoot
$crowdOut=Join-Path $crowdRoot ('Build\'+$Version)
New-Item -ItemType Directory -Path $crowdOut -Force | Out-Null
$crowdVs=& 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe' -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$crowdTool=Get-ChildItem -LiteralPath (Join-Path $crowdVs 'VC\Tools\MSVC') -Directory | Sort-Object Name -Descending | Select-Object -First 1
$crowdKits='C:\Program Files (x86)\Windows Kits\10'
$crowdSdk=Get-ChildItem -LiteralPath (Join-Path $crowdKits 'Include') -Directory | Sort-Object Name -Descending | Select-Object -First 1
$crowdCl=Join-Path $crowdTool.FullName 'bin\Hostx64\x64\cl.exe'
$crowdMl=Join-Path $crowdTool.FullName 'bin\Hostx64\x64\ml64.exe'
$crowdInclude=@(('/I'+(Join-Path $crowdTool.FullName 'include')),('/I'+(Join-Path $crowdSdk.FullName 'ucrt')),('/I'+(Join-Path $crowdSdk.FullName 'shared')),('/I'+(Join-Path $crowdSdk.FullName 'um')))
$crowdLib=@(('/LIBPATH:'+(Join-Path $crowdTool.FullName 'lib\x64')),('/LIBPATH:'+(Join-Path $crowdKits ('Lib\'+$crowdSdk.Name+'\ucrt\x64'))),('/LIBPATH:'+(Join-Path $crowdKits ('Lib\'+$crowdSdk.Name+'\um\x64'))))
$productHeader='#pragma once'+[Environment]::NewLine+'#define GAMEDAY_VERSION "'+$product.version+'"'+[Environment]::NewLine+'#define GAMEDAY_SCHEMA '+$product.schema+[Environment]::NewLine
Set-Content -LiteralPath (Join-Path $crowdOut 'ProductVersion.h') -Value $productHeader -Encoding ascii
$buildInfo='namespace DynamicGamedayAtmosphere { internal static class BuildInfo { public const string Version="'+$product.version+'"; public const string AssemblyVersion="'+$product.version+'.0"; public const string NativeFile="DynamicGamedayAtmosphereNative-'+$product.version+'.dll"; } }'
Set-Content -LiteralPath (Join-Path $crowdOut 'BuildInfo.cs') -Value $buildInfo -Encoding ascii
$crowdInclude+=('/I'+$crowdOut)
$crowdVendor=Join-Path $crowdRoot '..\WeeklyEvents\Vendor\minhook\src'
Push-Location $crowdOut
try {
    & $crowdCl /nologo /O2 /MT /W3 @crowdInclude /c (Join-Path $crowdVendor 'buffer.c') (Join-Path $crowdVendor 'hook.c') (Join-Path $crowdVendor 'trampoline.c') (Join-Path $crowdVendor 'hde\hde64.c')
    if($LASTEXITCODE -ne 0){throw 'MinHook build failed'}
    $crowdObjects=@()
    if(-not $Automatic){
        & $crowdMl /nologo /c (Join-Path $PSScriptRoot 'TickGate.asm')
        if($LASTEXITCODE -ne 0){throw 'Tick gate build failed'}
        $crowdObjects=@('TickGate.obj')
    }
    $crowdSource=Join-Path $PSScriptRoot 'Probe.cpp'
    $crowdDll='DynamicAttendanceProbe-'+$Version+'.dll'
    if($Automatic){$crowdSource=Join-Path $PSScriptRoot 'NativeRuntime.cpp';$crowdDll='DynamicGamedayAtmosphereNative-'+$product.version+'.dll'}
    & $crowdCl /nologo /O2 /MT /W4 /WX /EHsc /std:c++17 /DUNICODE /D_UNICODE /Zi @crowdInclude /LD $crowdSource @crowdObjects ('/Fe:'+$crowdDll) /link @crowdLib /DEBUG /INCREMENTAL:NO
    if($LASTEXITCODE -ne 0){throw 'Probe build failed'}
    if($Automatic){
        & $crowdCl /nologo /O2 /MT /W4 /WX /EHsc /std:c++17 @crowdInclude (Join-Path $crowdRoot 'Tests\RulesTests.cpp') /Fe:RulesTests.exe /link @crowdLib
        if($LASTEXITCODE -ne 0){throw 'Rule checks build failed'}
        & '.\RulesTests.exe'
        if($LASTEXITCODE -ne 0){throw 'Rule checks failed'}
        foreach($crowdTest in @('CoreAtmosphereTests','CrowdAudioTests','CrowdAudioRuntimeTests','ChantMixerTests','ChantRuntimeTests','AtmosphereTests','EnergyTests','AtmosphereBalanceTests','OpeningAndLimitsTests','MomentumOvertimeTests','DepartureModelTests','AttendanceBalanceTests','ArrivalTests','VisibilityDispatchTests','LoggingTests','SustainedCrowdTests','SwellRuntimeTests','ReactionRuntimeTests','CrowdIntensityTests','CrowdChangeWindowTests','AttendanceTimelineTests','ResumeBridgeTests','ReplayUpdateHookTests','ReplayPresentationTests','SeatPatternTests','SeatRuntimeTests')){
            & $crowdCl /nologo /O2 /MT /W4 /WX /EHsc /std:c++17 @crowdInclude (Join-Path $crowdRoot ('Tests\'+$crowdTest+'.cpp')) ('/Fe:'+$crowdTest+'.exe') /link @crowdLib
            if($LASTEXITCODE -ne 0){throw ($crowdTest+' build failed')}
            & ('.\'+$crowdTest+'.exe')
            if($LASTEXITCODE -ne 0){throw ($crowdTest+' failed')}
        }
        & $crowdCl /nologo /O2 /MT /W4 /WX /EHsc /std:c++17 /DUNICODE /D_UNICODE @crowdInclude (Join-Path $PSScriptRoot 'LoadRuntime.cpp') /Fe:LoadDynamicGamedayAtmosphere.exe /link @crowdLib
        if($LASTEXITCODE -ne 0){throw 'Loader build failed'}
        & 'C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe' /nologo /out:SettingsTests.exe (Join-Path $crowdRoot 'Tests\SettingsTests.cs') (Join-Path $PSScriptRoot 'PluginSettings.cs')
        if($LASTEXITCODE -ne 0){throw 'Settings tests build failed'}
        & '.\SettingsTests.exe'
        if($LASTEXITCODE -ne 0){throw 'Settings tests failed'}
        if(-not(Test-Path -LiteralPath 'DynamicGamedayAtmosphere.ini')){Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'DynamicGamedayAtmosphere.ini') -Destination 'DynamicGamedayAtmosphere.ini'}
    }
} finally {Pop-Location}
Write-Output ('Built '+$crowdOut)
