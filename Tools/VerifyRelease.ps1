param([Parameter(Mandatory=$true)][string]$Archive)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
$path=(Resolve-Path -LiteralPath $Archive).Path
$zip=[IO.Compression.ZipFile]::OpenRead($path)
try {
 $wanted=@('FEATURES.md','Plugins/DynamicGamedayAtmosphere.dll','README.md')
 $actual=@($zip.Entries | ForEach-Object {$_.FullName.Replace('\','/')} | Sort-Object)
 if(($actual -join '|') -ne ($wanted -join '|')){throw ('Unexpected ZIP contents: '+($actual -join ', '))}
 foreach($entry in $zip.Entries){
  if($entry.Length -le 0){throw ('Empty entry: '+$entry.FullName)}
  if($entry.FullName.EndsWith('.md')){
   $reader=New-Object IO.StreamReader($entry.Open())
   try {$text=$reader.ReadToEnd()}finally{$reader.Dispose()}
   if($text -match '(?i)DynamicAttendance\.dll'){throw 'A retired DLL name appears in public documentation.'}
  }
 }
}finally{$zip.Dispose()}
Write-Output ('PASS: plugin, README and feature list. SHA256 '+(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant())
