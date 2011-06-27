#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libguile.h>
#include "cell.h"
#include "board.h"

int main (int argc, char **argv) {
	const char *script = NULL;
	const char *entry = NULL;
	SCM entry_func_symbol;
	SCM entry_func;
	int c;

	while(1) {
		static struct option long_options[] = 
		{
			{"script", required_argument, 0, 's'},
			{"entry", required_argument, 0, 'e'},
			{0, 0, 0, 0}
		};
		int opt_index;
		c = getopt_long(argc, argv, "s:e:w:h:", long_options, &opt_index);

		if (c == -1) {
			break;
		}

		switch (c) {
			case 0:
				break;
			case 's':
				script = optarg;
				break;
			case 'e':
				entry = optarg;
				break;
			default:
				abort();
		}
	}

	scm_init_guile();

	init_cell_type();
	init_board_type();

	if (script) {
		scm_c_primitive_load(script);

		if (entry) {
			entry_func_symbol = scm_c_lookup(entry);
			entry_func = scm_variable_ref(entry_func_symbol);
			scm_call_0(entry_func);
		}
	} else {
		scm_shell(argc, argv);
	}

	return 0;
}
