#include <no_mali/dao/dao_gpu.h>
#include <no_mali/dao/dao_reg_op.h>
#include <no_mali/dao/dao_gpu_impl.h>

#include <mali_kbase.h>


u32 dao_reg_read(u16 offset) {
	return no_mali_regs[offset / 4];
}

void dao_reg_write(u16 offset, u32 value) {
    
    sj_dbg_all("SJ DAO: WRITE [%s]: 0x%lx \n", no_mali_reg_name[offset / 4] , value);
	no_mali_regs[offset / 4] = value;
}

void dao_reg_AND_write(u16 offset, u32 value) {

	no_mali_regs[offset / 4] &= value;
    sj_dbg_all("SJ DAO: WRITE [%s]: 0x%lx \n", no_mali_reg_name[offset / 4] , no_mali_regs[offset / 4]);

}

void dao_reg_OR_write(u16 offset, u32 value) {

	no_mali_regs[offset / 4] |= value;
    sj_dbg_all("SJ DAO: WRITE [%s]: 0x%lx \n", no_mali_reg_name[offset / 4] , no_mali_regs[offset / 4]);


}

void process_reg_write(int debug_depth) {
	
	u32 has_changed = false;

	if (debug_depth > 100) {
		sj_dbg_all("SJ DAO: ABORT! ABORT! ABORT! \n");
		sj_dbg_all("SJ DAO: It seems that we process too reg too many times! \n");
		sj_dbg_all("SJ DAO: ABORT! ABORT! ABORT! \n");
		return;
	}

	has_changed &= process_GPU_COMMAND();
	has_changed &= process_JS_COMMAND_NEXT();
	has_changed &= process_JS_COMMAND();
	has_changed &= process_POWER_COMMAND();
	has_changed &= process_IRQ_CLEAR();	
	// has_changed &= process_interrupt();	

	if (has_changed) {
		process_reg_write(debug_depth + 1);
	}
}