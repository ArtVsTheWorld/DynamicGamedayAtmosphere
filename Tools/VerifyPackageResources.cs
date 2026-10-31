using System;
using System.IO;
using System.IO.Compression;
using System.Reflection;
using System.Security.Cryptography;
class VerifyPackageResources {
 static int Main(string[] args){
  try{
   using(var zip=ZipFile.OpenRead(args[0])){
    var dll=zip.GetEntry("DynamicGamedayAtmosphere.dll");if(dll==null)throw new Exception("Plugin DLL missing");
    Assembly assembly;using(var stream=dll.Open())using(var buffer=new MemoryStream()){stream.CopyTo(buffer);assembly=Assembly.ReflectionOnlyLoad(buffer.ToArray());}
    string[] resources=assembly.GetManifestResourceNames();
    if(Array.IndexOf(resources,"Attendance.LoadDynamicGamedayAtmosphere.exe")>=0)throw new Exception("Release still embeds the loader EXE");
    if(Array.IndexOf(resources,"Attendance.DynamicGamedayAtmosphereNative-"+args[1]+".dll")<0)throw new Exception("Native runtime and loader versions differ");
    var identity=assembly.GetType("DynamicGamedayAtmosphere.ExternalLoaderInfo",true);
    string expected=(string)identity.GetField("Sha256",BindingFlags.Static|BindingFlags.NonPublic).GetRawConstantValue();
    var loader=zip.GetEntry("DynamicGamedayAtmosphere/Runtime-"+args[1]+"/LoadDynamicGamedayAtmosphere.exe");if(loader==null)throw new Exception("Separate runtime loader missing");
    using(var stream=loader.Open())using(var hash=SHA256.Create())if(!String.Equals(BitConverter.ToString(hash.ComputeHash(stream)).Replace("-",""),expected,StringComparison.OrdinalIgnoreCase))throw new Exception("Separate runtime loader hash differs from the plugin expectation");
   }
   Console.WriteLine("PASS: reflection-only resource inspection, matching runtime version and separate-loader SHA256");return 0;
  }catch(Exception error){Console.Error.WriteLine(error.Message);return 1;}
 }
}
