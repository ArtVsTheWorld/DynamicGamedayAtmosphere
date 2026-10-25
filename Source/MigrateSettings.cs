using System;using System.IO;
class MigrateSettings {
    static int Main(string[] args){try{
        DynamicGamedayAtmosphere.SettingsMigration.Ensure(args[0],args[1],File.ReadAllText(args[2]));return 0;
    }catch(Exception e){Console.Error.WriteLine(e.Message);return 1;}}
}
