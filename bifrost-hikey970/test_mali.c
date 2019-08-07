/* mod2.c */
// #include <linux/module.h>

#include <linux/delay.h>


#include "mali_kbase.h"
#include <linux/compat.h>
#include <linux/types.h>

#include <linux/kthread.h>


struct kbase_context* construct_kctx(void) {

	struct kbase_device *kbdev = NULL;
	struct kbase_context *kctx;

	int ret = 0;

	kbdev = kbase_find_device(55);

	if (!kbdev)
		return NULL;

	kctx = kbase_create_context(kbdev, is_compat_task());
	if (!kctx) {
		return NULL;
	}

	kctx->api_version = KBASE_API_VERSION((__u16)11, (__u16)4);

	if (atomic_cmpxchg(&kctx->setup_in_progress, 0, 1) != 0)
		return NULL;

	ret = kbase_context_set_create_flags(kctx, (u32)0);
	if (ret)
		return NULL;

	atomic_set(&kctx->setup_complete, 1);

	return kctx;
}


struct submit_for_thread {
	struct kbase_context* kctx;
	void __user *user_addr;
	u32 nr_atoms;
	u32 stride;
};

int submit_job_with_thread(void *data) {

	struct submit_for_thread *submit = (struct submit_for_thread *)data;

    kbase_jd_submit_SJ_kernel(submit->kctx, submit->user_addr, submit->nr_atoms, submit->stride, false);

    return 0;
}

static int __init mod2_init(void) {

	struct kbase_context* kctx;
	struct base_jd_atom_v2 user_atom;
	struct kbase_va_region *reg;


	int i = 0;
	for (i = 0; i < 3; ++i) {
		printk("--------------SJ: Launch the Test--------------\n");
	}

	kctx = construct_kctx();

	if (kctx == NULL) {
		sj_dbg_all("SJ: bie gen ge ren si de le\n");
		return -1;
	}

	

/*
    printk("--------------SJ: alloc memory --------------\n");


	u64 flags = (u32)(1 << 0) | (u32)(1 << 1) | (u32)(1 << 2) | (u32)(1 << 3) | (u32)(1 << 12) | (u32)(1 << 13);
	u64 gpu_va;

    sj_dbg_all("SJ: the gpu_va: %p \n", gpu_va);


	reg = kbase_mem_alloc(kctx, (u64)32,
			(u64)32,
			(u64)0x0,
			&flags, &gpu_va);

    printk("--------------SJ: sync memory --------------\n");


    // reg->cpu_alloc->pages[0]


    sj_dbg_all("SJ: the reg: %p, gpu_va: %p, start_pfn: %llu, nr_pages: %lu flags: %p \n", reg, gpu_va, reg->start_pfn, reg->nr_pages, reg->flags);

    struct basep_syncset sset = {
		.mem_handle.basep.handle = gpu_va,
		.user_addr = ((u64)(810000000) << PAGE_SHIFT),
		.size = (u64)0x4,
		.type = (__u8)(1U << 0)
	};

	kbase_sync_now(kctx, &sset);

*/
	printk("--------------SJ: going to submit--------------\n");




	user_atom.atom_number = 1;
	user_atom.pre_dep[0].atom_id = 0;
	user_atom.pre_dep[1].atom_id = 0;
	user_atom.device_nr = 0;
	user_atom.core_req = ((u32)1 << 10);//((u32)1 << 10) | ((u32)1 << 11); //((u32)1 << 1) | ((u32)1 << 2) | ((u32)1 << 3);
	user_atom.jc = ((u64) 6000);


	u32 nr_atoms = 0x1;
	u32 stride = sizeof(base_jd_atom_v2);
	void __user *user_addr = &user_atom;
	



	struct submit_for_thread submit;

	submit.kctx = kctx;
	submit.user_addr = user_addr;
	submit.nr_atoms = nr_atoms;
	submit.stride = stride;
	user_atom.prio = 0;

	printk("--------------SJ: quick submit 0--------------\n");

    kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);

	printk("--------------SJ: quick submit 1--------------\n");

	user_atom.atom_number = 2;
	user_atom.jc = ((u64) 6000);
    kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);

    msleep(1000);

	printk("--------------SJ: quick submit 2--------------\n");

	user_atom.prio = 1;
	user_atom.atom_number = 3;
	user_atom.jc = ((u64) 3000);
    kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);
	printk("--------------SJ: quick submit 3--------------\n");


	user_atom.atom_number = 4;
	user_atom.jc = ((u64) 3000);
    kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);


#ifdef NOOOOOOO
    // kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);
    kthread_run(submit_job_with_thread, &submit, "submit_job_with_thread 1");


	// user_atom.atom_number = 2;
	user_atom.jc = ((u64) 200);
    kthread_run(submit_job_with_thread, &submit, "submit_job_with_thread 2");


    msleep(5000);
#endif
 //    user_atom.atom_number = 3;
	// user_atom.jc = ((u64) 200);
 //    kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);


 //    user_atom.atom_number = 4;
	// user_atom.jc = ((u64) 200);
 //    kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);






    // kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);
    // kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);
    
    // msleep(100);
    // kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);
    // msleep(500);
    // kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);
    // msleep(500);
    // kbase_jd_submit_SJ_kernel(kctx, user_addr, nr_atoms, stride, false);

    return 0;
}

static void __exit mod2_exit(void)
{
    printk("mod2 exiting\n");
}

module_init(mod2_init); 
module_exit(mod2_exit);
// MODULE_LICENSE("GPL v2");