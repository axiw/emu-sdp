#ifndef __LOG_H__
#define __LOG_H__

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_EXTRA_INFO 2

extern int log_level;

void ponii_log(int level, const char* format, ...);

#endif
