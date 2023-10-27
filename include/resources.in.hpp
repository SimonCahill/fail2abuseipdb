/**
 * @file resources.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains compile-time resources for the application.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems.
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_RESOURCES_HPP
#define FAIL2ABUSEIPDB_INCLUDE_RESOURCES_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <map>
#include <string>

// libc
#include <getopt.h>

namespace f2abuseipdb::resources {

    using std::map;
    using std::string_view;

    inline static constexpr string_view APP_NAME{R"(@PROJECT_NAME@)"}; //!< The name of the application

    inline static constexpr string_view APP_VERS{R"(@PROJECT_VERSION@)"}; //!< The application version

    inline static constexpr string_view CONFIG_FILE_PATH{R"(@f2abuseipdb_DEFAULT_CONF_PATH@)"}; //!< The default path to the configuration

    inline static constexpr string_view DEFAULT_DB_FILE_PATH{R"(f2abuseipdb_F2B_DB_FILE)"};

    inline static constexpr const char* APP_ARGS_SHORT = "hvc:f:o:"; //!< Short-form options for getopt

    /**
     * @brief Long-form options for the application.
     */
    inline static constexpr option APP_ARGS_LONG[] = {
        { "help",           required_argument,  nullptr,    'h' },
        { "version",        required_argument,  nullptr,    'v' },
        { "config",         required_argument,  nullptr,    'c' },
        { "db-file",        required_argument,  nullptr,    'f' },
        { "output",         required_argument,  nullptr,    'o' },
        { nullptr,          no_argument,        nullptr,     0  }
    };

    namespace defaults {

        inline static map<string_view, string_view> DEFAULT_QUERIES = {
            { "get_jails_query", R"(@f2abuseipdb_GET_JAILS@)" },
            { "count_jails_query", R"(@f2abuseipdb_COUNT_JAILS@)" },
            { "get_all_banned_ips_query", R"(@f2abuseipdb_GET_ALL_BANS@)" },
            { "get_banned_ips_per_jail_query", R"(@f2abuseipdb_GET_BANS_FOR_JAIL@)" },
            { "get_banned_ips_after_tstamp_query", R"(@f2abuseipdb_GET_BANS_AFTER_TSTAMP@)" },
            { "get_banned_ips_before_tstamp_query", R"(@f2abuseipdb_GET_BANS_BEFORE_TSTAMP@)" },
            { "get_banned_ips_between_tstamps_query", R"(@f2abuseipdb_GET_BANS_BETWEEN_TSTAMP@)" }
        };

        inline static constexpr string_view DEFAULT_CONFIG{R"(@f2abuseipdb_CFG@)"};

    }

}

#endif // FAIL2ABUSEIPDB_INCLUDE_RESOURCES_HPP