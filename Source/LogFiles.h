#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <cstdint>
namespace attendance {
class DuplicateLogFilter {
    std::map<std::string,uint64_t> times_;
public:
    bool accept(const std::string& key,uint64_t now){
        auto old=times_.find(key);if(old!=times_.end()&&now>=old->second&&now-old->second<5000)return false;
        if(times_.size()>=128&&old==times_.end()){
            auto oldest=std::min_element(times_.begin(),times_.end(),[](const auto& a,const auto& b){return a.second<b.second;});times_.erase(oldest);
        }
        times_[key]=now;return true;
    }
    size_t size()const{return times_.size();}
};
inline bool ensureFolder(const std::filesystem::path& path){
    if(path.empty())return true;
    std::error_code ec;
    if(std::filesystem::is_directory(path,ec))return true;
    ec.clear();std::filesystem::create_directories(path,ec);
    return !ec;
}
// Clear before the next entry would exceed the cap. No archives accumulate.
inline bool appendLog(const std::filesystem::path& path,const std::string& entry,uintmax_t limit){
    if(!limit)return false;
    std::error_code ec;
    if(!ensureFolder(path.parent_path()))return false;
    size_t bytes=static_cast<size_t>((std::min)(limit,static_cast<uintmax_t>(entry.size())));
    uintmax_t existing=0;
    if(std::filesystem::exists(path,ec)){existing=std::filesystem::file_size(path,ec);if(ec)return false;}
    if(ec)return false;
    bool clear=existing>=limit||bytes>limit-existing;
    std::ofstream file(path,std::ios::binary|std::ios::out|(clear?std::ios::trunc:std::ios::app));
    file.write(entry.data(),static_cast<std::streamsize>(bytes));return file.good();
}
}
