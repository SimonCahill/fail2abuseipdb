/**
 * @file f2b_jail.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the declaration of the Jail class, which represents a single jail in fail2ban.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems and Pascal Lüttmann
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_F2B_FAIL_HPP
#define FAIL2ABUSEIPDB_INCLUDE_F2B_FAIL_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <memory>
#include <optional>
#include <string>
#include <vector>

// json
#include <nlohmann/json.hpp>

// sqlite
#include <sqlite3.h>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "config_parser.hpp"
#include "report_record.hpp"

namespace f2abuseipdb {

    using nlohmann::json;

    using std::optional;
    using std::shared_ptr;
    using std::string;
    using std::vector;

    class Jail;

    using jailvec_t = vector<Jail>;
    using optjail_t = optional<Jail>;
    using optstr_t = optional<string>;
    using recvec_t = vector<ReportRecord>;

    /**
     * @brief Represents a single jail in Fail2Ban.
     * 
     * A jail may be enabled or disabled, has a name and N bans.
     */
    class Jail {
        public: // +++ Constructor / Destructor +++
            explicit            Jail(const string& jailName, const bool isEnabled, const optstr_t& jailDescription = std::nullopt): m_isEnabled(isEnabled), m_jailDescription(jailDescription), m_jailName(jailName) {}
            ~                   Jail() = default;

        public: // +++ Static +++
            static optjail_t    loadJailFromDb(const string&, sqlite3*, shared_ptr<config::ConfigParser>);
            static jailvec_t    loadJailsFromDb(sqlite3*, shared_ptr<config::ConfigParser>);

            static size_t       getTotalJails(sqlite3*, shared_ptr<config::ConfigParser>);

        public: // +++ Getter / Setter +++
            bool                isEnabled() const { return m_isEnabled; }

            optstr_t            getJailDescription() const { return m_jailDescription; }

            string              getJailName() const { return m_jailName; }

        public: // +++ Business Logic +++
            json&&              generateReportObject() const;

            void                loadAllBanned(const sqlite3*);
            void                loadActiveBanned(const sqlite3*);
            void                loadFormerBanned(const sqlite3*);

        private:
            bool                m_isEnabled{true};

            optstr_t            m_jailDescription{std::nullopt};

            string              m_jailName{};

            recvec_t            m_reports{};
    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_F2B_FAIL_HPP