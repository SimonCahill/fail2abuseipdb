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

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "f2b_jail.hpp"

namespace f2abuseipdb {

    using std::string;
    using std::vector;

    optional<Jail> Jail::loadJailFromDb(const string& jail, sqlite3* dbConn, shared_ptr<config::ConfigParser> cfg) {
        const auto query = cfg->getQuery("get_banned_ips_per_jail_query");
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

            jails.emplace_back(Jail(nameCol, static_cast<bool>(enabledCol)));
        }

        if (retCode != SQLITE_DONE) {
            // TODO: Handle
        }
        
        sqlite3_reset(statement);

        return jails;
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


}