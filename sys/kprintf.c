#include <sys/kprintf.h>
#include <sys/defs.h>
#include <stdarg.h>


static int v_line_count = 1;
static char *line_adr = (char *)0xb8000;
static char *next_line_adrs = (char *)0xb8000+160;


void check_if_line_full(char **vm_adr,char **next_line_adr){
    if(*vm_adr==*next_line_adr){
        v_line_count++;
        //kprintf("     Line Number: %d",v_line_count);
        if(v_line_count > 25){
            //kprintf("     Line Number: %d",v_line_count);
            for(char * temp2 = (char*)0xb8000; temp2 < (char*)0xb8000+160*24; temp2 += 1) *temp2 = *(temp2+160);
            for(char * temp2 = (char*)0xb8000+160*24; temp2 < (char*)0xb8000+160*25; temp2 += 1) *temp2 = '\0';
            //v_line_count++;
            *vm_adr = *next_line_adr-160;
        }
        else{
            *next_line_adr = *next_line_adr+160;
        }
    }
}


void kprintf(const char *fmt, ...)
{
    char *vm_adr = line_adr;
    char *next_line_adr = next_line_adrs;
    char **next_line_ptr = &next_line_adr;
    char **present_line_ptr = &vm_adr;
    for(char * temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 6; /* white */;
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
            /*else if(*fmt == 'd'){
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
             check_if_line_full(vm_adr,next_line_ptr);
             y=y%dig_count;
             dig_count/=10;
             }
             //n_args++;
             }*/
            else if(*fmt == 'c'){
                char c = va_arg(valist,uint8_t);
                *vm_adr = c;
                vm_adr+=2;
                check_if_line_full(present_line_ptr,next_line_ptr);
            }
            else if(*fmt == 's'){
                char *c = va_arg(valist,char *);
                while(*c != '\0'){
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
