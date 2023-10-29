/**
 * @file iformatter.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the interface for defining a basic contract between the application and any given formatter object.
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_IFORMATTER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_IFORMATTER_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <string>
#include <vector>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "output_type.hpp"
#include "f2b_jail.hpp"

namespace f2abuseipdb::output {

    using std::string;
    using std::vector;

    /**
     * @brief Basic contract between the application and output formatters.
     * 
     * This contract defines the means by which users can select between different output formats.
     */
    class IFormatter {
        public:
            virtual OutputType  getOutputType() const = 0;
            virtual string      formatData(const vector<Jail>&) = 0;
    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_OUTPUT_FORMATTERS_IFORMATTER_HPP