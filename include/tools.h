#ifndef _TOOLS_MODULE
#define _TOOLS_MODULE

#define read8(address) (*(u8*)address)
#define write8(address, value) (*(u8*)address = (u8)value)

#define read16(address) (*(u16*)address)
#define write16(address, value) (*(u16*)address = (u16)value)

#define read32(address) (*(u32*)address)
#define write32(address, value) (*(u32*)address = (u32)value)

void __clearConsole();
void __rebootWii();
void __shutdownIos();
void __hexdump(void *d, int len);
void __setupRam();
int __errorCheck(int ret, int fatal);
u32 getLowestMem2Address();
u32 checkAvailableMem();

#endif
