#include <linux/timer.h>

#include <no_mali/dao/dao_gpu.h>
#include <no_mali/gpu_dummy.h>
#include <mali_midg_regmap.h>
#include <no_mali/dao/dao_reg_op.h>
#include <no_mali/dao/dao_gpu_impl.h>


u32 no_mali_regs[4096];
const char *no_mali_reg_name[4096];
struct kbase_device *dao_kbdev;

void dao_gpu_init(void) {
 	no_mali_reg_names();
	no_mali_init_reg();   
}



void dao_gpu_term(void) {
	dao_stop_all_jobs_from_gpu();
}






