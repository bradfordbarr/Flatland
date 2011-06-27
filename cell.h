#ifndef _CELL_H_
#define _CELL_H_

#include <stdlib.h>
#include <libguile.h>

scm_t_bits cell_tag;

struct cell {
	int x, y;
	int status;

	SCM update_func;
};

SCM make_cell (SCM s_x, SCM s_y, SCM s_status);
SCM set_cell(SCM cell_smob, SCM s_status);
SCM get_status(SCM cell_smob);
void init_cell_type (void);
#endif
