#include "table/block_based/adaptive_prefetcher.h"

#include <stdio.h>

#include "rocksdb/file_system.h"
#include "table/block_based/block_based_table_reader.h"
namespace ROCKSDB_NAMESPACE {
void AdaptivePrefetcher::InitPrefetchBuffer(const BlockBasedTable::Rep* rep,
                                            const BlockHandle& handle,
                                            const size_t prefetch_size) {
  // printf("call InitPrefetchBuffer\n");
  // fixed_size prefetch
  if (prefetch_size > 0) {
    rep->CreateSmartPrefetchBufferIfNotExists(prefetch_size, prefetch_size,
                                              &prefetch_buffer_, false, 0, 0);
    return;
  }

  // Adaptive auto prefetch, which will be enabled if the number of reads
  // reached 2 and read patterns are sequential.
  // TODO: determine max_auto_prefetch_size_ by actual performance
  size_t len = BlockBasedTable::BlockSizeWithTrailer(handle);
  size_t offset = handle.offset();

  // If max_auto_readahead_size is set to be 0, no data will be prefetched.
  max_auto_prefetch_size_ = rep->table_options.max_auto_readahead_size;
  if (max_auto_prefetch_size_ == 0) {
    return;
  }

  if (!IsBlockSequential(offset)) {
    UpdateReadPattern(offset, len);
    ResetPrefetchSize(initial_auto_prefetch_size_);
    return;
  }

  UpdateReadPattern(offset, len);
  num_file_reads_++;
  if (num_file_reads_ <= 2) {
    return;
  }

  rep->CreateSmartPrefetchBufferIfNotExists(
      initial_auto_prefetch_size_, max_auto_prefetch_size_, &prefetch_buffer_,
      true, num_file_reads_, 2);
  return;
}
}  // namespace ROCKSDB_NAMESPACE
