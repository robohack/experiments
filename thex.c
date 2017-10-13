#include <stdio.h>
 
int main(int argc, char *argv[])
{
    int n;
    unsigned long long int count = 0;
    FILE *fp;
 
    if(argc != 2){
        puts("Usage: hex [file_name]");
        return 0;
    }
 
    if((fp = fopen(argv[1], "rb")) == NULL)
        printf("Error: %s is not found.\n", argv[1]);
    else{
        while((n = fgetc(fp)) >= 0){
            printf("%02x ", n);
            count++;
            if(count%16 == 0)
                putchar('\n');
        }
        fclose(fp);
    }
 
    return 0;
}