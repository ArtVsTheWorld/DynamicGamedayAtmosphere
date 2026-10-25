using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Security.Cryptography;
using System.Threading;
using System.Text;
using Frosty.Core;
using Frosty.Core.Attributes;
using FrostySdk.Interfaces;

[assembly: AssemblyTitle("Dynamic Gameday Atmosphere")]
[assembly: AssemblyCompany("Ace")]
[assembly: AssemblyProduct("Dynamic Gameday Atmosphere")]
[assembly: AssemblyDescription("Dynamic attendance, crowd volume, opening roar and winning eruptions")]
[assembly: AssemblyVersion(DynamicGamedayAtmosphere.BuildInfo.AssemblyVersion)]
[assembly: AssemblyFileVersion(DynamicGamedayAtmosphere.BuildInfo.AssemblyVersion)]
[assembly: PluginVersion(DynamicGamedayAtmosphere.BuildInfo.Version)]
[assembly: PluginAuthor("Ace")]
[assembly: PluginDisplayName("Dynamic Gameday Atmosphere")]
[assembly: RegisterExecutionAction(typeof(DynamicGamedayAtmosphere.AttendanceAction))]

namespace DynamicGamedayAtmosphere
{
    public sealed class AttendanceAction : ExecutionAction
    {
        private static int watching;
        private static Mutex watcherMutex;
        private static readonly object noteLock=new object();
        public AttendanceAction()
        {
            // Some MMC launch paths skip execution callbacks. Registration still runs.
            using(var host=Process.GetCurrentProcess())
                if(String.Equals(host.ProcessName,"MMCModManager",StringComparison.OrdinalIgnoreCase))Start(null);
        }
        public static string RuntimeFolder { get { return Path.Combine(Path.GetDirectoryName(typeof(AttendanceAction).Assembly.Location),"DynamicGamedayAtmosphere","Runtime-"+BuildInfo.Version); } }
        public override Action<ILogger,PluginManagerType,CancellationToken> PreLaunchAction { get { return (log,type,token)=>Start(log); } }
        public override Action<ILogger,PluginManagerType,CancellationToken> PostLaunchAction { get { return (log,type,token)=>Start(log); } }
        public static string PluginsFolder { get { return Path.GetDirectoryName(typeof(AttendanceAction).Assembly.Location); } }
        public static string LogsFolder { get { return Path.Combine(PluginsFolder,"DynamicGamedayAtmosphere","Logs"); } }
        private static void Note(ILogger logger,string text)
        {
            Directory.CreateDirectory(LogsFolder);
            lock(noteLock){
                string path=Path.Combine(LogsFolder,"launcher.log");
                byte[] entry=Encoding.UTF8.GetBytes(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")+" | "+text+Environment.NewLine);
                const int limit=2*1024*1024;
                int count=Math.Min(limit,entry.Length);long size=File.Exists(path)?new FileInfo(path).Length:0;
                using(var file=new FileStream(path,size>=limit||size+count>limit?FileMode.Create:FileMode.Append,FileAccess.Write,FileShare.Read))file.Write(entry,0,count);
            }
            if(logger!=null)try{logger.Log("[Gameday] "+text,new object[0]);}catch{}
        }
        private static void Extract(string name,bool preserve)
        {
            string destination=Path.Combine(name.EndsWith(".ini",StringComparison.OrdinalIgnoreCase)?PluginsFolder:RuntimeFolder,name);
            if(preserve&&File.Exists(destination))return;
            byte[] bytes;
            using(var resource=typeof(AttendanceAction).Assembly.GetManifestResourceStream("Attendance."+name))
            using(var buffer=new MemoryStream())
            { if(resource==null)throw new InvalidOperationException("Missing embedded resource: "+name);resource.CopyTo(buffer);bytes=buffer.ToArray(); }
            using(var hash=SHA256.Create())
            {
                byte[] expected=hash.ComputeHash(bytes);
                if(File.Exists(destination))using(var old=File.OpenRead(destination))
                    if(Convert.ToBase64String(hash.ComputeHash(old))==Convert.ToBase64String(expected))return;
                File.WriteAllBytes(destination,bytes);
                using(var saved=File.OpenRead(destination))if(Convert.ToBase64String(hash.ComputeHash(saved))!=Convert.ToBase64String(expected))throw new IOException("Extracted runtime hash differs");
            }
        }
        private static void Start(ILogger logger)
        {
            if(Interlocked.CompareExchange(ref watching,1,0)!=0)return;
            bool first=false;watcherMutex=new Mutex(false,"Local\\Ace.DynamicGamedayAtmosphere.MMC."+Process.GetCurrentProcess().Id,out first);
            if(!first)return;
            new Thread(()=>
            {
                var attempted=new HashSet<string>();
                try
                {
                    Directory.CreateDirectory(RuntimeFolder);
                    Extract(BuildInfo.NativeFile,false);Extract("LoadDynamicGamedayAtmosphere.exe",false);
                    string defaults;using(var input=typeof(AttendanceAction).Assembly.GetManifestResourceStream("Attendance.DynamicGamedayAtmosphere.ini"))
                    using(var reader=new StreamReader(input))defaults=reader.ReadToEnd();
                    SettingsMigration.Ensure(Path.Combine(PluginsFolder,"DynamicGamedayAtmosphere.ini"),Path.Combine(PluginsFolder,"DynamicAttendance.ini"),defaults);
                    Note(logger,"Launch watcher started. Automatic crowd departures are restricted to the verified offline Dynasty game layout.");
                    while(true)
                    {
                        foreach(var game in Process.GetProcessesByName("CollegeFB27"))using(game)
                        {
                            try
                            {
                                if(game.HasExited||game.MainWindowHandle==IntPtr.Zero||(DateTime.UtcNow-game.StartTime.ToUniversalTime()).TotalSeconds<20)continue;
                                string identity=game.Id+":"+game.StartTime.ToUniversalTime().Ticks;
                                if(!attempted.Add(identity))continue;
                                string arguments=game.Id+" \""+Path.Combine(RuntimeFolder,BuildInfo.NativeFile)+"\"";
                                using(var child=Process.Start(new ProcessStartInfo(Path.Combine(RuntimeFolder,"LoadDynamicGamedayAtmosphere.exe"),arguments)
                                {WorkingDirectory=RuntimeFolder,UseShellExecute=false,CreateNoWindow=true,WindowStyle=ProcessWindowStyle.Hidden,RedirectStandardOutput=true,RedirectStandardError=true}))
                                {
                                    if(child==null)throw new IOException("Loader did not start");
                                    string output=child.StandardOutput.ReadToEnd();string errors=child.StandardError.ReadToEnd();child.WaitForExit();
                                    if(child.ExitCode!=0)throw new IOException("Loader exited "+child.ExitCode+": "+errors+output);
                                    Note(logger,"Runtime loaded for PID "+game.Id+". Activation status is in "+Path.Combine(LogsFolder,"DynamicGamedayAtmosphere.log")+".");
                                }
                            }
                            catch(Exception error){Note(logger,"Could not activate: "+error.Message+". Restart the game before retrying.");}
                        }
                        Thread.Sleep(1000);
                    }
                }
                catch(Exception error){try{Note(logger,"Watcher stopped: "+error.Message);}catch{}}
                finally{Interlocked.Exchange(ref watching,0);}
            }){IsBackground=true,Name="Dynamic Gameday Atmosphere launch watcher"}.Start();
        }
    }
}
