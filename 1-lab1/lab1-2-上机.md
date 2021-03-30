# 上机

这次上机，真是见证了人与人之间的参商——有的人Extra拿满分，有的人exam拿0分。

这次上机，考察了很多c语言的基础知识，对高工的同学极为不利，几乎团灭。

俗话说，前人栽树，后人乘凉，现在看来，并非如此。经历了这次上机，感觉OS有年年变难的趋势。

我也只做了exam，，对本菜鸟来说，真是太难了。

## 建议

早点到教室，开机要比较久的时间（吐槽垃圾电脑），连chrome打开都要将近一分钟。不禁感慨，苍天啊，掉块陨石把机房砸了吧，利国利民啊！

我个人会在桌面上新建一个txt，用来存放：

1. 常用的命令，如`gxemul -E testmips -C R3000 -M 64 vmlinux`和`git push ...`
2. 虚拟机上编译报的错，一是方便查看哪错了，二是lab挂了可以粘贴到虚拟机的文件带回课下研究

我还会同时打开`dev c++`，好处有两个：

1. 把自己的一些代码粘贴进去，方便自己看
2. 写一些超级小的程序，一般不超过20行，测试下自己的思路

## exam

### 题目

我们已经补全了print的代码，现在有俩结构体：

```cpp
struct s1 {
	int a;
	char b;
	char c;
	int d;
};

struct s2 {
	int size;
	int c[c_size];
};
```

说明：

1. `struct s2`里，`c_size`保证和`size`相等
2. 给`printf`传入的相关参数是`struct`的**地址**

`printf("%$1T", addr_of_s1)`，输出第一个结构体中的内容，格式如`{1,"a","b",2}`

`printf("%$2T", addr_of_s2)`，输出第二个结构体中的内容，格式如`{4,3,1,2,4}`。也就是说，size和数组里面的值要一起输出。

好了，开始做吧😄

### 解答

在自己的print.c里面，用`typedef`声明这几个结构体。

```cpp
typedef struct {
        int a;
        char b;
        char c;
        int d;
} s1;

typedef struct {
        int size;
        int c[10000];
} s2;
```

为什么要用`typedef`，原因很简单，`struct`的语法忘得差不多了，而`readelf`文件夹下的`types.h`里面的结构体就是这么声明的，这样一来避免后面定义指针的不便，二来有个参考。

然后声明几个和`struct`相关的指针和变量，前者用来接收地址。

```cpp
int type;
s1* a_s1;
s2* a_s2;
int var_for; // for 循环里面的参数
int size;
int* arr_int; // type in struct s2
```

通过`$`判断要输出结构体然后再获取`type`的代码我就不写了，这个比较简单。

最最重要的就是那个case里，真可谓又臭又长。

首先，用指针接受参数，顺便输出`'{'`：

```cpp
case 'T':
        if (type == 1) {
                a_s1 = (s1*)va_arg(ap, s1*); // 保险起见，做了强制转换
        } else {
                a_s2 = (s2*)va_arg(ap, s2*);
        }
        // output the '{'
        length = PrintChar(buf, '{', 1, 0);
        OUTPUT(arg, buf, length);
```

如果`type == 1`，逐个接受参数：

```cpp
if (type == 1) {
        // output int a, char b, char c, int d;
                int a = a_s1->a;
                char b = a_s1->b;
                char c = a_s1->c;
                int d = a_s1->d;
```

然后逐个输出abcd，注意格式：

```cpp
// print int a;
negFlag = 0;
if (a < 0) {
        a = -a;
        negFlag = 1;
}
length = PrintNum(buf, a, 10, negFlag, width, ladjust, padc, 0);
OUTPUT(arg, buf, length);

// print ','
length = PrintChar(buf, ',', 1, 0);
OUTPUT(arg, buf, length);

// print char b
length = PrintChar(buf, b, width, ladjust);
OUTPUT(arg, buf, length);

// print ','
length = PrintChar(buf, ',', 1, 0);
OUTPUT(arg, buf, length);

// print char c
length = PrintChar(buf, c, width, ladjust);
OUTPUT(arg, buf, length);

// print ','
length = PrintChar(buf, ',', 1, 0);
OUTPUT(arg, buf, length);

// print int d
negFlag = 0;
if (d < 0) {
        d = -d;
        negFlag = 1;
}
length = PrintNum(buf, d, 10, negFlag, width, ladjust, padc, 0);
OUTPUT(arg, buf, length);
```

很遗憾没有排版，不过模仿教程中的语气，

> 想必**聪明**的你已经明白我想说什么了吧~❤️

如果`type != 1`，输出size和数组：

```cpp
else {
    size = a_s2->size; // 拿到size
    arr_int = a_s2->c; // 拿到数组
    for (var_for = 0; var_for < size; var_for++) { // var_for 已经预先定义好了，为0
            int x = arr_int[var_for];
            negFlag = 0;
            if (x < 0) {
                    x = -x;
                    negFlag = 1;
            }
            length = PrintNum(buf, x, 10, negFlag, width, ladjust, padc, 0);
            OUTPUT(arg, buf, length);

            if (var_for != size-1) {
                    length = PrintChar(buf, ',', 1, 0);
                    OUTPUT(arg, buf, length);
            }

    }
}
```

其实上面是不对的。因为没有输出size，这也是我de了半天空气bug的原因之一。

为啥不修改呢？因为答案不能全说出来，不然那样就**没意思**了，呵呵。

最后别忘了输出`}`以及`break`

```cpp
// print '}'
length = PrintChar(buf, '}', 1, 0);
OUTPUT(arg, buf, length);
break;
```

### 测试

测试比较简单，在`init`目录下的`init.c`文件中，声明struct，初始化后输出就行。

```cpp
typedef struct {
        int a;
        char b;
        char c;
        int d;
} s1;

typedef struct {
        int size;
        int c[100];
} s2;


void mips_init()
{
        printf("init.c:\tmips_init() is called\n");

        s2 s = {3, {1,2,3}};
        printf("%$2T", &s);
        printf("\n");
```

## Extra

没做，但是可以告诉同学们，

1. 要写汇编，写函数，所以要熟悉寄存器的使用约定
2. 要从c代码去调用自己写的汇编函数
3. 把这个任务交给当时计组的自己，应该不难，可以，大家已经不是“从前那个少年”😕

## 总结

很多人都挂了，我运气好在及格的边缘徘徊，感觉就是吔屎了。

讲下自己的刺激体验吧，以及一些坑点。

### struct的定义和初始化

其实我是不会`struct`的定义的，当场学习了readelf文件夹下的types.h，学会了定义。

然后就是如何给一个`struct`赋值，简直懵逼了，我鼓起勇气直接问助教哥哥`struct`怎么初始化，感谢nice的助教哥哥。

附一个初始化实例：

```cpp
typedef struct {
	int a;
	int b;
} s2;

s2 s = {1, 2};
```

问的时候，我觉得助教一定被我的无知折服了😄，如果没有，我已经被自己的无知折服了。

### struct获取成员变量

时隔一年有余，早就忘记了通过`.`和`->`获取struct中成员变量的区别了。

现在告诉那些忘记了的同学，`struct.var`和`struct_pointer->var`。

怪不得我在`devc++`里面“温习”`struct`的使用时，要用`.`来获取元素值，而在`print.c`里，我最后用`->`获取元素的值。我以为二者用法是相同的，但是`struct_pointer.var`编译会报错，提交的时候误打误撞写对了。

附上一个样例：

```cpp
#include<stdio.h>

typedef struct {
	int size;
	int c[100];
} s2;

int main() {
	s2 s= {1, {1, 2, 3}};
	printf("%d\n", s.c[2]);
	s2* s_ptr = &s;
	printf("%d\n", s_ptr->size);
	return 0;
}
```

学习struct的初识化，有助于我们修改init.c，测试自己的程序，甚至猜一猜自己的程序错在哪里，不过调试是不存在的。

下main这种初始化方式是错误的：

```cpp
#include<stdio.h>

typedef struct {
	int size;
	int *c; // 注意这里，声明成了一个指针而不是数组
} s2;

int main() {
	s2 s= {1, {1, 2, 3}}; // 将无法正确接受参数
	printf("%d\n", s.c[2]);
	s2* s_ptr = &s;
	printf("%d\n", s_ptr->size);
	return 0;
}
```

诡异的是，这样写，可以编译通过，在虚拟机上跑，会输出一些奇怪的数，这也是我在本地测试时犯下的第二个错误。

### byebye

大家可以看下17级学长的exam，当时是让大家修改print，支持数组的输出。
