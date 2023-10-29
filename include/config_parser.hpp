/**
 * @file f2b_jail.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the declaration of the Jail class, which represents a single jail in fail2ban.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems and Pascal Lüttmann
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_CONFIG_PARSER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_CONFIG_PARSER_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

// json
#include <nlohmann/json.hpp>

namespace f2abuseipdb::config {

    namespace fs = std::filesystem;

    using std::vector;
    using std::optional;
    using std::string;

    using nlohmann::json;

    using optstring_t = optional<string>;
    using optpath_t = optional<fs::path>;
    using optstringvector = optional<vector<string>>;
    
    /**
    * Reads and parses a given config file from json format 
    */
    class ConfigParser {

        public: // Constructor / Destructor
            explicit        ConfigParser(const fs::path& path);
            ~               ConfigParser() = default;

        public:
            void            readConfigFromPath(const fs::path& configPath);
            void            readConfigFromJson(const json& config);

        public:
            json            getConfig() const { return m_config; }

            optpath_t       getF2bDatabaseFile() const { return m_config["f2b_db_file"]; }

            optstringvector getQueries() const;

            string          getQuery(const string& query) const;

            optstringvector getJailDescriptions() const;

            optstring_t     getJailDescription(const string& jail) const;

            time_t          getBanIgnoreThreshold() const;

        private:
            json            m_config;
    };

} // namespace f2abuseipdb::config

#endif // FAIL2ABUSEIPDB_INCLUDE_CONFIG_PARSER_HPP