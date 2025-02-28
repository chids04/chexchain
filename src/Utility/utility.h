#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>


namespace BlockchainAssignment::Utility{

    //gets current time as a unix timestamp, in ms
    inline long long genTimeStamp(){
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    
        return duration.count();
    }

    inline std::string printTime(long long timestamp){

        std::chrono::seconds  seconds(timestamp/1000); //convert ms to s
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(0) + seconds);
        
        std::tm* localTime = std::localtime(&time);
        std::ostringstream oss;

        oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

        return oss.str();
    }
    
}