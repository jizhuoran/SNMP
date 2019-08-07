#ifndef NOMALI_DAO_GPU_IMPL_HEADER
#define NOMALI_DAO_GPU_IMPL_HEADER


int process_JS_COMMAND(void);
int process_JS_COMMAND_NEXT(void);
int process_POWER_COMMAND(void);
int process_GPU_COMMAND(void);
// int process_JS_COMMAND(void);
int process_IRQ_CLEAR(void);
int process_interrupt(void);

void dao_stop_all_jobs_from_gpu(void);


#endif //NOMALI_DAO_GPU_IMPL_HEADER