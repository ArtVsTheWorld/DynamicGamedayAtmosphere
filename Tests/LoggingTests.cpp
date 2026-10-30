#include "../Source/LogFiles.h"
#include "../Source/CallbackTiming.h"
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
 std::filesystem::remove_all(folder/"Logs");
 assert(attendance::appendLog(path,"recreated\n",16)&&contents(path)=="recreated\n");
 auto blocker=folder/"not-a-directory";{std::ofstream f(blocker);f<<"keep";}
 assert(!attendance::appendLog(blocker/"invalid.log","ignored",16)&&contents(blocker)=="keep");
 std::filesystem::remove(blocker);
 std::filesystem::remove(path);std::filesystem::remove(folder/"Logs");std::filesystem::remove(folder);
 attendance::LogQueue queue;queue.push("first");queue.push("second");auto batch=queue.take(1);assert(batch.size()==1&&batch[0]=="first"&&queue.size()==1);
 for(int i=0;i<1000;++i)queue.push(std::to_string(i));assert(queue.size()==512&&queue.dropped()==489);assert(queue.take(512).back()=="999"&&queue.size()==0);
 queue.push(std::string(100000,'X'));assert(queue.take()[0].size()==16384);
 attendance::timing::Counts counts;counts.record(20);counts.record(4000);counts.record(20000);counts.record(10);assert(counts.calls==4&&counts.maximumUs==20000&&counts.over4ms==2&&counts.over16ms==1);
 attendance::DuplicateLogFilter filter;assert(filter.accept("history/fault",1000));assert(filter.accept("audio/fault",1100));assert(!filter.accept("history/fault",1200));assert(filter.accept("history/fault",6000));
 for(int i=0;i<1000;++i)filter.accept(std::to_string(i),7000+i);assert(filter.size()==128);assert(filter.accept("rollback",1));
 std::cout<<"PASS: nested log folder, bounded append, clear-before-overflow, oversized entries, no archives and bounded per-message duplicate suppression across interleaved faults.\n";
}
