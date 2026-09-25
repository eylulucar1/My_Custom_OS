#include "types.h"

void clear_screen() {
    char *vidmem = (char *) 0xb8000;
    int i = 0;
    while (i < 80 * 25 * 2) {
       vidmem[i] = ' ';
       vidmem[i+1] = 0x0f;
       i = i + 2;
    }
}
void printf(const char* str) {
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    int i = 0;
    for (i = 0 ; str[i] != '\0';i++) {
       VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
}
}
typedef void(*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
     for(constructor* i = &start_ctors;i != &end_ctors;i++)
         (*i)();
}


extern "C" void kernel_main(void* multiboot_structure, uint32_t magicnumber){
     clear_screen();
     printf("Merhaba DUnya! Isletim sistemim hazir.");
     while(1);
}
