#include <stdio.h>
#include <stdlib.h>
#include "types.h"

extern int readelf(u_char *binary, int size);
/*
        overview: input a elf format file name from control line, call the readelf function
                  to parse it.
        params:
                argc: the number of parameters
                argv: array of parameters, argv[1] shuold be the file name.

*/
int main(int argc, char *argv[])
{
        FILE *fp;
        int fsize;
        unsigned char *p;

        if (argc < 2)
        {
                printf("Please input the filename.\n");
                return 0;
        }
        if ((fp = fopen(argv[1], "rb")) == NULL)
        {
                printf("File not found\n");
                return 0;
        }

        // 将fp定位到文件结尾，通过ftell函数获得文件大小
        fseek(fp, 0L, SEEK_END);
        fsize = ftell(fp);

        p = (u_char *)malloc(fsize + 1);
        if (p == NULL)
        {
                fclose(fp);
                return 0;
        }

        // 将fp定位回文件开头
        fseek(fp, 0L, SEEK_SET);

        // 将文件内容写入p指向的内存块，并在结尾补0
        fread(p, fsize, 1, fp);
        p[fsize] = 0;

        // 调用read_elf函数，参数分别为指针和内存大小
        readelf(p, fsize);
        return 0;
}
