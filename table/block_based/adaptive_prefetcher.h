#pragma once
#include "table/block_based/block_based_table_reader.h"

namespace ROCKSDB_NAMESPACE {
class AdaptivePrefetcher {
 public:
  AdaptivePrefetcher(size_t initial_auto_prefetch_size)
      : prefetch_size_(initial_auto_prefetch_size),
        initial_auto_prefetch_size_(initial_auto_prefetch_size) {}

  void PrefetchIfNeeded(const BlockBasedTable::Rep* rep,
                        const BlockHandle& handle, size_t prefetch_size);

  SmartPrefetchBuffer* prefetch_buffer() { return prefetch_buffer_.get(); }

  void UpdateReadPattern(const size_t& offset, const size_t& len) {
    prev_offset_ = offset;
    prev_len_ = len;
  }

  bool IsBlockSequential(const size_t& offset) {
    return (prev_len_ == 0 || (prev_offset_ + prev_len_ == offset));
  }

  void ResetPrefetchSize(size_t initial_auto_prefetch_size) {
    num_file_reads_ = 1;
    initial_auto_prefetch_size_ = initial_auto_prefetch_size;
    prefetch_size_ = initial_auto_prefetch_size_;
    return;
  }

 private:
  // current prefetch size
  size_t prefetch_size_;
  // for adaptive prefetch size
  size_t initial_auto_prefetch_size_ = 512;
  size_t max_auto_prefetch_size_ = 8192;
  size_t num_file_reads_ = 0;
  // info of prev read, for judging if the read pattern is sequential
  size_t prev_offset_ = 0;
  size_t prev_len_ = 0;
  std::unique_ptr<SmartPrefetchBuffer> prefetch_buffer_;
};
}  // namespace ROCKSDB_NAMESPACE
