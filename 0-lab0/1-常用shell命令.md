# 常用shell命令

要熟练地掌握命令，最好的方法，就是自己动手做一遍。可以新建一个Test文件夹，里面放个hello.txt，然后跟着课程网址上的教程做。

首先，默认同学们会使用vim，如何使用vim，可以见此[视频](https://www.bilibili.com/video/BV1Yt411X7mu)。

其次，在Windows，除了登陆课程网址或使用PuTTY，还可以直接用`Git Bash`，我们可以在连接到校园网的情况下，使用命令

```
ssh your_student_ID@buaaos.studio
```

尝试登陆平台。推荐使用工具链文章中这个[教程](https://blog.csdn.net/qq_46359697/article/details/107756317)来美化`Git Bash`。后续还可以为其添加make等功能，详见工具链文章。

## 文件操作

### 新建，修改、删除、复制和移动

过于简单，见cscore即可，会用下面的命令即可。

```
touch vim rm cp mv
```

## 目录

### 新建和移动

简单

### 删除

忘记`rmdir`吧。。。

```
rm -r dir_name
```

### 拷贝

将一个整个文件夹，或者其内部的文件拷贝到另一个文件夹中，可以利用`cp -r`，`r`表示`recursion`，就是递归。使用过`rm -r dir_name`的话，一定不会陌生。

cp只能操作存在的文件（夹），所以如果要拷贝的路径不存在，要新建一下。

```
mkdir Test && cp -r ./be_copied/ ./Test
mkdir Test && cp -r ./be_copied/* ./Test
```

第一条指令会把整个`be_copied`文件夹拷贝到`Test`文件夹内，

第二条指令只会把`be_copied`文件夹**内的文件**拷贝到`Test`文件夹内。不要忽视了`-r`这个参数，如果`be_copied`里面还有其他文件夹，不用`-r`是不会被拷贝的。

## 基础操作介绍

find: 在文件夹下找文件

grep：在文件内找文本

ctags？？：这个东西很好用啊，你们自己去找资料看怎么用。

吐槽：菜狗真滴找不到，找到了也看不懂

## 分屏

使用tmux，要了解：会话-窗口（window）-窗格（pane）三个概念。

输入tmux，按下回车，默认得到一个窗口，被分成一个pane

`ctrl+b "`横着分两屏

`ctrl+b %`竖着分两屏

`ctrl+b o`切换pane

`ctrl+b x`关闭当前使用的pane

`tmux kill-pane -t 0`关闭0号pane，t表示taget，会给出提示是否关闭当前窗口，按下y确认即可

`tmux kill-pane -a`关闭所有pane

## 编译

按下gcc --help看

```
gcc filenamegcc filename_1 -o filename_2
```

3多个程序文件的编译

通常整个程序是由多个源文件组成的，相应地也就形成了多个编译单元，使用GCC能够很好地管理这些编译单元。假设有一个由test1.c和 test2.c两个源文件组成的程序，为了对它们进行编译，并最终生成可执行程序test，可以使用下面这条命令：

gcc test1.c test2.c -o test

详见[此](https://www.cnblogs.com/ggjucheng/archive/2011/12/14/2287738.html)。

## vim复制粘贴

复制当前行：yy，复制5行：y5y

剪切当前行：dd；剪切5行：d5d

粘贴：p

## makefile

[简易教程](https://www.bilibili.com/video/BV1Mx411m7fm)

[强者教程](https://seisman.github.io/how-to-write-makefile/introduction.html)——由讨论区大佬提供

## git add

详见[此](https://blog.csdn.net/qq_42351033/article/details/89765281)。

## grep

查找再输出

```
grep - print lines matching a pattern
用法:grep [选项]... PATTERN [FILE]...
选项（常用）：
        -a      不忽略二进制数据进行搜索
        -i      忽略文件大小写差异
        -r      从文件夹递归查找
        -n      显示行号
```

用的最多的是`grep -n "int" filename`，因为我们常常要看哪一行出现了相应的字符，然后跳转到那一行（vim在esc模式下，ngg跳转）

在filename中查找含“int”的行，然后补上n再输出

## awk

分割然后输出

详见[此](https://blog.51cto.com/bubble/1179036)。

## 查找文件

使用find命令并加上-name选项可以在当前目录下递归地查找符合参数所示文件名的文件，并将文件的路径输出至屏幕上。

```
find - search for files in a directory hierarchy
用法:find -name 文件名
```

## 查找文本

```
grep - print lines matching a pattern
用法:grep [选项]... PATTERN [FILE]...
选项（常用）：
        -a      不忽略二进制数据进行搜索
        -i      忽略文件大小写差异
        -r      从文件夹递归查找
        -n      显示行号
```

## sed

## 变量的赋值、运算

详见[此](https://www.cnblogs.com/hjnzs/p/12218467.html)。
