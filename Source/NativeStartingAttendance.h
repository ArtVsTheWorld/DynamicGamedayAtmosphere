#pragma once
#include <array>
#include "StartingAttendance.h"
#include "SwellAudioRuntime.h"

namespace attendance::turnout::native {
using A=sustained::Address;
inline constexpr A Formula=0x8566710,LaunchCopy=0x7f13490,StatManagerVtable=0xbdc16f8;
// Reset publication is unconditional; final rivalry publication is optional.
// The shared launchhook boundary runs after both paths join.
inline constexpr A LaunchResetReturn=0x7f13859;
inline constexpr A PlayableFormulaReturn=0x871a5c4;
inline constexpr A OptionService=0xe2dd3e8,OptionVtable=0xb298a70,OptionType=0xe3a48d0,OptionSet=0x909b940,UnsignedType=0xe2dab38;
inline constexpr std::array<unsigned char,17> FormulaPrefix={0x48,0x89,0x4c,0x24,0x08,0x55,0x53,0x56,0x41,0x56,0x41,0x57,0x48,0x8d,0x6c,0x24,0xb0};
inline constexpr std::array<unsigned char,16> LaunchPrefix={0x48,0x89,0x5c,0x24,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
inline constexpr std::array<unsigned char,15> SetterPrefix={0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x30};
struct Api {
    using Count=A(__fastcall*)(A);
    using Same=bool(__fastcall*)(A,A*,A*);
    using Set=bool(__fastcall*)(A,unsigned,A);
    Count count;Same same;Set set;
    static Api engine(A image){return {reinterpret_cast<Count>(image+0x37f18a0),reinterpret_cast<Same>(image+0x85ffd60),reinterpret_cast<Set>(image+OptionSet)};}
};
struct Reader {
    A image;
    bool read(A p,void* out,size_t n)const{SIZE_T copied=0;return p>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&copied)&&copied==n;}
    template<class T>bool value(A p,T& out)const{return read(p,&out,sizeof(out));}
    A method(A object,unsigned slot)const{
        A vt=0,fn=0;if(!value(object,vt)||vt<image+0xac00000||vt>=image+0xdc00000||!value(vt+slot,fn)||fn<image+0x1000||fn>=image+0xac00000)return 0;return fn;
    }
    void release(A object)const{A fn=method(object,8);if(fn)sustained::invoke(reinterpret_cast<void(__fastcall*)(A)>(fn),object);}
    bool field(A record,int key,A& table,int& row,A& column)const{
        A fn=method(record,0x38),schema=0,list=0;short count=0;
        if(!fn||!sustained::obtain(schema,reinterpret_cast<A(__fastcall*)(A)>(fn),record)||
           !value(schema+0xd0,list)||!value(schema+0xd8,count)||key<0||key>=count||count>4096||
           !value(list+8*key,column)||column==A(-1)||column>4096||!value(record+0x10,table)||!value(record+0x18,row)||row<0||row>1000000)return false;
        return true;
    }
    bool integer(A record,int key,int& out,bool boolean=false,bool enumeration=false)const{
        A table=0,col=0,result=0;int row=0;if(!field(record,key,table,row,col))return false;
        A fn=method(table,boolean?0xf8:enumeration?0x108:0x100);if(!fn)return false;
        if(boolean){if(!sustained::obtain(result,reinterpret_cast<bool(__fastcall*)(A,int,A)>(fn),table,row,col))return false;}
        else if(!sustained::obtain(result,reinterpret_cast<int(__fastcall*)(A,int,A)>(fn),table,row,col))return false;
        out=static_cast<int>(result);return true;
    }
    bool related(A record,int key,A& out)const{
        A table=0,col=0,result=0;int row=0;if(!field(record,key,table,row,col))return false;
        A fn=method(table,0x128);return fn&&sustained::obtain(result,reinterpret_cast<A(__fastcall*)(A,A*,int,A)>(fn),table,&out,row,col);
    }
    bool element(A array,int index,A& out)const{
        // Native Array::operator[] (85fde50): the ordinal is the table column,
        // not a named schema field. Caller bounds it with the native count.
        A table=0,result=0;int row=0;
        if(index<0||index>=512||!value(array+0x10,table)||!value(array+0x18,row)||row<0||row>1000000)return false;
        A fn=method(table,0x128);return fn&&sustained::obtain(result,reinterpret_cast<A(__fastcall*)(A,A*,int,A)>(fn),table,&out,row,static_cast<A>(index));
    }
};
struct Record {
    const Reader& r;A p=0;
    explicit Record(const Reader& reader):r(reader){}
    ~Record(){if(p)r.release(p);}
    Record(const Record&)=delete;Record& operator=(const Record&)=delete;
};
inline bool team(const Reader& r,A p,Team& t){
    int hw=0,hl=0,rw=0,rl=0;
    if(!r.integer(p,0xd1,t.id)||!r.integer(p,0x182,t.rank)||!r.integer(p,0x9e,t.apRank)||!r.integer(p,0x191,t.toughest)||!r.integer(p,0x180,t.prestige)||
       !r.integer(p,0x7a,hw)||!r.integer(p,0x77,hl)||!r.integer(p,0xec,rw)||!r.integer(p,0xe9,rl))return false;
    t.wins=hw+rw;t.losses=hl+rl;t.homeGames=hw+hl;return true;
}
inline bool school(const Reader& r,A p,Team& t){
    Record tracking(r);
    // Team::MySchoolTrackingTable, then actual MySchoolTrackingTable LetterGrade fields.
    return r.related(p,0xa5,tracking.p)&&tracking.p&&
        r.integer(tracking.p,34,t.atmosphere,false,true)&&r.integer(tracking.p,23,t.tradition,false,true)&&
        r.integer(tracking.p,5,t.brand,false,true)&&r.integer(tracking.p,19,t.conference,false,true)&&grades(t);
}
inline bool capture(A image,A manager,A gameRef,int ea,Seed& s,const Api* custom=nullptr){
    Api api=custom?*custom:Api::engine(image);
    Reader r{image};A vt=0,game=0,league=0,season=0;
    if(!r.value(manager,vt)||vt!=image+StatManagerVtable||!r.value(gameRef,game)||!game||ea<=0)return false;
    Record home(r),away(r),stadium(r),bowl(r),teams(r),array(r),homeVenue(r),alternate(r);
    int type=-1,practice=0,homeScore=0,awayScore=0;
    if(!r.related(game,0x2a,home.p)||!home.p||!r.related(game,0x12,away.p)||!away.p||
       !r.integer(home.p,0xd1,s.home.id)||!r.integer(away.p,0xd1,s.away.id)||
       !r.related(game,0x40,stadium.p))return false;
    if(!stadium.p&&(!r.related(home.p,0x118,stadium.p)||!stadium.p))return false;
    if(!r.integer(stadium.p,6,s.stadium)||!r.integer(stadium.p,22,s.capacity)||!r.integer(stadium.p,7,s.roof,false,true)||
       !r.integer(game,0x3e,type,false,true)||!r.integer(game,0x33,practice,true)||!r.related(game,0x15,bowl.p)||
       !r.integer(game,0x3d,s.week)||!r.integer(game,0x3f,s.year)||!r.integer(game,36,homeScore)||!r.integer(game,12,awayScore))return false;
    s.ea=ea;s.resumed=homeScore!=0||awayScore!=0;s.regular=type==1&&!practice&&!bowl.p;
    if(!s.regular||s.resumed)return true;
    if(!r.related(home.p,0x118,homeVenue.p)||!homeVenue.p||!r.integer(homeVenue.p,6,s.homeStadium)||!r.related(home.p,0xb,alternate.p))return false;
    if(alternate.p&&!r.integer(alternate.p,6,s.homeAlternate))return false;
    if(!team(r,home.p,s.home)||!team(r,away.p,s.away)||!r.value(manager+0x230,league)||!league||
       !r.related(league,0xd,teams.p)||!teams.p)return false;
    A fn=r.method(teams.p,0x28),unused=0,count=0;
    if(!fn||!sustained::obtain(unused,reinterpret_cast<A(__fastcall*)(A,A*,unsigned)>(fn),teams.p,&array.p,0x41525259u)||!array.p||
       !api.count||!sustained::obtain(count,api.count,array.p)||count<100||count>512)return false;
    s.schools=static_cast<int>(count);
    std::vector<Team> cohort;cohort.reserve(s.schools);
    for(int i=0;i<static_cast<int>(count);++i){
        Record entry(r);int id=-1,rank=0;
        if(!r.element(array.p,i,entry.p)||!entry.p||!r.integer(entry.p,0xd1,id)||!r.integer(entry.p,0x182,rank))return false;
        // League arrays also contain placeholders/FCS. Real FBS schools own a
        // MySchool tracking record and a current rank; never normalize by 139 blindly.
        if(id<0||id>=2047||rank<1||fcs(id))continue;
        Team member;if(!team(r,entry.p,member)||!school(r,entry.p,member))return false;
        cohort.push_back(member);
    }
    if(!assignStandings(cohort))return false;
    s.schools=static_cast<int>(cohort.size());bool haveHome=false,haveAway=fcs(s.away.id);
    for(const auto& member:cohort){if(member.id==s.home.id){s.home=member;haveHome=true;}if(member.id==s.away.id){s.away=member;haveAway=true;}}
    if(!haveHome||!haveAway)return false;
    int championshipWeek=-1;
    if(!r.value(manager+0x228,season)||!season||!r.integer(season,0x2c,championshipWeek))return false;
    if(s.week==championshipWeek){
        // Same native condition as EA's conference-championship attendance bonus.
        A schema=0;fn=r.method(league,0x38);
        if(!fn||!sustained::obtain(schema,reinterpret_cast<A(__fastcall*)(A)>(fn),league))return false;
        A wrapper[5]={image+0xbd1dd08,league,schema,0,0},same=0;
        if(!api.same||!sustained::obtain(same,api.same,reinterpret_cast<A>(wrapper),&home.p,&away.p))return false;
        if(same)s.regular=false;
    }
    return true;
}
// Descriptor validation prevents raw field-offset assumptions after a title update.
inline bool option(const Reader& r,A service,unsigned key,const char* name,unsigned width,unsigned& value){
    A table=0,vt=0;unsigned short count=0;
    if(!r.value(service,vt)||vt!=r.image+OptionVtable||!r.value(r.image+OptionType+0x30,table)||
       !r.value(r.image+OptionType+0x38,count)||count<1||count>512)return false;
    for(unsigned i=0;i<count;++i){
        A p=table+48*i;unsigned flags=0;unsigned short offset=0;A text=0;
        if(!r.value(p+0x20,flags))return false;
        if((flags&0xffffff00u)!=key)continue;
        if((flags&1)||!r.value(p+8,text)||!sustained::named(text,name)||!r.value(p+0x28,offset)||offset>0x800)return false;
        value=0;return (width==1||width==2||width==4)&&r.read(service+offset,&value,width);
    }
    return false;
}
// MMC already excludes online play. This identifies the supported Dynasty
// setup only; no gameplay-only configuration object is needed during loading.
inline bool setupMode(A image,int& mode){Reader r{image};return r.value(image+0xd9e3818,mode);}
inline bool dynastySetup(A image,bool transition=false){
    int mode=-2;return setupMode(image,mode)&&(mode==14||(transition&&mode==-1));
}
// Identity reads cannot authorize a write; they only select a captured seed.
inline bool launchIdentity(A image,Launch& l){
    Reader r{image};A service=0;unsigned home=0,away=0,stadium=0,ea=0;
    if(!r.value(image+OptionService,service)||!option(r,service,0x9e8d2700,"gameHomeTeamId",2,home)||
       !option(r,service,0x9e1d2700,"gameAwayTeamId",2,away)||!option(r,service,0xce7a6400,"gameStadiumId",2,stadium)||
       !option(r,service,0x9e1d2500,"gameAttendance",4,ea))return false;
    l.home=static_cast<int>(home);l.away=static_cast<int>(away);l.stadium=static_cast<int>(stadium);l.ea=static_cast<int>(ea);return true;
}
// Capacity was already read from the selected season-game stadium record.
// Bind that value only after the actual launch identity has been checked;
// the broadcast RPC used during gameplay is not ready at this boundary.
inline bool capturedCapacity(const Seed& seed,Launch& l){
    if(seed.home.id!=l.home||seed.away.id!=l.away||seed.stadium!=l.stadium||seed.ea!=l.ea||
       seed.capacity<1000||seed.capacity>200000)return false;
    l.capacity=seed.capacity;return true;
}
inline bool launch(A image,Launch& l,const Seed& seed,const char** failure=nullptr,bool transition=false){
    auto fail=[&](const char* reason){if(failure)*failure=reason;return false;};if(failure)*failure=nullptr;
    Reader r{image};A service=0;unsigned h=0,a=0,stadium=0,ea=0,time=0,precip=0,rivalry=1023,bowl=127;
    if(!dynastySetup(image,transition))return fail("Dynasty setup/transition guard");
    if(!r.value(image+OptionService,service)||!service)return fail("launch option service");
    struct Field {unsigned key;const char* name;unsigned width;unsigned* value;};
    const Field fields[]={{0x9e8d2700,"gameHomeTeamId",2,&h},{0x9e1d2700,"gameAwayTeamId",2,&a},
        {0xce7a6400,"gameStadiumId",2,&stadium},{0x9e1d2500,"gameAttendance",4,&ea},
        {0x9f4be400,"gameTimeOfDay",2,&time},{0x9f7c2700,"gameWeatherPrecipitation",1,&precip},
        {0x9f2e6900,"gameRivalryId",2,&rivalry},{0x9e2b2900,"gameBowlId",1,&bowl}};
    for(const auto& field:fields)if(!option(r,service,field.key,field.name,field.width,*field.value))return fail(field.name);
    if(bowl!=127)return fail("gameBowlId is postseason");
    l.home=static_cast<int>(h);l.away=static_cast<int>(a);l.stadium=static_cast<int>(stadium);l.ea=static_cast<int>(ea);
    l.minutes=static_cast<int>(time);l.precipitation=static_cast<int>(precip);l.rivalry=rivalry!=1023;
    if(stadium>=1023)return fail("gameStadiumId range");
    if(!capturedCapacity(seed,l))return fail("captured stadium capacity/launch identity mismatch");
    return true;
}
inline bool setAttendance(A image,int count,const Api* custom=nullptr,bool transition=false){
    Api api=custom?*custom:Api::engine(image);
    Reader r{image};A service=0,vt=0,set=0;
    if(count<1||count>200000||!dynastySetup(image,transition)||!r.value(image+OptionService,service)||!r.value(service,vt)||vt!=image+OptionVtable||
       !r.value(vt+0x80,set)||set!=image+0x909b700)return false;
    unsigned before=0;if(!option(r,service,0x9e1d2500,"gameAttendance",4,before))return false;
    struct Reference {A type,data;} ref{image+UnsignedType,reinterpret_cast<A>(&count)};
    A okay=0;if(!api.set||!sustained::obtain(okay,api.set,service,bindings::AttendanceOption,reinterpret_cast<A>(&ref))||!okay)return false;
    unsigned after=0;return option(r,service,bindings::AttendanceOption,"gameAttendance",4,after)&&after==static_cast<unsigned>(count);
}
}
