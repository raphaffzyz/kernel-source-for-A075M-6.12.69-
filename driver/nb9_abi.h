#ifndef NB9_ABI_H
#define NB9_ABI_H

#include <linux/types.h>

#define OP_CMD_READ                 601
#define OP_CMD_WRITE                602
#define OP_CMD_BASE                 603
// ... (Adicione os outros #defines que você precisa aqui)

typedef struct _COPY_MEMORY {
    pid_t     pid;
    uintptr_t addr;
    void     *buffer;
    size_t    size;
} COPY_MEMORY;

// ... (Adicione as outras structs do seu kernel.h aqui)

#endif
