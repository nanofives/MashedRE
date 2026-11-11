/* BIND-CRT-STDIO: print the CRT time-zone state after _tzset, built once with the XP DDK libcmt (the original's CRT)
   and once with the VS2022 UCRT (the exe's CRT). Same source, same inputs, outputs compared by tz_compare.py. */
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
int main(void) {
    __int64 t = 1100000000;  /* fixed instant: 2004-11-09 11:33:20 UTC */
    struct tm *lt;
    _tzset();
    printf("timezone=%ld daylight=%d dstbias=%ld\n", (long)_timezone, (int)_daylight, (long)_dstbias);
    printf("tzname0=%s tzname1=%s\n", _tzname[0], _tzname[1]);
    lt = _localtime64(&t);
    printf("localtime=%04d-%02d-%02d %02d:%02d:%02d isdst=%d yday=%d\n", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
           lt->tm_hour, lt->tm_min, lt->tm_sec, lt->tm_isdst, lt->tm_yday);
    printf("ctime64=%s", _ctime64(&t));
    t = 1120000000;  /* 2005-06-28, the other half of the year */
    lt = _localtime64(&t);
    printf("localtime_jun=%04d-%02d-%02d %02d:%02d:%02d isdst=%d\n", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
           lt->tm_hour, lt->tm_min, lt->tm_sec, lt->tm_isdst);
    return 0;
}
