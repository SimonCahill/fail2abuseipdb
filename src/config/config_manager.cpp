/**
 * @file config_manager.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of a configuration manager class for C++ using nlohmann::json in the backend.
 * @version 1.0
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

//////////////////////////////
//      SYSTEM INCLUDES     //
//////////////////////////////
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// nlohmann/json
#include <nlohmann/json.hpp>

// spdlog
#include <spdlog/spdlog.h>

#if __cpp_lib_format >= 201907L
    #include <format>
#else
    #include <spdlog/fmt/fmt.h>
#endif

//////////////////////////////
//       LOCAL INCLUDES     //
//////////////////////////////
#include "config/config_manager.hpp"

namespace f2abuseipdb::config {

    #if __cpp_lib_format >= 201907L
        using std::format;
    #else
        using fmt::format;
    #endif

    using std::error_code;
    using std::exception;
    using std::generic_category;
    using std::ifstream;
    using std::shared_ptr;
    using std::string;
    using std::system_error;
    using std::vector;

    /**
     * @brief The regular expression pattern matching the config nomenclature.
     */
    const string ConfigManager::CONFIG_NOMENCLATURE = R"(^([A-z]{1}[A-z0-9_]{1,})(\.([A-z]{1}[A-z0-9_]{1,})+)+$)";

    /**
     * @brief Gets a value indicating whether or not this config context contains the desired context.
     * 
     * This function supports use of dot-notation for searching nested configs!
     * 
     * @param configPath The dot-notation config path. E.g.: `logging.log_level`
     * 
     * @return true If the config exists
     * @return false Otherwise.
     */
    bool ConfigManager::hasConfig(const string& configPath) const { return hasConfig(configPath, m_configObject); }

    /**
     * @brief Gets a value indicating whether or not a given JSON container contains a config.
     * 
     * This function supports dot-notation.
     * 
     * @param configPath The dot-notation config path. E.g.: `logging.log_level`
     * @param container The JSON container to search within.
     * 
     * @return true If the config exists.
     * @return false Otherwise.
     */
    bool ConfigManager::hasConfig(const string& configPath, const json& container) {
        string searchString{};
        string nextSearch{};

        if (extensions::regexMatch(configPath, CONFIG_NOMENCLATURE)) {
            const auto firstPeriod = configPath.find_first_of('.');
            searchString = configPath.substr(0, firstPeriod);
            nextSearch = configPath.substr(firstPeriod + 1);
        } else {
            searchString = configPath;
        }

        if (!container.contains(searchString)) { return false; }
        if (nextSearch.empty()) { return true; }
        if (!container[searchString].is_object()) { return false; }

        return hasConfig(nextSearch, container[searchString]);
    }

    bool ConfigManager::isConfigArray(const string& key) const {vector<json> _; return isConfigArray(key, _); }
    /**
     * @brief Gets a value indicating whether or not a given config value is an array.
     * 
     * @param key The (dot-notation) config key to search for.
     * 
     * @return true If the value is an array.
     * @return false Otherwise.
     */
    bool ConfigManager::isConfigArray(const string& key, vector<json>& outval) const {
        try {
            const auto& cfg = getConfig<json>(key);
            if (cfg.is_array()) {
                std::transform(cfg.begin(), cfg.end(), std::back_inserter(outval), [&](const auto& x) { return x; });
                return true;
            }
        } catch (...) { }
        
        return false;
    }

    bool ConfigManager::isConfigBool(const string& key) const { bool _; return isConfigBool(key, _); }
    /**
     * @brief Gets a value indicating whether or not a given config value is a boolean value.
     * 
     * @param key The (dot-notation) config key to search for.
     * 
     * @return true If the value is a boolean value.
     * @return false Otherwise.
     */
    bool ConfigManager::isConfigBool(const string& key, bool& outval) const {
        try {
            const auto& cfg = getConfig<json>(key);
            if (cfg.is_boolean()) {
                outval = cfg;
                return true;
            }
        } catch (...) { }
        
        return false;
    }
    
    /**
     * @brief Gets a value indicating whether or not a given config (value) is set to null.
     * 
     * @param key The dot-notation path to the configuration to be checked.
     * 
     * @return true If the value of the config is `null`.
     * @return false Otherwise.
     */
    bool ConfigManager::isConfigNull(const string& key) const {
        try { return getConfig<json>(key).is_null(); }
        catch (...) { return false; }
    }

    bool ConfigManager::isConfigNumber(const string& key) const { uint64_t _; return isConfigNumber(key, _); }
    /**
     * @brief Gets a value indicating whether or not a given config value is a number.
     * 
     * @param key The (dot-notation) config key to search for.
     * 
     * @return true If the value is a number.
     * @return false Otherwise.
     */
    bool ConfigManager::isConfigNumber(const string& key, uint64_t& outval) const {
        try {
            const auto& cfg = getConfig<json>(key);
            if (cfg.is_number()) {
                outval = cfg;
                return true;
            }
        } catch (...) { }
        
        return false;
    }

    bool ConfigManager::isConfigObject(const string& key) const { json _; return isConfigObject(key, _); }
    /**
     * @brief Gets a value indicating whether or not a given config value is an object.
     * 
     * @param key The (dot-notation) config key to search for.
     * 
     * @return true If the value is an object.
     * @return false Otherwise.
     */
    bool ConfigManager::isConfigObject(const string& key, json& outval) const {
        try {
            const auto& cfg = getConfig<json>(key);
            if (cfg.is_object()) {
                outval = cfg;
                return true;
            }
        } catch (...) { }
        
        return false;
    }

    bool ConfigManager::isConfigString(const string& key) const { string _; return isConfigString(key, _); }

    /**
     * @brief Gets a value indicating whether or not a given config value is a string.
     * 
     * @param key The (dot-notation) config key to search for.
     * 
     * @return true If the value is a string.
     * @return false Otherwise.
     */
    bool ConfigManager::isConfigString(const string& key, string& outval) const {
        try {
            const auto& cfg = getConfig<json>(key);
            if (cfg.is_string()) {
                outval = cfg;
                return true;
            }
        } catch (...) { }
        
        return false;
    }

    /**
     * @brief Loads the application's configs from disk.
     * 
     * @remarks This method will either use @link CONFIGMAN_DEFAULT_CONFIG_PATH @endlink or @link _overriddenConfigPath @endlink
     * depending on whether the config path was overridden or not.
     * 
     * @throw exception If something went wrong.
     */
    void ConfigManager::loadConfiguration() {
        ifstream inStream(m_configPath);

        try { m_configObject = json::parse(inStream, nullptr, true, true); }
        catch (const json::exception& ex) { spdlog::critical("Failed to parse config! Error: {0:s}", ex.what()); }
    }
    
    /**
     * @brief Construct a new ConfigManager::ConfigManager object
     * 
     * @param cfgPath The path to the config file.
     */
    ConfigManager::ConfigManager(const fs::path& cfgPath): m_configPath(cfgPath) { setDefaultConfigContents(resources::defaults::DEFAULT_CONFIG.data()); }

}