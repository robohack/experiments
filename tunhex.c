#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char *argv[])
{
    int n;
    char str[255];
    FILE *fp,*fp2;
 
    if(argc != 2){
        puts("Usage: unhex [file_name]");
        return 0;
    }
 
    if((fp = fopen(argv[1], "rb")) == NULL){
        printf("Error: %s Not Available.\n", argv[1]);
        return 0;
    }
 
    printf("out file name:");
    scanf("%254s%*[^\n]", str);
 
    if((fp2 = fopen(str, "wb")) == NULL){
        printf("Error: %s Not Available.\n", str);
        return 0;
    }
 
    while(fscanf(fp, "%x", &n) != EOF)
        fputc(n, fp2);
 
    fclose(fp);
    fclose(fp2);
 
    return 0;
}