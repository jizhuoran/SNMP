#include <linux/kthread.h>

#include <no_mali/dao/dao_gpu.h>
#include <no_mali/gpu_dummy.h>

void gpu_model_set_dummy_prfcnt_base_cpu(void* cpu_va) {

}
 
int gpu_device_create(struct kbase_device *kbdev) {
	dao_kbdev = kbdev;
	dao_gpu_init();
	return 0;
}

int gpu_device_destroy(struct kbase_device *kbdev) {
	dao_gpu_term();
	return 0;
}


void kbase_reg_write(struct kbase_device *kbdev, u16 offset, u32 value,
						struct kbase_context *kctx) {

    sj_dbg_all("SJ: WRITE [%s]: 0x%lx \n", no_mali_reg_name[offset / 4], value);

	no_mali_regs[offset / 4] = value;

	process_reg_write(0);
}

u32 kbase_reg_read(struct kbase_device *kbdev, u16 offset,
						struct kbase_context *kctx) {

	sj_dbg_all("SJ: READ [%s]: 0x%lx \n", no_mali_reg_name[offset / 4], no_mali_regs[offset / 4]);

	return no_mali_regs[offset / 4];
}


struct task_struct *interrupt_task;
int kbase_install_interrupts(struct kbase_device *kbdev) {
    sj_dbg_all("SJ DAO: INTERRUPT INSTALL! \n");
	interrupt_task = kthread_run(&interrupt_rasier, kbdev, "interrupt_rasier");
	return 0;
}

void kbase_release_interrupts(struct kbase_device *kbdev) {
    sj_dbg_all("SJ DAO: INTERRUPT RELEASE! \n");
    kthread_stop(interrupt_task);
    sj_dbg_all("SJ DAO: INTERRUPT RELEASED! \n");
}

void kbase_synchronize_irqs(struct kbase_device *kbdev) {

}