#ifndef SNMP_FRONTEND
#define SNMP_FRONTEND

#include <inttypes.h>

#define SNMP_TATD_TASK 0x2
#define SNMP_TPD_TASK 0x6
#define SNMP_MI_KERNEL 0x23
#define SNMP_CI_KERNEL 0x21

struct kbase_ioctl_job_submit {
	uint64_t addr;
	uint32_t nr_atoms;
	uint32_t stride;
};

typedef uint8_t base_atom_id; /**< Type big enough to store an atom number in */
typedef uint8_t base_jd_dep_type;
typedef uint8_t base_jd_prio;
typedef uint32_t base_jd_core_req;


typedef struct base_jd_udata {
	uint64_t blob[2];	 /**< per-job data array */
} base_jd_udata;


struct base_dependency {
	base_atom_id  atom_id;               /**< An atom number */
	base_jd_dep_type dependency_type;    /**< Dependency type */
};

typedef struct base_jd_atom_v2 {
	uint64_t jc;			    /**< job-chain GPU address */
	struct base_jd_udata udata;		    /**< user data */
	uint64_t extres_list;	    /**< list of external resources */
	uint16_t nr_extres;			    /**< nr of external resources */
	uint16_t compat_core_req;	            /**< core requirements which correspond to the legacy support for UK 10.2 */
	struct base_dependency pre_dep[2];  /**< pre-dependencies, one need to use SETTER function to assign this field,
	this is done in order to reduce possibility of improper assigment of a dependency field */
	base_atom_id atom_number;	    /**< unique number to identify the atom */
	base_jd_prio prio;                  /**< Atom priority. Refer to @ref base_jd_prio for more details */
	uint8_t device_nr;			    /**< coregroup when BASE_JD_REQ_SPECIFIC_COHERENT_GROUP specified */
	uint8_t padding[1];
	base_jd_core_req core_req;          /**< core requirements */
} base_jd_atom_v2;



int SNMP_frontend(uint64_t context, int task_type, int kernel_type);


#endif /* SNMP_FRONTEND */