/**
 * @file ban_selection.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the declaration of an enum containing the ban selection type.
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023 Simon Cahill / Procyon Systems
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_BAN_SELECTION_HPP
#define FAIL2ABUSEIPDB_INCLUDE_BAN_SELECTION_HPP

namespace f2abuseipdb {

    /**
     * @brief Simple enumeration of possible ban selection types available with this application.
     */
    enum class BanSelection {

        /**
         * @brief Select only active bans from the database.
         */
        ACTIVE_BANS,

        /**
         * @brief Select only previous bans from the database.
         */
        PREVIOUS_BANS,

        /**
         * @brief Select all entries from the database.
         */
        ALL_BANS,

    };

}

#endif // FAIL2ABUSEIPDB_INCLUDE_BAN_SELECTION_HPP