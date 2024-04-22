#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Sets the selected cache index for direct cache access.
/// @param index The index to select.
static inline void cache_setIndex(u32 index)
{
    asm volatile("mcr p15, 3, %0, c15, c0, 0\n" :: "r"(index));
}

/// @brief Invalidates the entire instruction cache.
static inline void ic_invalidateAll()
{
    asm volatile("mcr p15, 0, %0, c7, c5, 0\n" :: "r"(0));
}

/// @brief Prefetch an instruction cache line.
/// @param code A pointer to the code to prefetch.
static inline void ic_prefetch(const void* code)
{
    asm volatile("mcr p15, 0, %0, c7, c13, 1\n" :: "r"(code));
}

/// @brief Sets the lock settings of the instruction cache.
/// @param segment The selected cache segment.
/// @param load Enables load mode when true.
static inline void ic_setLockdown(u32 segment, bool load)
{
    u32 lockConfig = segment | (load ? 0x80000000 : 0);
    asm volatile("mcr p15, 0, %0, c9, c0, 1\n" :: "r"(lockConfig));
}

/// @brief Sets the tag of the instruction cache line selected with cache_setIndex.
/// @param tag The tag value to set.
static inline void ic_setTag(u32 tag)
{
    asm volatile("mcr p15, 3, %0, c15, c1, 0\n" :: "r"(tag));
}

/// @brief Drains the write buffer.
static inline void dc_drainWriteBuffer()
{
    asm volatile("mcr p15, 0, %0, c7, c10, 4\n" :: "r"(0));
}

/// @brief Flushes the data cache in the given range. The effected cache lines will be marked as invalid.
///        Ptr should be 32-byte aligned. ByteCount is rounded up to 32-byte multiples.
/// @param ptr A pointer to the memory block to flush. Should be 32-byte aligned.
/// @param byteCount The number of bytes to flush. Will be rounded up to 32-byte multiples.
extern void dc_flushRange(const void* ptr, u32 byteCount);

/// @brief Invalidates the data cache in the given range.
/// @param ptr A pointer to the memory block to invalidate. Should be 32-byte aligned.
/// @param byteCount The number of bytes to invalidate. Will be rounded up to 32-byte multiples.
extern void dc_invalidateRange(void* ptr, u32 byteCount);

/// @brief Invalidates a single cache line.
/// @param ptr A pointer to an address in the cache line to invalidate.
static inline void dc_invalidateLine(void* ptr)
{
    asm volatile("mcr p15, 0, %0, c7, c6, 1\n" :: "r"(((u32)ptr) & ~0x1F));
}

/// @brief Halts the processor until an interrupt occurs.
static inline void arm_waitForInterrupt(void)
{
    asm volatile("mcr p15, 0, %0, c7, c0, 4\n" :: "r"(0));
}

#ifdef __cplusplus
}
#endif
