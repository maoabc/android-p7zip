//============================================================================
// Name        : SevenZipBinding.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <stdlib.h>

#include "SevenZipJBinding.h"


/**
 * Fatal error
 */
void fatal(const char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fputs("FATAL ERROR: ", stdout);
	vprintf(fmt, args);
	va_end(args);

	fputc('\n', stdout);
	fflush(stdout);

	TRACE_PRINT_OBJECTS

	// exit(-1);

	printf("Crash jvm to get a stack trace\n");
	fflush(stdout);
	int * i = NULL;
	*i = 0;
}

