
#include "main.h"


int main(void)

{        
	osal_init_driver();

	osal_init_system();	

	osal_run_system();

	return 0;
}
