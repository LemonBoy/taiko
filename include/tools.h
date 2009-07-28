#ifndef _TOOLS_MODULE
#define _TOOLS_MODULE

#ifdef DEBUG
	#define debugPrint(fmt, args...) \
		printf("\t %s:%d:" fmt, __FUNCTION__, __LINE__, ##args)
#else
	#define debugPrint(fmt, args...)
#endif

void __clearConsole();
void __rebootWii();
void __shutdownIos();
void __hexdump(void *d, int len);
void __setupRam();

#endif
