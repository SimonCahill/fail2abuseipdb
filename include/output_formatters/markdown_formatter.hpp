/**
 * @file markdown_formatter.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of the Markdown formatter.
 * @version 0.1
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_MARKDOWN_FORMATTER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_MARKDOWN_FORMATTER_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "output_formatters/iformatter.hpp"
#include "output_type.hpp"
#include "f2b_jail.hpp"

namespace f2abuseipdb::output {

    using std::stringstream;

    /**
     * @brief This output formatter outputs detected bans in a markdown-compatible format so that bad IPs may be put on display.
     */
    class MarkdownFormatter: public IFormatter {
        public: // +++ Constructor / Destructor +++
            explicit MarkdownFormatter(shared_ptr<config::ConfigManager> cfg): m_cfg(cfg) {}
            virtual ~MarkdownFormatter() = default;

        public: // +++ Overridden Functions +++
            virtual OutputType  getOutputType() const { return OutputType::MARKDOWN; }

            virtual string      formatData(const vector<Jail>&);

        private:
            size_t              getLongestJailName(const vector<Jail>&) const;

            string              getCentredString(const size_t totalLen, const string& text) const;
    
            /**
             * @brief Gets a string containing whitespace to centre text
             * 
             * @param totalWidth The total width of the area where the string should be centred
             * @param strLength The length of the string to be centre-printed
             * 
             * @return string The spacer string
             */
            string              getSpacerString(const size_t totalWidth, const size_t strLength) const {
                return string(totalWidth / 2 - strLength / 2, ' ');
            }

            void                generateJailStatistics(stringstream& dataStream, const Jail& jail) const;

            void                generateOverview(stringstream&, const vector<Jail>&) const;
            void                generateOverviewHeaders(stringstream&, const vector<Jail>&) const;

        private:
            shared_ptr<config::ConfigManager> m_cfg{nullptr};
            const string m_activeBansHeader{" Active Bans in Jail "};
            const string m_jailActiveheader{" Jail Active "};
            const string m_jailNameHeader{" Jail Name "};
            const string m_totalBansHeader{" Total Bans in Jail "};
    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_MARKDOWN_FORMATTER_HPP