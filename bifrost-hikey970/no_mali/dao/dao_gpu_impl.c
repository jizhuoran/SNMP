#include <no_mali/dao/dao_reg_op.h>
#include <no_mali/dao/dao_gpu_impl.h>

#include <mali_kbase.h>

#define DAO_JOB_PER_SLOT 2

struct timer_list js_running_jobs[GPU_MAX_JOB_SLOTS][DAO_JOB_PER_SLOT];


int process_POWER_COMMAND(void) {

	bool has_changed = false;



	
	u32 l2_pwron = dao_reg_read(GPU_CONTROL_REG(L2_PWRON_LO));
	u32 shader_pwron = dao_reg_read(GPU_CONTROL_REG(SHADER_PWRON_LO));
	u32 l2_pwroff = dao_reg_read(GPU_CONTROL_REG(L2_PWROFF_LO));
	u32 shader_pwroff = dao_reg_read(GPU_CONTROL_REG(SHADER_PWROFF_LO));


	if(l2_pwron != 0 || shader_pwron != 0 || l2_pwroff != 0 || shader_pwroff != 0) {
		dao_reg_OR_write(GPU_CONTROL_REG(GPU_IRQ_STATUS), POWER_CHANGED_ALL); 
		has_changed = true;
	}

	if(l2_pwron != 0) {
		sj_dbg_all("SJ DAO: Power On L2 Cache from 0x%lx to 0x%lx \n", dao_reg_read(GPU_CONTROL_REG(L2_READY_LO)), dao_reg_read(GPU_CONTROL_REG(L2_READY_LO)) | l2_pwron);
		dao_reg_OR_write(GPU_CONTROL_REG(L2_READY_LO), l2_pwron); 
		dao_reg_write(GPU_CONTROL_REG(L2_PWRON_LO), 0); 
	}

	if(shader_pwron != 0) {

		sj_dbg_all("SJ DAO: Power On SHADER from 0x%lx to 0x%lx \n", dao_reg_read(GPU_CONTROL_REG(SHADER_READY_LO)), dao_reg_read(GPU_CONTROL_REG(SHADER_READY_LO)) | shader_pwron);
		dao_reg_OR_write(GPU_CONTROL_REG(SHADER_READY_LO), shader_pwron); 
		dao_reg_write(GPU_CONTROL_REG(SHADER_PWRON_LO), 0); 
	}


	if(l2_pwroff != 0) {

		sj_dbg_all("SJ DAO: Power OFF L2 Cache from 0x%lx to 0x%lx \n", dao_reg_read(GPU_CONTROL_REG(L2_READY_LO)), dao_reg_read(GPU_CONTROL_REG(L2_READY_LO)) | l2_pwroff);
		dao_reg_AND_write(GPU_CONTROL_REG(L2_READY_LO), ~l2_pwroff); 
		dao_reg_write(GPU_CONTROL_REG(L2_PWROFF_LO), 0); 
	}

	if(shader_pwroff != 0) {

		sj_dbg_all("SJ DAO: Power OFF SHADER from 0x%lx to 0x%lx \n", dao_reg_read(GPU_CONTROL_REG(SHADER_READY_LO)), dao_reg_read(GPU_CONTROL_REG(SHADER_READY_LO)) | shader_pwroff);
		dao_reg_AND_write(GPU_CONTROL_REG(SHADER_READY_LO), ~shader_pwroff); 
		dao_reg_write(GPU_CONTROL_REG(SHADER_PWROFF_LO), 0); 
	}

	return has_changed;
	
}



int process_GPU_COMMAND(void) {

	u32 val = dao_reg_read(GPU_CONTROL_REG(GPU_COMMAND));

	if (!val) { 
		return false; 
	}

	if(val & GPU_COMMAND_SOFT_RESET) {
		dao_reg_OR_write(GPU_CONTROL_REG(GPU_IRQ_RAWSTAT), RESET_COMPLETED); //unmask it
		dao_reg_OR_write(GPU_CONTROL_REG(GPU_IRQ_STATUS), RESET_COMPLETED); //unmask it
	}

	dao_reg_write(GPU_CONTROL_REG(GPU_COMMAND), 0);

	return true; 

}


int process_IRQ_CLEAR(void) {

	bool has_changed = false;

	u32 job_irq_clear = dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_CLEAR));
	u32 gpu_irq_clear = dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_CLEAR));
	u32 mmu_irq_clear = dao_reg_read(MMU_REG(MMU_IRQ_CLEAR));

	if (job_irq_clear | gpu_irq_clear | mmu_irq_clear) {
		has_changed = true;
	}

	if(job_irq_clear != 0) {

		sj_dbg_all("SJ DAO: Receive JOB_IRQ_CLEAR clear command 0x%lx, 0x%lx, 0x%lx \n", job_irq_clear, dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_RAWSTAT)), dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_STATUS)));

		dao_reg_AND_write(JOB_CONTROL_REG(JOB_IRQ_RAWSTAT), ~job_irq_clear);
		dao_reg_AND_write(JOB_CONTROL_REG(JOB_IRQ_STATUS), ~job_irq_clear);

		dao_reg_write(JOB_CONTROL_REG(JOB_IRQ_CLEAR), 0x0);

	}

	if(gpu_irq_clear != 0) {

		sj_dbg_all("SJ DAO: Receive GPU_IRQ_CLEAR clear command 0x%lx, 0x%lx, 0x%lx \n", gpu_irq_clear, dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_RAWSTAT)), dao_reg_read(GPU_CONTROL_REG(GPU_IRQ_STATUS)));

		dao_reg_AND_write(GPU_CONTROL_REG(GPU_IRQ_RAWSTAT), ~gpu_irq_clear);
		dao_reg_AND_write(GPU_CONTROL_REG(GPU_IRQ_STATUS), ~gpu_irq_clear);

		dao_reg_write(GPU_CONTROL_REG(GPU_IRQ_CLEAR), 0x0);

	}

	if(mmu_irq_clear != 0) {

		sj_dbg_all("SJ DAO: Receive MMU_IRQ_CLEAR clear command 0x%lx, 0x%lx, 0x%lx \n", mmu_irq_clear, dao_reg_read(MMU_REG(MMU_IRQ_RAWSTAT)), dao_reg_read(MMU_REG(MMU_IRQ_STATUS)));

		dao_reg_AND_write(MMU_REG(MMU_IRQ_RAWSTAT), ~mmu_irq_clear);
		dao_reg_AND_write(MMU_REG(MMU_IRQ_STATUS), ~mmu_irq_clear);

		dao_reg_write(MMU_REG(MMU_IRQ_CLEAR), 0x0);

	}

	return has_changed;
}


int process_JS_COMMAND(void) {

	//SJ TODO: noticed that for 4,5,6,7 we meed to check job chain
	int js, i;
	int has_change;

	has_change = false;

	for (js = 0; js < GPU_MAX_JOB_SLOTS; ++js) {
		u32 js_command = dao_reg_read(JOB_SLOT_REG(js, JS_COMMAND));

		if(js_command >= 2 && js_command <= 7) {
			
			if (js_command % 2 == 0) { //SOFT_STOP
				sj_dbg_all("SJ DAO: Receive a SOFT stop command \n");
				dao_reg_write(JOB_SLOT_REG(js, JS_STATUS), 0x03);
			} else {
				sj_dbg_all("SJ DAO: Receive a HARD stop command \n");
				dao_reg_write(JOB_SLOT_REG(js, JS_STATUS), 0x04);
			}

			for (i = 0; i < 2; ++i) {
				del_timer(&js_running_jobs[js][i]);
				sj_dbg_all("SJ DAO: pass delete job %d in %d \n", i, js);
			}

			dao_reg_AND_write(JOB_CONTROL_REG(JOB_IRQ_JS_STATE), ~((u32)1 << js | (u32)1 << (js+16)));

			dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_RAWSTAT), (u32)1 << (js+16));
			dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_STATUS), (u32)1 << (js+16));

			dao_reg_write(JOB_SLOT_REG(js, JS_COMMAND), 0);

			has_change = true;
		}
	}

	return has_change;

}


void js_running_jobs_callback(unsigned long data) {

	u32 js = data >> 1;
	u32 js_state = dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_JS_STATE));
	
	sj_dbg_all("SJ DAO: ------------------------------------- \n");
	sj_dbg_all("SJ DAO: Job denoted as idx %lu on slot %lu finished \n", data & 0x1, data >> 1);
	sj_dbg_all("SJ DAO: ------------------------------------- \n");


	dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_RAWSTAT), (u32)1 << js);
	dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_STATUS), (u32)1 << js);
	
	dao_reg_write(JOB_SLOT_REG(js, JS_STATUS), BASE_JD_EVENT_DONE);

	
	if (js_state & ((u32)1 << js) && js_state & ((u32)1 << (js+16))) {
		dao_reg_AND_write(JOB_CONTROL_REG(JOB_IRQ_JS_STATE), ~((u32)1 << (js+16)));
	} else if (js_state & ((u32)1 << js)) {
		dao_reg_AND_write(JOB_CONTROL_REG(JOB_IRQ_JS_STATE), ~((u32)1 << js));
	}
	else {	
		sj_dbg_all("SJ DAO: ERROR!! ERROR !! idx %lu on slot %lu is empty but some job finished \n", data & 0x1, data >> 1);
	}

}

bool execute_job(u32 js, u64 runing_time) {

	u32 i;

	sj_dbg_all("SJ DAO: DEBUG going to submit a job slot %u for %llu ms \n", js, runing_time);


	for (i = 0; i < 2; ++i) {

		if(timer_pending(&js_running_jobs[js][i]) == 0) {
  			
  			setup_timer(&js_running_jobs[js][i], js_running_jobs_callback, js << 1 | i);
			mod_timer(&js_running_jobs[js][i], jiffies + msecs_to_jiffies(runing_time));
			
			sj_dbg_all("SJ DAO: Run a job with timer %u on slot %u for %llu ms \n", i, js, runing_time);
			return true;
		}
	}
	sj_dbg_all("SJ DAO: DEBUG Fail to submit a job slot %u for %llu ms \n", js, runing_time);
	return false;
}

int process_JS_COMMAND_NEXT(void) {

	bool has_change = false;
	u32 js = 0;

	for (js = 0; js < GPU_MAX_JOB_SLOTS; ++js) {
		
		u32 command_next;

		command_next = dao_reg_read(JOB_SLOT_REG(js, JS_COMMAND_NEXT));

		if(command_next == JS_COMMAND_START) { //JOB SUBMIITED
			
			u64 runing_time;
			u32 js_state;

			//in DAO, js_head is used to store how long this job is supports to be run
			runing_time = (u64)dao_reg_read(JOB_SLOT_REG(js, JS_HEAD_NEXT_HI)) << 32 | (u64)dao_reg_read(JOB_SLOT_REG(js, JS_HEAD_NEXT_LO));

			js_state = dao_reg_read(JOB_CONTROL_REG(JOB_IRQ_JS_STATE));

			if (js_state & ((u32)1 << js)) {
				if (js_state & ((u32)1 << (js + 16))) {
					sj_dbg_all("SJ DAO: ERROR!! ERROR !! Job Slot %u is full !!! \n", js);
				} else {
					dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_JS_STATE), (u32)1 << (16 + js));
				}
			} else {
				dao_reg_OR_write(JOB_CONTROL_REG(JOB_IRQ_JS_STATE), (u32)1 << js);
			}

			if (execute_job(js, runing_time)) {
				dao_reg_write(JOB_SLOT_REG(js, JS_STATUS), BASE_JD_EVENT_ACTIVE);
			} else {
				sj_dbg_all("SJ: ERROR!! ERROR !! Fail to run the job!!! \n");
			}
			
			dao_reg_write(JOB_SLOT_REG(js, JS_COMMAND_NEXT), 0);
			
			has_change = true;
		}
	}
	
	return has_change;
}


void dao_stop_all_jobs_from_gpu(void) {

	int idx = 0, js = 0;
	for (js = 0; js < GPU_MAX_JOB_SLOTS; ++js) {
		for (idx = 0; idx < DAO_JOB_PER_SLOT; ++idx) {
	  		del_timer(&js_running_jobs[js][idx]);
		}
	}
	
}

