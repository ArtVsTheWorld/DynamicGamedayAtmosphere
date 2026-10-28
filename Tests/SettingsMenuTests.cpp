#define WIN32_LEAN_AND_MEAN
#include "../Source/NativeSettingsMenu.h"
#include <cassert>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
using namespace attendance::settingsmenu;
using attendance::settingsmenu::native::A;
struct Reader {
    A* vt;int value;
    static int __fastcall field(A p,const char*){return reinterpret_cast<Reader*>(p)->value;}
    static int __fastcall number(A p){return reinterpret_cast<Reader*>(p)->value;}
    inline static A table[4]={0,0,reinterpret_cast<A>(field),reinterpret_cast<A>(number)};
    explicit Reader(int n):vt(table),value(n){}
    A address(){return reinterpret_cast<A>(this);}
};
struct Output {
    A* vt;
    std::map<int,std::map<std::string,std::string>> fields;
    static void __fastcall integer(A p,int row,const char* key,int value){reinterpret_cast<Output*>(p)->fields[row][key]=std::to_string(value);}
    static void __fastcall text(A p,int row,const char* key,const char* value){reinterpret_cast<Output*>(p)->fields[row][key]=value;}
    static void __fastcall boolean(A p,int row,const char* key,bool value){integer(p,row,key,value?1:0);}
    inline static A table[7]={0,0,0,reinterpret_cast<A>(boolean),reinterpret_cast<A>(integer),0,reinterpret_cast<A>(text)};
    Output():vt(table){}
    A address(){return reinterpret_cast<A>(this);}
};
namespace attendance::settingsmenu::native {
struct Fixture {
    inline static int initialized=0,finished=0,passed=0;
    static void __fastcall initNote(A){++initialized;}
    static void __fastcall finishNote(A,int key,bool okay,const char* reason){assert(key==Category&&okay&&!*reason);++finished;}
    static void __fastcall pass(A p,A out,A category,A key,A value,A reason){assert(p==17&&out==18&&category&&key&&value&&reason==23);++passed;}
    static void __fastcall passSlider(A p,A out,A category,A key,int value,A reason){assert(p==17&&out==18&&category&&key&&value==42&&reason==23);++passed;}
    static void configure(Menu& m){m.initNotify_=reinterpret_cast<A>(initNote);m.shutdownNotify_=reinterpret_cast<A>(finishNote);m.stock_[6]=reinterpret_cast<A>(pass);m.stock_[8]=reinterpret_cast<A>(passSlider);}
    static double current(const Menu& m,size_t i){return m.current_[i];}
    static const auto& choices(const Menu& m,size_t i){return m.choices_[i];}
};
}
int main(){
    using namespace attendance::settingsmenu::native;
    assert(Count==35);std::set<std::wstring> unique;
    Output catalog;ArrayWriter writer{catalog.address()};attendance::settingsmenu::list(writer);assert(writer.okay);
    size_t controls=0,headers=0;
    for(const auto& row:catalog.fields){const auto& fields=row.second;
        if(fields.at("type")=="0"){++headers;assert(fields.at("isInteractive")=="0");continue;}
        int key=std::stoi(fields.at("key"));assert(owned(key)&&find(key));++controls;
        const auto& o=*find(key);assert(o.low<=o.factory&&o.factory<=o.high&&std::strlen(o.help)>10);
        assert(unique.insert(std::wstring(o.section)+L"/"+o.key).second);
        assert(fields.at("isEnabled")=="1"&&fields.at("isFocusable")=="1"&&fields.at("isInteractive")=="1");
        for(double value:{o.low,o.factory,o.high}){
            Choices choices(o,value);assert(choices.index(value)>=0&&choices.index(o.factory)>=0);
            if(o.kind==Kind::Toggle)assert(choices.values.size()==2);
            Output states;ArrayWriter stateWriter{states.address()};attendance::settingsmenu::toggle(stateWriter,o,choices,value);
            int selected=0,resets=0;for(const auto& item:states.fields){selected+=item.second.at("isDefault")=="1";resets+=item.second.at("isResetOption")=="1";}
            assert(selected==1&&resets==1);
        }
    }
    assert(controls==Count&&headers==3);
    std::map<std::string,size_t> sizes;std::map<std::string,bool> numericSeen;
    for(const auto& o:Options){++sizes[o.group];if(o.kind!=Kind::Toggle)numericSeen[o.group]=true;else assert(!numericSeen[o.group]);}
    assert(sizes.size()==3&&sizes["Attendance"]==10&&sizes["Soundscape"]==12&&sizes["Broadcast"]==13);
    for(const auto& group:sizes)assert(group.second<=15);
    // Execute the new bytecode fallback's branch semantics: only DGA can pass
    // through the finite native tab switch; every other unknown key returns zero.
    auto patch=categoryFallback();assert(patch[0]==0xb9&&patch[1]==2&&patch[7]==0x66&&patch[8]==0xb8&&patch[16]==0xb9&&patch[18]==0x3e);
    int encoded=0;std::memcpy(&encoded,patch.data()+3,4);assert(encoded==Category);
    int branch=0;std::memcpy(&branch,patch.data()+12,4);assert(0x2893c+branch==0x2894c);
    for(int key:{-1,0,7,21,Category,Category+1})assert((key==encoded?key:0)==(key==Category?Category:0));
    wchar_t temp[MAX_PATH]{};GetTempPathW(MAX_PATH,temp);auto path=std::filesystem::path(temp)/(L"DgaSettingsFixture-"+std::to_wstring(GetCurrentProcessId())+L".ini");
    {std::ofstream f(path);f<<"; keep this comment\n[General]\nConfigVersion=2\n[Unrelated]\nCustom=keep\n[Dynamic Gameday Soundscape]\nMaximumBoostPercent=23.25\n";}
    Menu menu;menu.configure(0,path,nullptr);Fixture::configure(menu);Reader category(Category);std::array<unsigned char,0x120> provider{};A p=reinterpret_cast<A>(provider.data());
    menu.initialize(p,category.address());assert(Fixture::initialized==1&&provider[0x62]==1);
    for(size_t i=0;i<Count;++i){const auto& o=Options[i];Reader key(FirstKey+static_cast<int>(i));
        if(o.kind==Kind::Slider){menu.setSlider(p,0,category.address(),key.address(),static_cast<int>(o.high)+1,0);assert(Fixture::current(menu,i)==o.factory);menu.setSlider(p,0,category.address(),key.address(),static_cast<int>(o.low),0);}
        else {
            auto choices=Fixture::choices(menu,i);int selected=-1;for(size_t n=0;n<choices.size();++n)if(choices[n]==o.low)selected=static_cast<int>(n);
            assert(selected>=0);Reader value(selected);menu.setToggle(p,0,category.address(),key.address(),value.address(),0);
        }
        assert(Fixture::current(menu,i)==o.low);
    }
    Reader stockCategory(7),stockKey(3),stockValue(1);menu.setToggle(17,18,stockCategory.address(),stockKey.address(),stockValue.address(),23);assert(Fixture::passed==1);
    menu.setSlider(17,18,stockCategory.address(),stockKey.address(),42,23);assert(Fixture::passed==2);
    menu.finish(p,0);assert(Fixture::finished==1&&provider[0x62]==0);
    std::ifstream f(path);std::ostringstream text;text<<f.rdbuf();f.close();assert(text.str().find("keep this comment")!=std::string::npos&&text.str().find("Custom=keep")!=std::string::npos);
    // Saving this trimmed menu cannot rewrite advanced values omitted from it.
    wchar_t hidden[32]{};GetPrivateProfileStringW(L"Dynamic Gameday Soundscape",L"MaximumBoostPercent",L"",hidden,32,path.c_str());assert(std::wcscmp(hidden,L"23.25")==0);
    // Reopening reads saved values. A newer config schema never gets overwritten.
    menu.initialize(p,category.address());for(size_t i=0;i<Count;++i)assert(Fixture::current(menu,i)==Options[i].low);
    WritePrivateProfileStringW(L"General",L"ConfigVersion",L"3",path.c_str());
    size_t sliderIndex=0;while(Options[sliderIndex].kind!=Kind::Slider)++sliderIndex;Reader key(FirstKey+static_cast<int>(sliderIndex));
    menu.setSlider(p,0,category.address(),key.address(),static_cast<int>(Options[sliderIndex].high),0);assert(Fixture::current(menu,sliderIndex)==Options[sliderIndex].low);
    menu.finish(p,0);DeleteFileW(path.c_str());
    std::puts("PASS: native settings rows/choice flags, unique owned IDs, exact bounds and factories, category bytecode branch, INI persistence/reopen/comments, future schema guard and complete six-argument stock passthrough.");
}
