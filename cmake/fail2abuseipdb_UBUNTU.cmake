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

set(f2abuseipdb_DEFAULT_CONF_PATH "/etc/fail2abuseipdb/fail2abuseipdb.conf")
set(f2abuseipdb_F2B_DB_FILE "/var/lib/fail2ban/fail2ban.sqlite3")
set(f2abuseipdb_GET_JAILS "SELECT * FROM jails;")
set(f2abuseipdb_COUNT_JAILS "SELECT COUNT(*) FROM jails;")
set(f2abuseipdb_GET_ALL_BANS "SELECT * FROM bips;")
set(f2abuseipdb_GET_BANS_FOR_JAIL "SELECT * FROM bips WHERE jail = \\\"\${JAIL}\\\";")
set(f2abuseipdb_GET_BANS_AFTER_TSTAMP "SELECT * FROM bips WHERE timeofban >= \${DATE};")
set(f2abuseipdb_GET_BANS_BEFORE_TSTAMP "SELECT * FROM bips WHERE timeofban <= \${DATE};")
set(f2abuseipdb_GET_BANS_BETWEEN_TSTAMP "SELECT * FROM bips WHERE timeofban <= \${DATE_1} AND timeofban >= \${DATE_2};")