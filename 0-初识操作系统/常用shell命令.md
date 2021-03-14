# 常用shell命令

首先,默认同学们会使用vim

## 文件的增删改

使用vim来新建/修改文件

```
vim file_name
```

若文件存在,则修改,不存在,则创建

还可以使用echo来新建文件,比如

```
echo 1st line > file1.txt
```

`file1.txt`里面的内容就是

```
1st line
```

echo是覆盖写入,比如再执行`echo 2nd line > file1.txt`

`file1.txt`里的内容就是

```
2nd line
```

尾部追加的方式是`echo message >> filename`,这样就不会覆盖文件内容了.

删除文件:

```
rm filename
```

## 文件复制和移动

```
cp old_path new_path
```

## 目录的增删改

```
mkdir dir_name
```

目录不存在则报错`mkdir: cannot create directory 'dir_name': File exists`

删除目录

```
rmdir dir_name
```

[复制至不存在目录则创建](https://blog.csdn.net/xfxf996/article/details/106154387?)。

## 分屏

会话-窗口（window）-窗格（pane）

tmux进入mode，默认得到一个窗口，被分成一个pane

ctrl+b “ 横着分两屏

ctrl+b %竖着分两屏

ctrl+b o切换pane

ctrl+b x关闭当前使用的pane

tmux kill-pane -t 0关闭0号panel，t表示taget，会给出提示是否关闭当前窗口，按下y确认即可

tmux kill-pane -a关闭所有pane

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

yy

dd

p

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

`grep -n "int" filename`

在filename中查找含“int”的行，然后补上n再输出

## awk

分割然后输出

详见[此](https://blog.51cto.com/bubble/1179036)。

## 变量的赋值、运算

详见[此](https://www.cnblogs.com/hjnzs/p/12218467.html)。

## git

试题见[此](https://blog.csdn.net/weixin_45086881/article/details/90610606)。
