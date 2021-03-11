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

gcc filename

gcc -o

## makefile

[简易教程](https://www.bilibili.com/video/BV1Mx411m7fm)

[强者教程](https://seisman.github.io/how-to-write-makefile/introduction.html)
