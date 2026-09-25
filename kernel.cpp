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
    unsigned short* VideoMemory = (unsigned short*)0xb8000;
    int i = 0;
    for (i = 0 ; str[i] != '\0';i++) {
       VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
       
}
}
extern "C" void kernel_main(void* multiboot_structure, unsigned int magicnumber){
     clear_screen();
     printf("Merhaba DUnya! Isletim sistemim hazir.");
     while(1);
}
