/**
 * @file f2b_jail.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the declaration of the Jail class, which represents a single jail in fail2ban.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems and Pascal Lüttmann
 */

 #include "config_parser.hpp"
 #include "resources.hpp"

 #include <iostream>
 #include <fstream>
 #include <spdlog/spdlog.h>


namespace f2abuseipdb::config {
    using std::exception;
    using std::ifstream;
    using std::map;
    using std::nullopt;
    using std::string_view;

    inline static constexpr string_view QUERIES_KEYWORD{"queries"}; //!< The keyword for queries in the config
    inline static constexpr string_view DESCRIPTIONS_KEYWORD{"jail_descriptions"}; //!< The keyword for jail descriptions in the config

    /**
     * @brief Construct a new Config Parser:: Config Parser object
     * 
     * @param configPath the path to the config
     */
    ConfigParser::ConfigParser(const fs::path& configPath) {
        ConfigParser::readConfigFromPath(configPath);
    }

    /**
     * @brief Assigns the content of configPath to member variable m_config
     *
     * @param configPath the path to the config
     */
    void ConfigParser::readConfigFromPath(const fs::path& configPath) {
        if (configPath.empty()) { spdlog::error("Config path is empty. Please provide a non empty path"); return; }
        if (auto fileStream = ifstream(configPath)) {
            try {
                m_config = json::parse(fileStream, nullptr, true, true);
            } catch (const json::parse_error& ex) {
                spdlog::error("Cannot parse given config. \n Details: {}", ex.byte);
            }
        } else {
            spdlog::error("Cannot open config at path {}", configPath.string());
        }
    }

    /**
     * @brief Assigns the given json config to the member variable m_config
     * 
     * @param jsonConfig a hopefully valid json config
     */
    void ConfigParser::readConfigFromJson(const json& jsonConfig) {
        m_config = jsonConfig;
    }

    /**
     * @brief Returns a vector of strings that represent all available/configured queries
     *
     * @returns vector<string> Read the brief
     */
    optstringvector ConfigParser::getQueries() const {
        vector<string> tempVector{};

        if (m_config.contains(QUERIES_KEYWORD) && m_config[QUERIES_KEYWORD].is_object()) {
            for (const auto& elem : m_config[QUERIES_KEYWORD].items()) {
                if (elem.value().is_string()) {
                    tempVector.push_back(elem.value());
                }
            }
        }

        return tempVector.empty() ? nullopt : optstringvector{tempVector};
    }

    /**
     * @brief Returns the query that is stored in a map with the passed query keyword
     *
     * @returns string Read the brief
     */
    string ConfigParser::getQuery(const string& query) const {
        if (m_config.contains(QUERIES_KEYWORD) && m_config[QUERIES_KEYWORD].is_object()) {
            for (const auto& elem : m_config[QUERIES_KEYWORD].items()) {
                if (elem.value().is_string() && query == elem.value()) {
                    return elem.value();
                }
            }
        }

        auto defaultMap = f2abuseipdb::resources::defaults::DEFAULT_QUERIES;
        return defaultMap.find(query) == defaultMap.end() ? string{} : string{defaultMap[query]};
    }

    /**
     * @brief Return the jail description that is stored in a map with the passed description keyword
     *
     * @returns optstring_t Read the brief
     */
    optstring_t ConfigParser::getJailDescription(const string& jail) const {
        if (m_config.contains(DESCRIPTIONS_KEYWORD) && m_config[DESCRIPTIONS_KEYWORD].is_object()) {
            for (const auto& elem : m_config[DESCRIPTIONS_KEYWORD].items()) {
                if (elem.value().is_string() && jail == elem.value()) {
                    return elem.value();
                }
            }
        }

        return {};
    }

    /**
     * @brief Returns a vector of strings that represent all available/configured descriptions 
     *
     * @returns optstring_t Read the brief
     */
    optstringvector ConfigParser::getJailDescriptions() const{
        vector<string> tempVector{};

        if (m_config.contains(DESCRIPTIONS_KEYWORD) && m_config[DESCRIPTIONS_KEYWORD].is_object()) {
            for (const auto& elem : m_config[DESCRIPTIONS_KEYWORD].items()) {
                if (elem.value().is_string()) {
                    tempVector.push_back(elem.value());
                }
            }
        }

        return tempVector.empty() ? nullopt : optstringvector{tempVector};
    }

} // namespace f2abuseipdb::config