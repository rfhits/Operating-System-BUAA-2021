# PV操作

真滴太难了。

通常要从经典的问题入手，

还有一个问题，什么时候用`while(true)`？

如果是一次性，就不用，比如写者，写了就跑，

如果要反复地访问，就要用`while(true)`，比如生产者不停地生产。

## 读者写者

不考虑饥饿，最重要的就是Lightswitch操作。

那就涉及在准备进入临界区内部进程的个数，cnt和cntMutex。

先对src上锁还是对cntMutex上锁？对后者，否则所有读者都被堵住了。

第一个读者要申请srcMutex，最后一个读者要释放

```python
reader:
    P(cntMutex)
    cnt += 1
    if cnt == 1:
        P(scrMutex)
    else :
        pass
    V(cntMutex)
    read src
    P(cntMutex)
    cnt -=1
    if cnt == 0:
        V(srcMutex)
    else:
        pass
```

## 闸机

或者说是barrier，特点是，一旦某个进程来了，那么其他进程都要等待。
