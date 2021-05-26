# LAB5流程分析

本文装载自[hky1999](https://github.com/hky1999/BUAA_OS_Files/blob/master/%E5%AE%9E%E9%AA%8C%E6%8A%A5%E5%91%8A/LAB5%E6%B5%81%E7%A8%8B%E5%88%86%E6%9E%90.md)。

**实名感谢ZXH！！！**

LAB5主要就是对文件进行读写，然后这个文件的读写……看代码看得想吐

首先是第一部分，IDE的驱动。

IDE属于硬件，硬件的操作都被抽象为了对于某个物理地址的读写操作。（如果计组做过P8的同学就知道当时是怎么操纵硬件的）。所以实际上**操作硬件==访问内存**

而只有在内核态才能对硬件进行操作。并且要求**硬件的“访存”操作不经过缓存**。缓存在我们读写内存的时候是好东西，但问题是我们想操作硬件的时候，缓存反而带来不利影响，因为我们希望向硬件写东西的时候，被缓存兜住了。

因此，我们要把硬件的**物理地址**给转换成**kesg1**的内核地址，告诉CPU，这部分读写请直接操作，不要用缓存。

## int sys_write_dev(int sysno, u_int va, u_int dev, u_int len)

> 位于**lib/syscall_all.c**中

这个函数很简单，就是先判断**[dev,dev+len]**在不在提示里面给的范围内，如果不在，返回**-E_INVAL**代表范围错误

如果范围正确，那么使用**bcopy**来把**va**开始的长度为**len**字节的内容拷贝到**dev+0xA0000000**中去。

dev+0xA0000000即是设备转换成kseg1的地址。

**注意，一定要用bcopy，不要用memcpy**

因为bcopy的逻辑是，优先**按字拷贝**，零头再**按字节拷贝**

而memcpy则是**只按字节拷贝**

做到第二部分的时候，如果出现未完成的写入，很可能就是因为用的是memcpy，造成了原本一次要写完的东西分成了4次写，于是报错。

## int sys_read_dev(int sysno, u_int va, u_int dev, u_int len)

> 和上面的那个函数基本一毛一样，就是把读写交换了而已，就不多说了

## void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)

> 位于**fs/ide.c**中

这个函数的作用是向**diskno**号磁盘的**secno**号扇区开始，写入内存中**dst**位置开始的**nsecs**个扇区大小的内容。

此处我贡献一个关于磁盘相关偏移的宏定义：

```c
#define                   IDE_BEGIN_ADDR 0x13000000
#define IDE_OFFSET_ADDR   IDE_BEGIN_ADDR + 0x0000
#define IDE_OFFSETHI_ADDR IDE_BEGIN_ADDR + 0x0008
#define IDE_ID_ADDR       IDE_BEGIN_ADDR + 0x0010
#define IDE_OP_ADDR       IDE_BEGIN_ADDR + 0x0020
#define IDE_STATUS_ADDR   IDE_BEGIN_ADDR + 0x0030
#define IDE_BUFFER_ADDR   IDE_BEGIN_ADDR + 0x4000
#define IDE_BUFFER_SIZE 0x0200
```

其实这个函数的框架已经基本给我们写好了，挨个扇区地读取。循环已经直接写好，所以我们只用关心一个山区内的读取怎么完成：

先定义下面三个变量：

```c
//diskno在传入参数的时候已经给我们了，不需要单独建立变量
int va      = offset_begin + offset;
char op     = 0;
char status = 0;
```

一定要定义，因为我们要往磁盘里面写东西的时候，需要从内存中拷贝。所以必须要有个真实存在的内容给他拷贝，于是我们创建几个变量。

然后依次做下面的操作（注意，涉及变量的时候记得要用取地址符&）：

- 利用**syscall_write_dev**向**IDE_ID_ADDR**写入**diskno**，len设置为4（字节）。此步骤是为了选择要写入的磁盘号。
- 利用**syscall_write_dev**向**IDE_OFFSET_ADDR**写入**va**，len设置为4（字节）。此步骤是为了选择要写入的磁盘地址（偏移）。
- 利用**syscall_write_dev**向**IDE_OP_ADDR**写入**op**，len设置为1（字节）。此步骤是为了选择要进行的操作，在这里，我们的op=0，代表要读取。

写完上面的内容之后，正常情况下，我们所需要读取的内容应该会被送到了缓冲区内。所以我们进行如下操作：

- 利用**syscall_read_dev**从**IDE_STATUS_ADDR**读取一个字节的内容到**status**
- 如果上一步得到的status结果为0，代表读取失败了，panic
- 如果结果非0，代表读取成功，则：
- 使用**syscall_read_dev**从**IDE_BUFFER_ADDR**读取**IDE_BUFFER_SIZE**大小的内容到**dst+offset**中去

接下来就是不断地循环，直到所有的内容都被读取完毕了

## void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)

> 功能和上面的差不多，只不过是写而已

与上面的的那个函数唯一的区别是

**先把要写入的内容写到BUFFER里面，然后再把op=1给写到对应位置，启动ide的写入**

---

上面的内容都比较简单，但是到后面的内容就非常的坑爹了

先跳过fs.c，我们先来看看fsformat

## fsformat.c

这个文件位于**fs.fsformat.c**，是一个**运行在linux上的程序**，并非运行在我们写的操作系统上。

大家可以看到，他include了stdio.h这个东西……

先从他的main函数看起，可以发现，这个是一个用来生成fs.img的程序

```c
int main(int argc, char **argv) {
    int i;

    init_disk();
    ///*********

    if(argc < 3 || (strcmp(argv[2], "-r") == 0 && argc != 4)) {
        fprintf(stderr, "\
Usage: fsformat gxemul/fs.img files...\n\
       fsformat gxemul/fs.img -r DIR\n");
        exit(0);
    }

    if(strcmp(argv[2], "-r") == 0) {
        for (i = 3; i < argc; ++i) {
            write_directory(&super.s_root, argv[i]);
        }
    }
    else {
        for(i = 2; i < argc; ++i) {
            write_file(&super.s_root, argv[i]);
        	//**********************
        }
    }

    flush_bitmap();
    //*******
    finish_fs(argv[1]);
	//*******
    return 0;
}

```

可以看到，main函数主要的作用就是接受参数，然后根据参数去创建fs.img。接下来我们要看的函数，我用了星号注释标记在下面了。那个write_directory我没写，所以就不看了，233。

这个函数是先在内存里面把磁盘内容给组织好，再写入磁盘中，就类似于我们在数据结构里面，先存好图什么的，再写入文件里面。

### 全局变量

这个是一个很复杂的文件，里面有几个关键的全局变量，还有结构体的定义，先列在下面：

```c
#define NBLOCK 1024 //一块磁盘里面的block数目
uint32_t nbitblock; // 用于存储bitmap的block数目
uint32_t nextbno;   // 下一块可用的block编号

struct Super super; // 超级块
struct Block {
    uint8_t data[BY2BLK];
    uint32_t type;
} disk[NBLOCK];

///////////////////下面的内容在include里面的fs.h里面
#define BY2BLK		BY2PG//一个block对应的字节，也就是说，一个block正好等于一页大小
#define BIT2BLK		(BY2BLK*8)//一个block对应的*位数*，一字节等于8位，所以要乘8

#define MAXNAMELEN	128//用于存文件名的char数组大小，由于最后一个必定为'\0',所以只能存127个字符
#define MAXPATHLEN	1024//和上面的类似，只不过是用来存路径的

#define NDIRECT		10//直接引用的个数，可以认为是10个指针，这里用int存block的下标来代替了指针的作用
#define NINDIRECT	(BY2BLK/4)//间接引用块的指针个数，由于一个int是32位，也就是4byte，所以是除4

#define MAXFILESIZE	(NINDIRECT*BY2BLK)//最大文件大小，那么就是引用指针的个数*一块大小

#define BY2FILE     256//定义了一个FIle结构体（文件索引结构体）所占用的大小，实际上比FILE结构体实际占用大小要大，是为了保持扩展性？

struct File {//文件索引结构体
	u_char f_name[MAXNAMELEN];	// 文件名
	u_int f_size;			// 文件大小（字节），其实就是引用块所占的总大小
	u_int f_type;			// 文件类型，有两种，一种是普通文件，一种是目录
	u_int f_direct[NDIRECT];//直接引用指针
	u_int f_indirect;//指向间接引用块

	struct File *f_dir;		// 指向包含这个文件的目录，只在内存中存在着（真正写入文件之后没有这部分）
	u_char f_pad[BY2FILE - MAXNAMELEN - 4 - 4 - NDIRECT * 4 - 4 - 4];//为了填满by2file，而开了这部分内容
};

#define FILE2BLK	(BY2BLK/sizeof(struct File))//就是一个block能容纳多少个file索引

// File types
#define FTYPE_REG		0	// 普通文件
#define FTYPE_DIR		1	// 目录


// File system super-block (both in-memory and on-disk)

#define FS_MAGIC	0x68286097	// 神秘代码，用来表示这是个合法的文件

struct Super {
	u_int s_magic;		// Magic number: FS_MAGIC
	u_int s_nblocks;	// Total number of blocks on disk
	struct File s_root;	// Root directory node
};
```

### init_disk()

用于初始化磁盘

- 首先，标注0号块为**BLOCK_BOOT**
- 计算**bitmap**存储所需要的Block数量（**nbitblock**），也就是**向上取整**NBLOCK/BIT2BLOCK
- 把下一个空闲块设置为2+bnitblock，因为第一块block是boot用的，第二快是超级块，后面的bintblock是存储bitmap用的
- 把存储bitmap用的block类型置为**BLOCK_BMAP**
- 然后开始初始化bitmap
- 设置超级块相关的内容

### void write_file(struct File *dirf, const char *path)

向dirf下面写入path所表示的文件

- 先创建一个block大小+1的buffer
- 利用**create_file（）**在**dirf**下面新建一个文件索引结构体，记做**target**
- 获取文件名，也就是把类似于“dir/subdir/filename.xxx”给变成“filename.xxx”
- 把获取到的文件名给拷贝到**target**里面
- 使用lseek(fd, 0, SEEK_END)获取文件大小，（在此前已经用open来打开了fd文件）
- 然后设置文件类型为普通文件
- 使用 lseek(fd, 0, SEEK_SET);把fd文件指针重新设置为文件开头
- 开始用while进行疯狂读入文件：
  - read(fd, disk[nextbno].data, n))，这一句代码的意思是，向新的一块block写入fd读入的长度为n的内容，其实n的大小就是sizeof（disk[0].data）,也就是一个block的大小
  - 完成上面那步之后，disk[nextbno]就已经有了内容，接下来需要使用save_block_link(target, iblk++, next_block(BLOCK_DATA));来把这个块给连接到我们新创建的文件里面。
  - 其中，iblk指的是当前文件里面存在的引用指针数
  - 整个文件读取结束之后，关闭文件

### struct File *create_file(struct File *dirf)

这个函数的作用是，在**dirf**这个目录文件下面，创建一个新的文件索引

目录型的文件大概是这样子的。

- 一个struct File，作为其索引，里面存放了这个是个目录文件，目录的名字之类的信息，最关键的是里面有10个直接引用指针，还有一个间接引用指针
  - 通过直接引用指针或者间接引用指针link到若干个block，每个block被分成了16个部分，16个部分都可以用来存储文件索引

所以，实际上这个程序要做的事情有：

- 通过f_size/BY2BLK来计算出，dirf目前link着多少个block
- 如果目前一个block都没有，那么就利用**make_link_block**创建一个新的block，并且返回其首地址指针(其实也就是这个block的16个文件索引中的第一个)
- 如果目前存在着block，那么遍历**最后一块**blcok，查找还有没有空闲的文件索引（也就是文件名为空）
- 如果不能找到空闲的文件索引，那么就创建一个新的block

### void flush_bitmap()

就是把**nextbno**之前的所有块对应的bitmap标记为0，也就是不可用

```cpp
void flush_bitmap() {
    int i;
    // update bitmap, mark all bit where corresponding block is used.
    for(i = 0; i < nextbno; ++i) {
        ((uint32_t *)disk[2+i/BIT2BLK].data)[(i%BIT2BLK)/32] &= ~(1<<(i%32));
    	//前两块blcok分别是boot和superblock，所以要加2
    }
}
```

## void finish_fs(char *name)

之前我们所做的一切，都是在内存中完成的。在内存中把文件结构组织好了之后，就要把内存中的内容给真正地写到文件之中。

- 首先，使用memcpy把超级块的内容给拷贝到第二块block中（disk[1].data)
- 用open打开**name**文件，也就是我们要创建的文件
- 我们的一个磁盘文件里面有1024个block，对每个block:
  - 利用reverse_block()进行大小端的转换
  - write(fd, disk[i].data, BY2BLK)，把对应的block给写入文件中
- 关闭文件

---

下面是lab5-2的部分

说实话，很让人头疼

## fsserver

这个是文件相关操作的服务

主要文件在**fs/serrv.c**中。

```c
void umain(void)
{
	user_assert(sizeof(struct File) == BY2FILE);

	writef("FS is running\n");

	writef("FS can do I/O\n");

	serve_init();
	fs_init();
	fs_test();

	serve();
}
```

所以大概就是运行了4个函数，接下来分别说这几个函数

### void serve_init(void)

```c
struct Filefd {
	struct Fd f_fd;
	u_int f_fileid;
	struct File f_file;
};//位于fd.h
struct Open {
	struct File *o_file;	// mapped descriptor for open file
	u_int o_fileid;			// file id
	int o_mode;				// open mode
	struct Filefd *o_ff;	// va of filefd page
};//opentab是这种类型
#define MAXOPEN			1024
#define FILEVA 			0x60000000

void serve_init(void)
{
	int i;
	u_int va;
	va = FILEVA;
	for (i = 0; i < MAXOPEN; i++) {
		opentab[i].o_fileid = i;
		opentab[i].o_ff = (struct Filefd *)va;
		va += BY2PG;
	}
}
```

这个函数似乎是初始化opentab数组，分别用来映射对应的block,

TODO:

## void fs_init(void)

```c
void fs_init(void)
{
	read_super();
	check_write_block();
	read_bitmap();
}
```

调用了三个函数，所以继续一个一个看吧

### void read_super(void)

```c
void read_super(void)
{
	int r;
	void *blk;

	// Step 1: read super block.
	if ((r = read_block(1, &blk, 0)) < 0) {
		user_panic("cannot read superblock: %e", r);
	}
	super = blk;

	// Step 2: Check fs magic nunber.
	if (super->s_magic != FS_MAGIC) {
		user_panic("bad file system magic number %x %x", super->s_magic, FS_MAGIC);
	}

	// Step 3: validate disk size.
	if (super->s_nblocks > DISKMAX / BY2BLK) {
		user_panic("file system is too large");
	}

	writef("superblock is good\n");
}
```

这个函数大概就是做了下面的步骤：

- 利用read_block来读取了1号块，也就是超级块
- 检查超级块的魔数
- 检查文件系统的大小

这个地方唯一需要看的是**read_block**这个函数

### int  read_block(u_int blockno, void **blk, u_int *isnew)

大概就是，接受一个磁盘块号。然后判断这一块的内容是否已经在内存中了，如果不在内存中，就分配一页内存，然后把内容给拷贝过去

```c
int read_block(u_int blockno, void **blk, u_int *isnew)
{
	u_int va;

	//这部分是检查block的编号是否合法，以及这一块block是否free
    //需要注意的是，super和bitmap此时可能均未加载，所以要小心空指针的问题
	if (super && blockno >= super->s_nblocks) {
		user_panic("reading non-existent block %08x\n", blockno);
	}
	if (bitmap && block_is_free(blockno)) {
		user_panic("reading free block %08x\n", blockno);
	}

	// 获取这个block对应的va
    // 其实也就是 DISKMAP + blockno*BY2BLK;
    // 即：磁盘挂载的位置+偏移
	va = diskaddr(blockno);

	if (block_is_mapped(blockno)) {	//如果这个块已经map在内存中了，也就是有PTE_V
   									//在函数内，是判断将要挂载的内存位置是否已经存在了一页内存
		if (isnew) {//小心空指针
			*isnew = 0;//设置isnew为假，
		}
	} else {			//这个块没有map
		if (isnew) {
			*isnew = 1;//那么这标记这是一个新块
		}
		syscall_mem_alloc(0, va, PTE_V | PTE_R);//给新块在需要挂载的位置分配一页内存
		ide_read(0, blockno * SECT2BLK, (void *)va, SECT2BLK);
        //利用之前写的ide_read函数，把对应位置的块给加载到刚才分配的位置
        //其实也就是从磁盘上的 blockno * SECT2BLK位置，
        //拷贝到内存中的 DISKMAP + blockno*BY2BLK位置
	}

	if (blk) {
		*blk = (void *)va;//返回在内存中挂载的位置
	}
	return 0;
}
```

### void check_write_block(void)

大概就是，读取超级块，然后把超级块给炸了，再写回磁盘。再次读取超级块，看看是否是我们刚刚炸掉的样子。最后恢复超级块

```c
void check_write_block(void)
{
	super = 0;//把super指针给重置
    		//因为如果不重置，后面许多操作会根据超级块的内容去判断操作是否合法
    		//然而后面我们会破坏超级块，这样子一来，检测的时候就会炸了

	read_block(0, 0, 0);//读取第一block
	user_bcopy((char *)diskaddr(1), (char *)diskaddr(0), BY2PG);
    //把超级块的内容给备份到第一块
   

	// 炸掉superblock
	strcpy((char *)diskaddr(1), "OOPS!\n");
    //此语句是在内存中把superblock给炸了
	write_block(1);
    //然后把损坏的superblock给写回
	user_assert(block_is_mapped(1));//不太明白这里为什么要assert，明明在写回里面就有要求这一块已经map了

	// unmap掉内存中超级块的位置，然后测试是否成功unmap
	syscall_mem_unmap(0, diskaddr(1));
	user_assert(!block_is_mapped(1));

	// 重新读一遍被炸掉的超级块
	read_block(1, 0, 0);
    // 然后确认里面的内容是被我们炸掉之后的结果
	user_assert(strcmp((char *)diskaddr(1), "OOPS!\n") == 0);

	// 恢复超级块，在内存中把备份的内容拷回去
	user_bcopy((char *)diskaddr(0), (char *)diskaddr(1), BY2PG);
	write_block(1);
    //写回超级块
	super = (struct Super *)diskaddr(1);
}
```

## void read_bitmap(void)

大概做的就是把bitmap对应的块加载到内存中，然后确认他的内容是对的（0号块，超级块，还有bitmap块自身都是被标记为使用过的）

```c
void read_bitmap(void)
{
	u_int i;
	void *blk = NULL;

	// 计算存储bitmap所占用的block数目，然后把他们全部加载到内存中
    // 不是很明白为什么要+1，难道为了向上取整吗？
	nbitmap = super->s_nblocks / BIT2BLK + 1;
	for (i = 0; i < nbitmap; i++) {
		read_block(i + 2, blk, 0);
	}

	bitmap = (u_int *)diskaddr(2);
	//设置bitmap数组为加载到内存中的bitmap块的首地址

	// 确保bitmap中，0号块还有超级块都是被占用的
    // 这个函数是利用bitmap进行判断的，并非根据内存中的PTE_V来判断
	user_assert(!block_is_free(0));
	user_assert(!block_is_free(1));

	// 确保bitmap自身也被标记了在使用中
	for (i = 0; i < nbitmap; i++) {
		user_assert(!block_is_free(i + 2));
	}

	writef("read_bitmap is good\n");
}

```

## void fs_test(void)

```c
void fs_test(void)
{
	struct File *f;
	int r;
	void *blk;
	u_int *bits;

    //在当前进程的最底下，4096的位置分配一页，可能是因为这个地方肯定不会被用，所以被当作临时页区
    //然后把bitmap的这一页给备份到刚才分配的这一页
	if ((r = syscall_mem_alloc(0, BY2PG, PTE_V | PTE_R)) < 0) {
		user_panic("syscall_mem_alloc: %e", r);
	}
	bits = (u_int *)BY2PG;
	user_bcopy(bitmap, bits, BY2PG);
  
	// 申请一个新的block
    // 然后检查是否这个block原先是free的，后来变成了not free
	if ((r = alloc_block()) < 0) {
		user_panic("alloc_block: %e", r);
	}
	user_assert(bits[r / 32] & (1 << (r % 32)));
	user_assert(!(bitmap[r / 32] & (1 << (r % 32))));
	writef("alloc_block is good\n");
//////////////////////////////////////////////
  
  
    //试图打开一个不存在的文件，检查其是否返回了找不到文件
	if ((r = file_open("/not-found", &f)) < 0 && r != -E_NOT_FOUND) {
		user_panic("file_open /not-found: %e", r);
	} else if (r == 0) {
		user_panic("file_open /not-found succeeded!");
	}
    //检测能否正常打开一个文件
	if ((r = file_open("/newmotd", &f)) < 0) {
		user_panic("file_open /newmotd: %d", r);
	}
	writef("file_open is good\n");
  
/////////////////////////////////////////////////
  
    //打开上面的"/newmotd"文件的第0个索引块，并加载到内存中
	if ((r = file_get_block(f, 0, &blk)) < 0) {
		user_panic("file_get_block: %e", r);
	}
	//检查这个文件里面的内容是否正确
	if (strecmp(blk, msg) != 0) {
		user_panic("file_get_block returned wrong data");
	}
	writef("file_get_block is good\n");
  
///////////////////////////////////////////////////////
  
	*(volatile char *)blk = *(volatile char *)blk;
    //无法理解这一句是在做什么，volatile的意思是，这个内容可能会在意料之外被改变，因此每次都会从内存中读取内容，禁止编译器使用寄存器进行“缓存操作”，或者是优化。但是啥时候会被改变？有点晕
  
    //结合下面的内容，可能是强行让系统认为我们修改过文件中的内容（虽然修改前后不变）
    //然后方便我们测试文件写回
    //把修改过的文件内容写回磁盘
	file_flush(f);
	writef("file_flush is good\n");

    //////////////////////////////////////////
  
   	//测试调整文件大小的功能，把文件大小强行弄成0
	if ((r = file_set_size(f, 0)) < 0) {
		user_panic("file_set_size: %e", r);
	}
    //因为文件被强行裁剪为0了，所以其不包含任意一个引用块
	user_assert(f->f_direct[0] == 0);
	writef("file_truncate is good\n");
/////////////////////////////////////
  
    //修改文件大小，使其大小刚好为diff_msg的大小
    //然后往这个文件的0号引用块里面写入这个diff_msg
    //最后同步并关闭文件
	if ((r = file_set_size(f, strlen(diff_msg))) < 0) {
		user_panic("file_set_size 2: %e", r);
	}
	if ((r = file_get_block(f, 0, &blk)) < 0) {
		user_panic("file_get_block 2: %e", r);
	}
	strcpy((char *)blk, diff_msg);
	file_flush(f);
	file_close(f);
	writef("file rewrite is good\n");
}
```

## int alloc_block(void)

作用是，分配一个新的block,并且把他map到内存中

```c
int alloc_block(void)
{
	int r, bno;
	// Step 1: find a free block.
	if ((r = alloc_block_num()) < 0) { // failed.
		return r;
	}
	bno = r;

	// Step 2: map this block into memory. 
	if ((r = map_block(bno)) < 0) {
		free_block(bno);
		return r;
	}

	// Step 3: return block number.
	return bno;
}
```

## int alloc_block_num(void)

作用是，寻找一个空闲的block,然后把他标记为已使用，最后返回得到的block号

```c
int alloc_block_num(void)
{
	int blockno;
	for (blockno = 3; blockno < super->s_nblocks; blockno++) {
		if (bitmap[blockno / 32] & (1 << (blockno % 32))) {
            //找到一个空闲的块
			bitmap[blockno / 32] &= ~(1 << (blockno % 32));
            //在内存中的bitmap标记这个块已被分配
			write_block(blockno / BIT2BLK); 
            //把bitmap给更新到磁盘中
			return blockno;
		}
	}
	// no free blocks.
	return -E_NO_DISK;
}
```

## int file_get_block(struct File *f, u_int filebno, void **blk)

获取这个文件里面对应的索引块，并把它读到内存中来。如果索引块不存在，将会创建一块新的

```c
int file_get_block(struct File *f, u_int filebno, void **blk)
{
	int r;
	u_int diskbno;
	u_int isnew;

	// 在文件f中寻找filebno这个引用块，如果这个块不存在，那么创建一个空块
	if ((r = file_map_block(f, filebno, &diskbno, 1)) < 0) {
		return r;
	}

	// 把这一块内容给读到内存中
	if ((r = read_block(diskbno, blk, &isnew)) < 0) {
		return r;
	}
	return 0;
}
```

## int file_map_block(struct File *f, u_int filebno, u_int *diskbno, u_int alloc)

找到文件里面对应的索引块磁盘编号，如果索引块不存在，视alloc参数置位与否，创建一个新的块

```c
int file_map_block(struct File *f, u_int filebno, u_int *diskbno, u_int alloc)
{
	int r;
	u_int *ptr;

	// 试图去找到文件索引内部对应编号的索引指针
	if ((r = file_block_walk(f, filebno, &ptr, alloc)) < 0) {
		return r;
	}

	// 如果这个索引指针是空的，并且alloc置位了，那么就分配一个新的block给它
	if (*ptr == 0) {
		if (alloc == 0) {
			return -E_NOT_FOUND;
		}

		if ((r = alloc_block()) < 0) {
			return r;
		}
		*ptr = r;
	}

	// 成功返回
	*diskbno = *ptr;
	return 0;
}
```

## int file_block_walk(struct File *f, u_int filebno, u_int **ppdiskbno, u_int alloc)

用于在一个File索引中，找到filebno对应的索引指针

```c
int file_block_walk(struct File *f, u_int filebno, u_int **ppdiskbno, u_int alloc)
{
	int r;
	u_int *ptr;
	void *blk;

	if (filebno < NDIRECT) {
		//如果这个索引号在直接指针号内（小于10），则返回直接索引
		ptr = &f->f_direct[filebno];
	} else if (filebno < NINDIRECT) {
		//需要用间接索引的情况
		if (f->f_indirect == 0) {
            //如果间接索引块不存在
			if (alloc == 0) {
                //索引块不存在，参数又不允许创建一个块，那么返回找不到
				return -E_NOT_FOUND;
			}
			//试图分配一个间接索引块
			if ((r = alloc_block()) < 0) {
				return r;
			}
			f->f_indirect = r;
		}

		// 把这个间接索引块给载入到内存中
		if ((r = read_block(f->f_indirect, &blk, 0)) < 0) {
			return r;
		}
        //返回间接索引指针
		ptr = (u_int *)blk + filebno;
	} else {
        //超出了最大索引范围
		return -E_INVAL;
	}
	// 储存得到的索引指针，然后返回0，代表成功找到
	*ppdiskbno = ptr;
	return 0;
}
```

## void file_flush(struct File *f)

把在内存中的文件给写回硬盘，也就是更新其在磁盘中的内容

```c
void file_flush(struct File *f)
{
	u_int nblocks;
	u_int bno;
	u_int diskno;
	int r;

	nblocks = f->f_size / BY2BLK + 1;
	//我还是不知道为什么要额外+1
   	//对于所有的索引块，如果被载入到内存中，并且被修改过，那么回写这一块
	for (bno = 0; bno < nblocks; bno++) {
		if ((r = file_map_block(f, bno, &diskno, 0)) < 0) {
			continue;
		}
		if (block_is_dirty(diskno)) {
			write_block(diskno);
		}
	}
}
```

## int file_set_size(struct File *f, u_int newsize)

用于强行设置文件大小

```c
int file_set_size(struct File *f, u_int newsize)
{
    //新的文件大小要比旧大小小的时候，需要用这个函数去调节
	if (f->f_size > newsize) {
		file_truncate(f, newsize);
	}
	//否则直接设置大小就好了
	f->f_size = newsize;

    //如果这个文件包含在某个目录当中，应该更新其目录文件在磁盘中的信息
	if (f->f_dir) {
		file_flush(f->f_dir);
	}

	return 0;
}
```

## void file_truncate(struct File *f, u_int newsize)

```c
void file_truncate(struct File *f, u_int newsize)
{
	u_int bno, old_nblocks, new_nblocks;

	old_nblocks = f->f_size / BY2BLK + 1;
	new_nblocks = newsize / BY2BLK + 1;
	if (newsize == 0) {
		new_nblocks = 0;
	}
//计算新老文件大小所占用的引用块
  
    //下面是判断是否需要删除间接引用块
    //然后把原来引用块的所有block给释放
    //我怀疑这里有“文件泄漏”，因为他没有释放间接引用块的内容
	if (new_nblocks <= NDIRECT) {
		f->f_indirect = 0;
		for (bno = new_nblocks; bno < old_nblocks; bno++) {
			file_clear_block(f, bno);
		}
	} else {
		for (bno = new_nblocks; bno < old_nblocks; bno++) {
			file_clear_block(f, bno);
		}
	}

	f->f_size = newsize;
}
```

## int file_clear_block(struct File *f, u_int filebno)

大概就是，找到文件里面filebno这一块，然后释放它

```c
int file_clear_block(struct File *f, u_int filebno)
{
	int r;
	u_int *ptr;

	if ((r = file_block_walk(f, filebno, &ptr, 0)) < 0) {
		return r;
	}

	if (*ptr) {
		free_block(*ptr);
		*ptr = 0;
	}

	return 0;
}
```

## void free_block(u_int blockno)

在bitmap里面，把这一块使用状态设置为0

```c
free_block(u_int blockno)
{
	// Step 1: Check if the parameter `blockno` is valid (`blockno` can't be zero).
	if(blockno == 0){
		user_panic("blockno为0");
	}

	// Step 2: Update the flag bit in bitmap.
	bitmap[blockno / 32] &= (1<<(blockno%32));
}
```

## void file_close(struct File *f)

关闭文件。说是关闭文件，其实只是刷新文件而已。并不需要把对应的内存给unmap.因为我们操作系统就一个磁盘，然后那一片内存空间是专门给这个磁盘挂载用的。所以一直保留在内存里面也没关系。

```c
void file_close(struct File *f)
{
	//刷新文件本身，然后刷新其所在的目录文件
	file_flush(f);
	if (f->f_dir) {
		file_flush(f->f_dir);
	}
}

```

## void serve(void)

终于结束检查，开始文件服务了！

接受一个ipc请求，并且要求把对应的参数塞到一个结构体里面，把这个结构体放入一页的头部位置，然后把这个页给传过来

除了open请求比较复杂，别的请求基本上就是找到文件所在在块，然后调用相关函数进行操作。我就不一个一个去分析了，实在想吐

```c
#define REQVA	0x0ffff000
struct Fsreq_open {
	char req_path[MAXPATHLEN];
	u_int req_omode;
};

struct Fsreq_map {
	int req_fileid;
	u_int req_offset;
};

struct Fsreq_set_size {
	int req_fileid;
	u_int req_size;
};

struct Fsreq_close {
	int req_fileid;
};

struct Fsreq_dirty {
	int req_fileid;
	u_int req_offset;
};

struct Fsreq_remove {
	u_char req_path[MAXPATHLEN];
};
void serve(void)
{
	u_int req, whom, perm;

	for (;;) {
		perm = 0;

		req = ipc_recv(&whom, REQVA, &perm);
		//接受文件请求
        //这个请求必须要带上一页可用内存
        //当进程发送请求的时候，会把一页内存给map到REQVA的位置
		if (!(perm & PTE_V)) {
			writef("Invalid request from %08x: no argument page\n", whom);
			continue; // 继续等待下一个请求
		}

        //传过来的时候，在那一页的头部放了一个结构体，类似与超大型传参。
        //结构体的定义我已经放在上面的地方了，基本上一个结构体对应一个操作所需要的参数
		switch (req) {
			case FSREQ_OPEN:
				serve_open(whom, (struct Fsreq_open *)REQVA);
				break;

			case FSREQ_MAP:
				serve_map(whom, (struct Fsreq_map *)REQVA);
				break;

			case FSREQ_SET_SIZE:
				serve_set_size(whom, (struct Fsreq_set_size *)REQVA);
				break;

			case FSREQ_CLOSE:
				serve_close(whom, (struct Fsreq_close *)REQVA);
				break;

			case FSREQ_DIRTY:
				serve_dirty(whom, (struct Fsreq_dirty *)REQVA);
				break;

			case FSREQ_REMOVE:
				serve_remove(whom, (struct Fsreq_remove *)REQVA);
				break;

			case FSREQ_SYNC:
				serve_sync(whom);
				break;

			default:
				writef("Invalid request code %d from %08x\n", whom, req);
				break;
		}

		syscall_mem_unmap(0, REQVA);
	}
}
```

### serve_open(u_int envid, struct Fsreq_open *rq)

```c
void
serve_open(u_int envid, struct Fsreq_open *rq)
{
	writef("serve_open %08x %x 0x%x\n", envid, (int)rq->req_path, rq->req_omode);

	u_char path[MAXPATHLEN];
	struct File *f;
	struct Filefd *ff;
	int fileid;
	int r;
	struct Open *o;

	//从传入参数的结构体中复制目的路径，并确保这个路径以\0结束
	user_bcopy(rq->req_path, path, MAXPATHLEN);
	path[MAXPATHLEN - 1] = 0;

	// 创建一个文件id
	if ((r = open_alloc(&o)) < 0) {
		user_panic("open_alloc failed: %d, invalid path: %s", r, path);
		ipc_send(envid, r, 0, 0);
	}

	fileid = r;

	// 打开文件，从根目录开始寻找，直到查找到该文件，然后把f指针指向该文件索引块
	if ((r = file_open((char *)path, &f)) < 0) {
	//	user_panic("file_open failed: %d, invalid path: %s", r, path);
		ipc_send(envid, r, 0, 0);
		return ;
	}

	// 把文件指针给保存了
	o->o_file = f;

	// 填写ff相关的内容
	ff = (struct Filefd *)o->o_ff;
	ff->f_file = *f;
	ff->f_fileid = o->o_fileid;
	o->o_mode = rq->req_omode;
	ff->f_fd.fd_omode = o->o_mode;
	ff->f_fd.fd_dev_id = devfile.dev_id;
	//返回ff
	ipc_send(envid, 0, (u_int)o->o_ff, PTE_V | PTE_R | PTE_LIBRARY);
}
```

### open_alloc(struct Open **o)

```c
int
open_alloc(struct Open **o)
{
	int i, r;

	// 在opentab里面找到一个有效的打开位置
	for (i = 0; i < MAXOPEN; i++) {
        //pageref这个参数是判断该页是否有效，如果有效则返回其引用次数，如果无效，返回0
		switch (pageref(opentab[i].o_ff)) {
			case 0://如果是空的，则创建一页，注意，此处没有break，所以是继续到case1的
				if ((r = syscall_mem_alloc(0, (u_int)opentab[i].o_ff,
										   PTE_V | PTE_R | PTE_LIBRARY)) < 0) {
					return r;
				}
			case 1://如果之前只有一次引用
				opentab[i].o_fileid += MAXOPEN;
                //实在无法理解的操作
				*o = &opentab[i];
				user_bzero((void *)opentab[i].o_ff, BY2PG);
				return (*o)->o_fileid;
		}
	}

	return -E_MAX_OPEN;
}
```

## fstest.c

终于结束了文件服务，现在这个程序是用来测试文件服务是否正确的

```c
#include "lib.h"

static char *msg = "This is the NEW message of the day!\r\n\r\n";
static char *diff_msg = "This is a different massage of the day!\r\n\r\n";

void umain()
{
        int r;
        int fdnum;
        char buf[512];
        int n;
		//尝试打开一个文件
        if ((r = open("/newmotd", O_RDWR)) < 0) {
                user_panic("open /newmotd: %d", r);
        }
        fdnum = r;
        writef("open is good\n");
		//从文件中读取内容到buf中，并验证内容的正确性
        if ((n = read(fdnum, buf, 511)) < 0) {
                user_panic("read /newmotd: %d", r);
        }
        if (strcmp(buf, diff_msg) != 0) {
                user_panic("read returned wrong data");
        }
        writef("read is good\n");
		//把文件大小给设置为0
        if ((r = ftruncate(fdnum, 0)) < 0) {
                user_panic("ftruncate: %d", r);
        }
    	//回到文件启始位置
        seek(fdnum, 0);
		//写入另一个消息，之后关闭并重新打开文件，看看是否真的写到了文件里
        if ((r = write(fdnum, msg, strlen(msg) + 1)) < 0) {
                user_panic("write /newmotd: %d", r);
        }

        if ((r = close(fdnum)) < 0) {
                user_panic("close /newmotd: %d", r);
        }
        if ((r = open("/newmotd", O_RDONLY)) < 0) {
                user_panic("open /newmotd: %d", r);
        }
        fdnum = r;
        writef("open again: OK\n");
        if ((n = read(fdnum, buf, 511)) < 0) {
                user_panic("read /newmotd: %d", r);
        }
        if (strcmp(buf, msg) != 0) {
                user_panic("read returned wrong data");
        }
        writef("read again: OK\n");
/////////////////////////////////////////////////////////////////////
        if ((r = close(fdnum)) < 0) {
                user_panic("close /newmotd: %d", r);
        }
        writef("file rewrite is good\n");
    ///////////////////
    //把这个文件给删除了，然后再看看能否打开。应该是不能打开的
	if((r = remove("/newmotd"))<0){
		user_panic("remove /newmotd: %d",r);
	}
	if((r = open("/newmotd", O_RDONLY))>=0){
		user_panic("open after remove /newmotd: %d",r);
	}
	writef("file remove: OK\n");
        while (1) {
                //writef("IDLE!");
        }
}
```
