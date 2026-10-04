#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
void clear_screen() {
    char *vidmem = (char *) 0xb8000;
    int i = 0;
    while (i < 80 * 25 * 2) {
       vidmem[i] = ' ';
       vidmem[i+1] = 0x0f;
       i = i + 2;
    }
}
void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0, y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
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
     
     GlobalDescriptorTable gdt;
     
     InterruptManager interrupts(&gdt);
     KeyboardDriver keyboard(&interrupts);
     interrupts.Activate();

while(1);
     while(1);
}

