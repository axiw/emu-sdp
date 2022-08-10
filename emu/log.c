#include <stdio.h>
#include <stdarg.h>

int log_level;

void ponii_log(int level, const char* format, ...) {
	if (level > log_level)
		return;
	
	va_list va;
	va_start(va, format);
	vprintf(format, va);
	va_end(va);
}
