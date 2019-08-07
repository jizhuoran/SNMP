#ifndef NOMALI_MISSING_HEADER
#define NOMALI_MISSING_HEADER
#include <mali_kbase.h>


void gpu_model_set_dummy_prfcnt_base_cpu(void* cpu_va); 
int gpu_device_create(struct kbase_device *kbdev);
int gpu_device_destroy(struct kbase_device *kbdev);


#endif //NOMALI_MISSING_HEADER