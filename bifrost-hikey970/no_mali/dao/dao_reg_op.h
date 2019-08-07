#ifndef NOMALI_DAO_REG_OP_HEADER
#define NOMALI_DAO_REG_OP_HEADER

#include <asm/types.h>

void no_mali_init_reg(void);
void no_mali_reg_names(void);


u32 dao_reg_read(u16 offset);
void dao_reg_write(u16 offset, u32 value);
void dao_reg_OR_write(u16 offset, u32 value);
void dao_reg_AND_write(u16 offset, u32 value);


#endif //NOMALI_DAO_REG_OP_HEADER