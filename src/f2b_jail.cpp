/**
 * @file f2b_jail.hpp
 * @author Simon Cahill (contact@simonc.eu), Pascal Lüttmann (pascal@pascal-luettmann.de)
 * @brief Contains the implementation of the Jail class.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems, Pascal Lüttmann
 */

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <optional>
#include <string>
#include <vector>

// json
#include <nlohmann/json.hpp>

// sqlite
#include <sqlite3.h>

// spdlog
#include <spdlog/spdlog.h>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "f2b_jail.hpp"

namespace f2abuseipdb {

    using std::string;
    using std::vector;

    optional<Jail> Jail::loadJailFromDb(const string& jail, sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        const static string STR_VAR{"${JAIL}"};
        auto query = cfg->getQuery("get_specific_jail_query");
        query.replace(query.find(STR_VAR), STR_VAR.size(), jail);

        sqlite3_stmt* statement{nullptr};
        if (int32_t retCode = sqlite3_prepare_v2(dbConn, query.c_str(), query.size(), std::addressof(statement), nullptr); retCode != 0) {
            // TODO: log error
            return {};
        }

        string jailName{};
        bool isEnabled{};
        
        if (int32_t retCode = sqlite3_step(statement); retCode == SQLITE_ROW) {
            // Read name column
            jailName = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));

            // Read enabled column
            isEnabled = sqlite3_column_int(statement, 1);
        }

        sqlite3_reset(statement);

        return Jail{jailName, isEnabled};
    }

    jailvec_t Jail::loadJailsFromDb(sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        const auto query = cfg->getQuery("get_jails_query");
        sqlite3_stmt* statement{nullptr};
        if (int32_t retCode = sqlite3_prepare_v2(dbConn, query.c_str(), query.size(), std::addressof(statement), nullptr); retCode != 0) {
            // TODO: log error
            return {};
        }

        vector<Jail> jails{};

        int32_t retCode = 0;
        while ((retCode = sqlite3_step(statement)) == SQLITE_ROW) {
            // Read name column
            const string nameCol = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));

            // Read enabled column
            const auto enabledCol = sqlite3_column_int(statement, 1);

            jails.emplace_back(nameCol, static_cast<bool>(enabledCol), cfg->getJailDescription(nameCol));
        }

        if (retCode != SQLITE_DONE) {
            // TODO: Handle
        }
        
        sqlite3_reset(statement);

        return jails;
    }

    json Jail::generateReportObject() const {
        json report{
            { "jail_name", getJailName() },
            { "jail_description", nullptr },
            { "is_enabled", isEnabled() },
            { "reports", json::array() }
        };

        if (m_jailDescription.has_value()) {
            report["jail_description"] = m_jailDescription.value();
        }

        for (const auto& record : m_reports) {
            report["reports"].push_back(record.toJson());
        }

        return report;
    }

    size_t Jail::getTotalJails(sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        const auto query = cfg->getQuery("count_jails_query");
        sqlite3_stmt* statement{nullptr};
        if (int32_t retCode = sqlite3_prepare_v2(dbConn, query.c_str(), query.size(), std::addressof(statement), nullptr); retCode != 0) {
            // TODO: log error
            return {};
        }

        int32_t result = 0;
        
        if (int32_t retCode = sqlite3_step(statement); retCode == SQLITE_ROW) {
            result = sqlite3_column_int(statement, 0);
        }

        sqlite3_reset(statement);

        return result;
    }

    void Jail::loadAllBanned(sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        m_reports.clear();
        const static string JAIL_VAR{"${JAIL}"};
        auto query = cfg->getQuery("get_banned_ips_per_jail_query");
        query.replace(query.find(JAIL_VAR), JAIL_VAR.size(), getJailName());

        sqlite3_stmt* statement{nullptr};
        if (int32_t retCode = sqlite3_prepare_v2(dbConn, query.c_str(), query.size(), std::addressof(statement), nullptr); retCode != 0) {
            spdlog::error("Error while preparing statement: {0:s}", sqlite3_errstr(retCode));
            return;
        }
        
        while (sqlite3_step(statement) == SQLITE_ROW) {
            const string ip{reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)), static_cast<size_t>(sqlite3_column_bytes(statement, 0))};
            const time_t timeOfBan = sqlite3_column_int(statement, 2);
            const time_t banTime = sqlite3_column_int(statement, 3);
            const size_t timesBanned = sqlite3_column_int64(statement, 4);
            json jsonData{};

            try {
                const string jsonString{reinterpret_cast<const char*>(sqlite3_column_blob(statement, 5)), static_cast<size_t>(sqlite3_column_bytes(statement, 5))};
                jsonData = json::parse(jsonString, nullptr, true, true);
            } catch (json::exception& ex) {
                spdlog::error("Failed to parse JSON data! Error: {0:s}", ex.what());
            }

            ReportRecord report{time_point<system_clock>(seconds{timeOfBan}), seconds{banTime}, jsonData, timesBanned, ip};
            m_reports.push_back(report);
        }

        sqlite3_reset(statement);
    }

    void Jail::loadActiveBanned(sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        m_reports.clear();
        const static string JAIL_VAR{"${JAIL}"};
        auto query = cfg->getQuery("get_banned_ips_per_jail_query");
        query.replace(query.find(JAIL_VAR), JAIL_VAR.size(), getJailName());

        sqlite3_stmt* statement{nullptr};
        if (int32_t retCode = sqlite3_prepare_v2(dbConn, query.c_str(), query.size(), std::addressof(statement), nullptr); retCode != 0) {
            spdlog::error("Failed to prepare statement: {0:s}", sqlite3_errstr(retCode));
            return;
        }
        
        while (sqlite3_step(statement) == SQLITE_ROW) {
            const string ip{reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)), static_cast<size_t>(sqlite3_column_bytes(statement, 0))};
            const int32_t timeOfBan = sqlite3_column_int(statement, 2);
            const int32_t banTime = sqlite3_column_int(statement, 3);
            const size_t timesBanned = sqlite3_column_int64(statement, 4);

            if (
                timeOfBan + banTime < time(nullptr) ||
                timeOfBan > cfg->getBanIgnoreThreshold()
            ) { continue; } // this ban is already over

            json jsonData{};

            try {
                const string jsonString{reinterpret_cast<const char*>(sqlite3_column_blob(statement, 5)), static_cast<size_t>(sqlite3_column_bytes(statement, 5))};
                jsonData = json::parse(jsonString, nullptr, true, true);
            } catch (json::exception&) { /* for now, we don't really care. Just means some fields won't be present */ }

            m_reports.emplace_back(time_point<system_clock>(seconds{timeOfBan}), seconds{banTime}, jsonData, timesBanned, ip);
        }

        sqlite3_reset(statement);
    }

    void Jail::loadFormerBanned(sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        m_reports.clear();
        const static string JAIL_VAR{"${JAIL}"};
        auto query = cfg->getQuery("get_banned_ips_per_jail_query");
        query.replace(query.find(JAIL_VAR), JAIL_VAR.size(), getJailName());

        sqlite3_stmt* statement{nullptr};
        if (int32_t retCode = sqlite3_prepare_v2(dbConn, query.c_str(), query.size(), std::addressof(statement), nullptr); retCode != 0) {
            // TODO: log error
            return;
        }
        
        while (sqlite3_step(statement) == SQLITE_ROW) {
            const string ip{reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)), static_cast<size_t>(sqlite3_column_bytes(statement, 0))};
            const time_t timeOfBan = sqlite3_column_int(statement, 2);
            const time_t banTime = sqlite3_column_int(statement, 3);
            const size_t timesBanned = sqlite3_column_int64(statement, 4);

            if (time(nullptr) >= timeOfBan + banTime) { continue; }

            json jsonData{};

            try {
                const string jsonString{reinterpret_cast<const char*>(sqlite3_column_blob(statement, 5)), static_cast<size_t>(sqlite3_column_bytes(statement, 5))};
                jsonData = json::parse(jsonString, nullptr, true, true);
            } catch (json::exception&) { /* for now, we don't really care. Just means some fields won't be present */ }

            m_reports.emplace_back(time_point<system_clock>(seconds{timeOfBan}), seconds{banTime}, jsonData, timesBanned, ip);
        }

        sqlite3_reset(statement);
    }

}