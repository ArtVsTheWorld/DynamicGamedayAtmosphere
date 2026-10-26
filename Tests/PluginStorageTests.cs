using System;using System.IO;using System.Linq;
using DynamicGamedayAtmosphere;
class PluginStorageTests {
 static void Check(bool okay){if(!okay)throw new Exception("Storage migration regression");}
 static int Main(){
  string root=Path.Combine(Path.GetTempPath(),"DGA-storage-"+Guid.NewGuid().ToString("N"));Directory.CreateDirectory(root);
  string defaults=File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.BaseDirectory,"..","..","Source","DynamicGamedayAtmosphere.ini"));
  try{
   string modern=Path.Combine(root,"DynamicGamedayAtmosphere.ini"),legacy=Path.Combine(root,"DynamicAttendance.ini"),nested=PluginStorage.SettingsFile(root);
   File.WriteAllText(modern,"[General]\nConfigVersion=2\nAudioBalanceVersion=39\n[Dynamic Gameday Attendance]\nStartPercent=47\n[Custom]\nMyValue=keep\n");
   File.WriteAllText(legacy,"old legacy content");File.WriteAllText(Path.Combine(root,"AnotherPlugin.ini"),"untouched");
   File.WriteAllText(Path.Combine(root,"DynamicGamedayAtmosphere.log"),"old log");File.WriteAllText(Path.Combine(root,"DynamicGamedayAtmosphere-control.txt"),"empty");
   PluginStorage.Ensure(root,defaults);
   string text=File.ReadAllText(nested);Check(text.Contains("StartPercent=47")&&text.Contains("MyValue=keep")&&!File.Exists(modern)&&!File.Exists(legacy));
   string backups=Path.Combine(PluginStorage.DataFolder(root),"Backups");Check(Directory.GetFiles(backups).Length==4&&File.ReadAllText(Path.Combine(root,"AnotherPlugin.ini"))=="untouched");
   PluginStorage.Ensure(root,defaults);Check(File.ReadAllText(nested)==text&&Directory.GetFiles(backups).Length==4);
   File.WriteAllText(modern,"[Custom]\nMyValue=stale\n");PluginStorage.Ensure(root,defaults);Check(File.ReadAllText(nested).Contains("MyValue=keep")&&Directory.GetFiles(backups).Length==5);
   File.WriteAllText(modern+".previous","previous settings");File.WriteAllText(modern+".tmp","interrupted migration");PluginStorage.Ensure(root,defaults);Check(Directory.GetFiles(backups).Length==7&&!File.Exists(modern+".previous")&&!File.Exists(modern+".tmp"));
   string future=Path.Combine(root,"Future");Directory.CreateDirectory(future);File.WriteAllText(Path.Combine(future,"DynamicGamedayAtmosphere.ini"),"[General]\nConfigVersion=999\nKeepExactly=yes\n");
   PluginStorage.Ensure(future,defaults);Check(File.ReadAllText(PluginStorage.SettingsFile(future)).Contains("ConfigVersion=999")&&!File.Exists(Path.Combine(future,"DynamicGamedayAtmosphere.ini")));
   string failure=Path.Combine(root,"Failure");Directory.CreateDirectory(failure);string old=Path.Combine(failure,"DynamicGamedayAtmosphere.ini");File.WriteAllText(old,"precious old settings");File.WriteAllText(PluginStorage.DataFolder(failure),"not a directory");
   bool failed=false;try{PluginStorage.Ensure(failure,defaults);}catch(IOException){failed=true;}Check(failed&&File.ReadAllText(old)=="precious old settings");
   Console.WriteLine("PASS: nested settings, preserved customs/future schemas, existing nested precedence, idempotent retirement, original backups, unrelated-file isolation and failed migration preservation.");return 0;
  }catch(Exception e){Console.Error.WriteLine(e);return 1;}
 }
}
