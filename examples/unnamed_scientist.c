#include "headcrab.h"
#include <stdarg.h>
#include <stdio.h>

int actually_fprintf(const char * stuff, ...)
{
	va_list arg_list;
	va_start(arg_list, stuff);
	vfprintf(stderr, stuff, arg_list);
	va_end(arg_list);
	return 0;
}

int main(int argc, char ** argv)
{
	headcrab_init("./");
	printf("FUCK YOU\n");
	LOG_MSG("We should not be able to do this.\n");
	headcrab_set_logging_callback(actually_fprintf);
	printf("FUCK YOU AGAIN!\n");
	LOG_MSG("We should not be able to do this.\n");
	getchar();
	return 0;
}
