# lab2笔记

在pmap.h里的`LIST_HEAD(Page_list, Page);`声明了一个叫做Page_list的结构体，宏替换如下

```cpp
Page_list {
    struct Page* lh_fist;
}
```

然后定义了一个Page_list，

```cpp
static struct Page_list page_free_list;
```

这个初始化看起来就很“脱裤子放屁”，(实际上，为了让list这种结构通用，不得已这样做?)

```cpp
// 让head被指向的对象的lh_first为NULL
#define LIST_INIT(head) do {                                            \
                LIST_FIRST((head)) = NULL;                                      \
        } while (0)
```

```cpp
LIST_INIT(&page_free_list)
```

## page_alloc

申请一个page的指针，然后写成0，将其返回给pp
