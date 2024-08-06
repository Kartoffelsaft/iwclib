#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define IWC_MEM_IMPLEMENTATION
#define IWC_WARN(s) fprintf(stderr, "%s\n", s)
#include "iwc_mem.h"

#define RUNTEST(fn) {\
    char* logStr = NULL;\
    size_t logLen = 0;\
    FILE* log = open_memstream(&logStr, &logLen);\
    printf("Running %s... ", #fn);\
    clock_t start = clock();\
    bool success = fn(log);\
    clock_t duration = clock() - start;\
    if (success) {\
        printf("SUCCESS (%fs)\n", (float)duration / CLOCKS_PER_SEC);\
        if (verbose) {\
            fflush(log);\
            printf("%s\n", logStr);\
        }\
    } else {\
        fflush(log);\
        printf("FAIL:\n%s\n", logStr);\
    }\
    free(logStr);\
}

bool genericAllocFreeTest(FILE* log, iwc_Allocator a) {
    char* dataA = iwc_alloc(a, sizeof(char));
    fprintf(log, "%p\n", dataA);
    if (dataA == NULL) return false;

    *dataA = 'A';
    fprintf(log, "dataA contains: %c\n", *dataA);

    char* dataB = iwc_alloc(a, sizeof(char));
    fprintf(log, "%p\n", dataB);
    if (dataB == NULL) return false;

    *dataB = 'B';
    fprintf(log, "dataA contains: %c (after alloc of dataB)\n", *dataA);
    fprintf(log, "dataB contains: %c\n", *dataB);

    if (*dataA != 'A') return false;
    if (*dataB != 'B') return false;

    iwc_free(a, dataB, sizeof(char));

    fprintf(log, "dataA contains: %c (after free of dataB)\n", *dataA);
    if (*dataA != 'A') return false;

    iwc_free(a, dataA, sizeof(char));

    return true;
}

bool cAllocator(FILE* log) {
    iwc_Allocator a;
    iwc_initCAllocator(&a);

    return genericAllocFreeTest(log, a);
}

bool stackBufferAllocator(FILE* log) {
    iwc_Allocator a;
    uint8_t buf[64];
    if (!iwc_initStackBufferAllocator(&a, buf, 64)) return false;

    return genericAllocFreeTest(log, a);
}

bool sbaAlignment(FILE* log) {
    uint8_t buf[64];
    uint8_t* malalignedBuf = buf+3;

    fprintf(log, "buf: %p\n", buf);
    fprintf(log, "malalignedBuf: %p\n", malalignedBuf);

    iwc_Allocator a;
    if (!iwc_initStackBufferAllocator(&a, buf, 61)) return false;

    uint64_t* pleaseAlignMe = iwc_alloc(a, sizeof(uint64_t));
    fprintf(log, "pleaseAlignMe: %p\n", pleaseAlignMe);

    if ((size_t)pleaseAlignMe % sizeof(uint64_t) != 0) return false;

    iwc_free(a, pleaseAlignMe, sizeof(uint64_t));

    return true;
}

int main(int argc, char** argv) {
    bool verbose = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v")
        ||  !strcmp(argv[i], "--verbose")) {
            verbose = true;
        }
    }

    RUNTEST(cAllocator);
    RUNTEST(stackBufferAllocator);
    RUNTEST(sbaAlignment);
}
