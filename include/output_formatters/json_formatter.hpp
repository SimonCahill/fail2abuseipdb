/**
 * @file json_formatter.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of a JSON formatter object.
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_JSON_FORMATTER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_JSON_FORMATTER_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <algorithm>
#include <string>
#include <vector>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "output_formatters/iformatter.hpp"
#include "output_type.hpp"
#include "f2b_jail.hpp"

namespace f2abuseipdb::output {

    using std::string;
    using std::vector;

    /**
     * @brief A formatter specifically created to output JSON-formatted data which can be parsed by other applications.
     */
    class JsonFormatter: public IFormatter {
        public: // +++ Overridden Functions +++
            virtual OutputType  getOutputType() const { return OutputType::JSON; }

            virtual string      formatData(const vector<Jail>& jails) {
                json objects = json::array();

                std::transform(jails.cbegin(), jails.cend(), std::back_inserter(objects), [&](const auto& jail) {
                    return jail.generateReportObject();
                });

                return objects.dump(2);
            }
    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_JSON_FORMATTER_HPP