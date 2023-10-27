# fail2abuseipdb

fail2abuseipdb is a server administration tool (currently WIP!) which allows server administrators to **easily** and **automagically** convert
fail2ban's output (which a weird semi-JSON format) to a CSV-format which can directly be published to abuseipdb!

![GitHub all releases](https://img.shields.io/github/downloads/SimonCahill/fail2abuseipdb/total?logo=github)
![GitHub](https://img.shields.io/github/license/SimonCahill/fail2abuseipdb)

# !!! COMPLETE RE-WORK IN PROGRESS !!!

# Changelog

**v2.0.0**

 - Completely new application.
 - Reworked everything from scratch.
 - More to come.

**v0.2.0b**

New features:
 - Added support for piping fail2ban-client output directly
 - Added support for calling fail2ban-client directly

New bugs:
 - Automagic search for fail2ban is bugged and I don't want to fix it just yet
    - Circumvent this issue by calling `fail2abuseipdb -% -e$(which fail2ban-client)`

**v0.1.0b**

 - Initial release
 - Very much still a beta release, **but it works**!

# To-Do

In no particular order, the to-dos are as follows. I just hope I update them 🫣

 1) ~~Add version information during build~~ Working
 2) ~~Add install target~~ Working
 3) ~~Add remove target~~ Use deb instead
 4) ~~Build deb-package?!~~ Working
 5) ~~Add pipe support~~ Working
 6) ~~Add support for calling fail2ban directly~~ Working, except for automagic search
 7) Clean up code for first *real* release
 8) Add debug messages (printed to stderr)
 9) Unit tests? Don't really care for them
 10) Add support for adding/overriding categories (both default and per jail)
