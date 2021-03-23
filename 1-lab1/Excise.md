# Excise

## Exercise 1.1

请修改include.mk文件，使交叉编译器的路径正确。之后执行make指令，如果配置一切正确，则会在gxemul目录下生成vmlinux的内核文件。

```
CROSS_COMPILE := /OSLAB/compiler/usr/bin/mips_4KC-
```

## Exercise 1.2

**阅读./readelf 文件夹中 kerelf.h、readelf.c 以及 main.c 三个文件中的代码，并完成 readelf.c 中缺少的代码，readelf 函数需要输出 elf 文件的所有 section header 的序号和其中记录的相应section的地址信息，对每个 section header，输出格式为:"%d:0x%x\n"，两个标识符分别代表序号和地址。序号即第几个section header，从0开始。**

**正确完成 readelf.c 代码之后，在 readelf 文件夹下执行 make 命令，即可生成可执行文件 readelf，它接受文件名作为参数，对 elf 文件进行解析**

fseek函数重定位指针

ftell返回指针位置


## Exercise 1.3

填写tools/scse0_3.lds中空缺的部分，将内核调整到正确的位置上。在 lab1 中，只需要填补.text、 .data 和.bss 段将内核调整到正确的位置上即可。


## Exercise 1.4

**完成boot/start.S中空缺的部分。设置栈指针，跳转到main函数。 使用gxemul -E testmips -C R3000 -M 64 elf-file运行(其中elf-file是你编译生成的vmlinux文件的路径)。关于如何退出gxemul，请看教程中“如何退出gxemul”一节。**
