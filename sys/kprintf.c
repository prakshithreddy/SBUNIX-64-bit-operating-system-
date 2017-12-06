#include <sys/kprintf.h>
#include <sys/defs.h>
#include <stdarg.h>
static uint64_t VGA_VIRTUAL_ADDRESS=0xb8000;

/*void set_vga_virtual_address(uint64_t adr){
  VGA_VIRTUAL_ADDRESS=adr;
}

char* get_vga_virtual_address(){
  return (char*)VGA_VIRTUAL_ADDRESS;
}*/

static int v_line_count = 1;
static char *line_adr = (char*)0xb8000;
static char *next_line_adrs = (char*)0xb8000+160;

void mapVGA(uint64_t vga_virtual_adrs){
    VGA_VIRTUAL_ADDRESS = vga_virtual_adrs;
    next_line_adrs=(char*)(VGA_VIRTUAL_ADDRESS+(next_line_adrs-0xb8000));
    line_adr=(char*)(VGA_VIRTUAL_ADDRESS+(line_adr-0xb8000));
}

void check_if_line_full(char **vm_adr,char **next_line_adr){
    if(*vm_adr==*next_line_adr){
        v_line_count++;
        //kprintf("     Line Number: %d",v_line_count);
        if(v_line_count > 24){
            //kprintf("     Line Number: %d",v_line_count);
            for(char * temp2 = (char*)VGA_VIRTUAL_ADDRESS; temp2 < (char*)VGA_VIRTUAL_ADDRESS+160*23; temp2 += 1) *temp2 = *(temp2+160);
            for(char * temp2 = (char*)VGA_VIRTUAL_ADDRESS+160*23; temp2 < (char*)VGA_VIRTUAL_ADDRESS+160*24; temp2 += 1) *temp2 = '\0';
            //v_line_count++;
            *vm_adr = *next_line_adr-160;
        }
        else{
          *next_line_adr = *next_line_adr+160;
            /*while(*next_line_adr < *next_line_adr+160){
              **next_line_adr = 0x20;
              *next_line_adr=*next_line_adr+2;
            }*/
            
        }
    }
}
char * time_concatenate(char *s,char *s1,char *hours,char *minutes,char *seconds){
    //static char s[35];
    int z=0;
    while(*s1 != '\0'){
        s[z] = *s1;
        z++;
        s1++;
    }
    while(*hours != '\0'){
        s[z] = *hours;
        z++;
        hours++;
    }
    s[z] = ':';
    z++;
    while(*minutes != '\0'){
        s[z] = *minutes;
        z++;
        minutes++;
    }
    s[z] = ':';
    z++;
    while(*seconds != '\0'){
        s[z] = *seconds;
        z++;
        seconds++;
    }
    s[z] = '\0';
    return s;
}

char * itoa(char *s,int x){
    int z = 0;
    //static char s[6];
    int y = x;
    int dig_count = 1;
    while(x>=10){
        x = x/10;
        dig_count*=10;
    }
    if(dig_count == 1){
        s[z] = '0';
        z++;
    }
    while(dig_count>0){
        int c = y/dig_count;
        s[z] = c+'0';
        z++;
        y=y%dig_count;
        dig_count/=10;
    }
    s[z] = '\0';
    return s;
}

void time_bar(int hours,int minutes,int seconds,int color){
    char *pointer = (char*)(VGA_VIRTUAL_ADDRESS+0xF00);
    char *c_pointer = (char*)(VGA_VIRTUAL_ADDRESS+0xF01);
    char h_pointer[6];
    char *h_pointer1 = itoa(h_pointer,hours);
    char m_pointer[6];
    char *m_pointer1 = itoa(m_pointer,minutes);
    char s_pointer[6];
    char *s_pointer1 = itoa(s_pointer,seconds);
    char *string = "TIME(NYC) : ";
    char s1[35];
    char *s = time_concatenate(s1,string,h_pointer1,m_pointer1,s_pointer1);
    //int z=0;
    while(pointer < (char*)(VGA_VIRTUAL_ADDRESS+0xF32)){
        if(*s != '\0'){
            *pointer = *s;
            *c_pointer = color;
            s++;
        }
        else{
            *pointer = 0x20;
            *c_pointer = color;
        }
        pointer+=2;
        c_pointer+=2;
    }
}

void keypress_bar(char *s,int color){
    char *pointer = (char*)(VGA_VIRTUAL_ADDRESS+0xF32);
    char *c_pointer = (char*)(VGA_VIRTUAL_ADDRESS+0xF33);
    while(pointer < (char*)(VGA_VIRTUAL_ADDRESS+0xF6E)){
        if(*s != '\0'){
            *pointer = *s;
            *c_pointer = color;
            s++;
        }
        else{
            *pointer = 0x20;
            *c_pointer = color;
        }
        pointer+=2;
        c_pointer+=2;
    }
}

void boot_time_bar(int hours,int minutes,int seconds,int color){
    char *pointer = (char*) (VGA_VIRTUAL_ADDRESS+0xF6E);
    char *c_pointer = (char*) (VGA_VIRTUAL_ADDRESS+0xF6F);
    char h_pointer[6];
    char *h_pointer1 = itoa(h_pointer,hours);
    char m_pointer[6];
    char *m_pointer1 = itoa(m_pointer,minutes);
    char s_pointer[6];
    char *s_pointer1 = itoa(s_pointer,seconds);
    char *string = "timeSinceBoot: ";
    char s1[35];
    char *s = time_concatenate(s1,string,h_pointer1,m_pointer1,s_pointer1);
    //int z=0;
    while(pointer < (char*)(VGA_VIRTUAL_ADDRESS+0xFA0)){
        if(*s != '\0'){
            *pointer = *s;
            *c_pointer = color;
            s++;
        }
        else{
            *pointer = 0x20;
            *c_pointer = color;
        }
        pointer+=2;
        c_pointer+=2;
    }
}

void kprintf(const char *fmt, ...)
{
    char *vm_adr = line_adr;
    char *next_line_adr = next_line_adrs;
    char **next_line_ptr = &next_line_adr;
    char **present_line_ptr = &vm_adr;
    for(char * temp2 = (char*)(VGA_VIRTUAL_ADDRESS+0x1); temp2 < (char*)VGA_VIRTUAL_ADDRESS+160*24; temp2 += 2) *temp2 = 7; /* white */;
    va_list valist;
    va_start(valist, fmt);
    while(*fmt != '\0'){
        if(*fmt == '\n'){
            while(vm_adr != next_line_adr){
                *vm_adr = 0x20;
                vm_adr+=2;
            }
            check_if_line_full(present_line_ptr,next_line_ptr);
            fmt+=1;
        }
        else if(*fmt == '\r'){
            while(vm_adr != next_line_adr-160){
                //*vm_adr = 0x20;
                vm_adr-=2;
            }
            fmt+=1;
        }
        else if(*fmt == '\b'){//TODO: Make sure it can handle \b just after \n,\r. For now not handling it.
            if(vm_adr == next_line_adr-160){
                next_line_adr=next_line_adr-160;
            }
            vm_adr-=2;
            *vm_adr=0x20;
            fmt+=1;
        }
        else if(*fmt != '%'){
            *vm_adr = *fmt;
            fmt+=1;
            vm_adr+=2;
            check_if_line_full(present_line_ptr,next_line_ptr);
        }
        else{
            fmt+=1;
            if(*fmt == '\\'){
                //raise unsupported error
                ;
            }
            else if(*fmt == '%'){
                *vm_adr = '%';
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
            }
            else if(*fmt == 'd'){
                int x = va_arg(valist,int);
                int y = x;
                int dig_count = 1;
                while(x>=10){
                    x = x/10;
                    dig_count*=10;
                }
                while(dig_count>0){
                    int c = y/dig_count;
                    *vm_adr = c+'0';
                    vm_adr+=2;
                    check_if_line_full(present_line_ptr,next_line_ptr);
                    y=y%dig_count;
                    dig_count/=10;
                }
            }
            else if(*fmt == 'l' && *(fmt+1) == 'd'){
                unsigned long long x = va_arg(valist,unsigned long long);
                unsigned long long y = x;
                unsigned long long dig_count = 1;
                while(x>=10){
                    x = x/10;
                    dig_count*=10;
                }
                while(dig_count>0){
                    int c = y/dig_count;
                    *vm_adr = c+'0';
                    vm_adr+=2;
                    check_if_line_full(present_line_ptr,next_line_ptr);
                    y=y%dig_count;
                    dig_count/=10;
                }
            }
            else if(*fmt == 'c'){
                char c = va_arg(valist,int);
                if(c == '\n'){
                    while(vm_adr != next_line_adr){
                        *vm_adr = 0x20;
                        vm_adr+=2;
                    }
                    check_if_line_full(present_line_ptr,next_line_ptr);
                    fmt+=1;
                    continue;
                }
                else if(*fmt == '\r'){
                    while(vm_adr != next_line_adr-160){
                        //*vm_adr = 0x20;
                        vm_adr-=2;
                    }
                    fmt+=1;
                    continue;
                }
                *vm_adr = c;
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
            }
            else if(*fmt == 's'){
                char *c = va_arg(valist,char *);
                while(*c != '\0'){
                    if(*c == '\n'){
                        while(vm_adr != next_line_adr){
                            *vm_adr = 0x20;
                            vm_adr+=2;
                        }
                        check_if_line_full(present_line_ptr,next_line_ptr);
                        c+=1;
                        continue;
                    }
                    else if(*c == '\r'){
                        while(vm_adr != next_line_adr-160){
                            //*vm_adr = 0x20;
                            vm_adr-=2;
                        }
                        c+=1;
                        continue;
                    }
                    *vm_adr = *c;
                    c+=1;
                    vm_adr+=2;
                    check_if_line_full(present_line_ptr,next_line_ptr);
                }
            }
            else if(*fmt == 'x'){
                int x = va_arg(valist,int);
                int y = x;
                int dig_count = 1;
                *vm_adr = '0';
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
                *vm_adr = 'x';
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
                while(x>=16){
                    x = x/16;
                    dig_count*=16;
                }
                while(dig_count>0){
                    int c = y/dig_count;
                    if(c<10){
                        *vm_adr = c+'0';
                        vm_adr+=2;
                        check_if_line_full(present_line_ptr,next_line_ptr);
                    }
                    else{
                        char d = '-';
                        if(c==10)
                            d='A';
                        if(c==11)
                            d='B';
                        if(c==12)
                            d='C';
                        if(c==13)
                            d='D';
                        if(c==14)
                            d='E';
                        if(c==15)
                            d='F';
                        *vm_adr = d;
                        vm_adr+=2;
                        check_if_line_full(present_line_ptr,next_line_ptr);
                    }
                    y=y%dig_count;
                    dig_count/=16;
                }
            }
            else if(*fmt == 'p'){
                unsigned long long x = (unsigned long long)va_arg(valist,void *);;
                //kprintf("usigned long int in x is %d",x);
                //break;
                unsigned long long y = x;
                unsigned long long dig_count = 1;
                *vm_adr = '0';
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
                *vm_adr = 'x';
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
                while(x>=16){
                    x = x/16;
                    dig_count*=16;
                }
                while(dig_count>0){
                    int c = y/dig_count;
                    if(c<10){
                        *vm_adr = c+'0';
                        vm_adr+=2;
                        check_if_line_full(present_line_ptr,next_line_ptr);
                    }
                    else{
                        char d = '-';
                        if(c==10)
                            d='A';
                        if(c==11)
                            d='B';
                        if(c==12)
                            d='C';
                        if(c==13)
                            d='D';
                        if(c==14)
                            d='E';
                        if(c==15)
                            d='F';
                        *vm_adr = d;
                        vm_adr+=2;
                        check_if_line_full(present_line_ptr,next_line_ptr);
                    }
                    y=y%dig_count;
                    dig_count/=16;
                }
            }
            else{
                //raise not supported error
                ;
            }
            fmt+=1;
        }
    }
    line_adr = vm_adr;
    next_line_adrs = next_line_adr;
    va_end(valist);
}
