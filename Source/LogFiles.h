#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
namespace attendance {
// Clear before the next entry would exceed the cap. No archives accumulate.
inline bool appendLog(const std::filesystem::path& path,const std::string& entry,uintmax_t limit){
    if(!limit)return false;
    std::error_code ec;
    if(!path.parent_path().empty())std::filesystem::create_directories(path.parent_path(),ec);
    if(ec)return false;
    size_t bytes=static_cast<size_t>((std::min)(limit,static_cast<uintmax_t>(entry.size())));
    uintmax_t existing=0;
    if(std::filesystem::exists(path,ec)){existing=std::filesystem::file_size(path,ec);if(ec)return false;}
    if(ec)return false;
    bool clear=existing>=limit||bytes>limit-existing;
    std::ofstream file(path,std::ios::binary|std::ios::out|(clear?std::ios::trunc:std::ios::app));
    file.write(entry.data(),static_cast<std::streamsize>(bytes));return file.good();
}
}
