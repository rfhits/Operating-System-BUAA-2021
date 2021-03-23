# 上机

分成exam和Extra，主要考察的还是elf文件格式。

粗略画了下elf文件的示意图，其中{}表示结构体，[]表示数组

```
|--------------------
|elf_header{
|       section_header_table_offset
|       program_header_table_offset
|       ...
|}
|--------------------
|       ...
|--------------------
|       section header table:
|               an array of section header{}
|       section header-1 {
|               section addr
|               ...
|       }
|
|       section header-2 {
|               section addr
|       }
|
|       ...
|--------------------
```

## exam

lab1课下，只需将section addr输出，课上，要将program header里的offset和align输出。

## Extra

### 题目

我们知道，数据的存储分为大小端，并且elf header中的e_ident[5]指明了文件中的数据编码格式，你也可以在ELF手册中找到相关的信息。

现在，要你修改readelf.c文件，

当其解析大端存储的elf格式文件时，输出每个section的addr

当其解析小端存储的文件时，输出每个program的filesz和memsz

gexmul文件夹内的vmlinux文件以大端存储，readelf文件夹内testELF文件以小端存储，它们可以用于测试你改写的readelf文件的正确性。

### 解决

难点有两个，一是大端存储怎么转换为小端存储，二是要把哪些数据从大端转换成小端。

先简单讲下大小端，大端`0x12_34_56_78`，对应小端就是`0x78_56_34_12`，将四个字节reverse即可。

这一部分可以看往届学长的实现

对于难点一：

```
#define REVERSE_32(n_n) \
	((((n_n)&0xff) << 24) | (((n_n)&0xff00) << 8) | (((n_n) >> 8) & 0xff00) | (((n_n) >> 24) & 0xff))

#define REVERSE_16(n_n) \
	((((n_n)&0xff) << 8) | (((n_n) >> 8) & 0xff))
```

就是拿到一个32位的integer，通过位运算，取得想要的字节，再左右移动，使得顺序改变即可。

或是如此:

```
uint32_t rvs_32(uint32_t* ptr) {
        u_char char1, char2, char3, char4;
        char1 = *((u_char*) ptr);
        char2 = *((u_char*) ptr + 1);
        char3 = *((u_char*) ptr + 2);
        char4 = *((u_char*) ptr + 3);
        return ((uint32_t) char1 << 24) + ((uint32_t) char2 << 16) + ((uint32_t) char3 << 8) + ((uint32_t) char4);
}

uint16_t rvs_16(uint16_t* ptr) {
        u_char char1, char2;
        char1 = *((u_char*) ptr);
        char2 = *((u_char*) ptr + 1);
        // char3 = *((U_char*) ptr + 2);
        // cahr4 = *((u_char*) ptr + 3);
        return ((uint16_t) char1 << 8) + ((uint16_t) char2);
}
```

先给个指向32bit的指针，把指针给强转成指向8bit，然后把这8bit一个一个通过*运算符拿出来，最后把8bit强转位32bit，拼接好就行。

第二个难点——哪些数据需要反转

或许有人说，输出地址，那直接把地址反转就好了，其实不然。

首先要通过`offset`拿到`program header table`的地址时，`offset`需要翻转。

遍历每个`segement`时,需要`count`和`size`，这个也需要翻转。

所以，对于本次`Extra`，有四处需要翻转。

## 总结

Extra只拿了50分,因为debug失败，为了得点分，Extra只做了小端的题。然后就是回来复盘

1. 如果以某种方式了解到了课上的题目，比如lab1-1-Extra考察endian，那么可以先自己实现到服务器里
2. 如果想复盘的话，不要把无关代码删掉，尽量注释掉，方便课下看看自己当时写得咋样

最后是我的bug：

`undefined reference to "function_name"`，据我的猜测，应该是命名函数时，使用了rvs表示reverse，在调用时，打成了rsv，所以没法编译成功。

不禁要抱怨下学校服务器，gcc版本非常老，如果有error，会告诉大家犯了啥错，而不会给出可能犯错的原因。如果我的代码在自己的机子上跑，gcc会提示我是否将rsv或rvs两个函数名打错了，~非常贴心~。
