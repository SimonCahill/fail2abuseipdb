/**
 * @file markdown_formatter.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of the Markdown formatter.
 * @version 0.1
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

// libc
#include <cstdlib>
#include <unistd.h>

// spdlog
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "output_formatters/markdown_formatter.hpp"
#include "output_type.hpp"
#include "f2b_jail.hpp"

namespace f2abuseipdb::output {

    using std::stringstream;
    using std::string;

    size_t MarkdownFormatter::getLongestJailName(const vector<Jail>& jails) const {
        size_t longest{0};

        for (const auto& jail : jails) {
            if (jail.getJailName().size() > longest) {
                longest = jail.getJailName().size();
            }
        }

        return longest;
    }

    string MarkdownFormatter::formatData(const vector<Jail>& jails) {
        stringstream dataStream{};

        generateOverview(dataStream, jails);

        for (const auto& jail : jails) {
            dataStream << std::endl << std::endl;
            generateJailStatistics(dataStream, jail);
        }

        return dataStream.str();
    }

    string MarkdownFormatter::getCentredString(const size_t totalLen, const string& text) const {
        const auto spacer = getSpacerString(totalLen, text.size());
        return spacer + text + string(totalLen - text.size() - spacer.size(), ' ');
    }

    void MarkdownFormatter::generateJailStatistics(stringstream& dataStream, const Jail& jail) const {
        dataStream << "# Statistics for " << jail.getJailName() << std::endl
                   << "## Jail description: " << jail.getJailDescription().value_or("A Fail2Ban jail") << std::endl;

        
    }

    void MarkdownFormatter::generateOverview(stringstream& dataStream, const vector<Jail>& jails) const {
        const auto hostName = m_cfg->getHostName();

        dataStream << fmt::format("# General Overview for {0:s}", hostName) << std::endl;

        // Print headers
        generateOverviewHeaders(dataStream, jails);

        // Header should look something like
        // |      Jail Name     | Jail Active | Total Bans in Jail | Active Bans in Jail |

        const auto longestJailName = getLongestJailName(jails);
        for (const auto& jail : jails) {
            dataStream << '|' << getCentredString(longestJailName, jail.getJailName())
                       << '|' << getCentredString(m_jailActiveheader.size(), jail.isEnabled() ? "yes" : "no")
                       << '|' << getCentredString(m_totalBansHeader.size(), std::to_string(jail.countAllReports(nullptr, nullptr)))
                       << '|' << getCentredString(m_activeBansHeader.size(), std::to_string(jail.getCurrentReportCount()))
                       << '|'
                       << std::endl;
        }
        
    }

    void MarkdownFormatter::generateOverviewHeaders(stringstream& dataStream, const vector<Jail>& jails) const {
        // "Jail Name"
        const auto longestJailName = getLongestJailName(jails); // TODO: Handle cases where "Jail Name" is longer than longest jail name
        
        auto spacer = getSpacerString(longestJailName, m_jailNameHeader.size());
        dataStream << '|' << spacer << m_jailNameHeader << string(longestJailName - m_jailNameHeader.size() - spacer.size(), ' ') << '|'
        // "Jail Active"
                   << m_jailActiveheader << '|'
        // "Total Bans"
                   << m_totalBansHeader << '|'
        // "Active Bans"
                   << m_activeBansHeader << '|'
                   << std::endl;

        // Print markdown table separator
        dataStream << '|' << string(longestJailName, '-')
                   << '|' << string(m_jailActiveheader.size(), '-')
                   << '|' << string(m_totalBansHeader.size(), '-')
                   << '|' << string(m_activeBansHeader.size(), '-')
                   << '|' << std::endl;
    }

}