#include<stdio.h>
#include<stdbool.h>

#define ever ;;

int main()
{
    FILE *fptr;

    fptr = fopen("/dev/my_gpio_driver", "wb");

    if(fptr == NULL)
    {
        printf("Error while accessing gpio driver!\n");
        return -1;
    }

    for(ever)
    {
        fprintf(fptr, "1");

        for(int i=0; i < 0xFFF; i++)
        {
            __asm("nop");
        }

        fprintf(fptr, "0");
    }

    fclose(fptr);

    return 0;
}