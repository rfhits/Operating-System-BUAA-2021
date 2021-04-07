# code

> 这代码真是给爷看吐了



```cpp
// this is a logical concept
// declared a struct named "name", 
// it has a pointer named lh_first, point to the type
#define LIST_HEAD(name, type)                                           \
        struct name {                                                           \
                struct type *lh_first;  /* first element */                     \
        }
```


```cpp
// head is the LIST_HEAD' addr, head points to a struct "logical list" name
// 
#define LIST_FIRST(head)        ((head)->lh_first)
```


```cpp
head --> the_name_of_the_list_struct {
             struct type* lh_first
         } 
```


```cpp
// 让head被指向的对象的lh_first为NULL
#define LIST_INIT(head) do {                                            \
                LIST_FIRST((head)) = NULL;                                      \
        } while (0)
```
