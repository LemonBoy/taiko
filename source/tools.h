#ifndef _TOOLS_MODULE
#define _TOOLS_MODULE

void debugPrint(const char *text, ...);
void __clearConsole();
void __rebootWii();
void __shutdownIos();
void __hexdump(void *d, int len);
void __setupRam();

#endif
