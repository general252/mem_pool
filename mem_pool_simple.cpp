struct MemoryPool {
  void* data; // 内存块指针
  size_t block_size; // 内存块大小
  std::vector<void*> free_blocks; // 空闲内存块链表
};

void init_memory_pool(MemoryPool* pool, size_t block_size, size_t num_blocks) {
  pool->block_size = block_size;
  pool->data = malloc(block_size * num_blocks);
  for (size_t i = 0; i < num_blocks; ++i) {
    void* block = static_cast<char*>(pool->data) + i * block_size;
    pool->free_blocks.push_back(block);
  }
}


void* allocate_memory_block(MemoryPool* pool) {
  if (pool->free_blocks.empty()) {
    return nullptr;
  }
  void* block = pool->free_blocks.back();
  pool->free_blocks.pop_back();
  return block;
}

void deallocate_memory_block(MemoryPool* pool, void* block) {
  pool->free_blocks.push_back(block);
}


void destroy_memory_pool(MemoryPool* pool) {
  free(pool->data);
  pool->data = nullptr;
  pool->free_blocks.clear();
  pool->block_size = 0;
}


int main() {
  MemoryPool pool;
  init_memory_pool(&pool, 256, 100);

  void* block1 = allocate_memory_block(&pool);
  void* block2 = allocate_memory_block(&pool);

  deallocate_memory_block(&pool, block1);
  deallocate_memory_block(&pool, block2);

  destroy_memory_pool(&pool);

  return 0;
}

