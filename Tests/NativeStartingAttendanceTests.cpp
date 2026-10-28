#define WIN32_LEAN_AND_MEAN
#include "../Source/NativeStartingAttendance.h"
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <cstdio>
using namespace attendance::turnout;
using namespace attendance::turnout::native;
template<class T>void put(A p,T v){std::memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
A address(void* p){return reinterpret_cast<A>(p);}
struct Table;
struct Row {A vt=0,unused=0;Table* table=nullptr;int index=0;int refs=0;};
struct Table {
    A vt=0;unsigned char schema[0xe0]{};A columns[512]{};
    std::map<int,int> ints;std::map<int,Row*> related;
};
A image=0,recordVt=0,tableVt=0;int enums=0,sets=0;bool sameConference=false;
A __fastcall schema(A p){return address(reinterpret_cast<Row*>(p)->table->schema);}
void __fastcall release(A p){--reinterpret_cast<Row*>(p)->refs;}
int __fastcall integer(A table,int row,A column){assert(row==0);return reinterpret_cast<Table*>(table)->ints[static_cast<int>(column)];}
int __fastcall enumeration(A table,int row,A column){++enums;return integer(table,row,column);}
bool __fastcall boolean(A table,int row,A column){return integer(table,row,column)!=0;}
A __fastcall related(A table,A* out,int row,A column){
    assert(row==0);Row* record=reinterpret_cast<Table*>(table)->related[static_cast<int>(column)];if(record)++record->refs;*out=reinterpret_cast<A>(record);return address(out);
}
A __fastcall query(A object,A* out,unsigned id){assert(id==0x41525259u);++reinterpret_cast<Row*>(object)->refs;*out=object;return address(out);}
A __fastcall count(A){return 139;}
bool __fastcall same(A wrapper,A* home,A* away){assert(wrapper&&*home&&*away);return sameConference;}
int __fastcall capacity(int stadium,int* out){assert(stadium==553);*out=60000;return 0;}
bool __fastcall set(A service,unsigned key,A reference){
    assert(key==attendance::bindings::AttendanceOption);
    auto ref=reinterpret_cast<A*>(reference);assert(ref[0]==image+UnsignedType);int value=0;std::memcpy(&value,reinterpret_cast<void*>(ref[1]),4);put(service+0x14,value);++sets;return true;
}
void init(Table& t,Row& r){
    t.vt=tableVt;for(unsigned i=0;i<512;++i)t.columns[i]=i;
    put(address(t.schema)+0xd0,address(t.columns));put<short>(address(t.schema)+0xd8,512);
    r.vt=recordVt;r.table=&t;
}
void initTeam(Table& t,int id){t.ints={{0xd1,id},{0x182,50},{0x191,70},{0x180,6},{0x7a,2},{0x77,1},{0xec,1},{0xe9,2}};}
struct Member {A type,name;unsigned char unused[16];unsigned flags;unsigned reserved;unsigned short offset;unsigned char tail[6];};
static_assert(sizeof(Member)==48);
int main(){
    image=reinterpret_cast<A>(GetModuleHandleW(nullptr));std::vector<void*> memory;
    auto page=[&](A p){A base=p&~A(0xffff);for(void* existing:memory)if(address(existing)==base)return;void* block=VirtualAlloc(reinterpret_cast<void*>(base),0x10000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);assert(block==reinterpret_cast<void*>(base));memory.push_back(block);};
    recordVt=image+0xb100000;tableVt=recordVt+0x100;page(recordVt);
    put(recordVt+0x38,reinterpret_cast<A>(schema));put(recordVt+8,reinterpret_cast<A>(release));put(recordVt+0x28,reinterpret_cast<A>(query));
    put(tableVt+0x100,reinterpret_cast<A>(integer));put(tableVt+0x108,reinterpret_cast<A>(enumeration));put(tableVt+0xf8,reinterpret_cast<A>(boolean));put(tableVt+0x128,reinterpret_cast<A>(related));
    Table gt,ht,at,st,lt,seasont,arrt;Row game,home,away,stadium,league,season,array;
    init(gt,game);init(ht,home);init(at,away);init(st,stadium);init(lt,league);init(seasont,season);init(arrt,array);
    initTeam(ht,1153);initTeam(at,1193);st.ints={{6,553},{22,60000}};
    gt.ints={{0x3e,1},{0x33,0},{0x3d,8},{0x3f,2026},{36,0},{12,0}};
    gt.related={{0x2a,&home},{0x12,&away},{0x40,&stadium},{0x15,nullptr}};ht.related={{0x118,&stadium},{0xb,nullptr}};lt.related={{0xd,&array}};seasont.ints={{0x2c,15}};
    unsigned char manager[0x240]{};put(address(manager),image+StatManagerVtable);put(address(manager)+0x230,address(&league));put(address(manager)+0x228,address(&season));A reference=address(&game);
    Api api{count,same,set,capacity};Seed seed;
    assert(capture(image,address(manager),address(&reference),45000,seed,&api)&&seed.regular&&!seed.resumed&&seed.schools==139&&seed.home.id==1153&&seed.home.homeGames==3&&seed.home.wins==3&&seed.home.losses==3&&enums>0);
    auto noLeaks=[&](){for(Row* r:{&game,&home,&away,&stadium,&league,&season,&array})assert(r->refs==0);};noLeaks();
    gt.ints[0x3e]=2;assert(capture(image,address(manager),address(&reference),45000,seed,&api)&&!seed.regular);noLeaks();gt.ints[0x3e]=1;
    gt.related[0x15]=&stadium;assert(capture(image,address(manager),address(&reference),45000,seed,&api)&&!seed.regular);noLeaks();gt.related[0x15]=nullptr;
    gt.ints[36]=7;assert(capture(image,address(manager),address(&reference),45000,seed,&api)&&seed.resumed);noLeaks();gt.ints[36]=0;
    gt.ints[0x3d]=15;sameConference=true;assert(capture(image,address(manager),address(&reference),45000,seed,&api)&&!seed.regular);noLeaks();sameConference=false;assert(capture(image,address(manager),address(&reference),45000,seed,&api)&&seed.regular);noLeaks();gt.ints[0x3d]=8;
    put(tableVt+0x128,A(0));assert(!capture(image,address(manager),address(&reference),45000,seed,&api));put(tableVt+0x128,reinterpret_cast<A>(related));noLeaks();
    // Real descriptor layout and typed setter contract; no writes to native records.
    for(A p:{image+OptionService,image+OptionType,image+OptionVtable,image+0xd9e3818,image+0xf6379e0})page(p);
    unsigned char options[0x200]{},config[0x100]{};put(image+OptionService,address(options));put(address(options),image+OptionVtable);put(image+OptionVtable+0x80,image+0x909b700);
    put<int>(image+0xd9e3818,14);put(image+0xf6379e0,address(config));
    const char* names[]={"gameHomeTeamId","gameAwayTeamId","gameStadiumId","gameAttendance","gameTimeOfDay","gameWeatherPrecipitation","gameRivalryId","gameBowlId"};
    unsigned keys[]={0x9e8d2700,0x9e1d2700,0xce7a6400,0x9e1d2500,0x9f4be400,0x9f7c2700,0x9f2e6900,0x9e2b2900};
    unsigned short offsets[]={0x30,0x20,0x46,0x14,0x3c,0x49,0x32,0x22};Member members[8]{};
    for(unsigned i=0;i<8;++i){members[i].name=reinterpret_cast<A>(names[i]);members[i].flags=keys[i];members[i].offset=offsets[i];}
    put(image+OptionType+0x30,address(members));put<unsigned short>(image+OptionType+0x38,8);
    put<unsigned short>(address(options)+0x30,1153);put<unsigned short>(address(options)+0x20,1193);put<unsigned short>(address(options)+0x46,553);put<int>(address(options)+0x14,45000);put<unsigned short>(address(options)+0x3c,1140);put<unsigned char>(address(options)+0x49,0);put<unsigned short>(address(options)+0x32,1023);put<unsigned char>(address(options)+0x22,127);
    Launch launchInput;assert(launch(image,launchInput,&api)&&launchInput.home==1153&&launchInput.capacity==60000&&launchInput.minutes==1140&&!launchInput.rivalry);
    unsigned char before[sizeof(options)];std::memcpy(before,options,sizeof(options));assert(setAttendance(image,50000,&api)&&sets==1);put<int>(address(before)+0x14,50000);assert(!std::memcmp(before,options,sizeof(options)));
    assert(gt.ints[9]==0&&seed.ea==45000); // The simulation/save attendance record never receives the launch override.
    members[3].flags|=1;assert(!setAttendance(image,51000,&api)&&sets==1);members[3].flags&=~1u;
    put<unsigned char>(address(config)+0x84,1);assert(!launch(image,launchInput,&api)&&!setAttendance(image,51000,&api)&&sets==1);put<unsigned char>(address(config)+0x84,0);
    put<int>(image+0xd9e3818,17);assert(!launch(image,launchInput,&api)&&!setAttendance(image,51000,&api));put<int>(image+0xd9e3818,14);
    put<unsigned char>(address(options)+0x22,47);assert(!launch(image,launchInput,&api));
    for(void* block:memory)assert(VirtualFree(block,0,MEM_RELEASE));
    std::puts("PASS: native record/schema/int/enum/boolean ABI, owned-reference cleanup, regular/postseason/conference/resume classification, validated launch descriptors and isolated typed option write.");
}
