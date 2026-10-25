#include "../Source/LogFiles.h"
#include <cassert>
#include <chrono>
#include <iostream>
static std::string contents(const std::filesystem::path& p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
int main(){
 auto folder=std::filesystem::temp_directory_path()/("GamedayLogTest-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
 auto path=folder/"Logs"/"atmosphere.log";
 assert(attendance::appendLog(path,"first\n",16)&&contents(path)=="first\n");
 assert(attendance::appendLog(path,"second\n",16)&&contents(path)=="first\nsecond\n");
 assert(attendance::appendLog(path,"new\n",16)&&contents(path)=="new\n");
 assert(attendance::appendLog(path,std::string(40,'X'),16)&&std::filesystem::file_size(path)==16);
 assert(attendance::appendLog(path,"after\n",16)&&contents(path)=="after\n");
 assert(!attendance::appendLog(path,"ignored",0)&&contents(path)=="after\n");
 assert(!std::filesystem::exists(path.string()+".1")&&!std::filesystem::exists(path.string()+".2"));
 std::filesystem::remove(path);std::filesystem::remove(folder/"Logs");std::filesystem::remove(folder);
 std::cout<<"PASS: nested log folder, bounded append, clear-before-overflow, oversized entries and no archives.\n";
}
