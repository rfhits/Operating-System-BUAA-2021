# 上机

参考[roife的博客](https://roife.github.io/2021/05/25/buaa-os-lab5-1-file-system/)

## lab5-1

### exam

> 新建系统调用来完成外设的读写

实验背景：

系统调用的一个重要作用就在于，它为用户进程提供了一种访问外部设备的途径。在完成实验的过程中你已经使用了syscall_read_dev等系统调用来访问磁盘设备，在这里希望你使用系统调用为用户进程提供更加丰富的功能。

#### 第1部分：syscall_get_time

• 添加新的系统调用 int syscall_get_time();（在user/lib.h中添加定义并在已有文件中进行实现）。

该系统调用的返回值是当前的UNIX标准时间（1970年1月1日至今的时间），单位为秒。
• 注意：获取UNIX标准时间需要直接对gxemul进行读取，用其他方式获得时间无法通过评测，对
gxemul设备的具体信息需要参考gxemul文档节选。
提示：

1. 当前UNIX标准时间是一个10位整数，且前三位数为162
2. 当系统调用输出一直为0时，请仔细阅读gxemul的设备规范！！！

#### 第2部分：syscall_read_str

添加新的系统调用 int syscall_read_str(char *buf, int secno); （在user/lib.h中添加定义并在已有文件中进行实现）

功能：系统调用从控制台中读取一个以换行('\r')结尾的字符串，并将读取的字符串写入0号磁盘的secno号扇区中。
系统调用的返回值为读取的字符串的长度，该系统调用需要将读取的字符串存入buf中（buf和写入磁盘的字符串都不包括'\r'且需要以'\0'结尾）。

当系统调用开始之后进入内核态并且不断读取控制台输入，直到读取到结束标志符号'\r'才从内核态返回。（提示：需要循环读取控制台地址，读取到的非零值即为控制台的输入，可参考gxemul文档节选）

保证secno在合适的范围内且buf的长度小于512字节，也就是说只需要写入一个扇区。 本地测试说明：可以使用已有的文件系统进程进行测试

#### 解析

添加系统调用挺烦的，os上机一个难点就是细节非常多，稍不注意，轻则编译错误，重则debug困难。

添加系统调用的流程：

1. include/unistd.h 添加系统调用号
2. user/lib.h 添加syscall
3. user/syscall_lib.c 实现syscall，jump into msyscall
4. lib/syscall.S 在结尾添加系统调用偏移
5. lib/syscall_all.c 实现具体系统调用的操作

最重要的是，trigger别忘了。

```cpp
int sys_get_time(int sysno)
{
        int dev = 0x15000000;
        int offset = 0x0010; // offset is 0x0010
        int phy_addr = dev + offset;
        int time = 0;
        int trigger = 1;
        sys_write_dev(0, &trigger, dev, 4); // trigger device to upd the time
        sys_read_dev(0, &time, phy_addr, 4); // read the time on device
        return time;
}
```

read_str没通过，我老是写不了磁盘，跪了。but，os上机的魅力就是，60=100。

```cpp

int sys_read_str(int sysno, char *buf, int secno)
{
        int dev = 0x10000000;
        char c;
        int len = 0;
        while (1) {
                sys_read_dev(0, &c, dev, 1); // read to c
                if (c == '\r') {
                        buf[len] = '\0';
                        // return len;
                        break; // now len got
                } else if (c == 0) {
                        continue;
                } else { // a regular char
                        buf[len++] = c;
                }
        }
        ...... //自行脑补后半部分
}
```

#### 感想

2020春和2019春，都是在用户态，通过`sys_write_dev()`和`sys_read_dev()`对console进行读写，很重要的就是那些`\r`、`\0`的处理，所以那个`while loop`里面的东西一定要理解。

关于2019和2020年的上机，可以看login256的lab5-1-exam分支，`user/dev_cons.c`和refkxh的`lab5-exam-a`分支，过了一年，把原来用户态实现的函数搬到内核态里了。

它这个题变换莫测，而且越来越难。当然对大佬来说就跟切菜一样简单了。

### Extra

实验背景：

从boot/start.S中可以看出，我们的操作系统在启动之前关闭了所有的中断。而在开始启用进程前，操作系统通过调用kclock_init()函数开启了时钟中断。现在我们希望能够开启操作系统的控制台中断，实现对控制台输入的中断响应。同时我们还希望记录下每一次触发控制台中断的时间。

#### 第1部分：打开控制台中断

首先你需要为系统打开控制台中断。在开启控制台中断时需要建议参考对于时间中断的实现。

建议参考的文件有: lib/genex.S lib/kclock.c lib/kclock_asm.S include/asm/cp0regdef.h。

* 你需要创建include/kcons.h文件，在其中声明函数void kcons_init()并对其进行实现，该函数可以开启
  控制台中断。
* 模仿时钟中断的实现，你需要创建文件lib/kcons.c、lib/kcons_asm.S和lib/handle_cons_ir.c并书写相应函数，并修改genex.S文件，使得触发控制台中断后可以跳转到控制台中断处理函数。除上述文件外请不要创建其他文件

#### 第2部分：处理控制台中断

你需要在handle_cons_ir.c中书写控制台中断的处理函数，

* 处理函数有如下要求：
* 在第一次触发控制台中断时处理函数需要打印printf("CP0 STATUS: %x, 1st interrupt: %d\n", status, time);
  status是进入中断处理程序时CP0 STATUS寄存器的值，time是第一次触发控制台中断时的UNIX标准时间，单位为秒。
* 在之后触发控制台中断时需要打印语句printf("interval: %d\n", interval);
  interval表示当前时间和第一次触发控制台中断的时间差，单位为秒。
* 当interval的值大于等于5时需要结束gxemul的模拟。在结束模拟前需要输出语句：printf("length=%d, string=%s\n", length, string);string是将所有控制台输入的字符进行拼接的结果，length表示string的长度。我们保证在触发控制台中断时仅输入字母和数字，且string的长度不超过128（提示：在处理函数中使用静态字符串数组实现）。

#### 解析

请自行使用beyong compare软件，对login256的lab5-1-Extra部分的代码进行分析。

这部分真的是烦，有以下流程

1. cp0regdef.h，添加status
2. init/init.c 开启kons_init()，关闭kclock_init()
3. include/kcons.h， define a function called `kcons_init()`
4. lib/kcons.c 实现`kcons_init()`，只有一句，jump to `set_console()`
5. lib/kcons_asm.S `set_console()`真正在的地方，可以参考kclock_asm.S
6. lib/genex.S 仿照clock_irq，添加并跳转到一个label `console_irq`，再在这个label中跳转到`handle_cons_ir`
7. lib/handle_cons_ir.c，实现`handle_cons_ir()`，这里就是真正的`handle_cons_ir()`
8. lib/Makefile，将添加的文件加入all

细细看来，一共8步，步步惊心，好在大部分文件的内容几乎就是kclock的复刻。具体细节可以参考login256的lab5-1-Extra和Eadral的lab5-1-Extra。

在我看来，最坑的就是，退出gxemul，这个在文档里，是通过写rtc的一个存储区域实现的，，可惜，最后才注意到这个提示的我时间已经不够了。

所以各位一定要认真看文档（破音！！！

## lab5-2

最后一次上机了，随便拿了60分就走了。听大佬们说，这回`Extra`难度是坠大的。

因为对我而言，实在太难，所以我就随便写一点60分的经验。

这个上机和去年**半毛钱关系都没有**。

如果是去年的话，认真看下`fd_lookup()`和`open()`，要熟悉各个结构体之间的转换。详情请参照Eadral的OS repo。

### exam

实验背景：在阅读文件系统源代码时你可能已经发现，操作系统中并没有提供创建文件的用户接口，这令人感到有些遗憾。因此我们希望你使用文件系统IPC机制给用户提供一个接口，用于对文件的创建。

实现函数 `int user_create(char* path, int isdir);`

其中path表示当前文件相对于根目录的路径，isdir规定了需要创建的类型，当isdir为0时，表示需要创建一个普通文件；当isdir为1时，表示需要创建一个目录。

你需要在user/lib.h中添加相应的定义语句，并在user/file.c中进行实现。

样例说明：

- 使用user_create("/myfile", 0)可以在根目录下创建一个名为myfile的文件。
- 使用user_create("/mydir", 1)可以在根目录下创建一个名为mydir的目录。
- 使用user_create("/mydir/myfile", 0)可以在mydir目录下创建一个名为myfile的文件，当mydir目录不存在时该函数出错。

当创建成功时你需要返回0，其他情况需要返回一个负值代表出错。错误情况如下所示：

1. 同一个目录下不能出现同名文件（大小写敏感），也不能出现同名目录（同上）。当出现同名文件或同名目录时需要返回`-E_FILE_EXISTS`。
2. 当你需要在非根目录下创建文件/目录时，可能出现指定目录不存在的情况，此时需要返回`-E_DIR_NOT_EXIST`（请你在include/mmu.h对这个新的错误类型进行定义，值为13）。
3. 其他错误情况不进行要求，可以返回任意负值。

注意：用户创建时必须使用IPC机制，使用其他方式无法得分！

为了降低实现难度，我们保证在exam测试中，一个目录下不会出现一个文件和一个文件夹同名的情况。

提示：使用文件系统IPC机制需要改动的文件有：include/fs.h    fs/serv.c    user/fsipc.c

本地测试说明：建议使用文件已有的文件系统测试进程进行测试。首先检查init/init.c中是否存在创建user_fstest进程和fs_serv进程的语句，之后可以在user/fstest.c中添加测试语句，对你的函数进行测试。

测试时需要运行的命令：gxemul -E testmips -C R3000 -M 64 -d gxemul/fs.img gxemul/vmlinux

给分标准：

* lab5课下强测通过：10
* exam编译通过：10
* isdir=0：
  1. 创建成功：20
  2. 正确返回-E：20
* isdir=1
  1. 创建成功：20
  2. 正确返回-E：20

#### 解析

这一回其实考察的是user和fsserve之间的接口关系，我们要仿照`file_open()`和`file_close`来完成，需要特别注意的是，我们会层层调用，然后最底层那个函数`file_create()`已经被实现了，所以不用绞尽脑汁去想怎么新建一个文件。

参考给分标准，最简单的其实是成功返回-E。我个人认为，不管文件创建了没，只要能区分什么时候`-E_FILE_EXISTS`和`-E_DIR_NOT_EXIST`，一股脑返回，就可以拿到60分。

1. user/lib.h：添加`user_create()`定义
2. user/file.c：完成`user_create()`，该函数调用`fsipc_create()`
3. include/fs.h：添加`Fsreq_create{}`结构体
4. user/fsipc.c：完成`fsipc_create()`
5. fs/serve.c：
   1. `serve()`实现对req的分流
   2. 添加`serve_create()`函数，调用`file_create()`
6. fs/fs.c：**修改**`file_create()`函数

接下来介绍一下60分的做法

```cpp
user/file.c:
int user_create(char *path, int isdir)
{
        int r;
        r = fsipc_create(path, isdir);
        return r;
}
```

```cpp
include/fs.h
#define FSREQ_CREATE 8

struct Fsreq_create {
        char req_path[MAXPATHLEN];
        int isdir;
};
```

```cpp
user/fsipc.c
int fsipc_create(const char *path, int isdir)
{
        struct Fsreq_create *req;

        req = (struct Fsreq_create *) fsipcbuf;

        // the path is too long
        if (strlen(path) >= MAXPATHLEN) {
                return -E_BAD_PATH;
        }

        strcpy((char *)req->req_path, path);
        req->isdir = isdir;
        return fsipc(FSREQ_CREATE, req, 0, 0);
}
```

```cpp
fs/serve.c
void serve(void)
{
        u_int req, whom, perm;

        for (;;) {
                perm = 0;

                req = ipc_recv(&whom, REQVA, &perm);


                // All requests must contain an argument page
                if (!(perm & PTE_V)) {
                        writef("Invalid request from %08x: no argument page\n", whom);
                        continue; // just leave it hanging, waiting for the next request.
                }

                switch (req) {
/*LOOK HERE ----> */    case FSREQ_CREATE:
                                serve_create(whom, (struct Fsreq_create *)REQVA);
                                break;

                        case FSREQ_OPEN:
                                ...
                }
                syscall_mem_unmap(0, REQVA);
        }
}

void serve_create(u_int envid, struct Fsreq_create *rq)
{
        u_char path[MAXPATHLEN];
        int isdir = rq->isdir;
        struct File *f;
        int r;
        user_bcopy(rq->req_path, path, MAXPATHLEN);
        path[MAXPATHLEN - 1] = 0;

        r = file_create((char *)path, &f, isdir);
        ipc_send(envid, r, 0, 0);
        return;
}
```

注意，接下来这个代码有bug，但是能提供**60分**的思路。

我能想到的bug有：

1. 创建目录时没有修改文件的type
2. ...

善用`walk_path()`这个函数，它是用来看看指定目录下是不是有该文件的。

```cpp
int file_create(char *path, struct File **file, int isdir)
{
        char name[MAXNAMELEN];
        int r;
        struct File *dir, *f;
        if (isdir) { // just create a dir
                if ((r = walk_path(path, &dir, &f, name)) == 0) {
                        return -E_FILE_EXISTS;
                }
        } else { // create a file

                int slash_num = 0;
                int i = 0;
                for (i = 0; i < MAXPATHLEN; i++) {
                        if (path[i] == '/') {
                                slash_num += 1;
                                if (slash_num == 2) {
                                        path[i] == '\0';
                                        break;
                                }
                        }
                }

                if (slash_num > 1) { // recur create, check dir
                        if ((r = walk_path(path, &dir, &f, name)) != 0) {
                                return -E_DIR_NOT_EXIST;
                        }

                        // till now, dir exists
                        path[i] = '/';
                        if ((r = walk_path(path, &dir, &f, name)) == 0) {
                                return -E_FILE_EXISTS;
                        }

                } else { // just create a simple file
                        if ((r = walk_path(path, &dir, &f, name)) == 0) {
                                return -E_FILE_EXISTS;
                        }
                }
        }

/*      I dont know whether below should be changed :(*/

        if (r != -E_NOT_FOUND || dir == 0) {
                return r;
        }

        if (dir_alloc_file(dir, &f) < 0) {
                return r;
        }

        strcpy((char *)f->f_name, name);
        *file = f;
        return 0;
}
```

### Extra

#### 第一部分

为isdir增加取值（完善exam中的user_create函数）

在创建文件之前还需要先创建一个目录，这太麻烦了。因此我们希望你对创建函数进行改进，使得我们不再受到-E_DIR_NOT_EXIST的困扰。

对isdir增加两种新的取值。当isdir为2时仍然是在指定路径创建普通文件，但当目录不存在时，user_create还会对目录进行创建（当沿途的目录不存在时，该函数都需要完成创建操作）。

当isdir为3时需要在指定目录下创建目录，并且和isdir=2类似，需要**对沿途的目录进行创建**。

注意：当路径中的目录已经存在时，创建过程仍然正常进行，不会返回负值。但在同一个目录下出现同名文件时仍然需要返回负值。例如在执行user_create("/A/B", 2)后执行user_create("/A/C", 2)不会出错，但再次执行user_create("/A/B", 2)需要返回-E_FILE_EXISTS。

#### 第二部分

允许同一目录下文件和子目录同名。

在Extra的测试中，你需要处理文件和目录同名的问题。可以选择对fs.c的部分函数进行修改或是编写新的函数。

### 总结

各位同学烤漆加油，没必要为了一门吐槽人数巨多的OS坏了自己的心情，不是吗？

像我这种凡人还是早早承认差距，过得会比较开心**亿点**。

![](/img/lab5/知道的太多反而不会快乐.jpg)
