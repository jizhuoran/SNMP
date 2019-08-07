#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>

#include <mali_kbase.h>
#include <backend/gpu/mali_kbase_device_internal.h>

#include <no_mali/dao/dao_reg_op.h>
#include <no_mali/dao/dao_gpu.h>


int kbase_gpu_irq_handler(struct kbase_device *kbdev) {

	u32 val = dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_STATUS));

	sj_dbg_all("SJ DAO: DEBUG kbase_gpu_irq_handler: val 0x%lx, mask 0x%lx \n", val, dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_MASK)));

	if (!val|| !(val & dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_MASK)))){
		return IRQ_NONE;
	}

	dao_reg_AND_write(GPU_CONTROL_REG(GPU_IRQ_MASK), ~val); //mask it

	kbase_gpu_interrupt(kbdev, val);

	dao_reg_OR_write(GPU_CONTROL_REG(GPU_IRQ_MASK), val); //unmask it

	return IRQ_HANDLED;
}


int kbase_job_irq_handler(struct kbase_device *kbdev) {

	u32 val = dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_STATUS));

	sj_dbg_all("SJ DAO: DEBUG kbase_job_irq_handler: val 0x%lx, mask 0x%lx \n", val, dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_MASK)));

	if (!val || !(val & dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_MASK)))) {
		return IRQ_NONE;
	}

	dao_reg_AND_write(JOB_CONTROL_REG(JOB_IRQ_MASK), ~val); //mask it

	kbase_job_done(kbdev, val);

	dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_MASK), val); //unmask it

	return IRQ_HANDLED;
}

int process_interrupt(void) {
	
	
	if(dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_STATUS))) {
		kbase_gpu_irq_handler(dao_kbdev);
	}

	if(dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_STATUS))) {
		kbase_job_irq_handler(dao_kbdev);
	}
	
	return 0;
}


int interrupt_rasier(void *data) {
	
	struct kbase_device *kbdev = (struct kbase_device *)data;

	do {

		msleep(5);

		if(dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_STATUS))) {
			kbase_gpu_irq_handler(kbdev);
		}

		if(dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_STATUS))) {
			kbase_job_irq_handler(kbdev);
		}

	} while(!kthread_should_stop());


    sj_dbg_all("SJ DAO: The interrupt Handler going to Release!!! \n");

	
	return 0;
}