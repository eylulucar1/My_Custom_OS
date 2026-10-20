#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "driver.h"
#include "keyboard.h"
#include "mouse.h"
#include "pci.h"
#include "vga.h"
void printf(char* str);
class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
    public:
        void OnKeyDown(char c)
        {
            char* foo = " ";
            foo[0] = c;
            printf(foo);
        }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
    public:
        MouseToConsole() { x = 40; y = 12; }
        
        void OnMouseMove(int xoffset, int yoffset)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
           
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                                | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                                | ((VideoMemory[80*y+x] & 0x00FF));
            
            x += xoffset;
            if(x < 0) x = 0;
            if(x >= 80) x = 79;
            y += yoffset;
            if(y < 0) y = 0;
            if(y >= 25) y = 24;
            
          
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                                | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                                | ((VideoMemory[80*y+x] & 0x00FF));
        }
};
void clear_screen() {
    char *vidmem = (char *) 0xb8000;
    int i = 0;
    while (i < 80 * 25 * 2) {
       vidmem[i] = ' ';
       vidmem[i+1] = 0x0f;
       i = i + 2;
    }
}

void printfHex(uint8_t key)
{
    char* hex = "0123456789ABCDEF";
    char foo[] = "00";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    printf(foo);
}

void printf(char* str){
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


extern "C" void kernel_main(void* multiboot_structure, uint32_t magicnumber)
{
     clear_screen();
     printf("Hello World! My Operating System is ready.\n");
     
     GlobalDescriptorTable gdt;
     InterruptManager interrupts(&gdt);
     
     printf("Initializing Hardware, Stage 1\n");
     
     DriverManager drvManager;
     

     PrintfKeyboardEventHandler kbhandler;
     KeyboardDriver keyboard(&interrupts, &kbhandler);
     drvManager.AddDriver(&keyboard);
      

     MouseToConsole mousehandler;
     MouseDriver mouse(&interrupts, &mousehandler);
     drvManager.AddDriver(&mouse);
     
     printf("Initializing Hardware, Stage 2\n");    
     drvManager.ActivateAll();
     
     PeripheralComponentInterconnectController pciController;
     pciController.SelectDrivers(&drvManager, &interrupts);
    
     printf("Initializing Hardware, Stage 3\n");
     interrupts.Activate();
     VideoGraphicsArray vga;
     vga.SetMode(320, 200, 8);
     vga.FillRectangle(0, 0, 320, 200, 0x00, 0x00, 0xA8); 
     while(1);
}
