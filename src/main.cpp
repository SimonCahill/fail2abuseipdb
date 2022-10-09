/**
 * @file main.cpp
 * @author Simon Cahill (simon@simonc.eu)
 * @brief Contains the main application logic.
 * @version 0.1
 * @date 2022-10-07
 * 
 * @copyright Copyright (c) 2022 Simon Cahill
 */

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <getopt.h>

namespace fs = std::filesystem;

using fmt::format;

using nlohmann::json;

using defcat_t = std::vector<int32_t>;
using lookup_t = std::map<std::string, int32_t>;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::ifstream;
using std::istreambuf_iterator;
using std::map;
using std::string;
using std::string_view;
using std::transform;
using std::vector;

// prototypes
static constexpr string_view getShortArgs();
static const option* getOptions();

using svec_t = vector<string>;

static bool     alreadyReported(const string&); //!< Indicates whether or not an IP has already been reported
static bool     outputCsv(const json&); //!< Dumps the CSV-encoded data to the terminal
static bool     parseArgs(int32_t argc, char** argv); //!< Parses the application arguments
static bool     parseFail2BanFromFile(); //!< Parses fail2ban output from a given file
static string   getCategoriesForJail(); //!< Gets the categories for the currently selected jail
static svec_t   getLinesFromJson(const json&, const string&); //!< Gets a CSV-formatted line from a JSON object
static void     printHelpText(const string&); //!< Prints the help text to the terminal
static void     transformFail2BanInput(string&); //!< Transforms the fail2ban output to valid JSON

// globals
static bool     g_readFromFile = false; //!< Whether or not to read f2b input from a file
static bool     g_readFromStdIn = false; //!< Whether or not to read f2b input from stdin

/**
 * @brief Category lookup table
 */
static map<string, int32_t> 
                g_categoryLookup = {
    { "sshd",                   22 },
    { "apache-auth",            21 },
    { "apache-batbots",         19 },
    { "apache-overflows",       21 },
    { "apache-nohome",          21 },
    { "apache-fakegooglebot",   19 },
    { "apache-modsecurity",     21 },
    { "apache-shellshock",      21 },
    { "php-url-fopen",          21 },
    { "roundcube-auth",         21 },
    { "postfix",                -1 },
    { "sendmail-auth",          20 },
    { "sendmail-reject",        -1 },
    { "dovecot",                -1 },
    { "mysqld-auth",            21 },
    { "pam-generic",            20 },
    { "postfix-flood-attack",   04 }
};

/**
 * @brief Default categories for each entry
 */
static defcat_t g_defaultCategories = {
    15, 18
};

static string   g_fileToRead = "fail2ban.json"; //!< The file to read input from
static string   g_jailName = ""; //!< The name of the jail (if specific jail exported from f2b)
static string   g_reportComment = "IP banned by fail2ban; banned in jail {0}. Report generated by fail2abuseipdb.";

// main
int main(int32_t argc, char** argv) {
    if (!parseArgs(argc, argv)) { return 0; }

    if (g_readFromFile) {
        parseFail2BanFromFile();
    } else if (g_readFromStdIn) {

    } else {

    }

    return 0;
}

bool alreadyReported(const string& ip) {
    return false;
}

/**
 * @brief Parses the f2b input from the file pointed to by @see g_fileToRead
 * 
 * @return true If the file was read correctly.
 * @return false Otherwise.
 */
bool parseFail2BanFromFile() {
    bool rval = true;

    string fileContents{};
    json entries;

    if (!fs::exists(g_fileToRead) || !fs::is_regular_file(g_fileToRead)) {
        cerr << "File " << g_fileToRead << " cannot be read! Aborting..." << endl;
        rval = false;
        goto Exit;
    }
    {
        ifstream fStream(g_fileToRead, ifstream::openmode::_S_in);
        if (!fStream.good()) {
            cerr << "Failed to open file " << g_fileToRead << ". Aborting..." << endl;
            rval = false;
            goto Exit;
        }
        fStream.unsetf(std::ios_base::skipws);
        vector<char> buffer((istreambuf_iterator<char>(fStream)), (istreambuf_iterator<char>()));
        fileContents = string(buffer.begin(), buffer.end());
    }

    transformFail2BanInput(fileContents);
    try {
        entries = json::parse(fileContents);
    } catch (const exception& ex) {
        cerr << "Failed to parse JSON! Invalid format?" << endl
             << "Error description: " << ex.what() << endl;
        rval = false;
        goto Exit;
    }

    rval = outputCsv(entries);

    Exit:
    return rval;
}

/**
 * @brief Outputs the CSV-encoded data to the terminal.
 * 
 * @param entries A @see nlohmann::json object containing the f2b entries
 * 
 * @return true If CSV could be generated and printed.
 * @return false Otherwise
 */
bool outputCsv(const json& entries) {
    bool rval = true;

    const time_t timeNow = time(nullptr);
    struct tm tStruct{0};
    localtime_r(&timeNow, &tStruct);
    string timeString(256, 0);
    strftime(&timeString[0], timeString.size(), "%F %T%z", &tStruct);
    timeString.shrink_to_fit();

    vector<string> csvLines{
        "IP,Categories,ReportDate,Comment"
    };

    // I'm expecting an array of strings (complete jail output) or an array of string (specific fail output)
    if (!entries.is_array()) {
        cerr << "Invalid input. Expected array, got " << entries.type_name() << endl;
        rval = false;
        goto Exit;
    }

    {
        const auto newLines = getLinesFromJson(entries, timeString);
        csvLines.insert(csvLines.end(), newLines.begin(), newLines.end());
    }

    for (const auto& line : csvLines) {
        cout << line << endl;
    }

    Exit:
    return rval;
}

svec_t getLinesFromJson(const json& entries, const string& timeString) {
    vector<string> lines;
    string currentIp{};
    for (const auto& entry : entries) {
        // This should probably be split up,
        // but right now I can't be bothered.
        // As long as it works, I'm fine with it, I guess.
        // For now at least
        if (!entry.is_object() && entry.is_string()) {
            currentIp = entry.get<string>();

            if (alreadyReported(currentIp)) { continue; }

            // Specific jail output
            // We don't need to look anything up, just grab default cats and append to list
            lines.push_back(format(
                R"({0:s},"{1:s}",{2:s},"{3:s}")",
                currentIp,
                getCategoriesForJail(),
                timeString,
                format(g_reportComment, g_jailName.empty() ? "UNKNOWN" : g_jailName)
            ));
        } else if (entry.is_object()) {
            const auto& obj = entry.get<json::object_t>();
            g_jailName = obj.begin()->first;
            const auto newLines = getLinesFromJson(*entry.begin(), timeString);
            lines.insert(lines.end(), newLines.begin(), newLines.end());
        }
    }
    return lines;
}

string getCategoriesForJail() {
    string categories{};
    for (size_t i = 0; i < g_defaultCategories.size(); i++) {
        if (i > 0) {
            categories.push_back(',');
        }

        categories.append(std::to_string(g_defaultCategories[i]));
    }

    const auto posInMap = std::find_if(g_categoryLookup.begin(), g_categoryLookup.end(), [](const auto x) {
        return x.first == g_jailName;
    });

    if (posInMap != g_categoryLookup.end()) {
        categories.append(format(",{0:d}", posInMap->second));
    }
}

// other impl
bool parseArgs(int32_t argc, char** argv) {
    bool rVal = true;

    int32_t curIdx = 0;
    char optVal = 0;

    if (argc == 1) {
        goto UglyHelp;
    }

    while ((optVal = getopt_long(argc, argv, getShortArgs().data(), getOptions(), &curIdx)) != -1) {
        switch (optVal) {
            case 'h':
            UglyHelp:
                printHelpText(argv[0]);
                rVal = false;
                goto Exit;
            case 's':
                g_readFromStdIn = true;
                break;
            case 'f':
                g_readFromFile = true;
                if (optarg == nullptr) {
                    cerr << "Warning: will use default file " << g_fileToRead << "." << endl;
                } else {
                    g_fileToRead = optarg;
                }
                break;
            case 'v':
                cerr << "Not yet implemented. Sorry." << endl;
                break;
            case 'c':
                if (optarg == nullptr) {
                    cerr << "Error: missing required argument for comment!" << endl;
                    break;
                }
                g_reportComment = optarg;
                break;
            case 'j':
                if (optarg == nullptr) {
                    cerr << "Error: missing required argument for jail name!" << endl;
                }
                g_jailName = optarg;
                break;
        }
    }

    Exit:
    return rVal;
}

void printHelpText(const string& binName) {
    const static string RAW = R"(
        {0} v - A simple utility for converting fail2ban entries into an abuseipdb CSV format

        Usage:
            {0} # to attempt to get output directly from fail2ban (requires elevated privileges!)
            {0} --stdin # to read input from stdin

        Arguments:
            --help, -h              Prints this text and exits
            --stdin, -s             Reads input from stdin
            --file=, -f[file]       Reads input from [file] or fail2ban.json if optarg is empty
            --version, -v           Prints the version information and exits
            --comment, -c[text]     Sets the value for the comment field. Variables are available below
            --jail-name, -j[jail]   Sets the name of the jail (useful if exporting specific jails from fail2ban)

        Comment variables:
            {0}                 Jail name
            {1}                 Report time
    )";

    cout << format(RAW, binName) << endl;
}

constexpr string_view getShortArgs() { return "hsf:vc:j:"; }

const option* getOptions() {
    const static option OPTIONS[] = {
        { "help",       no_argument,        nullptr,    'h' },
        { "stdin",      no_argument,        nullptr,    's' },
        { "file",       optional_argument,  nullptr,    'f' },
        { "version",    no_argument,        nullptr,    'v' },
        { "comment",    required_argument,  nullptr,    'c' },
        { "jail-name",  required_argument,  nullptr,    'j' },
        { nullptr,      no_argument,        nullptr,     0  }
    };

    return OPTIONS;
}

void transformFail2BanInput(string& input) {
    string output{};
    transform(input.begin(), input.end(), std::back_inserter(output), [&](const char c) {
        if (c == '\'') { return '"'; }
        return c;
    });

    input = output;
}
