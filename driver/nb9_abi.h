#ifndef NB9_ABI_H
#define NB9_ABI_H

#include <linux/types.h>

#define OP_CMD_READ                 601
#define OP_CMD_WRITE                602
#define OP_CMD_BASE                 603
#define OP_CMD_HOOK_STATUS          618
#define OP_CMD_READ_SAFE            620
#define OP_CMD_WRITE_SAFE           621

typedef struct _COPY_MEMORY {
    pid_t     pid;
    uintptr_t addr;
    void     *buffer;
    size_t    size;
} COPY_MEMORY;

typedef struct _HOOK_STATUS {
    uint8_t       active;
    uint8_t       armed;
    pid_t         pid;
    unsigned long vaddr;
    unsigned long target_addr;
    uint32_t      rot[3];
    uint32_t      stack_rot[3];
    uint32_t      stack_count;
    uint32_t      stack_offset;
    uint64_t      hit_count;
    uint64_t      captured_x0;
} HOOK_STATUS;

#endif
