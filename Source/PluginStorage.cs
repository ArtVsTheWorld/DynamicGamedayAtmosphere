using System;
using System.IO;
using System.Text;

namespace DynamicGamedayAtmosphere {
    // Only DGA-owned names are migrated. Other plugins' settings/logs are untouched.
    public static class PluginStorage {
        public static string DataFolder(string plugins){return Path.Combine(plugins,"DynamicGamedayAtmosphere");}
        public static string SettingsFile(string plugins){return Path.Combine(DataFolder(plugins),"DynamicGamedayAtmosphere.ini");}
        static void Retire(string path,string folder){
            if(!File.Exists(path))return;
            Directory.CreateDirectory(folder);
            string saved=Path.Combine(folder,Path.GetFileName(path)+"-"+DateTime.UtcNow.ToString("yyyyMMdd-HHmmssfff")+"-"+Guid.NewGuid().ToString("N")+".bak");
            File.Move(path,saved);
        }
        static string ExistingSettings(string plugins){
            foreach(string path in new[]{SettingsFile(plugins),Path.Combine(plugins,"DynamicGamedayAtmosphere.ini"),Path.Combine(plugins,"DynamicAttendance.ini")})
                if(File.Exists(path))return path;
            return null;
        }
        public static bool NeedsReset(string plugins,string defaults){
            string path=ExistingSettings(plugins);if(path==null)return false;
            string existing=File.ReadAllText(path);
            // A future schema belongs to a newer plugin. Preserve its refusal path.
            if(SettingsMigration.Number(existing,"General","ConfigVersion",1)>SettingsMigration.Number(defaults,"General","ConfigVersion",1))return false;
            int required=SettingsMigration.Number(defaults,"General","SettingsResetVersion",0);
            return required>0&&SettingsMigration.Number(existing,"General","SettingsResetVersion",0)<required;
        }
        public static string Ensure(string plugins,string defaults){
            string data=DataFolder(plugins),destination=SettingsFile(plugins);
            Directory.CreateDirectory(data);
            string modern=Path.Combine(plugins,"DynamicGamedayAtmosphere.ini"),legacy=Path.Combine(plugins,"DynamicAttendance.ini");
            if(!File.Exists(destination)){
                string source=File.Exists(modern)?modern:File.Exists(legacy)?legacy:null;
                // Copy before upgrading so even a future-schema file reaches
                // its new location unchanged and can refuse activation safely.
                if(source!=null)File.Copy(source,destination,false);
            }
            string backup=Path.Combine(data,"Backups"),resetBackup=null;
            if(NeedsReset(plugins,defaults)){
                Directory.CreateDirectory(backup);
                resetBackup=Path.Combine(backup,"DynamicGamedayAtmosphere.ini-before-Update3-"+DateTime.UtcNow.ToString("yyyyMMdd-HHmmssfff")+"-"+Guid.NewGuid().ToString("N")+".bak");
                string temp=destination+".reset-"+Guid.NewGuid().ToString("N")+".tmp";
                try{
                    // Replace atomically and keep the exact old bytes. Failure leaves
                    // the original active and stops startup instead of losing settings.
                    File.WriteAllText(temp,defaults,Encoding.Unicode);
                    File.Replace(temp,destination,resetBackup,true);
                }finally{if(File.Exists(temp))File.Delete(temp);}
            }
            SettingsMigration.Ensure(destination,legacy,defaults);
            if(!File.Exists(destination))throw new IOException("DGA settings were not created; old settings have been left in place.");
            Retire(modern,backup);Retire(legacy,backup);
            foreach(string file in new[]{modern,legacy})foreach(string suffix in new[]{".previous",".tmp"})Retire(file+suffix,backup);
            // Do not execute showcase commands left by an older runtime.
            Retire(Path.Combine(plugins,"DynamicGamedayAtmosphere-control.txt"),backup);
            foreach(string name in new[]{"DynamicGamedayAtmosphere.log","DynamicGamedayAtmosphere-launcher.log","DynamicGamedayAtmosphere-status.txt"})
                Retire(Path.Combine(plugins,name),backup);
            return resetBackup;
        }
    }
}
