/**
 * @file string_splitter.hpp
 * @author Simon Cahill (simon@simonc.eu)
 * @brief Contains the full implementation of an iterator-based string splitter based on an answer provided by StackOverflow user Calmarius.
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022 Simon Cahill and Contributors
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_STRING_SPLITTER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_STRING_SPLITTER_HPP

#include <string>

using std::string;
using std::string_view;

/**
 * @brief Splits a string into tokens, usable in an iterator loop, based on a delimiter.
 */
struct StringSplit {

    struct Iterator {

        size_t m_tokenStart = 0;
        size_t m_tokenEnd = 0;
        string m_input;
        string_view m_inputView;
        string m_delimiter;
        bool m_done = false;

        Iterator() {
            // End iterator.
            m_done = true;
        }

        Iterator(const string& input, const string& delimiter):
        m_input(std::move(input)), m_inputView(m_input), m_delimiter(std::move(delimiter)) {
            m_tokenEnd = m_inputView.find(m_delimiter, m_tokenStart);
        }

        string_view operator*() {
            return m_inputView.substr(m_tokenStart, m_tokenEnd - m_tokenStart);
        }

        Iterator& operator++() {
            if (m_tokenEnd == string::npos) {
                m_done = true;
                return *this;
            }

            m_tokenStart = m_tokenEnd + m_delimiter.size();
            m_tokenEnd = m_inputView.find(m_delimiter, m_tokenStart);
            return *this;
        }

        bool operator!=(const Iterator& other) {
            // We only check if both points to the end.
            return m_done && other.m_done;
        }
    };

    Iterator m_iteratorBeginning;

    StringSplit(const string& input, const string& delim): m_iteratorBeginning{std::move(input), std::move(delim)} { }

    Iterator begin() { return m_iteratorBeginning; }

    Iterator end() { return Iterator{}; }
};

#endif // FAIL2ABUSEIPDB_INCLUDE_STRING_SPLITTER_HPP