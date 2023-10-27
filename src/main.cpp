/**
 * @file main.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of the main entry point of fail2abuseipdb.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill
 */

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// json
#include <nlohmann/json.hpp>

// fmt
#include <fmt/format.h>

// libc
#include <getopt.h>

// sqlite3
#include <sqlite3.h>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "config_parser.hpp"
#include "resources.hpp"

namespace f2abuseipdb {

    namespace fs = std::filesystem;

    using std::optional;
    using std::shared_ptr;

    /**
     * @brief Anonymous namespace to keep globals in this file only.
     */
    namespace {
        using optpath_t = optional<fs::path>;
        using cfgparser_t = shared_ptr<config::ConfigParser>;

        cfgparser_t g_appConfig{nullptr};

        fs::path    g_configFile{resources::CONFIG_FILE_PATH};
        optpath_t   g_dbFile{std::nullopt};
        optpath_t   g_outputFile{std::nullopt};

        sqlite3*    g_sqlite{nullptr};

    }

    int32_t openSqlite();
    int32_t parseArgs(int32_t argc, char** argv);
    void    printHelp();
    void    printVersion();

    /**
     * @brief The program's main entry point.
     * 
     * @param argc The amount of arguments passed.
     * @param argv The actual arguments.
     * 
     * @return int The application's exit code.
     */
    int main(int argc, char** argv) {
        if (const auto retCode = parseArgs(argc, argv); retCode > 0) {
            return retCode - 1;
        }

        g_appConfig = std::make_shared<config::ConfigParser>(g_configFile);

        return 0;
    }

    /**
     * @brief Closes the SQLite database connection.
     * 
     * @return int32_t The return code. If this is non-zero, then an error occurred.
     */
    int32_t closeSqlite() {
        return sqlite3_close_v2(g_sqlite);
    }

    /**
     * @brief Attempts to open Fail2Ban's SQLite database file.
     * 
     * @return int32_t The return code. If this is non-zero, then an error occured. Stop the application.
     */
    int32_t openSqlite() {
        return sqlite3_open_v2(
            g_dbFile.value_or(g_appConfig->getF2bDatabaseFile().value_or(fs::path{resources::DEFAULT_DB_FILE_PATH})).c_str(),
            std::addressof(g_sqlite),
            SQLITE_OPEN_READONLY | SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX,
            nullptr
        );
    }

    /**
     * @brief Parses incoming application arguments from the command line interface.
     * 
     * @param argc The amount of args passed to the application.
     * @param argv The arguments passed.
     * 
     * @return int32_t 0 if the application should continue execution; gt 0 if the application should exit with returnCode - 1.
     */
    int32_t parseArgs(int32_t argc, char** argv) {
        int32_t optChar = 0;

        while ((optChar = getopt_long(argc, argv, resources::APP_ARGS_SHORT, resources::APP_ARGS_LONG, nullptr)) != -1) {
            switch (optChar) {
                case 'h':
                    printHelp();
                    return 1;
                case 'v':
                    printVersion();
                    return 1;
                case 'c': // override config file
                    g_configFile = fs::path{optarg};
                    break;
                case 'f': // override database file
                    g_dbFile = fs::path{optarg};
                    break;
                case 'o': // output to file instead of stdout
                    g_outputFile = fs::path{optarg};
                    break;
                default: break;
            }
        }

        return 0;
    }

    /**
     * @brief Prints the application's help text.
     */
    void    printHelp() { /*TODO*/ }

    /**
     * @brief Prints the application's version.
     */
    void    printVersion() { /*TODO*/ }

}

int main(int argc, char** argv) { return f2abuseipdb::main(argc, argv); }