#include <cstdio>
#include <cstdlib>
#include <cstring>

const size_t BLOCK_SIZE_MIN = 8; // 最小内存块大小
const size_t BLOCK_SIZE_MAX = 512; // 最大内存块大小

// 定义一个结构体 Block，包含指向内存块的指针、内存块大小以及下一个内存块的指针。内存块的大小需要满足要求，可以通过预处理指令来定义内存块大小的上下限。例如：
struct Block {
    char* ptr; // 指向内存块的指针
    size_t size; // 内存块大小
    Block* next; // 下一个内存块的指针
};


// 定义多级内存池（Memory Pool）结构体 MemoryPool，包含多个内存块的链表。内存池中的内存块按照大小分配到不同的链表中，每个链表中的内存块大小都在一个区间范围内，不同链表的内存块大小区间不同。
struct MemoryPool {
    size_t block_size_min;
    size_t block_size_max;
    Block* block_list[BLOCK_SIZE_MAX / BLOCK_SIZE_MIN]; // 内存块链表数组
};

// 定义内存池的初始化函数 Init()。在初始化函数中，为内存池中的每个内存块链表分配一个头结点，并将它们都初始化为空。
void Init(MemoryPool& pool) {
    memset(pool.block_list, 0, sizeof(pool.block_list));
}

// 定义内存池的内存块分配函数 Alloc()。在内存块分配函数中，根据要分配的内存块的大小，在相应的内存块链表中查找合适大小的内存块。
// 如果找到了合适的内存块，就从链表中删除该内存块，并返回指向内存块的指针。
// 如果没有找到合适的内存块，则向系统申请一个新的内存块，将其加入到对应的内存块链表中，然后返回指向内存块的指针。
void* Malloc(MemoryPool& pool, size_t size) {
    if (size > BLOCK_SIZE_MAX) {
        void* m = malloc(size);
        memset(m, 0, size);

        return m;
    }
    int index = size / BLOCK_SIZE_MIN - 1;
    if (size % BLOCK_SIZE_MIN != 0) {
        index++;
    }
    if (pool.block_list[index] == nullptr) {
        pool.block_list[index] = new Block();
        pool.block_list[index]->next = nullptr;
    }

    if (pool.block_list[index]->next == nullptr) {
        size_t block_size = (index + 1) * BLOCK_SIZE_MIN;
        size_t block_num = BLOCK_SIZE_MAX / block_size;
        void* ptr = malloc(block_size * block_num);
        memset(ptr, 0, block_size * block_num);
        for (size_t i = 0; i < block_num; ++i) {
            Block* block = new Block();
            block->ptr = static_cast<char*>(ptr) + i * block_size;
            block->size = block_size;
            block->next = pool.block_list[index]->next;
            pool.block_list[index]->next = block;
        }
    }

    Block* block = pool.block_list[index]->next;
    pool.block_list[index]->next = block->next;
    char* ptr = block->ptr;
    delete block;

    return ptr;
}

// 定义内存池的内存块释放函数 Free()。在内存块释放函数中，将要释放的内存块加入到相应的内存块链表的头部。
void Free(MemoryPool& pool, void* ptr, size_t size) {
    if (size > BLOCK_SIZE_MAX) {
        free(ptr);
        return;
    }
    int index = size / BLOCK_SIZE_MIN - 1;
    if (size % BLOCK_SIZE_MIN != 0) {
        index++;
    }

    Block* block = new Block();
    block->ptr = (char*)ptr;
    block->size = size;
    block->next = pool.block_list[index]->next;
    memset(block->ptr, 0, block->size);

    pool.block_list[index]->next = block;
}

// 定义内存池的销毁函数 Destroy()。在销毁函数中，释放所有内存块链表中的内存块，并释放所有头结点。
void Destroy(MemoryPool& pool) {
    for (int i = 0; i < BLOCK_SIZE_MAX / BLOCK_SIZE_MIN; ++i) {
        Block* p = pool.block_list[i]->next;
        while (p != nullptr) {
            Block* next = p->next;
            free(p->ptr);
            delete p;
            p = next;
        }
        delete pool.block_list[i];
    }
}

// 使用多级内存池管理不固定大小的内存块，可以有效地减少内存分配和释放时的系统调用次数，提高内存分配和释放的效率。同时，由于内存池中的内存块都是预先申请并管理的，可以避免内存碎片的产生。

// 定义内存池的测试函数 Test()。在测试函数中，首先通过内存池申请不同大小的内存块，然后使用内存块，最后释放内存块。测试函数的代码如下：

// 在测试函数中，首先定义了一个大小为 10 的数组 size，存储了需要申请的内存块的大小。然后，使用内存池依次申请内存块、使用内存块和释放内存块。在使用内存块时，将每个内存块的第一个字母设置为该内存块的大小对应的数字。最后，在释放所有内存块之后，调用内存池的销毁函数 Destroy()。

// 完整的代码如下：
void Test() {
    MemoryPool pool;
    pool.block_size_min = BLOCK_SIZE_MIN;
    pool.block_size_max = BLOCK_SIZE_MAX;
    Init(pool);

    const int NUM = 10;
    char* ptr[NUM];
    size_t size[NUM] = { 15, 20, 30, 40, 60, 80, 120, 200, 300, 512 };

    // Allocate memory blocks
    for (int i = 0; i < NUM; ++i) {
        ptr[i] = static_cast<char*>(Malloc(pool, size[i]));
    }

    // Use memory blocks
    for (int i = 0; i < NUM; ++i) {
        for (size_t j = 0; j < size[i]; ++j) {
            ptr[i][j] = i + 'A';
        }
        printf("ptr[%d]: %s\n", i, ptr[i]);
    }

    // Free memory blocks
    for (int i = 0; i < NUM; ++i) {
        Free(pool, ptr[i], size[i]);
    }

    Destroy(pool);
}

// 这段代码实现了一个不固定大小的内存池。首先定义了 `Block` 结构体来表示内存块，包括指向内存的指针、内存块大小和下一个内存块的指针。然后定义了 `MemoryPool` 结构体来表示内存池，包括最小块大小和最大块大小以及各个块大小对应的内存块列表。`Init` 函数用来初始化内存池，将内存块列表的指针都置为 `nullptr`。`Malloc` 函数用来分配内存，如果需要分配的大小超过了最大块大小，就直接调用系统的 `malloc` 函数。否则，找到块大小对应的列表，如果列表中没有可用的内存块，就分配一块连续的内存，然后按照块大小将内存切割成多个内存块，然后把这些内存块添加到列表中。从列表中取出一个内存块返回。`Free` 函数用来释放内存，如果需要释放的大小超过了最大块大小，就直接调用系统的 `free` 函数。否则，将要释放的内存块添加到对应大小的内存块列表的头部。`Destroy` 函数用来销毁内存池，释放所有内存块。`Test` 函数用来测试内存池，分配一些内存块，使用它们，然后释放它们。
int main() {
    Test();
    return 0;
}

