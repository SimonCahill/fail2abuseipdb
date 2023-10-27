##########################################################
##  Simple script which determines the current flavour  ##
##  of Linux running on the system.                     ##
##  This information is then used to determine, which   ##
##  CMake script should be included for the runtime     ##
##  params.                                             ##
##                                                      ##
##  This script works by reading /etc/os-release        ##
##  and then including the correct script.              ##
##                                                      ##
##  Written by Simon Cahil (contact@simonc.eu)          ##
##  Copyright ©️ 2023 Simon Cahill / Procyon Systems     ##
##########################################################

execute_process(COMMAND bash -c "/bin/grep -e \"^ID=\" /etc/os-release | /bin/sed 's/ID=//'" OUTPUT_VARIABLE OS_REL_INFO)
string(STRIP ${OS_REL_INFO} OS_REL_INFO_STRIPPED)
string(TOUPPER ${OS_REL_INFO_STRIPPED} OS_DISTRO)

set(f2abuseipdb_CFG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/fail2abuseipdb_${OS_DISTRO}.cmake)
if (EXISTS ${f2abuseipdb_CFG_FILE})
    include(${f2abuseipdb_CFG_FILE})
else()
    message(FATAL_ERROR "Configuration file ${f2abuseipdb_CFG_FILE} was not found! Do you have a weird distro?")
endif()