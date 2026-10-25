using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Globalization;
namespace DynamicGamedayAtmosphere {
    public static class SettingsMigration {
        static string Section(string line){string s=line.Trim();return s.StartsWith("[")&&s.EndsWith("]")?s.Substring(1,s.Length-2).Trim():null;}
        static string Key(string line){string s=line.Trim();int eq=s.IndexOf('=');return s.StartsWith(";")||s.StartsWith("#")||eq<1?null:s.Substring(0,eq).Trim();}
        static int Number(string text,string wantedSection,string wantedKey,int fallback){
            string section="";foreach(string line in text.Replace("\r","").Split('\n')){
                string next=Section(line);if(next!=null){section=next;continue;}
                if(String.Equals(section,wantedSection,StringComparison.OrdinalIgnoreCase)&&String.Equals(Key(line),wantedKey,StringComparison.OrdinalIgnoreCase)){
                    int value;string raw=line.Substring(line.IndexOf('=')+1).Split(';','#')[0].Trim();
                    return Int32.TryParse(raw,out value)?value:fallback;
                }
            }return fallback;
        }
        static double RealNumber(string text,string wantedSection,string wantedKey,double fallback){
            string section="";foreach(string line in text.Replace("\r","").Split('\n')){
                string next=Section(line);if(next!=null){section=next;continue;}
                if(String.Equals(section,wantedSection,StringComparison.OrdinalIgnoreCase)&&String.Equals(Key(line),wantedKey,StringComparison.OrdinalIgnoreCase)){
                    double value;string raw=line.Substring(line.IndexOf('=')+1).Split(';','#')[0].Trim();
                    return Double.TryParse(raw,NumberStyles.Float,CultureInfo.InvariantCulture,out value)&&!Double.IsNaN(value)&&!Double.IsInfinity(value)?value:fallback;
                }
            }return fallback;
        }
        static string RetuneOldDefaults(string existing,string defaults){
            int version=Number(defaults,"General","AudioBalanceVersion",0);
            if(version<2||Number(existing,"General","AudioBalanceVersion",0)>=2||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=2";continue;}
                if(!String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase))continue;
                int old=String.Equals(key,"NormalBoostDb",StringComparison.OrdinalIgnoreCase)?3:
                    String.Equals(key,"PeakBoostDb",StringComparison.OrdinalIgnoreCase)||String.Equals(key,"MaximumBoostDb",StringComparison.OrdinalIgnoreCase)?6:-1;
                int value;if(old<0||!Int32.TryParse(lines[i].Substring(lines[i].IndexOf('=')+1).Split(';','#')[0].Trim(),out value)||value!=old)continue;
                lines[i]=key+"="+Number(defaults,"Energy",key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneGamedayDefaults(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<3||Number(existing,"General","AudioBalanceVersion",0)>=3||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=3";continue;}
                int old=String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"MaximumBoostDb",StringComparison.OrdinalIgnoreCase)?12:
                    String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"StartPercent",StringComparison.OrdinalIgnoreCase)?85:-1;
                if(old>=0&&Number(existing,section,key,-1)==old)lines[i]=key+"="+Number(defaults,section,key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneHalftimeDefault(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<4||Number(existing,"General","AudioBalanceVersion",0)>=4||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]=key+"=4";
                if(String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"HalftimeSeatBreakPercent",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==3)
                    lines[i]=key+"="+Number(defaults,section,key,10);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneReleaseDefaults(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<5||Number(existing,"General","AudioBalanceVersion",0)>=5||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=5";continue;}
                // Keep inert legacy keys during an update: an already-running
                // older DLL must not fall back to its former loud defaults.
                if(String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"CommentaryFollowPercent",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=0";continue;}
                if(String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"BoostAmplitudePercent",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=100";continue;}
                int old=String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"MaximumBoostDb",StringComparison.OrdinalIgnoreCase)?15:
                    String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"StartPercent",StringComparison.OrdinalIgnoreCase)?70:
                    String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"LateSchoolStartPercent",StringComparison.OrdinalIgnoreCase)?60:-1;
                if(old>=0&&Number(existing,section,key,-1)==old)lines[i]=key+"="+Number(defaults,section,key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneEnergyDefaults(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<6||Number(existing,"General","AudioBalanceVersion",0)>=6||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]=key+"=6";
                if(String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AllCrowdBaselineDb",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==2)
                    lines[i]=key+"="+Number(defaults,section,key,3);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneAtmosphereDefaults(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<7||Number(existing,"General","AudioBalanceVersion",0)>=7||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=7";continue;}
                bool energy=String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase);
                if(energy&&String.Equals(key,"AllCrowdBaselineDb",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=0";continue;}
                if(energy&&String.Equals(key,"MaximumBoostDb",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,0)>8){lines[i]=key+"=8";continue;}
                int old=String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"Strength",StringComparison.OrdinalIgnoreCase)?35:-1;
                if(energy){
                    if(String.Equals(key,"NormalBoostDb",StringComparison.OrdinalIgnoreCase))old=5;
                    if(String.Equals(key,"BigGameBoostDb",StringComparison.OrdinalIgnoreCase))old=6;
                    if(String.Equals(key,"RankedRivalryBoostDb",StringComparison.OrdinalIgnoreCase))old=8;
                    if(String.Equals(key,"PlayoffBoostDb",StringComparison.OrdinalIgnoreCase)||String.Equals(key,"OvertimeBoostDb",StringComparison.OrdinalIgnoreCase))old=9;
                }
                if(old>=0&&Number(existing,section,key,-1)==old)lines[i]=key+"="+Number(defaults,section,key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneOpeningDefaults(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<8||Number(existing,"General","AudioBalanceVersion",0)>=8||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=8";continue;}
                if(!String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase))continue;
                if(String.Equals(key,"MaximumBoostDb",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,0)>6){lines[i]=key+"=6";continue;}
                if(String.Equals(key,"BigGameAmbienceExtraDb",StringComparison.OrdinalIgnoreCase)){lines[i]=key+"=0";continue;}
                int old=String.Equals(key,"NormalBoostDb",StringComparison.OrdinalIgnoreCase)?4:
                    String.Equals(key,"PeakBoostDb",StringComparison.OrdinalIgnoreCase)?8:
                    String.Equals(key,"BigGameBoostDb",StringComparison.OrdinalIgnoreCase)?3:
                    String.Equals(key,"RankedRivalryBoostDb",StringComparison.OrdinalIgnoreCase)?4:
                    String.Equals(key,"PlayoffBoostDb",StringComparison.OrdinalIgnoreCase)?5:
                    String.Equals(key,"ChampionshipBoostDb",StringComparison.OrdinalIgnoreCase)||String.Equals(key,"OvertimeBoostDb",StringComparison.OrdinalIgnoreCase)?6:-1;
                if(old>=0&&Number(existing,section,key,-1)==old)lines[i]=key+"="+Number(defaults,section,key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneCoreAudio(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<9||Number(existing,"General","AudioBalanceVersion",0)>=9||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            int enabled=Number(existing,"Energy","Enabled",1);
            int opening=Number(existing,"Energy","SustainedRoars",1)==0?0:Number(existing,"Energy","OpeningAtmosphere",1);
            int eruptions=Number(existing,"Energy","PlaySurges",1);
            int normal=Math.Max(0,Math.Min(4,Number(existing,"Energy","MaximumBoostDb",4)));
            int decisive=Math.Max(0,Math.Min(6,Number(existing,"Energy","DecisiveMaximumBoostDb",6)));
            var lines=new List<string>();string section="";bool core=false;
            foreach(string line in existing.Replace("\r","").Split('\n')){
                string next=Section(line);if(next!=null){section=next;if(String.Equals(next,"Atmosphere",StringComparison.OrdinalIgnoreCase))core=true;}
                if(String.Equals(section,"Energy",StringComparison.OrdinalIgnoreCase))continue;
                string key=Key(line);
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines.Add("AudioBalanceVersion=9");continue;}
                if(String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)&&
                   (String.Equals(key,"CloseGameBoostPercent",StringComparison.OrdinalIgnoreCase)||String.Equals(key,"MatchupBoostPercent",StringComparison.OrdinalIgnoreCase)||
                    String.Equals(key,"CommentaryFollowPercent",StringComparison.OrdinalIgnoreCase)||String.Equals(key,"ChantHalfCrowdAdjustment",StringComparison.OrdinalIgnoreCase)))continue;
                lines.Add(line);
            }
            if(!core){lines.Add("[Atmosphere]");lines.Add("Enabled="+enabled);lines.Add("OpeningRoar="+opening);lines.Add("WinningEruptions="+eruptions);lines.Add("MaximumBoostDb="+normal);lines.Add("EruptionBoostDb="+decisive);}
            // Disable a previously loaded runtime while it waits for the required restart.
            // Only upgraded files need this compatibility key; fresh settings omit it.
            lines.Add("[Energy]");lines.Add("; Retired audio system. Kept off for an older DLL until the game restarts.");lines.Add("Enabled=0");
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneReductionStrength(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<10||Number(existing,"General","AudioBalanceVersion",0)>=10||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]="AudioBalanceVersion=10";
                if(String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"Strength",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==20)lines[i]="Strength=35";
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneGentleAudio(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<11||Number(existing,"General","AudioBalanceVersion",0)>=11||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]="AudioBalanceVersion=11";
                if(String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"Strength",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==35)lines[i]="Strength=15";
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetunePercentageAudio(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<12||Number(existing,"General","AudioBalanceVersion",0)>=12||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>();string section="";
            bool explicitPercent=Number(existing,"Audio","MaximumReductionPercent",-1)>=0;
            int legacy=Math.Max(0,Math.Min(100,Number(existing,"Audio","Strength",15)));
            int percent=Math.Max(0,Math.Min(100,(int)Math.Round(legacy*20.0/15.0,MidpointRounding.AwayFromZero)));
            foreach(string line in existing.Replace("\r","").Split('\n')){
                string next=Section(line);if(next!=null){section=next;lines.Add(line);continue;}
                string key=Key(line);
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines.Add("AudioBalanceVersion=12");continue;}
                if(String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)){
                    if(String.Equals(key,"Strength",StringComparison.OrdinalIgnoreCase)){if(!explicitPercent){lines.Add("MaximumReductionPercent="+percent);explicitPercent=true;}continue;}
                    string trimmed=line.TrimStart();
                    if(trimmed.StartsWith("; Attendance-based quieting",StringComparison.OrdinalIgnoreCase))continue;
                }
                lines.Add(line);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetunePercentageMix(string existing,string defaults){
            if(Number(defaults,"General","AudioBalanceVersion",0)<13||Number(existing,"General","AudioBalanceVersion",0)>=13||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            bool normal=RealNumber(existing,"Atmosphere","MaximumBoostPercent",-1)>=0;
            bool eruption=RealNumber(existing,"Atmosphere","EruptionBoostPercent",-1)>=0;
            bool chant=RealNumber(existing,"Audio","ChantReductionPercent",-1)>=0;
            var lines=new List<string>();string section="";
            foreach(string line in existing.Replace("\r","").Split('\n')){
                string next=Section(line);if(next!=null){section=next;lines.Add(line);continue;}
                string key=Key(line);
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AudioBalanceVersion",StringComparison.OrdinalIgnoreCase)){lines.Add("AudioBalanceVersion=13");continue;}
                if(String.Equals(section,"Atmosphere",StringComparison.OrdinalIgnoreCase)){
                    if(String.Equals(key,"MaximumBoostDb",StringComparison.OrdinalIgnoreCase)){
                        if(!normal){double db=Math.Max(0,Math.Min(4,RealNumber(existing,section,key,4)));lines.Add("MaximumBoostPercent="+(100*(Math.Pow(10,db/20)-1)).ToString("0.##",CultureInfo.InvariantCulture));normal=true;}continue;
                    }
                    if(String.Equals(key,"EruptionBoostDb",StringComparison.OrdinalIgnoreCase)){
                        if(!eruption){double db=Math.Max(0,Math.Min(6,RealNumber(existing,section,key,6)));lines.Add("EruptionBoostPercent="+(100*(Math.Pow(10,db/20)-1)).ToString("0.##",CultureInfo.InvariantCulture));eruption=true;}continue;
                    }
                }
                if(String.Equals(section,"Audio",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"ChantReductionDb",StringComparison.OrdinalIgnoreCase)){
                    if(!chant){double db=Math.Max(0,Math.Min(24,RealNumber(existing,section,key,6)));double cut=db==6?50:100*(1-Math.Pow(10,-db/20));lines.Add("ChantReductionPercent="+cut.ToString("0.##",CultureInfo.InvariantCulture));chant=true;}continue;
                }
                lines.Add(line);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneAttendanceDefaults(string existing,string defaults){
            if(Number(defaults,"General","AttendanceBalanceVersion",0)<1||Number(existing,"General","AttendanceBalanceVersion",0)>=1||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AttendanceBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]=key+"=1";
                if(!String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase))continue;
                int old=String.Equals(key,"StartPercent",StringComparison.OrdinalIgnoreCase)?60:
                    String.Equals(key,"FinishPercentOfQuarter",StringComparison.OrdinalIgnoreCase)?50:
                    String.Equals(key,"LateSchoolFinishPercentOfQuarter",StringComparison.OrdinalIgnoreCase)?85:
                    String.Equals(key,"BigGameBonus",StringComparison.OrdinalIgnoreCase)?10:-1;
                if(old>=0&&Number(existing,section,key,-1)==old)lines[i]=key+"="+Number(defaults,section,key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneArrivalBaselines(string existing,string defaults){
            if(Number(defaults,"General","AttendanceBalanceVersion",0)<2||Number(existing,"General","AttendanceBalanceVersion",0)>=2||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AttendanceBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]=key+"=2";
                if(!String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase))continue;
                int old=String.Equals(key,"StartPercent",StringComparison.OrdinalIgnoreCase)||String.Equals(key,"PlayoffStartPercent",StringComparison.OrdinalIgnoreCase)?70:
                    String.Equals(key,"LateSchoolStartPercent",StringComparison.OrdinalIgnoreCase)?50:
                    String.Equals(key,"ModerateStartPercent",StringComparison.OrdinalIgnoreCase)?60:
                    String.Equals(key,"ChampionshipStartPercent",StringComparison.OrdinalIgnoreCase)?80:-1;
                if(old>=0&&Number(existing,section,key,-1)==old)lines[i]=key+"="+Number(defaults,section,key,old);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneQuarterCap(string existing,string defaults){
            if(Number(defaults,"General","AttendanceBalanceVersion",0)<3||Number(existing,"General","AttendanceBalanceVersion",0)>=3||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AttendanceBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]=key+"=3";
                if(String.Equals(section,"Retention",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"MaximumDeparturePerQuarter",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==30)
                    lines[i]=key+"="+Number(defaults,section,key,45);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneAttendanceBalance4(string existing,string defaults){
            if(Number(defaults,"General","AttendanceBalanceVersion",0)<4||Number(existing,"General","AttendanceBalanceVersion",0)>=4||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>();string section="";
            foreach(string raw in existing.Replace("\r","").Split('\n')){
                string line=raw;string next=Section(line);if(next!=null)section=next;
                string key=Key(line);
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"AttendanceBalanceVersion",StringComparison.OrdinalIgnoreCase))line=key+"=4";
                if(String.Equals(section,"Arrivals",StringComparison.OrdinalIgnoreCase)){
                    int old=String.Equals(key,"StartPercent",StringComparison.OrdinalIgnoreCase)?65:
                        String.Equals(key,"ModerateStartPercent",StringComparison.OrdinalIgnoreCase)?55:
                        String.Equals(key,"LateSchoolStartPercent",StringComparison.OrdinalIgnoreCase)?45:-1;
                    if(old>=0&&Number(existing,section,key,-1)==old)line=key+"="+Number(defaults,section,key,old);
                    if(String.Equals(key,"HalftimeSeatBreakPercent",StringComparison.OrdinalIgnoreCase)){
                        if(Number(existing,section,"HalftimeSeatBreakStrength",-1)>=0)continue;
                        line="HalftimeSeatBreakStrength="+Math.Max(0,Math.Min(200,Number(existing,section,key,10)*10));
                    }
                }
                if(String.Equals(section,"Retention",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"ComebackOriginalDeficit",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==28)line=key+"=21";
                lines.Add(line);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string RetuneReplayDefaults(string existing,string defaults){
            if(Number(defaults,"General","ReplayBalanceVersion",0)<1||Number(existing,"General","ReplayBalanceVersion",0)>=1||
               Number(existing,"General","ConfigVersion",1)>Number(defaults,"General","ConfigVersion",1))return existing;
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            for(int i=0;i<lines.Count;i++){
                string next=Section(lines[i]);if(next!=null){section=next;continue;}
                string key=Key(lines[i]);if(key==null)continue;
                if(String.Equals(section,"General",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"ReplayBalanceVersion",StringComparison.OrdinalIgnoreCase))lines[i]=key+"=1";
                if(String.Equals(section,"Replays",StringComparison.OrdinalIgnoreCase)&&String.Equals(key,"FullScreenMinimumYards",StringComparison.OrdinalIgnoreCase)&&Number(existing,section,key,-1)==20)
                    lines[i]=key+"="+Number(defaults,section,key,15);
            }
            return String.Join(Environment.NewLine,lines);
        }
        static string Merge(string existing,string defaults){
            var lines=new List<string>(existing.Replace("\r","").Split('\n'));string section="";
            foreach(string raw in defaults.Replace("\r","").Split('\n')){
                string next=Section(raw);if(next!=null){section=next;continue;}
                string key=Key(raw);if(key==null)continue;
                string current="";int start=-1,end=lines.Count;bool found=false;
                for(int i=0;i<lines.Count;i++){
                    string name=Section(lines[i]);
                    if(name!=null){
                        if(start>=0&&!String.Equals(name,section,StringComparison.OrdinalIgnoreCase)){end=i;break;}
                        current=name;if(String.Equals(name,section,StringComparison.OrdinalIgnoreCase))start=i;
                    }
                    if(String.Equals(current,section,StringComparison.OrdinalIgnoreCase)&&String.Equals(Key(lines[i]),key,StringComparison.OrdinalIgnoreCase)){found=true;break;}
                }
                if(found)continue;
                if(start<0){lines.Add("");lines.Add("["+section+"]");lines.Add(raw);}
                else lines.Insert(end,raw);
            }
            return String.Join(Environment.NewLine,lines).TrimEnd()+Environment.NewLine;
        }
        public static void Ensure(string destination,string legacy,string defaults){
            string existing=File.Exists(destination)?File.ReadAllText(destination):File.Exists(legacy)?File.ReadAllText(legacy):defaults;
            string upgraded=RetuneOldDefaults(existing,defaults);
            upgraded=RetuneGamedayDefaults(upgraded,defaults);
            upgraded=RetuneHalftimeDefault(upgraded,defaults);
            upgraded=RetuneReleaseDefaults(upgraded,defaults);
            upgraded=RetuneEnergyDefaults(upgraded,defaults);
            upgraded=RetuneAtmosphereDefaults(upgraded,defaults);
            upgraded=RetuneOpeningDefaults(upgraded,defaults);
            upgraded=RetuneCoreAudio(upgraded,defaults);
            upgraded=RetuneReductionStrength(upgraded,defaults);
            upgraded=RetuneGentleAudio(upgraded,defaults);
            upgraded=RetunePercentageAudio(upgraded,defaults);
            upgraded=RetunePercentageMix(upgraded,defaults);
            upgraded=RetuneAttendanceDefaults(upgraded,defaults);
            upgraded=RetuneArrivalBaselines(upgraded,defaults);
            upgraded=RetuneQuarterCap(upgraded,defaults);
            upgraded=RetuneAttendanceBalance4(upgraded,defaults);
            upgraded=RetuneReplayDefaults(upgraded,defaults);
            string result=Merge(upgraded,defaults);
            if(File.Exists(destination)&&File.ReadAllText(destination)==result)return;
            // Win32 profile APIs reliably support UTF-16 BOM settings files.
            string temp=destination+".tmp";File.WriteAllText(temp,result,Encoding.Unicode);
            if(File.Exists(destination))File.Replace(temp,destination,destination+".previous",true);
            else File.Move(temp,destination);
        }
    }
}
