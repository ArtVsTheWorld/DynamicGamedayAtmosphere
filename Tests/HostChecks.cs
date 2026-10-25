using System;
using System.IO;
using System.Reflection;
using System.Threading;
class HostChecks {
    static string root;
    static int Main(string[] args){
        try {
            root=Path.GetFullPath(args[0]);
            AppDomain.CurrentDomain.AssemblyResolve+=(sender,request)=>{
                string name=new AssemblyName(request.Name).Name+".dll";
                foreach(string dir in new[]{root,Path.Combine(root,"ThirdParty"),Path.Combine(root,"Plugins")}){
                    string p=Path.Combine(dir,name);if(File.Exists(p))return Assembly.LoadFrom(p);
                }
                return null;
            };
            var core=Assembly.LoadFrom(Path.Combine(root,"FrostyCore.dll"));
            string pluginFolder=args.Length>1?Path.GetFullPath(args[1]):Path.Combine(root,"Plugins");
            var plugin=Assembly.LoadFrom(Path.Combine(pluginFolder,"DynamicGamedayAtmosphere.dll"));
            string release=plugin.GetName().Version.ToString(3);
            var action=plugin.GetType("DynamicGamedayAtmosphere.AttendanceAction",true);
            if(!core.GetType("Frosty.Core.ExecutionAction",true).IsAssignableFrom(action))throw new Exception("Execution ABI mismatch");
            var instance=Activator.CreateInstance(action);
            foreach(string name in new[]{"PreLaunchAction","PostLaunchAction"}){
                var callback=(Delegate)action.GetProperty(name).GetValue(instance,null);
                var p=callback.Method.GetParameters();
                if(p.Length!=3||p[0].ParameterType.FullName!="FrostySdk.Interfaces.ILogger"||p[1].ParameterType!=core.GetType("Frosty.Core.PluginManagerType",true)||p[2].ParameterType!=typeof(CancellationToken))throw new Exception("Launch callback ABI mismatch");
            }
            bool registered=false;
            foreach(var attr in plugin.GetCustomAttributesData())if(attr.AttributeType.Name.StartsWith("RegisterExecutionAction")&&(Type)attr.ConstructorArguments[0].Value==action)registered=true;
            if(!registered)throw new Exception("Missing execution action registration");
            foreach(var item in new[]{new[]{"PluginAuthorAttribute","Ace"},new[]{"PluginDisplayNameAttribute","Dynamic Gameday Atmosphere"}}){
                bool found=false;foreach(var attr in plugin.GetCustomAttributesData())if(attr.AttributeType.Name==item[0]&&Convert.ToString(attr.ConstructorArguments[0].Value)==item[1])found=true;
                if(!found)throw new Exception("Metadata mismatch: "+item[0]);
            }
            var extract=action.GetMethod("Extract",BindingFlags.Static|BindingFlags.NonPublic);
            Directory.CreateDirectory((string)action.GetProperty("RuntimeFolder").GetValue(null,null));
            foreach(string name in new[]{"DynamicGamedayAtmosphereNative-"+release+".dll","LoadDynamicGamedayAtmosphere.exe","DynamicGamedayAtmosphere.ini"}){
                if(plugin.GetManifestResourceStream("Attendance."+name)==null)throw new Exception("Missing resource "+name);
                extract.Invoke(null,new object[]{name,name.EndsWith(".ini")});
                if(!File.Exists(name.EndsWith(".ini")?Path.Combine(pluginFolder,name):Path.Combine(pluginFolder,"DynamicGamedayAtmosphere","Runtime-"+release,name)))throw new Exception("Missing extracted file "+name);
            }
            action.GetMethod("Note",BindingFlags.Static|BindingFlags.NonPublic).Invoke(null,new object[]{null,"Package check passed"});
            if(!File.Exists(Path.Combine(pluginFolder,"DynamicGamedayAtmosphere","Logs","launcher.log")))throw new Exception("Launcher log is outside the plugin Logs folder");
            string log=Path.Combine(pluginFolder,"DynamicGamedayAtmosphere","Logs","launcher.log");
            File.WriteAllBytes(log,new byte[2*1024*1024]);
            action.GetMethod("Note",BindingFlags.Static|BindingFlags.NonPublic).Invoke(null,new object[]{null,"After size reset"});
            if(new FileInfo(log).Length>1024||!File.ReadAllText(log).Contains("After size reset")||File.Exists(log+".1"))throw new Exception("Launcher log cap failed");
            Console.WriteLine("PASS: MMC ABI, metadata, resources, extraction, nested launcher log and size reset");return 0;
        }catch(Exception e){Console.Error.WriteLine(e);return 1;}
    }
}
