######################################################
##  This CMake script sets all required variables   ##
##  for fail2abuseipdb at generation time, so that  ##
##  we have sane defaults during runtime.           ##
##                                                  ##
##  These values should be good for their           ##
##  Linux distribution.                             ##
##  Setting Ubuntu values on a Suse machine may or  ##
##  may not work. Do at your own discretion.        ##
##                                                  ##
##  Written by Simon Cahill.                        ##
##  Copyright ©️ 2023 Simon Cahill / Procyon Systems ##
######################################################

set(f2abuseipdb_JAILS_TABLE "jails")
set(f2abuseipdb_BANS_TABLE "bips") # change this to "bans" if fail2ban < 0.11.2!

set(f2abuseipdb_DEFAULT_CONF_PATH "/etc/fail2abuseipdb/fail2abuseipdb.conf")
set(f2abuseipdb_F2B_DB_FILE "/var/lib/fail2ban/fail2ban.sqlite3")
set(f2abuseipdb_GET_JAILS "SELECT * FROM ${f2abuseipdb_JAILS_TABLE};")
set(f2abuseipdb_COUNT_JAILS "SELECT COUNT(*) FROM ${f2abuseipdb_JAILS_TABLE};")
set(f2abuseipdb_GET_SPECIFIC_JAIL "SELECT * FROM ${f2abuseipdb_JAILS_TABLE} WHERE name = \\\"\${JAIL}\\\";")
set(f2abuseipdb_GET_ALL_BANS "SELECT * FROM ${f2abuseipdb_BANS_TABLE};")
set(f2abuseipdb_GET_BANS_FOR_JAIL "SELECT * FROM ${f2abuseipdb_BANS_TABLE} WHERE jail = \\\"\${JAIL}\\\";")
set(f2abuseipdb_GET_BANS_AFTER_TSTAMP "SELECT * FROM ${f2abuseipdb_BANS_TABLE} WHERE timeofban >= \${DATE};")
set(f2abuseipdb_GET_BANS_BEFORE_TSTAMP "SELECT * FROM ${f2abuseipdb_BANS_TABLE} WHERE timeofban <= \${DATE};")
set(f2abuseipdb_GET_BANS_BETWEEN_TSTAMP "SELECT * FROM ${f2abuseipdb_BANS_TABLE} WHERE timeofban <= \${DATE_1} AND timeofban >= \${DATE_2};")
set(f2abuseipdb_COUNT_JAIL_BANS "SELECT COUNT(*) FROM ${f2abuseipdb_BANS_TABLE} WHERE jail = \\\"\${JAIL}\\\";")