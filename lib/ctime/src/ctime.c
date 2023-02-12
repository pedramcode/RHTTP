#include "ctime.h"

char *ctime_get_now_str() {
    time_t now = time(NULL);
    struct tm *tm_now = gmtime(&now);

    char *now_str = malloc(sizeof(char) * 20);
    if (now_str == NULL) {
        return NULL;
    }

    strftime(now_str, 20, "%Y-%m-%d %H:%M:%S", tm_now);

    return now_str;
}

time_t ctime_get_now() {
    return time(NULL);
}

time_t ctime_get_from_str(char *date) {
    struct tm tm_date;

    sscanf(date, "%d-%d-%d %d:%d:%d", &tm_date.tm_year, &tm_date.tm_mon, &tm_date.tm_mday,
           &tm_date.tm_hour, &tm_date.tm_min, &tm_date.tm_sec);

    tm_date.tm_year -= 1900;
    tm_date.tm_mon--;

    return timegm(&tm_date);
}

char *ctime_str_from_time(time_t date) {
    struct tm *tm_date = gmtime(&date);

    char *date_str = malloc(sizeof(char) * 20);
    if (date_str == NULL) {
        return NULL;
    }

    strftime(date_str, 20, "%Y-%m-%d %H:%M:%S", tm_date);

    return date_str;
}

