using System;
using System.IO;

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
        public static void Ensure(string plugins,string defaults){
            string data=DataFolder(plugins),destination=SettingsFile(plugins);
            Directory.CreateDirectory(data);
            string modern=Path.Combine(plugins,"DynamicGamedayAtmosphere.ini"),legacy=Path.Combine(plugins,"DynamicAttendance.ini");
            if(!File.Exists(destination)){
                string source=File.Exists(modern)?modern:File.Exists(legacy)?legacy:null;
                // Copy before upgrading so even a future-schema file reaches
                // its new location unchanged and can refuse activation safely.
                if(source!=null)File.Copy(source,destination,false);
            }
            SettingsMigration.Ensure(destination,legacy,defaults);
            if(!File.Exists(destination))throw new IOException("DGA settings were not created; old settings have been left in place.");
            string backup=Path.Combine(data,"Backups");
            Retire(modern,backup);Retire(legacy,backup);
            foreach(string file in new[]{modern,legacy})foreach(string suffix in new[]{".previous",".tmp"})Retire(file+suffix,backup);
            // Do not execute showcase commands left by an older runtime.
            Retire(Path.Combine(plugins,"DynamicGamedayAtmosphere-control.txt"),backup);
            foreach(string name in new[]{"DynamicGamedayAtmosphere.log","DynamicGamedayAtmosphere-launcher.log","DynamicGamedayAtmosphere-status.txt"})
                Retire(Path.Combine(plugins,name),backup);
        }
    }
}
