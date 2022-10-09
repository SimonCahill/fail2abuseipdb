# fail2abuseipdb

fail2abuseipdb is a server administration tool (currently WIP!) which allows server administrators to **easily** and **automagically** convert
fail2ban's output (which a weird semi-JSON format) to a CSV-format which can directly be published to abuseipdb!

# Features
fail2abuseipdb provides a limited featureset, although it is enough for its task.

 - Reading fail2ban's output from a file
 - Reading fail2ban's output from stdin via pipes (**WIP**)
 - Reading fail2ban's output directly by calling fail2ban (requires elevated privileges and may be dangerous!) (**WIP**)
 - Comment customisation
 - Supports both individual jails and complete f2b output!
 - Jail names are detected automatically when full output is detected

# Arguments

The following table describes the individual arguments and switches fail2abuseipdb understands.
Arguments and switched marked as ((planned)) are not use available (or usable)!

| Long          | Short | Description                                                           | Status        |
|---------------|-------|-----------------------------------------------------------------------|---------------|
| --help        | -h    | Prints the help menu and exits.                                       | working       |
| --stdin       | -s    | Reads input from stdin (pipe).                                        | ((planned))   |
| --file=       | -f[f] | Reads input from the passed file or fail2ban.json if no file passed.  | working       |
| --version     | -v    | Prints the version information and exits.                             | ((planned))   |
| --comment     | -c    | Overrides the default value for the comment. Must be in quotes (")!   | working       |
| --jail-name=  | -j[j] | Useful when importing single jails; sets the name for the jail.       | working       |

## Comment variables
| Variable      | Function                                                                      | Status        |
|---------------|-------------------------------------------------------------------------------|---------------|
| {0}           | Prints the jail name in the comment.                                          | working       |
| {1}           | Prints the report time in the comment.                                        | ((planned))   |

# Usage

Getting started with fail2abuseipdb is simple. Install the file to /usr/bin (or where ever in your PATH) and call it as follows.

**Please not at the time of writing, ONLY reading from files works!**

## Reading from files
```bash
# Single jail

fail2ban-client get myjail banned >/tmp/myjail.txt
fail2abuseipdb -f/tmp/myjail.txt -jmyjail -c"Was banned for fcking w/ my shit. Fail2ban jail: {0}" >/tmp/myjail.csv

# ALL THE JAILS
fail2ban-client banned >/tmp/alljails.txt
fail2abuseipdb -f/tmp/alljails.txt -c"Brute-force attack against {0}" >/tmp/alljails.csv
```

## Reading from stdin (*WIP*)
```bash
# Single jail

fail2ban-client get myjail banned | fail2abuseipdb -s -jmyjail -c"Was banned for fcking w/ my shit. Fail2ban jail: {0}" >/tmp/myjail.csv

# ALL THE JAILS
fail2ban-client banned | fail2abuseipdb -s -c"Brute-force attack against {0}" >/tmp/alljails.csv
```

# Changelog

**v0.1.0b**

 - Initial release
 - Very much still a beta release, **but it works**!

# To-Do

In no particular order, the to-dos are as follows. I just hope I update them 🫣

 1) Add version information during build
 2) Add install target
 3) Add remove target
 4) Build deb-package?!
 5) Add pipe support
 6) Add support for calling fail2ban directly
 7) Clean up code for first *real* release
 8) Add debug messages (printed to stderr)
 9) Unit tests? Don't really care for them
 10) Add support for adding/overriding categories (both default and per jail)
