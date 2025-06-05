#include <stdint.h>
#include <sys/time.h> // struct timeval tv;
#include <stdio.h>


long HAL_GetTick(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long timestamp_ms =  (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return timestamp_ms;
}