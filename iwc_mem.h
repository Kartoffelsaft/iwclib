#ifndef IWC_MEM_H
#define IWC_MEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef IWC_WARN
#define IWC_WARN(s) (void)(s)
#endif // IWC_WARN

#ifndef IWC_ALIGNMENT
#define IWC_ALIGNMENT 8
#endif // IWC_ALIGNMENT

/**
 * Specifies which memory operation to do
 * @see iwc_MemFn
 * @see Allocator
 */
typedef enum {
    IWC_ALLOC,
    IWC_FREE,
    IWC_REALLOC,
    IWC_EMPTY,
} iwc_MemOperator;

/**
 * Function pointer for a particular allocator
 * @param info Arbitrary internal data for the allocator
 * @param operation Which memory operation is expected
 * @param ptr In/Out reference to the pointer to act on
 * @param oldSize The size of memory currently pointed at by *ptr
 * @param newSize The expected size of memory pointed at by *ptr after calling
 * @see iwc_MemOperator
 * @see Allocator
 */
typedef bool (*iwc_MemFn)(void* info, iwc_MemOperator operation, void** ptr, size_t oldSize, size_t newSize);

/**
 * Abstract struct for any allocator implementation
 * @see iwc_MemOperator
 * @see iwc_MemFn
 */
typedef struct {
    /**
     * Function that defines how this allocator manages memory
     *
     * @see bufferAllocOp for an example
     *
     * @see iwc_MemFn
     */
    iwc_MemFn operate;

    /**
     * Arbitrary data for a given allocator, passed with every allocation
     * operation for this allocator
     *
     * @see BufferAllocInfo for an example
     */
    void* info;
} iwc_Allocator;

/**
 * Abstracted memory allocation operation
 *
 * @param a Which allocator to retrieve the memory from
 * @param amount Number of bytes to retrieve
 * @return pointer to the allocated memory
 */
void* iwc_alloc(iwc_Allocator a, size_t amount);
/**
 * Abstracted memory free operation
 *
 * @param a Which allocator the memory came from
 * @param what Pointer to the memory to free
 * @param amount Size of the memory getting freed
 */
void iwc_free(iwc_Allocator a, void* what, size_t amount);
/**
 * Abstracted memory reallocation operation
 *
 * @param a Which allocator the memory came from
 * @param what In/Out reference to pointer that will have data resized
 * @param oldSize Current size of the memory pointed to by *ptr
 * @param newSize Desired size of the memory pointed to by *ptr
 */
void iwc_realloc(iwc_Allocator a, void** what, size_t oldSize, size_t newSize);
/**
 * Abstracted memory empty operation
 *
 * This frees all of the data of a particular allocator, invalidating all
 * pointers allocated by it. @see afree for freeing just a single pointer
 *
 * @param a Allocator to empty
 */
void iwc_empty(iwc_Allocator a);

/**
 * Move memory from one allocator to another
 *
 * @param oldPtr reference to the pointer to move
 * @param size Number of bytes pointed to by *oldPtr
 * @param from The allocator *oldPtr is currently pointing to
 * @param to The allocator to move the memory to
 */
void iwc_changeAllocator(void** oldPtr, size_t size, iwc_Allocator from, iwc_Allocator to);

#ifndef IWC_NO_BUILTIN_ALLOCATORS

/**
 * Create a stack-buffer allocator, using a user-provided buffer as a backing
 *
 * NOTE: the available size will be slightly less than the given size, as some
 * space will be used for internal data.
 *
 * SUPPORTS:
 * - ALLOC
 * - FREE (only for last alloc)
 * - REALLOC (only for last alloc)
 * - EMPTY
 *
 * @param out Where to put the stackBuffer allocator @param buf Buffer to use as a backing
 * @param size The size of data pointed to by buf
 *
 * @return Creation success
 */
bool iwc_initStackBufferAllocator(iwc_Allocator* out, void* buf, size_t size);

#ifndef IWC_NO_C_ALLOCATOR_WRAPPER

/**
 * Create an allocator that wraps around the standard C allocator
 *
 * SUPPORTS:
 * - ALLOC
 * - FREE (only for last alloc)
 * - REALLOC (only for last alloc)
 */
bool iwc_initCAllocator(iwc_Allocator* out);

#endif // IWC_NO_C_ALLOCATOR_WRAPPER

#endif // IWC_NO_BUILTIN_ALLOCATORS





#ifdef IWC_MEM_IMPLEMENTATION

void* iwc_alloc(iwc_Allocator a, size_t amount) {
    void* ret = NULL;
    a.operate(a.info, IWC_ALLOC, &ret, 0, amount);
    return ret;
}
void iwc_free(iwc_Allocator a, void* what, size_t amount) {
    a.operate(a.info, IWC_FREE, &what, amount, 0);
}
void iwc_realloc(iwc_Allocator a, void** what, size_t oldSize, size_t newSize) {
    a.operate(a.info, IWC_REALLOC, what, oldSize, newSize);
}
void iwc_empty(iwc_Allocator a) {
    a.operate(a.info, IWC_EMPTY, NULL, 0, 0);
}

void iwc_changeAllocator(void** oldPtr, size_t size, iwc_Allocator from, iwc_Allocator to) {
    void* newPtr = iwc_alloc(to, size);

    // could use memcpy but I'm avoiding including stdlib.h outside of the c allocator wrapper
    for (size_t i = 0; i < size; i++) ((uint8_t*)newPtr)[i] = ((uint8_t*)*oldPtr)[i];

    iwc_free(from, *oldPtr, size);
    *oldPtr = newPtr;
}


#ifndef IWC_NO_BUILTIN_ALLOCATORS

typedef struct {
    uint8_t* head;
    uint8_t* buf;
    size_t bufsize;
} iwc__StackBufferAllocInfo;

bool iwc__stackBufferAllocOp(void* info, iwc_MemOperator op, void** ptr, size_t oldSize, size_t newSize) {
    iwc__StackBufferAllocInfo* a = (iwc__StackBufferAllocInfo*)info;
    size_t osAligned = ((oldSize-1)/IWC_ALIGNMENT + 1)*IWC_ALIGNMENT;
    size_t nsAligned = ((newSize-1)/IWC_ALIGNMENT + 1)*IWC_ALIGNMENT;

    uint8_t** bptr = (uint8_t**)ptr;

    switch (op) {
        case IWC_ALLOC:
            if (a->head + nsAligned > a->buf + a->bufsize) {
                *ptr = NULL;
                return false;
            } else {
                *ptr = a->head;
                a->head += nsAligned;
                return true;
            }
            break;
        case IWC_FREE:
            if (*bptr + osAligned == a->head) {
                a->head = *bptr;
                return true;
            } else {
                return false;
                IWC_WARN("WARNING: stack buffer allocator can only free last alloc");
            }
            break;
        case IWC_REALLOC:
            if (*bptr + osAligned == a->head) {
                if (*bptr + nsAligned > a->buf + a->bufsize) {
                    IWC_WARN("WARNING: stack buffer allocator does not have enough memory");
                    return false;
                } else {
                    a->head = *bptr + nsAligned;
                    return true;
                }
            } else {
                IWC_WARN("WARNING: stack buffer allocator can only realloc last alloc");
                return false;
            }
            break;
        case IWC_EMPTY:
            a->head = a->buf;
            return true;
            break;
    }

    return false;
}

bool iwc_initStackBufferAllocator(iwc_Allocator* out, void* buf, size_t size) {
    if (size <= sizeof(iwc__StackBufferAllocInfo)) return false;
    iwc__StackBufferAllocInfo* sai = (iwc__StackBufferAllocInfo*) buf;
    uint8_t* remBuf = (uint8_t*)buf + ((sizeof(iwc__StackBufferAllocInfo)-1)/IWC_ALIGNMENT + 1)*IWC_ALIGNMENT;
    size_t remSize = size - ((sizeof(iwc__StackBufferAllocInfo)-1)/IWC_ALIGNMENT + 1)*IWC_ALIGNMENT;

    *sai = (iwc__StackBufferAllocInfo){0};
    sai->buf = remBuf;
    sai->head = sai->buf;
    sai->bufsize = remSize;

    *out = (iwc_Allocator){
        .operate = iwc__stackBufferAllocOp,
        .info = sai,
    };

    return true;
}

#ifndef IWC_NO_C_ALLOCATOR_WRAPPER

#include <stdlib.h>

bool iwc__cAllocOp(void* info, iwc_MemOperator op, void** ptr, size_t oldSize, size_t newSize) {
    switch (op) {
        case IWC_ALLOC:
            *ptr = malloc(newSize);
            return *ptr != NULL;
            break;
        case IWC_FREE:
            free(*ptr);
            return true;
            break;
        case IWC_REALLOC:
            *ptr = realloc(*ptr, newSize);
            return *ptr != NULL;
        case IWC_EMPTY:
            IWC_WARN("C Allocator does not support empty.");
            return false;
    }
}

bool iwc_initCAllocator(iwc_Allocator* out) {
    *out = (iwc_Allocator){
        .operate = iwc__cAllocOp,
        .info = NULL,
    };

    return true;
}

#endif // IWC_NO_BUILTIN_ALLOCATORS

#endif // IWC_NO_C_ALLOCATOR_WRAPPER

#endif // IWC_MEM_IMPLEMENTATION


#endif // IWC_MEM_H
