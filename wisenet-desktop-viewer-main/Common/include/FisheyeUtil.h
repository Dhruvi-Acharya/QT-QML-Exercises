#pragma once

#include <string>
#include <set>

namespace Wisenet
{
namespace Common
{

/*
 * Returns the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time.
 * This number is like the POSIX time_t variable, but expressed in milliseconds instead.
 */
inline bool GetFisheyeType(const std::string &modelName, std::string &fisheyeType)
{
    std::set<std::string> fisheyeSet{ "SNF-8010", "PNF-9010", "XNF-8010", "HNF-8010", "QNF-8010", "QNF-9010", "XNF-9010", "TNF-9010", "XNF-9013"};

    for(auto & item : fisheyeSet){
        if(modelName.find(item) != std::string::npos){
            fisheyeType = item;
            return true;
        }
    }
    return false;
}




}
}

