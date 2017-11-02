#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/keyboard.h>
#include <sys/kprintf.h>


struct idt_descriptor {
    uint16_t id_base_lo;
    uint16_t id_sel;
    uint8_t id_always0;
    uint8_t id_flags;
    uint16_t id_base_middle;
    uint32_t id_base_high;
    uint32_t id_always00;
}__attribute__((packed));


struct idtr_t {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));


struct idt_descriptor idt[256];
static const struct idtr_t idtp = {(sizeof(struct idt_descriptor)*256)-1,(uint64_t)idt};

void id_set_gate(uint8_t intr_num,uint64_t base_addr, uint8_t sel,uint8_t flags)
{
    idt[intr_num].id_flags = flags;
    idt[intr_num].id_always0 = 0;
    idt[intr_num].id_always00 = 0;
    idt[intr_num].id_sel = sel;
    
    //ADDRESS TO THE INTERRUPT SERVICE ROUTINE METHOD
    
    idt[intr_num].id_base_lo = (base_addr & 0xFFFF);
    idt[intr_num].id_base_middle = (base_addr >> 16) & 0xFFFF;
    idt[intr_num].id_base_high = (base_addr >> 32) & 0xFFFFFFFF;
    
}

static int shift=0,control=0;

void _key_press_handler(){
    
    unsigned char a;
    
    a = inb(0x60);
    
    //kprintf("\n%d\n",a);
    
    if(a<128) {
        
        switch(a)
        {
            case 42:
                shift=1;break;
            case 54:
                shift=1;break;
            case 29:
                control = 1;break;
            case 58:
                keypress_bar("LAST PRESSED: Caps",14);break;
            case 14:
                keypress_bar("LAST PRESSED: Backspace",14);break;
            case 28:
                keypress_bar("LAST PRESSED: Enter",14);break;
            case 77:
                keypress_bar("LAST PRESSED: Right Arrow",14);break;
            case 75:
                keypress_bar("LAST PRESSED: Left Arrow",14);break;
            case 72:
                keypress_bar("LAST PRESSED: Up Arrow",14);break;
            case 71:
                keypress_bar("LAST PRESSED: Home",14);break;
            case 70:
                keypress_bar("LAST PRESSED: Scroll Lock",14);break;
            case 73:
                keypress_bar("LAST PRESSED: Page Up",14);break;
            case 81:
                keypress_bar("LAST PRESSED: Page Down",14);break;
            case 82:
                keypress_bar("LAST PRESSED: Insert",14);break;
            case 83:
                keypress_bar("LAST PRESSED: Delete",14);break;
            case 79:
                keypress_bar("LAST PRESSED: End",14);break;
            case 80:
                keypress_bar("LAST PRESSED: Down Arrow",14);break;
            case 57:
                keypress_bar("LAST PRESSED: Space",14);break;
            case 56:
                keypress_bar("LAST PRESSED: Alt",14);break;
            case 15:
                keypress_bar("LAST PRESSED: Tab",14);break;
            case 1:
                keypress_bar("LAST PRESSED: Esc",14);break;
            case 59:
                keypress_bar("LAST PRESSED: F1",14);break;
            case 60:
                keypress_bar("LAST PRESSED: F2",14);break;
            case 61:
                keypress_bar("LAST PRESSED: F3",14);break;
            case 62:
                keypress_bar("LAST PRESSED: F4",14);break;
            case 63:
                keypress_bar("LAST PRESSED: F5",14);break;
            case 64:
                keypress_bar("LAST PRESSED: F6",14);break;
            case 65:
                keypress_bar("LAST PRESSED: F7",14);break;
            case 66:
                keypress_bar("LAST PRESSED: F8",14);break;
            case 67:
                keypress_bar("LAST PRESSED: F9",14);break;
            case 68:
                keypress_bar("LAST PRESSED: F10",14);break;
            case 87:
                keypress_bar("LAST PRESSED: F11",14);break;
            case 88:
                keypress_bar("LAST PRESSED: F12",14);break;
                
            default:
                if (shift)
                {
                    //shift = 0;
                    char s[] = {'L','A','S','T',' ','P','R','E','S','S','E','D',':',' ','s','h','i','f','t','+','\'',kbdus[a],'\'','=','\'',CAPS_kbdus[a],'\'','\0'};
                    keypress_bar(s,14);
                }
                else if (control)
                {
                    //control=0;
                    char s[] = {'L','A','S','T',' ','P','R','E','S','S','E','D',':',' ','^',CAPS_kbdus[a],'\0'};
                    keypress_bar(s,14);
                    
                }
                else
                {
                    char s[]={'L','A','S','T',' ','P','R','E','S','S','E','D',':',' ',kbdus[a],'\0'};
                    keypress_bar(s,14);
                }
        }
    }
    else{
      switch(a)
      {
        case 170:
          shift=0;
        case 182:
          shift=0;
        case 157:
          control=0;
      }
    }
    outb(0x20,0x20);
    outb(0x20,0xA0);
}

static int i = 0;
static int hour = 0;
static int minute = 0;
static int seconds = 0;

void _timer_intr_hdlr(){
    
    i++;
    
    if(i%19==0) {
        i = 0;
        seconds++;
        
        if(seconds%60==0) {
            seconds%=60;
            minute+=1;
            if(minute%60==0)
            {
                minute%=60;
                hour++;
            }
        }
        
        boot_time_bar(hour,minute,seconds,14);
        
    }
   
    //send EOI to PIC
    outb(0x20,0x20);
    outb(0x20,0xA0);
}


static int rtc_read_timeout = 0;

void _rtc_intr_hndlr(){
    rtc_read_timeout++;
    
    if(rtc_read_timeout%19==0){
        
        rtc_read_timeout=0;
    
    unsigned char rtc_second;
    outb(0x00,0x70);
    rtc_second = inb(0x71);
    rtc_second = ((rtc_second / 16) * 10) + (rtc_second & 0xf);
    
    unsigned char rtc_minute;
    outb(0x02,0x70);
    rtc_minute = inb(0x71);
    
    unsigned char rtc_hour;
    outb(0x04,0x70);
    rtc_hour = inb(0x71);
    
    unsigned char regb;
    
    outb(0x0B,0x70);
    regb = inb(0x71);
        
    //kprintf("\n%d:%d:%d\n",rtc_hour,rtc_minute,rtc_second);
        
    if (!(regb & 0x04)) {
        //rtc_second=(rtc_second & 0x0F)+((rtc_second/16)*10);
        rtc_minute=(rtc_minute & 0x0F)+ ((rtc_minute/16)*10);
        rtc_hour=((rtc_hour & 0x0F) +(((rtc_hour & 0x70)/16)*10));
    }
    
    if (!(regb & 0x02) && (rtc_hour & 0x80)) {
            rtc_hour = ((rtc_hour & 0x7F) + 12) % 24;
    }
        
    //kprintf("\n%d:%d:%d\n",rtc_hour,rtc_minute,rtc_second);
    //NYC Time   
    rtc_hour=rtc_hour-4<0?rtc_hour-4+24:rtc_hour-4;
    
    time_bar(rtc_hour,rtc_minute,rtc_second,14);
        
    }
    
    outb(0x20,0x20);
    outb(0x20,0xA0);
    
    outb(0x0C,0x70);
    inb(0x71);
   
    
}




void _key_board_intr();


void _timer_intr();

void _rtc_intr();

void init_idt()
{
    id_set_gate(40,(uint64_t)_rtc_intr,8,0x8E);
    id_set_gate(33,(uint64_t)_key_board_intr,8,0x8E);
    id_set_gate(32,(uint64_t)_timer_intr,8,0x8E);
   // id_set_gate(70,(uint64_t)_rtc_intr,8,0x8E);
    
    __asm__ __volatile__("lidt %0" : : "m" (idtp));
    
}
