using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace DynamicGamedayAtmosphere {
    // Only DGA-owned names are migrated. Other plugins' settings/logs are untouched.
    public static class PluginStorage {
        public static string DataFolder(string plugins){return Path.Combine(plugins,"DynamicGamedayAtmosphere");}
        public static string SettingsFile(string plugins){return Path.Combine(DataFolder(plugins),"DynamicGamedayAtmosphere.ini");}
        static void Retire(string path,string folder){
            if(!File.Exists(path))return;
            SettingsIo.Run("create retired-file backup folder",null,folder,null,()=>Directory.CreateDirectory(folder));
            string saved=Path.Combine(folder,Path.GetFileName(path)+"-"+Guid.NewGuid().ToString("N")+".bak");
            SettingsIo.Run("archive previous DGA file",path,saved,null,()=>File.Move(path,saved));
        }
        static string ExistingSettings(string plugins){
            foreach(string path in new[]{SettingsFile(plugins),Path.Combine(plugins,"DynamicGamedayAtmosphere.ini"),Path.Combine(plugins,"DynamicAttendance.ini")})
                if(File.Exists(path))return path;
            return null;
        }
        // Exact DGA helper names/versioned native runtimes only. The MMC adapter
        // stays beside the other plugins; unknown files are never swept up.
        static IEnumerable<string> LooseHelpers(string plugins){
            if(!Directory.Exists(plugins))yield break;
            foreach(string file in Directory.GetFiles(plugins)){
                string name=Path.GetFileName(file);
                if(String.Equals(name,"LoadDynamicGamedayAtmosphere.exe",StringComparison.OrdinalIgnoreCase)||String.Equals(name,"LoadDynamicAttendance.exe",StringComparison.OrdinalIgnoreCase)||String.Equals(name,"DgaDiagnostics.exe",StringComparison.OrdinalIgnoreCase)||
                   Regex.IsMatch(name,@"^(DynamicGamedayAtmosphereNative|DynamicAttendanceNative)-\d+\.\d+\.\d+\.dll$",RegexOptions.IgnoreCase))yield return file;
            }
        }
        public static bool NeedsRelocation(string plugins){foreach(string file in LooseHelpers(plugins)){if(File.Exists(file))return true;}return false;}
        public static bool NeedsReset(string plugins,string defaults){
            string path=ExistingSettings(plugins);if(path==null)return false;
            string existing=SettingsIo.ReadText(path);
            // A future schema belongs to a newer plugin. Preserve its refusal path.
            if(SettingsMigration.Number(existing,"General","ConfigVersion",1)>SettingsMigration.Number(defaults,"General","ConfigVersion",1))return false;
            int required=SettingsMigration.Number(defaults,"General","SettingsResetVersion",0);
            return required>0&&SettingsMigration.Number(existing,"General","SettingsResetVersion",0)<required;
        }
        public static string Ensure(string plugins,string defaults){
            string data=DataFolder(plugins),destination=SettingsFile(plugins);
            SettingsIo.Run("create DGA settings folder",null,data,null,()=>Directory.CreateDirectory(data));
            string modern=Path.Combine(plugins,"DynamicGamedayAtmosphere.ini"),legacy=Path.Combine(plugins,"DynamicAttendance.ini");
            if(!File.Exists(destination)){
                string source=File.Exists(modern)?modern:File.Exists(legacy)?legacy:null;
                // Copy before upgrading so even a future-schema file reaches
                // its new location unchanged and can refuse activation safely.
                if(source!=null)SettingsIo.Run("copy previous settings into DGA folder",source,destination,null,()=>File.Copy(source,destination,false));
            }
            string backup=Path.Combine(data,"Backups"),resetBackup=null;
            if(NeedsReset(plugins,defaults)){
                SettingsIo.Run("create settings-reset backup folder",null,backup,null,()=>Directory.CreateDirectory(backup));
                // Short, unique names keep migration within legacy MMC path limits.
                resetBackup=Path.Combine(backup,"settings-before-update3-"+Guid.NewGuid().ToString("N")+".bak");
                string temp=Path.Combine(data,"reset-"+Guid.NewGuid().ToString("N")+".tmp");Exception failure=null;
                try{
                    // Replace atomically and keep the exact old bytes. Failure leaves
                    // the original active and stops startup instead of losing settings.
                    SettingsIo.Run("write temporary Update 3 settings",null,temp,null,()=>File.WriteAllText(temp,defaults,Encoding.Unicode));
                    SettingsIo.Run("replace settings with Update 3 defaults",temp,destination,resetBackup,()=>File.Replace(temp,destination,resetBackup,true));
                }catch(Exception error){failure=error;throw;}
                finally{SettingsIo.Cleanup(temp,failure);}
            }
            SettingsMigration.Ensure(destination,legacy,defaults);
            if(!File.Exists(destination))throw new IOException("DGA settings were not created; old settings have been left in place.");
            Retire(modern,backup);Retire(legacy,backup);
            foreach(string file in new[]{modern,legacy})foreach(string suffix in new[]{".previous",".tmp"})Retire(file+suffix,backup);
            // Do not execute showcase commands left by an older runtime.
            Retire(Path.Combine(plugins,"DynamicGamedayAtmosphere-control.txt"),backup);
            foreach(string name in new[]{"DynamicGamedayAtmosphere.log","DynamicGamedayAtmosphere-launcher.log","DynamicGamedayAtmosphere-status.txt"})
                Retire(Path.Combine(plugins,name),backup);
            foreach(string file in LooseHelpers(plugins))Retire(file,backup);
            return resetBackup;
        }
    }
}
