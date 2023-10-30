/**
 * @file config_manager.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the declaration of a configuration manager class for C++ using nlohmann::json in the backend.
 * @version 1.0
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_CONFIG_CONFIG_MANAGER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_CONFIG_CONFIG_MANAGER_HPP

//////////////////////////
//    SYSTEM INCLUDES   //
//////////////////////////
// stl
#include <array>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

// json
#include <nlohmann/json.hpp>

// libc
#include <unistd.h>

//////////////////////////
//    LOCAL INCLUDES    //
//////////////////////////
#include "extensions/extensions.hpp"
#include "resources.hpp"

namespace f2abuseipdb::config {

    using nlohmann::json;
    using error_handler_t = json::error_handler_t;

    using std::array;
    using std::function;
    using std::shared_ptr;
    using std::string;
    using std::vector;

    namespace fs = std::filesystem;

    /**
     * @brief Configuration manager for my applications.
     * 
     * @remarks
     * This class can use dot-notation to access nested values within the JSON object(s).
     * 
     * Dot-notation is the same as accessing members on a class instance.
     * my_config.my_value is a good dot-noted accessor.
     */
    class ConfigManager {
        const static string CONFIG_NOMENCLATURE; //!< Describes the nomenclature used to get and set configs

        public: // +++ Destructor +++
            ~       ConfigManager() = default;

        public: // +++ Getter and Setter +++
        #pragma region "Getters and Setters"
            error_handler_t getErrorHandler() const { return m_jsonErrorHandler; }

            string          getConfigPath() const { return m_configPath; } //!< Sets the path to the config file.
            string          getDefaultConfig() const { return m_defaultConfig; } //!< Gets the default config contents.

            void            setConfigPath(const fs::path& value) { m_configPath = value; } //!< Chaining method; sets the config path.

            void            setErrorHandler(const error_handler_t val) { m_jsonErrorHandler = val; }

            void            getNextSearchString(const string& cfgPath, string& searchString, string& nextSearch) const {
                if (extensions::regexMatch(cfgPath, CONFIG_NOMENCLATURE)) {
                    const auto firstPeriod = cfgPath.find_first_of('.');
                    searchString = cfgPath.substr(0, firstPeriod);
                    nextSearch = cfgPath.substr(firstPeriod + 1);
                } else {
                    searchString = cfgPath;
                }
            }

            template<class T>
            T               tryGetConfig(const string& configPath, const T defaultVal) const { return tryGetConfig<T>(configPath, m_configObject, defaultVal); }

            template<class T>
            T               getConfig(const string& configPath) const { return getConfig<T>(configPath, m_configObject); }

            template<class T>
            T               getConfig(const string& configPath, const json& container) const {
                string searchString{};
                string nextSearch{};

                getNextSearchString(configPath, searchString, nextSearch);

                if (!hasConfig(searchString, container)) { throw "Config " + searchString + " not found."; /* TODO: Replace with object */ }

                if (!container.contains(searchString)) { throw "Config " + searchString + " not found."; /* TODO: Replace with object */ }
                if (nextSearch.empty()) { return container[searchString].get<T>(); }
                if (!container[searchString].is_object()) { throw "Config " + searchString + " is not an object."; /* TODO: Replace with object */ }

                return getConfig<T>(nextSearch, container[searchString]);
            }

            template<class T>
            T               tryGetConfig(const string& configPath, const json& container, const T defaultVal) const {
                try {
                    return getConfig<T>(configPath, container);
                } catch (...) { 
                    try { getConfig<T>(configPath, m_defaultConfig); }
                    catch (...) { }
                }
                return defaultVal;
            }
        #pragma endregion

        public: // +++ Management +++
        #pragma region "Management"
            static  bool    hasConfig(const string& configPath, const json& container); //!< Gets a value indicating whether or not a config exists in a given JSON container
            bool    hasConfig(const string& key) const; //!< Gets a value indicating whether or not a config exists in the current context.

            bool    isConfigArray(const string& key) const; //!< Gets a value indicating whether or not a given config is an array
            bool    isConfigArray(const string& key, vector<json>& outval) const; //!< Gets a value indicating whether or not a given config is an array
            bool    isConfigBool(const string& key) const; //!< Gets a value indicating whether or not a given config is a boolean
            bool    isConfigBool(const string& key, bool& outval) const; //!< Gets a value indicating whether or not a given config is a boolean
            bool    isConfigNull(const string& key) const; //!< Gets a value indicating whether or not a given config is null
            bool    isConfigNumber(const string& key) const; //!< Gets a value indicating whether or not a given config is a number
            bool    isConfigNumber(const string& key, uint64_t& outval) const; //!< Gets a value indicating whether or not a given config is a number
            bool    isConfigObject(const string& key) const; //!< Gets a value indicating whether or not a given config is an object
            bool    isConfigObject(const string& key, json& outval) const; //!< Gets a value indicating whether or not a given config is an object
            bool    isConfigString(const string& key) const; //!< Gets a value indicating whether or not a given config is a string
            bool    isConfigString(const string& key, string& outval) const; //!< Gets a value indicating whether or not a given config is a string

            bool    configFileExists() const { return fs::exists(m_configPath) && fs::is_regular_file(m_configPath); } //!< Indicates whether or not the config file exists

            void    loadConfiguration(); //!< Loads the configuration from disk.
        #pragma endregion
        
        public: // +++ Config Manipulation +++
            void    setDefaultConfigContents(const string& configContents) { m_defaultConfig = json::parse(configContents, nullptr, true, true); }

        public: // +++ Helpers +++
            fs::path        getF2bDatabaseFile() const {
                return getDbFile();
            }

            time_t          getBanIgnoreThreshold() const {
                return tryGetConfig<time_t>("ignore_bans_older_than", time(nullptr)) * 3600;
            }

            string          getDbFile() const {
                return tryGetConfig<string>("f2b_db_file", tryGetConfig<string>("f2b_db_file", m_defaultConfig, {}));
            }

            string          getHostName() const {
                array<char, 1024> hostBuf{};
                hostBuf.fill(0);
                gethostname(hostBuf.data(), hostBuf.size());

                return string(hostBuf.begin(), hostBuf.end());
            }

            string          getJailDescription(const string& jailName) const {
                return tryGetConfig<string>("jail_descriptions." + jailName, {});
            }

            string          getQuery(const string& queryName) const {
                return tryGetConfig<string>("queries." + queryName, resources::defaults::DEFAULT_QUERIES[queryName]);
            }

        public: // +++ Constructor +++
            explicit        ConfigManager(const fs::path& cfgPath); //!< Object constructor.

        public: // +++ Operators +++
            json            operator[](const string& config) const { return getConfig<json>(config); } //!< Operator supporting direct access. Supports dot-notation

        private:
            error_handler_t m_jsonErrorHandler{error_handler_t::strict}; //!< The error handler to use when dumping JSON

            mutable json    m_configObject; //!< The config object.
            mutable json    m_defaultConfig; //!< The default configuration string

            string          m_configPath; //!< The path to the config file.
    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_CONFIG_CONFIG_MANAGER_HPP