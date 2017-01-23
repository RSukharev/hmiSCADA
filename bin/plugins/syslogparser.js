var SYSLOG_LINE_REGEX = [
  /(\<[0-9]+\>)?/ig,               // 0   optional priority
  /([a-z]{3})\s+/ig,               // 1   month
  /([0-9]{1,2})\s+/ig,             // 2   date
  /([0-9]{2})\:/ig,                // 3   hours
  /([0-9]{2})\:/ig,                // 4   minutes
  /([0-9]{2})/ig,                  // 5   seconds
  /(\s+[\w\.\-]+)?\s+/ig,          // 6   host
  /([\w\-\(\)\.0-9\/]+)/ig,        // 7   process
  /(?:\[([a-z0-9\-\.]+)\])?\:/ig,  // 8   optional pid
  /(.+)/ig,                        // 9  message
];

function getValues(content) {

    var lastIndex = 0;
    var results = [];

    for(var i = 0; i < SYSLOG_LINE_REGEX.length; i++) {

        var regexp = SYSLOG_LINE_REGEX[i];
        regexp.lastIndex = lastIndex;
        var result = regexp.exec(content);

        if(!result) result = 'empty';
        else {
            lastIndex = result.index + 1;
            result = result[0];
        }

        //print('lastIndex:' + lastIndex +' result:'+ result);
        results[i] = result;
    }

    return results;
}

function parse (log) {

    //print(log);

    var values = getValues(log.trim());
    var priority =  (values[0] || '').replace(/[^0-9]/g, '');

    //print('priority:' + priority);

    var facilityCode = priority >> 3;
    var facility = FACILITY[facilityCode];
    var severityCode = priority & 7;
    var severity = SEVERITY[severityCode];

    var month = MONTHS.indexOf(values[1].trim());

    //print('month:' + month);

    var date = values[2];

    //print('date:' + date);

    var hours = parseInt(values[3]);

    //print('hours:' + hours);

    var minutes = parseInt(values[4]);

    //print('minutes' + minutes);

    var seconds = parseInt(values[5]);

    //print('seconds' + seconds);

    var time = new Date();

    time.setMonth(month);
    time.setDate(date);
    time.setHours(hours);
    time.setMinutes(minutes);
    time.setSeconds(seconds);

    var host = values[6].trim();

    //print('host:' + host);

    var process = values[7];

    //print('process:' + process);

    var pid = (values[8] || '');

    //print('pid:' + pid);

    var message = values[9];

    //print('message:' + message);

  return  {
    priority: priority,
    facilityCode: facilityCode,
    facility: facility,
    severityCode: severityCode,
    severity: severity,
    time: time,
    host: host,
    process: process,
    pid: pid,
    message: message
  };
};


// Constants

var FACILITY = [
  'kern',
  'user',
  'mail',
  'daemon',
  'auth',
  'syslog',
  'lpr',
  'news',
  'uucp',
  'cron',
  'authpriv',
  'ftp',
  'ntp',
  'logaudit',
  'logalert',
  'clock',
  'local0',
  'local1',
  'local2',
  'local3',
  'local4',
  'local5',
  'local6',
  'local7'
];

var SEVERITY = [
  'emerg',
  'alert',
  'crit',
  'err',
  'warning',
  'notice',
  'info',
  'debug'
];

var MONTHS = [
  'Jan',
  'Feb',
  'Mar',
  'Apr',
  'May',
  'Jun',
  'Jul',
  'Aug',
  'Sep',
  'Oct',
  'Nov',
  'Dec'
];

