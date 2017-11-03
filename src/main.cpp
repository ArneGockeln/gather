/*!
 *  Copyright(c)2017, Arne Gockeln
 *  http://www.arnegockeln.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <unistd.h>
#include <xtbclient/Client.h>
#include <fstream>
#include <getopt.h>

using namespace xtbclient;
using namespace std;

/*!
 * Output candle data to file or stdout
 *
 * @param StreamCandleRecord candleRecord
 * @param string logfilename
 * @param bool flag_verbose
 */
static void output_candle(StreamCandleRecord candleRecord, string logfilename, bool flag_verbose){
  ostringstream buf;

  // output candle record data in one line to file
  buf
      << candleRecord.m_symbol.c_str() << ";"
      << candleRecord.m_ctm << ";"
      << candleRecord.m_ctmString.c_str() << ";"
      << candleRecord.m_open << ";"
      << candleRecord.m_high << ";"
      << candleRecord.m_low << ";"
      << candleRecord.m_close << ";"
      << candleRecord.m_vol << ";"
      << candleRecord.m_quoteid;

  if(flag_verbose){
    // write to stdout
    printf("%s\n", buf.str().c_str());
  } else {

    if( logfilename.empty() ){
      printf("Err: No logfilename found.\n");
      exit(EXIT_FAILURE);
    }

    ofstream output_stream;

    // open file for logging
    output_stream.open( logfilename, ios::out | ios::app );
    // write to logfile
    if( output_stream.is_open() ){
      // write
      output_stream << buf.str() << endl;
      // close
      output_stream.close();
    } else {
      printf("Err: Unable to open output file.\n");
      exit(EXIT_FAILURE);
    }
  }
}

class Listener: public StreamListener {
private:
  string m_logfilename;
  bool m_flag_verbose;

public:
  Listener(const string& logfilename, bool flag_verbose = false){
    m_flag_verbose = flag_verbose;

    if( !flag_verbose ){
      if(!logfilename.empty()){
        m_logfilename = logfilename;
      }
    }
  }

  void onCandle(StreamCandleRecord candleRecord) override {
    output_candle(candleRecord, m_logfilename, m_flag_verbose);
  }
};

/*!
 * Show usage message
 * @param const string name
 */
static void show_usage(const std::string& name){
  cout << "Usage: " << name << " [options] [LOGFILE]\n"
       << "Options:\n"
       << "\t-h\t\tShow this help message\n"
       << "\t-s <symbol>\tThe Symbol like EURUSD\n"
       << "\t-u <username>\tXTB Account User\n"
       << "\t-p <password>\tXTB Account Password\n"
       << "\t-i <config>\tConfig file with USERNAME,PASSWORD,SYMBOL and LOGFILE\n"
       << "\t-t <seconds>\tFetch history beginning from now back in seconds.\n"
       << "\t-l\t\tConnect to live server.\n"
       << "\t-v\t\tVerbose mode. Show candle information on stdout instead of logging to a file.\n"
       << "\t-d\t\tDebug mode. Show json requests/response data on stdout.\n"
       << "\nOutput Format:\n"
       << "SYMBOL;ctm;ctmString;open;high;low;close;volume;quoteid"
       << endl;
}

/*!
 * Show about message
 */
static void show_about(){
  cout << "gather - gather candle data\n"
       << "Copyright(c)2017, Arne Gockeln. All rights reserved.\n"
       << "http://www.arnegockeln.com\n"
       << endl;
}

int main(int argc, char* argv[]) {

  string symbol;
  string username;
  string password;
  string logfile;
  string config;
  int seconds = 0;

  bool flag_live = false;
  bool flag_verbose = false;
  bool flag_debug = false;
  bool flag_fetch_history = false;

  if( argc < 2 ){
    show_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  int opt;

  while( (opt = getopt(argc, argv, "dhls:u:p:i:t:vb:e:")) != -1 ){
    switch(opt){
      case 'h':
        show_about();
        show_usage(argv[0]);
        exit(EXIT_SUCCESS);
      case 'l':
        flag_live = true;
        break;
      case 'v':
        flag_verbose = true;
        break;
      case 'd':
        flag_debug = true;
        break;
      case 's':
        symbol = optarg;
        break;
      case 'u':
        username = optarg;
        break;
      case 'p':
        password = optarg;
        break;
      case 'i':
        config = optarg;
        break;
      case 't':
        seconds = atoi(optarg);
        if( seconds > 0 ){
          flag_fetch_history = true;
        }
        break;
      default:
        show_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if( optind < argc ){
    // set logfile name
    logfile = argv[optind];
  }

  // check for config file to read
  if( !config.empty() ){
    ifstream configStream;
    configStream.open(config, ios::in);
    if( configStream.is_open() ){
      // parse config file
      string line;
      while( getline(configStream, line) ){
        // check if line is not a comment line
        if( !line.empty() ){
          if( line.at(0) != '#' ){
            // look for username=
            if( line.substr(0,9) == "USERNAME=" ){
              username = line.erase(0, 9);
            }
              // look for password
            else if( line.substr(0, 9) == "PASSWORD=" ){
              password = line.erase(0, 9);
            }
              // look for symbol
            else if( line.substr(0, 7) == "SYMBOL=" ){
              symbol = line.erase(0, 7);
            }
              // look for logfile
            else if( line.substr(0, 8) == "LOGFILE=" ){
              logfile = line.erase(0, 8);
            }
          }
        }
      }
      configStream.close();
    } else {
      printf("Err: Unable to read config file.\n");
      exit(EXIT_FAILURE);
    }
  }

  // valid data
  if(symbol.empty()){
    printf("Err: Symbol required.\n");
    exit(EXIT_FAILURE);
  }

  if(username.empty()){
    printf("Err: Username required.\n");
    exit(EXIT_FAILURE);
  }

  if(password.empty()){
    printf("Err: Password required.\n");
    exit(EXIT_FAILURE);
  }

  if( !flag_verbose ){
    if (logfile.empty()) {
      printf("Err: Logfile required.\n");
      exit(EXIT_FAILURE);
    }
  }

  // connect to api
  Client client( ( flag_live ? ClientType::REAL : ClientType::DEMO ) );
  client.setVerbose(flag_verbose);
  client.setDebugOutput(flag_debug, flag_debug);

  if( client.login( username.c_str(), password.c_str() ) ){

    // fetch data without streaming
    if( flag_fetch_history ){
      ChartRangeInfoRecord rangeInfoRecord;
      rangeInfoRecord.m_symbol = symbol;
      rangeInfoRecord.m_period = PERIOD_M1;
      rangeInfoRecord.m_end = Util::getMilliseconds( Util::getUTCTimestamp() ).count();
      rangeInfoRecord.m_start = Util::getMilliseconds( Util::getUTCTimestampDifference( seconds ) ).count();

      ChartLastRequestRecord rangeRecord = client.getChartRangeRequest( rangeInfoRecord );
      for( auto& normalCandle : rangeRecord.m_rateInfos ){
        StreamCandleRecord candle;
        candle.m_symbol = symbol;
        candle.m_ctm = normalCandle.m_ctm;
        candle.m_ctmString = normalCandle.m_ctmString;
        candle.m_vol = normalCandle.m_vol;
        candle.m_open = normalCandle.m_open;
        candle.m_high = normalCandle.m_high;
        candle.m_low = normalCandle.m_low;
        candle.m_close = normalCandle.m_close;

        output_candle(candle, logfile, flag_verbose);
      }

      client.logout();
      exit(EXIT_SUCCESS);
    }

    Client streamClient( ( flag_live ? ClientType::REAL_STREAM : ClientType::DEMO_STREAM ) );
    streamClient.setVerbose(flag_verbose);
    streamClient.setDebugOutput(flag_debug, flag_debug);

    streamClient.setStreamSessionId( client.getStreamSessionId() );
    streamClient.subscribeCandles( symbol );

    if( flag_verbose ){
      printf("Gathering data...\n");
    }

    // set stream listener
    Listener listener( logfile, flag_verbose );
    // start listening
    streamClient.setStreamListener( &listener );
  }

  exit(EXIT_SUCCESS);
}