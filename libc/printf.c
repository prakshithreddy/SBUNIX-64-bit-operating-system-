#include<stdio.h>
#include<sys/syscall.h>
#include<stdarg.h>

/*int printf(const char *fmt, ...)
{
    va_list valist;
    va_start(valist, fmt);
    while(*fmt != '\0'){
        if(*fmt == '\n'){
            putchar('\n');
            fmt+=1;
        }
        else if(*fmt == '\r'){
            putchar('\r');
            fmt+=1;
        }
        else if(*fmt == '\b'){//TODO: Make sure it can handle \b just after \n,\r. For now not handling it.
            putchar('\b');
            fmt+=1;
        }
        else if(*fmt != '%'){
            putchar(*fmt);
            fmt+=1;
        }
        else{
            fmt+=1;
            if(*fmt == '\\'){
                //raise unsupported error
                ;
            }
            else if(*fmt == '%'){
                putchar('%');
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
                    putchar(c+'0');
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
                    putchar(c+'0');
                    y=y%dig_count;
                    dig_count/=10;
                }
            }
            else if(*fmt == 'c'){
                char c = va_arg(valist,int);
                if(c == '\n'){
                    putchar('\n');
                    fmt+=1;
                    continue;
                }
                else if(*fmt == '\r'){
                    putchar('\r');
                    fmt+=1;
                    continue;
                }
                putchar(c); 
            }
            else if(*fmt == 's'){
                char *c = va_arg(valist,char *);
                while(*c != '\0'){
                    if(*c == '\n'){
                        putchar('\n');
                        c+=1;
                        continue;
                    }
                    else if(*c == '\r'){
                        putchar('\r');
                        c+=1;
                        continue;
                    }
                    putchar(*c);
                    c+=1;
                }
            }
            else if(*fmt == 'x'){
                int x = va_arg(valist,int);
                int y = x;
                int dig_count = 1;
                putchar('0');
                putchar('x');
                while(x>=16){
                    x = x/16;
                    dig_count*=16;
                }
                while(dig_count>0){
                    int c = y/dig_count;
                    if(c<10){
                        putchar(c+'0');
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
                        putchar(d);
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
                putchar('0');
                putchar('x');
                while(x>=16){
                    x = x/16;
                    dig_count*=16;
                }
                while(dig_count>0){
                    int c = y/dig_count;
                    if(c<10){
                        putchar(c+'0');
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
                        putchar(d);
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
    va_end(valist);
    return 0;
}*/