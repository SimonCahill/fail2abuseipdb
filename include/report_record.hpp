/**
 * @file report_record.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation and declaration of the Report class.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems and Pascal Lüttmann
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_REPORT_HPP
#define FAIL2ABUSEIPDB_INCLUDE_REPORT_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

// json
#include <nlohmann/json.hpp>

namespace f2abuseipdb {

    using nlohmann::json;

    using std::chrono::duration;
    using std::chrono::seconds;
    using std::chrono::system_clock;
    using std::chrono::time_point;
    using std::string;

    using bantime_t = time_point<system_clock>;

    /**
     * @brief Represents a record of reports for a given IP address.
     * 
     * This class contains information about all reports for a given IP address, such as first/last known report
     * time, comments, log excerpts, and more.
     */
    class ReportRecord {
        public: // +++ Constructor / Destructor +++
            explicit        ReportRecord(const bantime_t& bannedOn, const seconds& banTime, const json& banData, const size_t banCount, string bannedIp);
            ~               ReportRecord() = default;

        public: // +++ Static +++
            ReportRecord&&  fromRow();

        private:
            bantime_t       m_bannedOn{};

            json            m_banData{};

            seconds         m_banTime{};
            size_t          m_banCount{0};
            string          m_bannedIp{};
    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_REPORT_HPP