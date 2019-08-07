#include "snmp_frontend.hpp"

int SNMP_frontend(uint64_t context, int task_type, int kernel_type) {
	
	uint64_t cctx = *(uint64_t*)(context + 40);

 	struct kbase_ioctl_job_submit* submit = (struct kbase_ioctl_job_submit*)(cctx + 536784);
 	struct base_jd_atom_v2* job_atom_list = (struct base_jd_atom_v2*)submit->addr;
 	
 	job_atom_list->udata.blob[1] = (task_type << 8 | kernel_type);

}