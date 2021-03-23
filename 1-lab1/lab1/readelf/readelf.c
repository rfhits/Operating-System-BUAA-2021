/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>
/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
        if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
            ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
            ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
            ehdr->e_ident[EI_MAG3] == ELFMAG3)
        {
                return 1;
        }

        return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
    Exercise 1.2. Please complete func "readelf". 
*/
int readelf(u_char *binary, int size)
{
        // 32位ELF文件的
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary; // ehdr指向了elf_head

        int Nr;

        
        Elf32_Shdr *shdr = NULL; // 32位ELF文件的 Section segment header

        u_char *ptr_sh_table = NULL;
        Elf32_Half sh_entry_count;
        Elf32_Half sh_entry_size;

        // check whether `binary` is a ELF file.
        if (size < 4 || !is_elf_format(binary))
        {
                printf("not a standard elf format\n");
                return 0;
        }

        // get section table addr, section header number and section header size.
        shdr = binary + ehdr->e_shoff; // 一定要写binary，不能写ehdr，要找到节头表在file中的地址

        // 节头表作为一个数组，数组中几个元素？每个元素的大小是多少？
        // 大小可以用于切换索引的元素
        sh_entry_count = ehdr->e_shnum;
        sh_entry_size = ehdr->e_shentsize;

        // for each section header, output section number and section addr.
        // hint: section number starts at 0.
        ptr_sh_table = shdr;
        for (Nr = 0; Nr < sh_entry_count; Nr++)
        {
                Elf32_Addr sh_addr = ((Elf32_Shdr *)ptr_sh_table)->sh_addr;
                printf("%d:0x%x\n", Nr, sh_addr);
                ptr_sh_table += sh_entry_size;
        }

        return 0;
}
