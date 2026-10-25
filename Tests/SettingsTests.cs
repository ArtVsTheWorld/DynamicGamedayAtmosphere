using System;using System.IO;using System.Runtime.InteropServices;using System.Globalization;
using DynamicGamedayAtmosphere;
class SettingsTests {
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode)]static extern uint GetPrivateProfileInt(string section,string key,int fallback,string path);
 static void Check(bool okay,[System.Runtime.CompilerServices.CallerLineNumber]int line=0){if(!okay)throw new Exception("Settings migration regression at line "+line);}
 static double Level(string wantedSection,string wantedKey,string file){
  string section="";foreach(string line in File.ReadAllLines(file)){
   string text=line.Trim();if(text.StartsWith("[")&&text.EndsWith("]")){section=text.Substring(1,text.Length-2);continue;}
   int equal=text.IndexOf('=');if(section!=wantedSection||equal<1||text.Substring(0,equal)!=wantedKey)continue;
   return Double.Parse(text.Substring(equal+1).Split(';','#')[0].Trim(),CultureInfo.InvariantCulture);
  }throw new Exception("Missing percentage setting "+wantedKey);
 }
 static int Main(){
  string dir=Path.Combine(Path.GetTempPath(),"GamedaySettingsTest-"+Guid.NewGuid().ToString("N"));Directory.CreateDirectory(dir);
  string old=Path.Combine(dir,"old.ini"),next=Path.Combine(dir,"new.ini");
  try{
   string defaults=File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.BaseDirectory,"..","..","Source","DynamicGamedayAtmosphere.ini"));
   SettingsMigration.Ensure(next,old,defaults);string fresh=File.ReadAllText(next);
   Check(Math.Abs(Level("Atmosphere","MaximumBoostPercent",next)-58.49)<.001&&Math.Abs(Level("Atmosphere","EruptionBoostPercent",next)-99.53)<.001);
   Check(GetPrivateProfileInt("General","AudioBalanceVersion",0,next)==13&&!fresh.Contains("[Energy]")&&!fresh.Contains("Momentum="));
   Check(GetPrivateProfileInt("Arrivals","StartPercent",0,next)==65&&GetPrivateProfileInt("Arrivals","LateSchoolStartPercent",0,next)==45);
   Check(GetPrivateProfileInt("Audio","MaximumReductionPercent",0,next)==20);
   Check(GetPrivateProfileInt("Audio","ReductionStartPercent",0,next)==80&&!fresh.Contains("\nStrength="));
   Check(Level("Audio","ChantReductionPercent",next)==50&&!fresh.Contains("BoostDb=")&&!fresh.Contains("ChantReductionDb="));
   Check(GetPrivateProfileInt("Arrivals","FcsOpponentPenalty",0,next)==10&&GetPrivateProfileInt("Retention","FormulaStrength",0,next)==100);
   Check(GetPrivateProfileInt("Arrivals","ModerateStartPercent",0,next)==55&&GetPrivateProfileInt("Arrivals","FinishPercentOfQuarter",0,next)==25&&GetPrivateProfileInt("Arrivals","LateSchoolFinishPercentOfQuarter",0,next)==75);
   Check(GetPrivateProfileInt("General","AttendanceBalanceVersion",0,next)==3);
   Check(GetPrivateProfileInt("Retention","MaximumDeparturePerQuarter",0,next)==45);
   Check(GetPrivateProfileInt("Replays","Enabled",0,next)==1&&GetPrivateProfileInt("Replays","FullScreenMinimumYards",0,next)==15);
   Check(GetPrivateProfileInt("Replays","InsetAfterRoutinePlays",0,next)==1&&GetPrivateProfileInt("Replays","FullScreenBigPlays",0,next)==1);
   Check(GetPrivateProfileInt("Replays","DefensiveHighlights",0,next)==1&&GetPrivateProfileInt("General","ReplayBalanceVersion",0,next)==1);
   SettingsMigration.Ensure(next,old,defaults);Check(File.ReadAllText(next)==fresh);
   foreach(int version in new[]{0,3,5,6,7,8}){
    File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion="+version+"\n[Energy]\nEnabled=1\nSustainedRoars=1\nPlaySurges=1\nMaximumBoostDb=12\nDecisiveMaximumBoostDb=8\nMomentum=1\n[Audio]\nStrength=17\nCommentaryFollowPercent=40\nCloseGameBoostPercent=15\n[Arrivals]\nStartPercent=93\nLateSchoolStartPercent=43\n[Retention]\nQ4GradualThinning=0\nQ4Gradual28Points=3\n[Custom]\nUnknown=keep\n");
    SettingsMigration.Ensure(next,old,defaults);string migrated=File.ReadAllText(next);
    Check(Math.Abs(Level("Atmosphere","MaximumBoostPercent",next)-58.49)<.001&&Math.Abs(Level("Atmosphere","EruptionBoostPercent",next)-99.53)<.001);
    Check(GetPrivateProfileInt("Atmosphere","OpeningRoar",0,next)==1&&GetPrivateProfileInt("Atmosphere","WinningEruptions",0,next)==1);
    Check(GetPrivateProfileInt("Energy","Enabled",99,next)==0&&!migrated.Contains("Momentum=")&&!migrated.Contains("CommentaryFollowPercent="));
    Check(GetPrivateProfileInt("Arrivals","StartPercent",0,next)==93&&GetPrivateProfileInt("Arrivals","LateSchoolStartPercent",0,next)==43);
    Check(GetPrivateProfileInt("Retention","Q4GradualThinning",99,next)==0&&GetPrivateProfileInt("Retention","Q4Gradual28Points",0,next)==3&&migrated.Contains("Unknown=keep"));
    Check(GetPrivateProfileInt("Audio","MaximumReductionPercent",0,next)==23);
    SettingsMigration.Ensure(next,old,defaults);Check(File.ReadAllText(next)==migrated);
   }
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=8\n[Energy]\nEnabled=0\nSustainedRoars=0\nPlaySurges=0\nMaximumBoostDb=2\nDecisiveMaximumBoostDb=3\n");
   SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Atmosphere","Enabled",1,next)==0&&GetPrivateProfileInt("Atmosphere","OpeningRoar",1,next)==0&&GetPrivateProfileInt("Atmosphere","WinningEruptions",1,next)==0);
   Check(Math.Abs(Level("Atmosphere","MaximumBoostPercent",next)-25.89)<.001&&Math.Abs(Level("Atmosphere","EruptionBoostPercent",next)-41.25)<.001);
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=9\n[Atmosphere]\nOpeningRoar=0\nMaximumBoostDb=1\nEruptionBoostDb=2\n[Audio]\nStrength=8\n");
   SettingsMigration.Ensure(next,old,defaults);string custom=File.ReadAllText(next);
   Check(GetPrivateProfileInt("Atmosphere","OpeningRoar",1,next)==0&&Math.Abs(Level("Atmosphere","MaximumBoostPercent",next)-12.2)<.001&&GetPrivateProfileInt("Audio","MaximumReductionPercent",99,next)==11);
   SettingsMigration.Ensure(next,old,defaults);Check(File.ReadAllText(next)==custom);
   File.WriteAllText(next,"[General]\nConfigVersion=99\nAudioBalanceVersion=99\n[Energy]\nMomentum=1\n[Atmosphere]\nMaximumBoostDb=12\n[Arrivals]\nStartPercent=70\n");
   SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("General","ConfigVersion",0,next)==99&&GetPrivateProfileInt("Atmosphere","MaximumBoostDb",0,next)==12&&GetPrivateProfileInt("Energy","Momentum",0,next)==1&&GetPrivateProfileInt("Arrivals","StartPercent",0,next)==70);
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=9\n[Audio]\nStrength=20\n");
   SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Audio","MaximumReductionPercent",0,next)==20&&GetPrivateProfileInt("General","AudioBalanceVersion",0,next)==13);
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=10\n[Arrivals]\nStartPercent=60\nFinishPercentOfQuarter=50\nLateSchoolFinishPercentOfQuarter=85\nBigGameBonus=10\n[Audio]\nStrength=35\n");
   SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Arrivals","StartPercent",0,next)==65&&GetPrivateProfileInt("Arrivals","FinishPercentOfQuarter",0,next)==25&&GetPrivateProfileInt("Arrivals","LateSchoolFinishPercentOfQuarter",0,next)==75&&GetPrivateProfileInt("Arrivals","BigGameBonus",0,next)==20);
   string tuned=File.ReadAllText(next);SettingsMigration.Ensure(next,old,defaults);Check(tuned==File.ReadAllText(next));
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAttendanceBalanceVersion=1\nAudioBalanceVersion=10\n[Arrivals]\nStartPercent=70\nLateSchoolStartPercent=50\nModerateStartPercent=60\nPlayoffStartPercent=70\nChampionshipStartPercent=80\n[Audio]\nStrength=35\n");
   SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Arrivals","StartPercent",0,next)==65&&GetPrivateProfileInt("Arrivals","LateSchoolStartPercent",0,next)==45&&GetPrivateProfileInt("Arrivals","ModerateStartPercent",0,next)==55);
   Check(GetPrivateProfileInt("Arrivals","PlayoffStartPercent",0,next)==65&&GetPrivateProfileInt("Arrivals","ChampionshipStartPercent",0,next)==75&&GetPrivateProfileInt("Audio","MaximumReductionPercent",0,next)==20);
   Check(GetPrivateProfileInt("General","AttendanceBalanceVersion",0,next)==3&&GetPrivateProfileInt("General","AudioBalanceVersion",0,next)==13);
   foreach(int cap in new[]{30,22}){
    File.WriteAllText(next,"[General]\nConfigVersion=1\nAttendanceBalanceVersion=2\nAudioBalanceVersion=11\n[Retention]\nMaximumDeparturePerQuarter="+cap+"\n");
    SettingsMigration.Ensure(next,old,defaults);
    Check(GetPrivateProfileInt("Retention","MaximumDeparturePerQuarter",0,next)==(cap==30?45:cap)&&GetPrivateProfileInt("General","AttendanceBalanceVersion",0,next)==3);
    string migratedCap=File.ReadAllText(next);SettingsMigration.Ensure(next,old,defaults);Check(migratedCap==File.ReadAllText(next));
   }
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAttendanceBalanceVersion=99\nAudioBalanceVersion=10\n[Retention]\nMaximumDeparturePerQuarter=30\n[Arrivals]\nStartPercent=60\nFinishPercentOfQuarter=50\n");
   SettingsMigration.Ensure(next,old,defaults);Check(GetPrivateProfileInt("Arrivals","StartPercent",0,next)==60&&GetPrivateProfileInt("General","AttendanceBalanceVersion",0,next)==99&&GetPrivateProfileInt("Retention","MaximumDeparturePerQuarter",0,next)==30);
   foreach(int maximum in new[]{0,12,40}){
    File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=11\n[Audio]\nStrength=35\nMaximumReductionPercent="+maximum+"\nReductionStartPercent=85\n");
    SettingsMigration.Ensure(next,old,defaults);string converted=File.ReadAllText(next);
    Check(GetPrivateProfileInt("Audio","MaximumReductionPercent",99,next)==maximum&&GetPrivateProfileInt("Audio","ReductionStartPercent",0,next)==85&&!converted.Contains("\nStrength="));
    SettingsMigration.Ensure(next,old,defaults);Check(File.ReadAllText(next)==converted);
   }
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=11\n[Audio]\nStrength=0\n");SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Audio","MaximumReductionPercent",99,next)==0);
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=99\n[Audio]\nStrength=35\nMaximumReductionPercent=7\nReductionStartPercent=95\n");SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("General","AudioBalanceVersion",0,next)==99&&GetPrivateProfileInt("Audio","Strength",0,next)==35&&GetPrivateProfileInt("Audio","MaximumReductionPercent",0,next)==7&&GetPrivateProfileInt("Audio","ReductionStartPercent",0,next)==95);
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=12\n[Audio]\nMaximumReductionPercent=12\nChantReductionDb=12\n[Atmosphere]\nMaximumBoostDb=0\nEruptionBoostDb=3\n");SettingsMigration.Ensure(next,old,defaults);
   Check(Level("Atmosphere","MaximumBoostPercent",next)==0&&Math.Abs(Level("Atmosphere","EruptionBoostPercent",next)-41.25)<.001&&Math.Abs(Level("Audio","ChantReductionPercent",next)-74.88)<.001);
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=12\n[Audio]\nChantReductionDb=6\nChantReductionPercent=12.5\n[Atmosphere]\nMaximumBoostDb=4\nMaximumBoostPercent=20.25\nEruptionBoostDb=6\nEruptionBoostPercent=42.5\n");
   var culture=CultureInfo.CurrentCulture;try{System.Threading.Thread.CurrentThread.CurrentCulture=CultureInfo.GetCultureInfo("fr-FR");SettingsMigration.Ensure(next,old,defaults);}finally{System.Threading.Thread.CurrentThread.CurrentCulture=culture;}
   Check(Level("Atmosphere","MaximumBoostPercent",next)==20.25&&Level("Atmosphere","EruptionBoostPercent",next)==42.5&&Level("Audio","ChantReductionPercent",next)==12.5&&!File.ReadAllText(next).Contains("BoostDb="));
   File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=13\n[Replays]\nEnabled=0\nFullScreenMinimumYards=30\n");SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Replays","Enabled",1,next)==0&&GetPrivateProfileInt("Replays","FullScreenMinimumYards",0,next)==30&&GetPrivateProfileInt("Replays","InsetAfterRoutinePlays",0,next)==1);
   string replaySettings=File.ReadAllText(next);SettingsMigration.Ensure(next,old,defaults);Check(File.ReadAllText(next)==replaySettings);
   foreach(int cutoff in new[]{20,25}){
    File.WriteAllText(next,"[General]\nConfigVersion=1\nAudioBalanceVersion=13\n[Replays]\nFullScreenMinimumYards="+cutoff+"\nDefensiveHighlights=0\n");SettingsMigration.Ensure(next,old,defaults);
    Check(GetPrivateProfileInt("Replays","FullScreenMinimumYards",0,next)==(cutoff==20?15:cutoff)&&GetPrivateProfileInt("Replays","DefensiveHighlights",1,next)==0);
    Check(GetPrivateProfileInt("General","ReplayBalanceVersion",0,next)==1);string tunedReplays=File.ReadAllText(next);SettingsMigration.Ensure(next,old,defaults);Check(File.ReadAllText(next)==tunedReplays);
   }
   File.WriteAllText(next,"[General]\nConfigVersion=1\nReplayBalanceVersion=99\n[Replays]\nFullScreenMinimumYards=20\n");SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Replays","FullScreenMinimumYards",0,next)==20&&GetPrivateProfileInt("General","ReplayBalanceVersion",0,next)==99);
   File.Delete(next);File.WriteAllText(old,"[Attendance]\nEnabled=0\nProfile=BadWeather\n[Audio]\nStrength=17\n");SettingsMigration.Ensure(next,old,defaults);
   Check(GetPrivateProfileInt("Attendance","Enabled",1,next)==0&&GetPrivateProfileInt("Audio","MaximumReductionPercent",99,next)==23&&GetPrivateProfileInt("General","AudioBalanceVersion",0,next)==13);
   Console.WriteLine("PASS: clean core settings, legacy upgrades/disable, lower custom limits, attendance preservation, idempotence and future schemas.");return 0;
  }catch(Exception e){Console.Error.WriteLine(e);return 1;}
  finally{foreach(string file in Directory.GetFiles(dir))File.Delete(file);Directory.Delete(dir);}
 }
}
