#ifndef _TOOLS_MODULE
#define _TOOLS_MODULE

void __clearConsole();
void __rebootWii();
void __shutdownIos();
void __hexdump(void *d, int len);
void __setupRam();
int __errorCheck(int ret, int fatal);

#endif
