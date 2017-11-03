# gather
The tool gathers symbol candle data to an output file. The interval of every candle is 1 minute.

## Usage
There are two possible ways to run the program.
First by calling with parameters or second by reading a config file.

```bash
$ gather [options] [LOGFILE]
```

### Parameters

- -h Show help
- -s \<symbol> The symbol to fetch
- -u \<username> The xtb account user
- -p \<password> The xtb account password
- -i \<config> The config file with USERNAME,PASSWORD,SYMBOL and optional LOGFILE
- -t \<seconds>	Fetch history beginning from now back in seconds
- -l Connect to live server
- -v Verbose mode. Show candle information on stdout instead of logging to a file
- -d Debug mode. Show json requests/response data on stdout
- LOGFILE The logfile for output

**Call with parameters:**
```bash
$ gather -s EURUSD -u 1234567 -p supersecret eurusd.log
```

**Call with config file:**
```bash
$ gather -i eurusd.conf
```

**Call with config file and logfile parameter:**
```bash
$ gather -i eurusd.conf /tmp/eurusd.log
```

**Call with multiparameters:**

This will gather candle data for eurusd in verbose mode, from live server with debug flag on.

```bash
$ gather -dvli eurusd.conf
```

**Fetch historical data:**

This will fetch one hour (3600 seconds) back from now. One candle every minute. 

```bash
$ gather -t 3600 -i eurusd.conf
```

### Config file
```
# username
USERNAME=10091849
# password
PASSWORD=superawesome
# symbol
SYMBOL=EURUSD
# logfile for output, this is optional. Could be given by parameter
# will be ignored in verbose mode.
LOGFILE=eurusd.log
```
