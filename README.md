# Operating-System-BUAA-2021

keywords: BUAA OS 2021 北京航空航天大学 6系 计算机 操作系统 2021

2021春北航操作系统

## 实验任务💻

### lab1

1. 请修改include.mk文件，使交叉编译器的路径正确
2. 完成 readelf.c 中缺少的代码
3. 填写tools/scse0_3.lds中空缺的部分，将内核调整到正确的位置上
4. 完成boot/start.S中空缺的部分。设置栈指针，跳转到main函数。
5. 补全lib/print.c中lp_Print()函数中缺失的部分来实现字符输出。

### lab2

1. 请完成queue.h中的LIST_INSERT_AFTER函数和LIST_INSERT_TAIL函数
2. 完成mips_detect_memory()函数。
3. 完成page_init函数
4. 完成page_alloc和 page_free 函数
5. 完成boot__pgdir_walk和 pgdir_walk函数
6. 实现boot__map_ segment函数
7. 完成page_insert函数
8. 完成tlb__out函数

### lab3

1. mips_vm_init
2. env___init
3. envid2env
4. env__setup___vm
5. env__alloc
6. load__icode_mapper
7. load_elf and load_icode
8. env_create and env_create_priority
9. init.c
10. env__run
11. start.s
12. scse0_3.lds
13. kclock_init
14. sched_yield

### lab4

1. 完成msyscall 函数
2. 完成handle_sys函数
3. 实现 sys__mem_alloc函数
4. 实现sys_mem_map函数
5. 实现sys_mem_unmap函数
6. 实现sys_yield函数
7. 实现sys_ipc_recv函数和 sys_ipc_can_send函数
8. 填写sys_env_alloc函数
9. 填写fork函数中关于sys_env_alloc的部分和“子进程”执行的部分
10. 填写duppage函数
11. 完成page_fault_handler函数
12. 完成sys__set__pgfault_handler函数
13. 填写pgfault 函数
14. 填写sys_set_env_status函数
15. 填写fork函数中关于“父进程”执行的部分

### lab5

### lab6

### 挑战性任务

[lab4_challenge-信号量实现-lpxofbuaa](https://github.com/lpxofbuaa/BUAA_OS_2019/tree/lab4-challenge)

[lab5_challenge-内存保护-VOIDMalkuth](https://github.com/VOIDMalkuth/BUAA_OS_2019_Code/tree/master/Lab5-Challenge)

## 往届代码💩

因为每年课程要求可能会有变化，所以尽量看时间较近的代码。

login256学长的代码也不一定能完全适用于2020春的课程要求，在lab3的进程调度中就初见端倪。

[2020春-refkxh的代码](https://github.com/refkxh/BUAA_OS_2020Spring)

[2020春-C7ABT的代码](https://github.com/C7ABT/BUAA_OS_2020/tree/master)

[2019春-login256的代码](https://github.com/login256/BUAA-OS-2019)

[2019春-Eadral的代码](https://github.com/Eadral/BUAA_OS_2019)

## 代码解析和思考题🧠

[lab1~6实验报告-shh](https://github.com/shh2000/BUAA-OS-2019/tree/master/%E5%AE%9E%E9%AA%8C%E6%8A%A5%E5%91%8A)

[lab1~6实验报告-VOIDMalkuth](https://github.com/VOIDMalkuth/BUAA_OS_2019_Code/tree/master/%E5%AE%9E%E9%AA%8C%E6%8A%A5%E5%91%8A)

[lab1~6实验报告-hky1999](https://github.com/hky1999/BUAA_OS_Files/tree/master/%E5%AE%9E%E9%AA%8C%E6%8A%A5%E5%91%8A)

### lab1

[lab1思考题解答-moc-85422729](https://www.cnblogs.com/moc-85422729/p/xwc_os_lab1.html)

[lab1思考题解答-Suxy-99](https://www.cnblogs.com/Suxy-99/p/12592665.html)

### lab2

内存映射

[lab2-code_view-riyuejiuzhao](https://blog.csdn.net/riyuejiuzhao/article/details/105198758)

[Lab2-code_view-littlenyima](https://www.cnblogs.com/littlenyima/p/12764653.html)

[lab2思考题解答-moc-85422729](https://www.cnblogs.com/moc-85422729/p/xwc_os_lab2.html)

[lab2思考题解答-Suxy-99](https://www.cnblogs.com/Suxy-99/p/12716863.html)

### lab3

进程运行

[lab3-code_view-riyuejiuzhao](https://blog.csdn.net/riyuejiuzhao/article/details/105550324?)

[lab3-code_view-FFChyan](https://blog.csdn.net/qq_36740940/article/details/89198569)

[lab3-code_view-SivilTaram](https://www.cnblogs.com/SivilTaram/p/oslab3.html)

[lab3-code_view-CindyZhou](https://www.cnblogs.com/CindyZhou/p/12852837.html)

[lab3-code_view-puublog](https://www.cnblogs.com/puublog/p/10707188.html)

[lab3思考题解答-blshuidi](https://www.cnblogs.com/blshuidi/p/13563390.html)

[lab3思考题解答-moc-85422729](https://www.cnblogs.com/moc-85422729/p/xwc_os_lab3.html)

[lab3思考题解答-Suxy-99](https://www.cnblogs.com/Suxy-99/p/12969157.html)

### lab4

系统调用

[lab4-code_view-FFChyan](https://blog.csdn.net/qq_36740940/article/details/89523911)

[lab4-code_view-CindyZhou](https://www.cnblogs.com/CindyZhou/p/12858468.html)

[lab4-code_view-sharinka0715](https://www.cnblogs.com/sharinka0715/p/10776860.html)

[lab4-code_view-SivilTaram](https://www.cnblogs.com/SivilTaram/p/os_lab4.html)

[lab4-code_view-p_wk](https://www.cnblogs.com/p-wk/p/13183479.html)

[lab4-Thinkings-Suxy-99](https://www.cnblogs.com/Suxy-99/p/12969200.html)

[lab4思考题解答-blshuidi](https://www.cnblogs.com/blshuidi/p/13563405.html)

## 汁源入口😍

[2017春-期中试卷部分解答-super_dmz](https://blog.csdn.net/super_dmz/article/details/91351313)

## 参考资料📕

[linux下输入输出重定向](https://blog.csdn.net/hongkangwl/article/details/21000703)

[Linux中GCC编译器运行第一个程序"Helloworld!"](https://blog.csdn.net/wangdd_199326/article/details/77842798)

[Makefile的写法-bilibili-正月点灯笼](https://www.bilibili.com/video/BV1Mx411m7fm)

## 致谢❤

感谢所有为这门课提供资料、奉献精力的老师、助教、学长和大佬们！
