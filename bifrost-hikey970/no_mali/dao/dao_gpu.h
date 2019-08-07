#ifndef NOMALI_DAO_GPU_HEADER
#define NOMALI_DAO_GPU_HEADER

#include <asm/types.h>

extern u32 no_mali_regs[4096];
extern const char *no_mali_reg_name[4096];

extern struct kbase_device *dao_kbdev;

void dao_gpu_init(void);
void dao_gpu_term(void);

int interrupt_rasier(void *data);

void process_reg_write(int debug_depth);


#endif //NOMALI_DAO_GPU_HEADER