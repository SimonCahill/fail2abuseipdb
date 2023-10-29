/**
 * @file output_type.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains a basic declaration of output types supported by fail2abuseipdb
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_OUTPUT_TYPE_HPP
#define FAIL2ABUSEIPDB_INCLUDE_OUTPUT_TYPE_HPP

namespace f2abuseipdb {

    /**
     * @brief A simple enumeration of output types available with fail2abuseipdb.
     */
    enum class OutputType {

        /**
         * @brief Output jail information as AbuseIPDB-compatible CSV.
         */
        ABUSEIPDB_CSV,

        /**
         * @brief Output jail information as JSON array.
         */
        JSON,

        /**
         * @brief Output jail information as markdown document for publicly shaming IPs or something - I don't know.
         */
        MARKDOWN,

    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_OUTPUT_TYPE_HPP