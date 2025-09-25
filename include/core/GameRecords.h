#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>  
#include <fstream>
#include <algorithm>
#include <iomanip>

#include "Config.h"

namespace Records{
    using json = nlohmann::json;

    struct GameRecord 
    {
        int m_level {};
        int m_score {};
        std::string m_dateCompleted {};
        std::string m_name {};

        GameRecord(int level, int score, std::string dateCompleted, std::string name)
        : m_level(level), m_score(score), m_dateCompleted(dateCompleted), m_name(name)
        {}

        GameRecord()
        : m_level(0), m_score(0), m_dateCompleted(""), m_name("")
        {}
    };

    // Function declarations
    std::string getCurrentDate();
    std::vector<GameRecord> loadAllGameRecords();
    void saveGameRecord(const GameRecord& gm);
    std::vector<GameRecord> getTop5Records(int level);
    void sortRecords(std::vector<GameRecord>& gr);

    inline std::vector<GameRecord> loadAllGameRecords()
    {
        std::vector<GameRecord> records;
        std::ifstream file(Config::HIGH_SCORES);
        
        if (!file.is_open()) {
            return records; // Return empty vector if file doesn't exist
        }
        
        json j;
        try {
            file >> j;
            
            // Handle both single record and array of records
            if (j.is_array()) {
                for (const auto& item : j) {
                    GameRecord record;
                    record.m_name = item.value("playerName", "Unknown");
                    record.m_score = item.value("score", 0);
                    record.m_level = item.value("level", 1);
                    record.m_dateCompleted = item.value("date", "Unknown");
                    records.push_back(record);
                }
            } else {
                std::cerr << "Error loading objects\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading game records: " << e.what() << std::endl;
        }
        
        return records;
    }

    inline void saveGameRecord(const GameRecord& gm)
    {
        // Load existing records
        std::vector<GameRecord> records = loadAllGameRecords();
        
        // Create new record with current date
        GameRecord newRecord = gm;
        newRecord.m_dateCompleted = getCurrentDate();
        
        // Add new record
        records.push_back(newRecord);

        //sort the records
        sortRecords(records);
        
        // Convert to JSON array
        json j = json::array();
        for (const auto& record : records) {
            json recordJson;
            recordJson["playerName"] = record.m_name;
            recordJson["date"] = record.m_dateCompleted;
            recordJson["score"] = record.m_score;
            recordJson["level"] = record.m_level;
            j.push_back(recordJson);
        }
        
        // Save to file
        std::ofstream file(Config::HIGH_SCORES);
        if (file.is_open()) {
            file << j.dump(4); // 4 space indent
        } else {
            std::cerr << "Error: Could not open \n";
        }
    }

    inline void replaceGameRecord(const GameRecord& gm_old, const GameRecord& gm_new)
    {
        std::vector<GameRecord> records = loadAllGameRecords();
        
        // Find and replace the old record
        bool found = false;
        for (auto& record : records) {
            if (record.m_name == gm_old.m_name && 
                record.m_score == gm_old.m_score && 
                record.m_level == gm_old.m_level &&
                record.m_dateCompleted == gm_old.m_dateCompleted) {
                
                // Replace with new record
                record = gm_new;
                record.m_dateCompleted = getCurrentDate(); // Update date
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cerr << "Error finding save record\n";
        }

        // sort the records
        sortRecords(records);
        
        // Save all records back to file
        json j = json::array();
        for (const auto& record : records) {
            json recordJson;
            recordJson["playerName"] = record.m_name;
            recordJson["date"] = record.m_dateCompleted;
            recordJson["score"] = record.m_score;
            recordJson["level"] = record.m_level;
            j.push_back(recordJson);
        }
        
        std::ofstream file(Config::HIGH_SCORES);
        if (file.is_open()) {
            file << j.dump(4);
        } else {
            std::cerr << "Error: Could not open high scores file for writing" << std::endl;
        }
    }



    inline std::vector<GameRecord> getTop5Records(int level)
    {
        std::vector<GameRecord> records = loadAllGameRecords();

        records.erase(std::remove_if(records.begin(), records.end(), [level](const GameRecord& a) {
            return a.m_level != level;
        }), records.end());
        
        // Sort by score (highest first)
        sortRecords(records);
        
        // Return top 5 (or fewer if less than 5 records exist)
        if (records.size() > 5) {
            records.resize(5);
        }
        
        return records;
    }


    inline std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        std::ostringstream oss;
        oss << std::setfill('0') 
            << std::setw(2) << (tm.tm_mon + 1) << "/"  // Month (0-11, so +1)
            << std::setw(2) << tm.tm_mday << "/"       // Day
            << (tm.tm_year + 1900);                    // Year (years since 1900)
        
        return oss.str();
    }

    inline void sortRecords(std::vector<GameRecord>& gr)
    {
        std::sort(gr.begin(), gr.end(), [](const GameRecord& a, const GameRecord& b) {
            if (a.m_level != b.m_level)
                return a.m_level < b.m_level;
            else   
                return a.m_score > b.m_score;
        });
    }
}