/**
 * @file extensions.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains useful extension methods that aren't provided by C++ out of the box.
 * @version 0.1
 * @date 2022-10-15
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 * 
 * @remarks This file was cloned from https://raw.githubusercontent.com/SimonCahill/EndlesshReportGenerator/master/include/extensions.hpp
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_EXTENSIONS_EXTENSIONS_HPP
#define FAIL2ABUSEIPDB_INCLUDE_EXTENSIONS_EXTENSIONS_HPP

// stl
#include <chrono>
#include <cmath>
#include <functional>
#include <regex>
#include <string>
#include <vector>

// date
#include <date/date.h>

// fmt
#include <fmt/format.h>

namespace f2abuseipdb::extensions {

    using std::chrono::seconds;
    using std::chrono::system_clock;
    using std::cmatch;
    using std::function;
    using std::regex;
    using std::string;
    using std::vector;

    /**
     * @brief Splits a given string by the passed delimiters in to a vector.
     *
     * @param str string& The string to split.
     * @param delimiters string& A string containing the delimiters to split by (chars).
     * @param tokens vector<string>& A vector that will contain the elements.
     * 
     * @return true If tokens were found.
     * @return false Otherwise.
     */
    inline bool splitString(const string& str, const string& delimiters, vector<string> &tokens) {
        // Skip delimiters at beginning.
        string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // Find first "non-delimiter".
        string::size_type pos = str.find_first_of(delimiters, lastPos);

        while (string::npos != pos || string::npos != lastPos) {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            // Skip delimiters.
            lastPos = string::npos == pos ? string::npos : pos + 1;
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }

        return tokens.size() > 0;
    }

    /**
     * @brief Matches a string against a regular expression.
     * 
     * @param pattern The expression to compare against
     * @param needle The string to compare
     * 
     * @return true If the string matches the expression
     * @return false Otherwise
     */
    inline bool regexMatch(const string& pattern, const string& needle) {
        regex regPattern{pattern};

        return std::regex_match(needle, regPattern);
    }

    /**
     * @brief Rounds a given double to the desired amount of decimal places.
     * 
     * @param x The double to round
     * @param decimalPlaces The decimal places the double should be rounded to
     * 
     * @return double The resulting double.
     */
    inline double roundNumber(const double x, const uint32_t decimalPlaces) {
        const double multiplificationFaktor = std::pow(10.0, decimalPlaces);
        return std::ceil(x * multiplificationFaktor) / multiplificationFaktor;
    }

    /**
     * @brief Gets the current timestamp as an ISO timestamp, accurate to the nearest second.
     * 
     * @return string A string containing the current timestamp in ISO format.
     */
    inline string getCurrentIsoTimestamp() {
        auto timeNow = system_clock::now();
        return date::format("%FT%TZ", date::floor<seconds>(timeNow));
    }

    /**
     * @brief Converts a number of bytes to a human-readable format, such as KiB, MiB, GiB, ...
     * 
     * @param bytes The bytes to convert.
     * 
     * @return string The human-readable string.
     */
    inline string getHumanReadableBytes(const size_t bytes) {
        const static size_t ONE_KIB = 1024;
        const static size_t ONE_MIB = ONE_KIB * ONE_KIB;
        const static size_t ONE_GIB = ONE_MIB * ONE_MIB;
        
        if (bytes > ONE_GIB) { return fmt::format("{0:.2f}GiB", static_cast<double>(bytes / ONE_GIB)); }
        if (bytes > ONE_MIB) { return fmt::format("{0:.2f}MiB", static_cast<double>(bytes / ONE_MIB)); }
        if (bytes > ONE_KIB) { return fmt::format("{0:.2f}KiB", static_cast<double>(bytes / ONE_KIB)); }

        // We're likely less than a KiB
        return fmt::format("{0:d}B", bytes);
    }

    /**
     * @brief Converts n seconds into a human-readable form. E.g. 300s would become 5 minutes
     * 
     * @param seconds The amount of seconds to convert.
     * 
     * @return string The human-readable string
     */
    inline string getHumanReadableTime(const double seconds) {
        if (seconds < 60) { return fmt::format("{0:.0f}s", floor(seconds)); }

        string result{};
        time_t secondsAsTime = static_cast<time_t>(seconds);
        struct tm timeStruct = {0};
        gmtime_r(&secondsAsTime, &timeStruct);

        if (timeStruct.tm_yday > 0) {
            result += fmt::format("{0:d}d ", timeStruct.tm_yday);
        } if (timeStruct.tm_hour > 0) {
            result += fmt::format("{0:d}h ", timeStruct.tm_hour);
        } if (timeStruct.tm_min > 0) {
            result += fmt::format("{0:d}m ", timeStruct.tm_min);
        } if (timeStruct.tm_sec > 0) {
            result += fmt::format("{0:d}s", timeStruct.tm_sec);
        }

        return result;
    }

    /**
     * @brief Gets a string containing whitespace to centre text
     * 
     * @param totalWidth The total width of the area where the string should be centred
     * @param strLength The length of the string to be centre-printed
     * 
     * @return string The spacer string
     */
    inline string getSpacerString(const uint32_t totalWidth, const uint32_t strLength) {
        return string(totalWidth / 2 - strLength / 2, ' ');
    }

    /**
     * @brief Trims the beginning of a given string.
     *
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trimStart(string nonTrimmed, const string& trimChar) {
        function<bool(char)> shouldTrimChar = [=](char c) -> bool { return trimChar.size() == 0 ? isspace(c) : trimChar.find(c) != string::npos; };

        nonTrimmed.erase(nonTrimmed.begin(), find_if(nonTrimmed.begin(), nonTrimmed.end(), std::not_fn(shouldTrimChar)));

        return nonTrimmed;
    }

    /**
     * @brief Trims the end of a given string.
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trimEnd(string nonTrimmed, const string& trimChar) {
        function<bool(char)> shouldTrimChar = [=](char c) -> bool { return trimChar.size() == 0 ? isspace(c) : trimChar.find(c) != string::npos; };
        nonTrimmed.erase(find_if(nonTrimmed.rbegin(), nonTrimmed.rend(), std::not_fn(shouldTrimChar)).base(), nonTrimmed.end());

        return nonTrimmed;
    }

    /**
     * @brief Trims both the beginning and the end of a given string.
     *
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trim(const string& nonTrimmed, const string& trimChar) { return trimStart(trimEnd(nonTrimmed, trimChar), trimChar); }


}

#endif // FAIL2ABUSEIPDB_INCLUDE_EXTENSIONS_EXTENSIONS_HPP