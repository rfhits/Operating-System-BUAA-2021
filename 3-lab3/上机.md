<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [lab3上机](#lab3%E4%B8%8A%E6%9C%BA)
  - [lab3-1](#lab3-1)
    - [exam](#exam)
      - [PART1](#part1)
      - [PART2](#part2)
      - [PART3](#part3)
      - [解答](#%E8%A7%A3%E7%AD%94)
    - [Extra](#extra)
      - [题目](#%E9%A2%98%E7%9B%AE)
      - [解答](#%E8%A7%A3%E7%AD%94-1)
  - [lab3-2](#lab3-2)
    - [exam](#exam-1)
      - [PART1](#part1-1)
      - [PART2](#part2-1)
      - [解答](#%E8%A7%A3%E7%AD%94-2)
    - [Extra](#extra-1)
      - [题目要求](#%E9%A2%98%E7%9B%AE%E8%A6%81%E6%B1%82)
      - [解答](#%E8%A7%A3%E7%AD%94-3)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# lab3上机

## lab3-1

### exam

#### PART1

在今天的实验里我们要求你实现一个简易的 fork 函数（并不包括实际load代码段），通过给定的原始进程块(输入参数struct Env *e)生成一个新的进程控制块，并返回新进程控制块的env_id。同学们需要在 lib/env.c 和 include/env.h 中分别定义和声明 fork 函数，函数接口如下：
u_int fork(struct Env *e);
要求如下：

1. 从 env_free_list 中从头申请一个新的进程控制块
2. 新进程控制块的 env_status、env_pgdir、env_cr3、env_pri和原进程控制块保持一致。
3. 为新进程控制块生成对应的 env_id
4. env_parent_id 的值为原进程控制块的 env_id
5. 返回值为新进程的env_id

#### PART2

void lab3_output(u_int env_id);

本部分要求修改struct Env，在进程控制块中增加字段（具体增加哪些内容请自行组织）组织起进程间的父子、兄弟关系，并按照要求在 lib/env.c 和 include/env.h 中分别定义和声明 lab3_output 函数输出相关内容，详情如下：
函数lab3_output的定义如下：
`void lab3_output(u_int env_id);`
要求输出的内容有其父进程的env_id、其第一个子进程的env_id、其前一个兄弟进程的env_id以及其后一个兄弟进程的env_id
所有的子进程都由fork创建，两个进程如果是兄弟，它们的父进程一定相同。
以某进程第一个子进程是指，由该进程作为父进程使用fork创建的第一个子进程。
兄弟进程间的顺序即为这些进程被创建的顺序，前一个兄弟进程为较早被创建的进程
需要在PART1的fork函数中进行对添加字段的修改
输出格式为：printf("%08x %08x %08x %08x\n", a, b, c, d);
其中a, b, c, d分别为父进程的env_id、第一个子进程的env_id、前一个兄弟进程的env_id以及后一个兄弟进程的env_id
如果a, b, c, d中有不存在的参数，则输出0

#### PART3

在PART2的基础上，在 lib/env.c 和 include/env.h 中分别定义和声明 lab3_get_sum 函数，函数的功能为：给定一个进程的env_id，返回以该进程为根节点的子进程树中进程的数目（包括它本身），具体接口如下：
int lab3_get_sum(u_int env_id);

#### 解答

比较简单，但是小坑不断。有的人第一题没有用`env_alloc()`结果忘记给`free_env_list`remove掉分配的`env`

斗胆放上自己的代码。

首先是修改env结构体，这次exam用数组写起来比较简单。为什么我没用链表呢，因为C语言功底太差，，用指针怕错。。。

```cpp
struct Env {
        struct Trapframe env_tf;        // Saved registers
        LIST_ENTRY(Env) env_link;       // Free list
        u_int env_id;                   // Unique environment identifier
        u_int env_parent_id;            // env_id of this env's parent
        u_int env_status;               // Status of the environment
        Pde  *env_pgdir;                // Kernel virtual address of page dir
        u_int env_cr3;
        LIST_ENTRY(Env) env_sched_link;
        u_int env_pri;
        // Lab 4 IPC
        u_int env_ipc_value;            // data value sent to us
        u_int env_ipc_from;             // envid of the sender
        u_int env_ipc_recving;          // env is blocked receiving
        u_int env_ipc_dstva;            // va at which to map received page
        u_int env_ipc_perm;             // perm of page mapping received

        // Lab 4 fault handling
        u_int env_pgfault_handler;      // page fault state
        u_int env_xstacktop;            // top of exception stack

        // Lab 6 scheduler counts
        u_int env_runs;                 // number of times been env_run'ed
        u_int env_nop;                  // align to avoid mul instruction

        int son_num;            // add on exam
        u_int son_id_arr[1024]; // add on exam

};
```

```cpp
u_int fork(struct Env *e)
{
        struct Env *e_son;
        env_alloc(&e_son, e->env_id);
        e_son->env_status = e->env_status;
        e_son->env_pgdir = e->env_pgdir;
        e_son->env_cr3 = e->env_cr3;
        e_son->env_pri = e->env_pri;

        // ---- father ----
        int son_num = e->son_num;
        e->son_id_arr[son_num] = e_son->env_id;
        e->son_num += 1;

        return e_son->env_id;
}
```

```cpp

void lab3_output(u_int env_id)
{
        struct Env *e_now;
        u_int fa_id = 0;
        u_int first_son_id = 0;
        u_int bro_bf_id = 0; // "bf" means "before"
        u_int bro_af_id = 0; // "af" means "after"

        envid2env(env_id, &e_now, 0);
        // son part
        first_son_id = e_now->son_id_arr[0];

        // parent part
        fa_id = e_now->env_parent_id;
        if (fa_id == 0) { // do not have parent
                // three 0 now
                bro_bf_id = 0;
                bro_af_id = 0;
        } else { // have a parent
                struct Env *e_fa;
                envid2env(fa_id, &e_fa, 0);
                int index = 0;
                for (index = 0; index < 1024; index++) {
                        if (e_fa->son_id_arr[index] == env_id) {
                                break;
                        } else {
                                continue;
                        }
                }
                // index is the env of father now
                if (index > 0) { // have bro bf
                        bro_bf_id = e_fa->son_id_arr[index - 1];
                }

                // have a bro_af
                if (e_fa->son_num > index + 1) {
                        bro_af_id = e_fa->son_id_arr[index + 1];
                }
        }
        // fa_id, fist_son_id, bro_bf, bro_af
        printf("%08x %08x %08x %08x\n", fa_id, first_son_id, bro_bf_id, bro_af_id);
}
```

```cpp
int lab3_get_sum(u_int env_id)
{
        struct Env *e_now;
        envid2env(env_id, &e_now, 0);
        int son_num = e_now->son_num;
        // if e_now has no son
        if (son_num == 0) {
                return 1;
        } else {
                // have many sons, recuring
                int ans = 1;
                int i = 0;
                for (i = 0; i < son_num; i++) {
                        struct Env *e_son;
                        u_int son_id = e_now->son_id_arr[i];
                        envid2env(son_id, &e_son, 0); // now got a son
                        ans += lab3_get_sum(son_id);
                }
                return ans;
        }
}

```

### Extra

#### 题目

b/env.c 和 include/env.h 中分别定义和声明 lab3_kill 函数，功能为：杀死一个进程。进程树的根节点代表的进程收养（接管）他的孤儿进程。这些孤儿进程将依次排列在根进程的子进程的尾部。测试中保证不会杀死根节点代表的进程。函数接口如下：
`void lab3_kill(u_int env_id);`
在完成一次删除后，基础测试PART2 lab3_output，输出的结果在新的树结构下仍应正确。
一颗进程树的根节点代表的进程由env_alloc创建，其余节点代表的进程皆由PART1 fork创建。
本部分中“杀死进程”的要**清空进程控制块相关字段**（可以参考`free_env()`，但是不能调用`free_env()`）以及其他相关处部分测试较强，请务必考虑完善。

#### 解答

一堆人以为自己的数据结构出问题了，结果，，是在杀死一个进程后的状态处理上出问题了。

我一push，得11分，呵呵，也跑去de自己结构的bug了，不过已经没时间了。

早知道exam拿60分就跑路了，现在发现原来exam60和100最终得分没差。麻了，战术失误。

![](/img/分值安排及考核时间.JPG)

看了大佬的[博客](https://roife.github.io/2021/04/20/buaa-os-lab-lab3-1-process/)，发现自己可能是没有free后插回list里，我呵呵了😇。

```cpp
void lab3_kill(u_int env_id)
{
        // get e_now and e_root
        struct Env *e_now;
        struct Env *e_root;
        envid2env(env_id, &e_now, 0);

        u_int root_id = e_now->env_parent_id;
        envid2env(root_id, &e_root, 0);

        // while this "root" have a father
        while (e_root->env_parent_id != 0) {
                root_id = e_root->env_parent_id;
                envid2env(root_id, &e_root, 0);
        }

        // now we get the root
        int now_son_num = e_now->son_num;
        int root_son_num = e_root->son_num;

        int i = 0;

        // remove from father, careful!: root == father
        struct Env *e_fa;
        u_int fa_id = e_now->env_parent_id;
        envid2env(fa_id, &e_fa, 0);
        // get the index
        for (i = 0; i < e_fa->son_num; i++) {
                if (e_fa->son_id_arr[i] == env_id) {
                        break;
                } else {
                        continue;
                }
        }

        // cover
        for (; i < e_fa->son_num-1; i++) {
                e_fa->son_id_arr[i] = e_fa->son_id_arr[i+1];
        }
        e_fa->son_id_arr[i] = 0;
        e_fa->son_num--;


        // attach all sons to root
        for (i = 0; i < now_son_num; i++) {
                u_int now_son_id = e_now->son_id_arr[i];
                e_root->son_id_arr[e_root->son_num] = now_son_id;
                e_root->son_num += 1;
        }

        // free the env
        e_now->son_num = 0;
        for (i = 0; i < e_now->son_num; i++) {
                e_now->son_id_arr[i] = 0;
        }
        e_now->env_status = ENV_FREE;
        e_now->env_cr3 = 0;
        e_now->env_pgdir = 0;

        // maybe i should add, then i can pass
        LIST_INSERT_HEAD(&env_free_list, e, env_link);

}
```

## lab3-2

题目要求：
在本次课上测试中，我们对env_pri的含义进行如下约定：

1. env_pri的第0-7位仍然表示进程的优先级（PRI）
2. env_pri的第8-15位为功能字段1（FUNC_1）
3. env_pri的第16-23位为功能字段2（FUNC_2）
4. env_pri的第24-31位为功能字段3（FUNC_3）
5. FUNC_1, FUNC_2, FUNC_3均为无符号数

### exam

#### PART1

在这次的课上测试中，我们需要你实现一个新的调度算法，具体要求如下：
1.修改sched_yield函数，使得在进程调度时高优先级（用PRI表示）的进程永远比低优先级（用PRI表示）的进程先调度。
2.为了简化问题，不需要考虑两个进程优先级相同的情况。
例如：如果有两个进程A和B，优先级分别为1和2，且env_status都为ENV_RUNNABLE，则只有B会得到执行；当B的状态不为ENV_RUNNABLE时，A才会得到执行。

#### PART2

在PART1中的sched_yield上继续修改，要求如下：
在当前进程（记为进程X）被时钟中断暂停执行时，进行如下操作：
1.根据FUNC_1的值更新进程X的优先级：
若FUNC_1的值为0，则不修改进程X的优先级
若FUNC_1的值为正，则将进程X的优先级减少FUNC_1的值（如果减少之后优先级小于0则优先级置零）
2.完成PART1中的进程调度

#### 解答

有一些坑：

1. `(x<<24)>>24`可能导致结果为负数，所以最好`x&(0x000000ff)`
2. 没有时间片的概念，所以不存在一个进程的时间片用完这种说法，所以不用在两个list之间倒腾来倒腾去
3. 本地测试时，永远输出一个数字，不存在进程切换

### Extra

#### 题目要求

Extra部分要求在exam部分的基础上继续修改sched_yield：
在当前进程（记为进程X）被时钟中断暂停执行时，进行如下操作：
1.如果当前的时钟中断是第FUNC_2个（从1开始计数）时钟中断，那么在接下来的FUNC_3个时间片里，将进程X的env_status改为ENV_NOT_RUNNABLE，不进行调度，FUNC_3个时间片之后（无论这时是哪一个进程被中断），将进程X的env_status改为ENV_RUNNABLE，参与调度。
2.否则，不进行处理（注意，为了简化问题，我们保证这一部分测试数据中FUNC_1始终为0）
3.按照PART1中的策略进行调度

#### 解答

题干中虽然提到了“时间片”的概念，其实一个时间片就是一次中断。

测试样例很弱，挺多人过Extra的，所以不能保证自己的程序是否正确。

我觉得简单的做法应该是修改env.h里的env结构体，置一个time_to_run，每个中断更新下，更新到0的话就置为RUNNABLE。然后遍历list取RUNNABLE里pri最大的就行。

一个list就够了。
