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
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// json
#include <nlohmann/json.hpp>

// fmt
#include <spdlog/spdlog.h>

// libc
#include <getopt.h>

// sqlite3
#include <sqlite3.h>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "ban_selection.hpp"
#include "config/config_manager.hpp"
#include "f2b_jail.hpp"
#include "output_formatters/json_formatter.hpp"
#include "output_formatters/markdown_formatter.hpp"
#include "output_type.hpp"
#include "resources.hpp"
#include "string_splitter.hpp"

namespace f2abuseipdb {

    namespace fs = std::filesystem;

    using std::cout;
    using std::endl;
    using std::optional;
    using std::shared_ptr;
    using std::vector;

    /**
     * @brief Anonymous namespace to keep globals in this file only.
     */
    namespace {
        using optpath_t = optional<fs::path>;
        using optstring_t = optional<string>;
        using cfgparser_t = shared_ptr<config::ConfigManager>;

        BanSelection    g_banSelectionType{BanSelection::ACTIVE_BANS};

        cfgparser_t     g_appConfig{nullptr};

        fs::path        g_configFile{resources::CONFIG_FILE_PATH};

        optpath_t       g_dbFile{std::nullopt};
        optpath_t       g_outputFile{std::nullopt};
        OutputType      g_outputType{OutputType::ABUSEIPDB_CSV};
        optstring_t     g_selectedJails{std::nullopt};

        sqlite3*        g_sqlite{nullptr};

        vector<Jail>    g_jails{};

    }

    fs::path    getDbFile();
    int32_t     closeSqlite();
    int32_t     openSqlite();
    int32_t     parseArgs(int32_t argc, char** argv);
    void        loadAllBansFromJails();
    void        loadCurrentBansFromJails();
    void        loadJails();
    void        loadPreviousBansFromJails();
    void        printHelp();
    void        printVersion();

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

        g_appConfig = std::make_shared<config::ConfigManager>(g_configFile);
        g_appConfig->loadConfiguration();

        if (const auto retCode = openSqlite(); retCode != SQLITE_OK) {
            spdlog::error("Failed to open Fail2Ban DB file. Error: SQLite: {0:s}", sqlite3_errstr(retCode));
            spdlog::warn("Do you have read permissions for {0:s}?", getDbFile().string());
            return 1;
        }

        loadJails();

        switch (g_banSelectionType) {
            case BanSelection::ALL_BANS:
                loadAllBansFromJails();
                break;
            case BanSelection::PREVIOUS_BANS:
                loadPreviousBansFromJails();
                break;
            default:
                loadCurrentBansFromJails();
                break;
        }

        cout << output::MarkdownFormatter(g_appConfig).formatData(g_jails) << endl;

        if (const auto retCode = closeSqlite(); retCode != SQLITE_OK) {
            spdlog::error("Failed to close Fail2Ban DB file. Error: SQLite: {0:s}", sqlite3_errstr(retCode));
            return 1;
        }

        return 0;
    }

    fs::path getDbFile() {
        return g_dbFile.value_or(g_appConfig->getF2bDatabaseFile());
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
            getDbFile().c_str(),
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
                case 'j':
                    // output JSON data
                    g_outputType = OutputType::JSON;
                    break;
                case 'J':
                    // select entries from these jails ONLY
                    g_selectedJails = optarg;
                    break;
                case 'm':
                    // output markdown document
                    g_outputType = OutputType::MARKDOWN;
                    break;
                case 'a':
                    // get all ban entries from the database
                    g_banSelectionType = BanSelection::ALL_BANS;
                    break;
                case 'p':
                    // get only previous bans from the database
                    g_banSelectionType = BanSelection::PREVIOUS_BANS;
                    break;
                default: break;
            }
        }

        return 0;
    }

    /**
     * @brief Loads all known bans for each jail.
     */
    void    loadAllBansFromJails() {
        for (auto& jail : g_jails) {
            jail.loadAllBanned(g_sqlite, g_appConfig);
        }
    }

    /**
     * @brief Loads all current bans from the jails being inspected.
     */
    void    loadCurrentBansFromJails() {
        for (auto& jail : g_jails) {
            jail.loadActiveBanned(g_sqlite, g_appConfig);
        }
    }

    /**
     * @brief Loads all selected jails from the database.
     */
    void    loadJails() {
        if (!g_selectedJails.has_value()) {
            g_jails = Jail::loadJailsFromDb(g_sqlite, g_appConfig);
            return;
        }

        for (const auto& jail : StringSplit(g_selectedJails.value(), ",")) {
            const auto optJail = Jail::loadJailFromDb(jail.data(), g_sqlite, g_appConfig);
            if (optJail.has_value()) {
                g_jails.push_back(optJail.value());
            }
        }
    }

    /**
     * @brief Loads all previously banned (inactive) hosts for each jail.
     */
    void    loadPreviousBansFromJails() {
        for (auto& jail : g_jails) {
            jail.loadFormerBanned(g_sqlite, g_appConfig);
        }
    }

    /**
     * @brief Prints the application's help text.
     */
    void    printHelp() { cout << resources::APP_HELP_TEXT << endl; }

    /**
     * @brief Prints the application's version.
     */
    void    printVersion() { cout << resources::APP_VER_TEXT << endl; }

}

int main(int argc, char** argv) { return f2abuseipdb::main(argc, argv); }