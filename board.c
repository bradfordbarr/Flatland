#include "board.h"

static scm_t_bits board_tag;

static SCM make_board (SCM s_width, SCM s_height) {
	int i;
	int j;

	SCM smob;
	struct board *board;
	int width = scm_to_int(s_width);
	int height = scm_to_int(s_height);

	board = (struct board *) scm_gc_malloc(sizeof(struct board), "board");

	board->width = width;
	board->height = height;
	board->update_func = SCM_BOOL_F;
	board->cell_list = SCM_EOL;

	for (i = height - 1; i >= 0; i--) {
		SCM row = SCM_EOL;
		for (j = width - 1; j >= 0; j--) {
			SCM y_offset = scm_from_int(i);
			SCM x_offset = scm_from_int(j);
			row = scm_cons(make_cell(x_offset, y_offset, scm_from_int(0)), row);
		}
		board->cell_list = scm_cons(row, board->cell_list);
	}

	SCM_NEWSMOB(smob, board_tag, board);

	return smob;
}

SCM clear_board (SCM board_smob) {
	int i;
	int j;
	struct board *board;
	struct cell *cell;

	scm_assert_smob_type(board_tag, board_smob);
	
	board = (struct board *) SCM_SMOB_DATA(board_smob);
	for (i = 0; i < board->height; i++) {
		SCM row = scm_list_ref(board->cell_list, scm_from_int(i));
		for (j = 0; j < board->width; j++) {
			cell = (struct cell *) SCM_SMOB_DATA(scm_list_ref(row, scm_from_int(j)));
			cell->status = 0;
		}
	}

	scm_remember_upto_here_1(board_smob);
	return SCM_UNSPECIFIED;
}

SCM get_cell (SCM board_smob, SCM s_x, SCM s_y) {
	struct board *board;

	scm_assert_smob_type(board_tag, board_smob);

	board = (struct board *) SCM_SMOB_DATA(board_smob);

	return scm_list_ref(scm_list_ref(board->cell_list, s_y), s_x);
}

SCM get_neighbors (SCM board_smob, SCM cell_smob) {
	struct board *board;
	struct cell *cell;
	SCM list;
	SCM neighbor;
	int i;
	int j;
	int x;
	int y;

	scm_assert_smob_type(board_tag, board_smob);
	scm_assert_smob_type(cell_tag, cell_smob);

	board = (struct board *) SCM_SMOB_DATA(board_smob);
	cell = (struct cell *) SCM_SMOB_DATA(cell_smob);

	list = SCM_EOL;
	for (i = -1; i < 2; i++) {
		for (j = -1; j < 2; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			x = cell->x + i;
			y = cell->y + j;
			if (x >= 0 && x < board->width && y >= 0 && y < board->height) {
				neighbor = scm_list_ref(scm_list_ref(board->cell_list, scm_from_int(y)), scm_from_int(x));
				list = scm_cons(neighbor, list);
			}
		}

	}

	return list;
}

SCM get_living_neighbors (SCM board_smob, SCM cell_smob) {
	SCM list; struct cell *cell;
	int i;
	int count;

	scm_assert_smob_type(board_tag, board_smob);
	scm_assert_smob_type(cell_tag, cell_smob);

	list = get_neighbors(board_smob, cell_smob);

	count = 0;
	for (i = 0; i < scm_to_int(scm_length(list)); i++) {
		cell = (struct cell *) SCM_SMOB_DATA(scm_list_ref(list, scm_from_int(i)));
		if (cell->status > 0) {
			count++;
		}
	}

	return scm_from_int(count);
}

SCM apply_rule (SCM board_smob, SCM rule_func) {
	SCM cell;
	struct board *board;
	int i; 
	int j;

	scm_assert_smob_type(board_tag, board_smob);
	board = (struct board *) SCM_SMOB_DATA(board_smob);

	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			cell = scm_list_ref(scm_list_ref(board->cell_list, scm_from_int(j)), scm_from_int(i));
			scm_call_2(rule_func, cell, get_living_neighbors(board_smob, cell));
		}
	}

	return SCM_UNSPECIFIED;
}

SCM status_list (SCM board_smob) {
	struct board *board;
	struct cell *cell;
	int i;
	int j;
	SCM cell_smob;
	SCM list;
	SCM row;

	scm_assert_smob_type(board_tag, board_smob);
	board = (struct board *) SCM_SMOB_DATA(board_smob);

	list = SCM_EOL;
	for (i = board->height - 1; i >= 0; i--) {
		row = SCM_EOL;
		for (j = board->width - 1; j >= 0; j--) {
			cell_smob = scm_list_ref(scm_list_ref(board->cell_list, scm_from_int(j)), scm_from_int(i));
			cell = (struct cell *) SCM_SMOB_DATA(cell_smob);
			row = scm_cons(get_status(cell_smob), row);
		}
		list = scm_cons(row, list);
	}

	return list;
}

static SCM mark_board (SCM board_smob) {
	struct board *board = (struct board *) SCM_SMOB_DATA(board_smob);

	scm_gc_mark(board->update_func);
	scm_gc_mark(board->cell_list);

	return SCM_BOOL_F;
}

static size_t free_board (SCM board_smob) {
	struct board *board = (struct board *) SCM_SMOB_DATA(board_smob);

	scm_gc_free(board, sizeof(struct board), "board");

	return 0;
}

static int print_board (SCM board_smob, SCM port, scm_print_state *pstate) {
	struct board *board = (struct board *)SCM_SMOB_DATA(board_smob);

	scm_puts("#<board ", port);
	scm_display(scm_from_int(board->width), port);
	scm_puts(":", port);
	scm_display(scm_from_int(board->height), port);
	scm_puts(">", port);

	return 1;
}

void init_board_type (void) {
	board_tag = scm_make_smob_type("board", sizeof(struct board));
	scm_set_smob_mark(board_tag, mark_board);
	scm_set_smob_free(board_tag, free_board);
	scm_set_smob_print(board_tag, print_board);

	scm_c_define_gsubr("make-board", 2, 0, 0, make_board);
	scm_c_define_gsubr("clear-board", 1, 0, 0, clear_board);
	scm_c_define_gsubr("get-cell", 3, 0, 0, get_cell);
	scm_c_define_gsubr("get-neighbors", 2, 0, 0, get_neighbors);
	scm_c_define_gsubr("get-living-neighbors", 2, 0, 0, get_living_neighbors);
	scm_c_define_gsubr("apply-rule", 2, 0, 0, apply_rule);
	scm_c_define_gsubr("status-list", 1, 0, 0, status_list);
}
