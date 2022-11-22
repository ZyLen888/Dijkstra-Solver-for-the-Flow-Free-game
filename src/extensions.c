#include "extensions.h"
#include "options.h"

//////////////////////////////////////////////////////////////////////
// For sorting colors

int color_features_compare(const void* vptr_a, const void* vptr_b) {

	const color_features_t* a = (const color_features_t*)vptr_a;
	const color_features_t* b = (const color_features_t*)vptr_b;

	int u = cmp(a->user_index, b->user_index);
	if (u) { return u; }

	int w = cmp(a->wall_dist[0], b->wall_dist[0]);
	if (w) { return w; }

	int g = -cmp(a->wall_dist[1], b->wall_dist[1]);
	if (g) { return g; }

	return -cmp(a->min_dist, b->min_dist);

}

//////////////////////////////////////////////////////////////////////
// Place the game colors into a set order

void game_order_colors(game_info_t* info,
                       game_state_t* state) {

	if (g_options.order_random) {
    
		srand(now() * 1e6);
    
		for (size_t i=info->num_colors-1; i>0; --i) {
			size_t j = rand() % (i+1);
			int tmp = info->color_order[i];
			info->color_order[i] = info->color_order[j];
			info->color_order[j] = tmp;
		}

	} else { // not random

		color_features_t cf[MAX_COLORS];
		memset(cf, 0, sizeof(cf));

		for (size_t color=0; color<info->num_colors; ++color) {
			cf[color].index = color;
			cf[color].user_index = MAX_COLORS;
		}
    

		for (size_t color=0; color<info->num_colors; ++color) {
			
			int x[2], y[2];
			
			for (int i=0; i<2; ++i) {
				pos_get_coords(state->pos[color], x+i, y+i);
				cf[color].wall_dist[i] = get_wall_dist(info, x[i], y[i]);
			}

			int dx = abs(x[1]-x[0]);
			int dy = abs(y[1]-y[0]);
			
			cf[color].min_dist = dx + dy;
			
		

		}


		qsort(cf, info->num_colors, sizeof(color_features_t),
		      color_features_compare);

		for (size_t i=0; i<info->num_colors; ++i) {
			info->color_order[i] = cf[i].index;
		}
    
	}

	if (!g_options.display_quiet) {

		printf("\n************************************************"
		       "\n*               Branching Order                *\n");
		if (g_options.order_most_constrained) {
			printf("* Will choose color by most constrained\n");
		} else {
			printf("* Will choose colors in order: ");
			for (size_t i=0; i<info->num_colors; ++i) {
				int color = info->color_order[i];
				printf("%s", color_name_str(info, color));
			}
			printf("\n");
		}
		printf ("*************************************************\n\n");

	}

}

// check if a position is deadend
int pos_check_deadends(const game_info_t* info, const game_state_t* state, pos_t pos) {


	int num_free_neighbor = 0;
	uint8_t last_color = state->last_color;
	pos_t last_pos = state->pos[last_color];
	uint8_t curr_type = cell_get_type(state->cells[pos]);

	for (int dir = DIR_LEFT; dir <= DIR_DOWN; dir++) {

		pos_t neighbor_pos = pos_offset_pos(info, pos, dir);
		

		if (neighbor_pos != INVALID_POS) {

			cell_t neighbor_cell = state->cells[neighbor_pos];
			uint8_t neighbor_type = cell_get_type(neighbor_cell);
			uint8_t neighbor_color = cell_get_color(neighbor_cell);

			// free cell neighbor is free neighbor
			if (neighbor_type == TYPE_FREE) {
				num_free_neighbor++;
			} else if (neighbor_pos == last_pos) {
				// head of current pip is free neighbor
				num_free_neighbor++;
			}

			if (curr_type == TYPE_FREE) {

				if (!(state->completed & (1 << neighbor_color))) {

					// init cell of incomplete and non-current pipe is free neighbor
					if (neighbor_type == TYPE_INIT && neighbor_color != last_color) {
						num_free_neighbor++;
					}
					// check if incomplete goal cell is deadend
					else if (neighbor_type == TYPE_GOAL) {
						num_free_neighbor++;
					}
				}
			}
		}
	}




	// count free neighbor of current position
	if (curr_type == TYPE_FREE) {
		// free cell must have at least 2 free neighbor
		if (num_free_neighbor < 2) {
			return 1;
		}
		
	} else {
		// init and goal cell must have at least 1 free neighbor
		if (num_free_neighbor < 1) {
			return 1;
		}
	}	

	return 0;
}



//////////////////////////////////////////////////////////////////////
// Check for dead-end regions of freespace where there is no way to
// put an active path into and out of it. Any freespace node which
// has only one free neighbor represents such a dead end. For the
// purposes of this check, cur and goal positions count as "free".



int game_check_deadends(const game_info_t* info,
                        const game_state_t* state) {

	// find last position colored
	uint8_t last_color = state->last_color;
	pos_t last_pos = state->pos[last_color];

	for (int dir = DIR_LEFT; dir <= DIR_DOWN; dir++){
		
		pos_t neighbor_pos = pos_offset_pos(info, last_pos, dir);

		if (neighbor_pos != INVALID_POS) {

			cell_t neighbor_cell = state->cells[neighbor_pos];
			uint8_t neighbor_type = cell_get_type(neighbor_cell);
			uint8_t neighbor_color = cell_get_color(neighbor_cell);

			// check if free cell neighbor is deadend
			if (neighbor_type == TYPE_FREE) {
				if (pos_check_deadends(info, state, neighbor_pos)) {
					return 1;
				}
			} else if (!(state->completed & (1 << neighbor_color))) {
				// check if incomplete and not current color init cell is deadend
				if (neighbor_type == TYPE_INIT && neighbor_color != last_color) {
					if (pos_check_deadends(info, state, neighbor_pos)) {
						return 1;
					}
				}
				// check if incomplete goal cell is deadend
				else if (neighbor_type == TYPE_GOAL) {
					if (pos_check_deadends(info, state, neighbor_pos)) {
						return 1;
					}
				}
			}

			// check neighbors of neighbor of last move
			for (int dir1 = DIR_LEFT; dir1 <= DIR_DOWN; dir1++){
		
				pos_t neighbor_pos1 = pos_offset_pos(info, neighbor_pos, dir1);

				if (neighbor_pos1 != INVALID_POS) {

					cell_t neighbor_cell1 = state->cells[neighbor_pos1];
					uint8_t neighbor_type1 = cell_get_type(neighbor_cell1);
					uint8_t neighbor_color1 = cell_get_color(neighbor_cell1);

					// check if free cell neighbor is deadend
					if (neighbor_type1 == TYPE_FREE) {
						if (pos_check_deadends(info, state, neighbor_pos1)) {
							return 1;
						}
					} else if (!(state->completed & (1 << neighbor_color1))) {
						// check if incomplete and not current color init cell is deadend
						if (neighbor_type1 == TYPE_INIT && neighbor_color1 != last_color) {
							if (pos_check_deadends(info, state, neighbor_pos1)) {
								return 1;
							}
						}
						// check if incomplete goal cell is deadend
						else if (neighbor_type1 == TYPE_GOAL) {
							if (pos_check_deadends(info, state, neighbor_pos1)) {
								return 1;
							}
						}
					}

				}
			}
			
		}
	}

	
	return 0;

}
                                         
