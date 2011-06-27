#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdlib.h>
#include <libguile.h>
#include "cell.h"

struct board {
	int width, height;
	SCM cell_list;

	SCM update_func;
};

void init_board_type (void);

#endif
