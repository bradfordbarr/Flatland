#include "cell.h"

SCM make_cell (SCM s_x, SCM s_y, SCM s_status) {
	SCM smob;
	struct cell *cell;
	int x = scm_to_int(s_x);
	int y = scm_to_int(s_y);
	int status = scm_to_int(s_status);

	cell = (struct cell *)scm_gc_malloc(sizeof(struct cell), "cell");

	cell->x = x;
	cell->y = y;
	cell->status = status;
	cell->update_func = SCM_BOOL_F;

	SCM_NEWSMOB(smob, cell_tag, cell);

	return smob;
}

SCM set_cell (SCM cell_smob, SCM s_status) {
	struct cell *cell; 

	scm_assert_smob_type(cell_tag, cell_smob);

	cell = (struct cell *) SCM_SMOB_DATA(cell_smob);
	cell->status = scm_to_int(s_status);

	return SCM_UNSPECIFIED;
}

SCM get_status (SCM cell_smob) {
	struct cell *cell;

	scm_assert_smob_type(cell_tag, cell_smob);
	cell = (struct cell *) SCM_SMOB_DATA(cell_smob);

	return scm_from_int(cell->status);
}

static SCM mark_cell (SCM cell_smob) {
	struct cell *cell = (struct cell *) SCM_SMOB_DATA(cell_smob);

	scm_gc_mark(cell->update_func);

	return SCM_BOOL_F;
}

static size_t free_cell (SCM cell_smob) {
	struct cell *cell = (struct cell *) SCM_SMOB_DATA(cell_smob);

	scm_gc_free(cell, sizeof(struct cell), "cell");
	return 0;
}

static int print_cell (SCM cell_smob, SCM port, scm_print_state *pstate) {
	struct cell *cell = (struct cell *) SCM_SMOB_DATA(cell_smob);

	scm_puts("#<cell ", port);
	scm_display(scm_from_int(cell->x), port);
	scm_puts(":", port);
	scm_display(scm_from_int(cell->y), port);
	scm_puts(" ", port);
	scm_display(scm_from_int(cell->status), port);
	scm_puts(">", port);

	return 1;
}

void init_cell_type (void) {
	cell_tag = scm_make_smob_type("cell", sizeof(struct cell));
	scm_set_smob_mark(cell_tag, mark_cell);
	scm_set_smob_free(cell_tag, free_cell);
	scm_set_smob_print(cell_tag, print_cell);

	scm_c_define_gsubr("make-cell", 3, 0, 0, make_cell);
	scm_c_define_gsubr("set-cell", 2, 0, 0, set_cell);
	scm_c_define_gsubr("get-status", 1, 0, 0, get_status);
}
