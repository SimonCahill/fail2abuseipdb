# fail2abuseipdb

fail2abuseipdb is a server administration tool (currently WIP!) which allows server administrators to **easily** and **automagically** convert
fail2ban's output (which a weird semi-JSON format) to a CSV-format which can directly be published to abuseipdb!

![GitHub all releases](https://img.shields.io/github/downloads/SimonCahill/fail2abuseipdb/total?logo=github)
![GitHub](https://img.shields.io/github/license/SimonCahill/fail2abuseipdb)

# Features
fail2abuseipdb provides a limited featureset, although it is enough for its task.

 - Reading fail2ban's output from a file
 - Reading fail2ban's output from stdin via pipes
 - Reading fail2ban's output directly by calling fail2ban (requires elevated privileges and may be dangerous!) (**Search is bugged!**)
 - Comment customisation
 - Supports both individual jails and complete f2b output!
 - Jail names are detected automatically when full output is detected

# Arguments

The following table describes the individual arguments and switches fail2abuseipdb understands.
Arguments and switched marked as ((planned)) are not use available (or usable)!

| Long          | Short | Description                                                           | Status        |
|---------------|-------|-----------------------------------------------------------------------|---------------|
| --help        | -h    | Prints the help menu and exits.                                       | working       |
| --stdin       | -s    | Reads input from stdin (pipe).                                        | working       |
| --file=       | -f[f] | Reads input from the passed file or fail2ban.json if no file passed.  | working       |
| --version     | -v    | Prints the version information and exits.                             | ((planned))   |
| --comment     | -c    | Overrides the default value for the comment. Must be in quotes (")!   | working       |
| --jail-name=  | -j[j] | Useful when importing single jails; sets the name for the jail.       | working       |       
| --f2b=        | -e[f] | Sets the location of fail2ban directory                               | working       |
| --call-f2b    | -%    | No, that's not a typo. Call fail2ban directly                         | (kinda)working|

## Comment variables
| Variable      | Function                                                                      | Status        |
|---------------|-------------------------------------------------------------------------------|---------------|
| {0}           | Prints the jail name in the comment.                                          | working       |
| {1}           | Prints the report time in the comment.                                        | ((planned))   |

# Exit Codes
| Code          | Meaning                                                                       |
|---------------|-------------------------------------------------------------------------------|
| 0             | Success                                                                       |
| 1             | Failed to parse input from file                                               |
| 2             | Failed to parse input from stdin                                              |
| 3             | Failed to parse input from fail2ban directly                                  |
| 4             | Insufficient execution rights                                                 |
| 5             | Could not find fail2ban-client                                                |

# Usage

Getting started with fail2abuseipdb is simple. Install the file to /usr/bin (or where ever in your PATH) and call it as follows.


## Reading from files
```bash
# Single jail

fail2ban-client get myjail banned >/tmp/myjail.txt
fail2abuseipdb -f/tmp/myjail.txt -jmyjail -c"Was banned for fcking w/ my shit. Fail2ban jail: {0}" >/tmp/myjail.csv

# ALL THE JAILS
fail2ban-client banned >/tmp/alljails.txt
fail2abuseipdb -f/tmp/alljails.txt -c"Brute-force attack against {0}" >/tmp/alljails.csv
```

## Reading from stdin
```bash
# Single jail

fail2ban-client get myjail banned | fail2abuseipdb -s -jmyjail -c"Was banned for fcking w/ my shit. Fail2ban jail: {0}" >/tmp/myjail.csv

# ALL THE JAILS
fail2ban-client banned | fail2abuseipdb -s -c"Brute-force attack against {0}" >/tmp/alljails.csv
```

# Building the application

If you don't trust the .deb packages uploaded in the releases, or your system doesn't use .deb packages, you can download, build and install the application yourself.

To do so is fairly simple:
```bash
# getting ready
$ git clone --recursive https://github.com/SimonCahill/fail2abuseipdb.git && cd fail2abuseipdb
$ mkdir build && cd build
$ cmake ..

# making the .deb:
$ cpack .

# making the application:
$ make -j

# generating documentation (requires Doxygen!)
$ make docs

# installing the freshly generated binary
$ sudo make install
```

# Changelog

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
