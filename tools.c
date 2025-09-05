#include <stdint.h>
#include <sys/time.h>  // struct timeval tv;
#include <stdio.h>
#include <ctype.h>     //isspace
#include <string.h>  //strlen

char* trim(char* str) 
{
    char* end;

    // Left
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // Ha csak sz�k�z volt
        return str;

    // Right
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Null
    *(end + 1) = '\0';

    return str;
}

long HAL_GetTick(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long timestamp_ms =  (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return timestamp_ms;
}