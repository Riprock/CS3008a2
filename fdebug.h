/*
  Fergal's debugging macros
*/

#if DEBUG == 1
#include <unistd.h>
#define DPRINTF(...) fprintf(stderr, __VA_ARGS__)
#define DUSLEEP(X) usleep((X))
#else
#define DPRINTF(...)
#define DUSLEEP(X)
#endif

#define SHOW_ERROR(func, ...) \
	do {\
		if (func(__VA_ARGS__) < 0) {\
			DPRINTF("[%s] %d: ", __FILE__, __LINE__);\
			perror(0);\
		}\
	} while (0)

#define REPORT_ERROR(value) {\
	do {\
		if (value < (long) 0) {\
			DPRINTF("[%s] %d: ", __FILE__, __LINE__);\
			fprintf(stderr, "%s\n", strerror((long) value));\
		}\
	} while (0)

#define SHOW_EXIT_ERROR(func, ...)\
	do {\
		if (func(__VA_ARGS__) < 0) {\
			DPRINTF("[%s] %d: ", __FILE__, __LINE__);\
			perror(0);\
			return errno;\
		}\
	} while (0)

#define REPORT_EXIT_ERROR(value)\
	do {\
		if (value < 0) {\
			DPRINTF("[%s] %d: ", __FILE__, __LINE__);\
			fprintf(stderr, "%s\n", strerror((long) value));\
			return (long) value;\
		}\
	} while (0)
