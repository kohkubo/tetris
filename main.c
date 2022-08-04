#include <limits.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tetris.h"
#include "ttrs_action.h"
#include "ttrs_matrix.h"
#include "ttrs_mino.h"
#include "ttrs_print.h"
#include "ttrs_time.h"
#include "wrapper.h"

#define SCORE_UNIT 100

#define GAME_OVER "\nGame over!"

extern t_keyhook_func g_keyhooks[UCHAR_MAX];

static void end_tetris(const t_tetris *tetris)
{
	print_matrix(tetris->matrix, printf);
	Puts(GAME_OVER);
	print_score(tetris->score, printf);
}

static t_tetris create_tetris()
{
	t_tetris tetris = {
		.matrix = {},
		.score = 0,
		.is_moved = false,
	};
	tetris.time.interval = INIT_INTERVAL_TIME,
	tetris.time.decrease_time = INIT_DECREASE_TIME;
	return tetris;
}

static void update_score(t_tetris *tetris, int clear_line_count)
{
	tetris->score += SCORE_UNIT * clear_line_count;
}

static t_status update_game(t_tetris *tetris, t_mino *mino, int clear_line_count)
{
	update_drop_speed(&tetris->time, clear_line_count);
	update_score(tetris, clear_line_count);
	return create_new_mino(tetris->matrix, mino);
}

static t_status handle_locked_down(t_tetris *tetris, t_mino *mino)
{
	place_mino_on_matrix(tetris->matrix, mino);
	int clear_line_count = clear_filled_lines(tetris->matrix);
	return update_game(tetris, mino, clear_line_count);
}

static t_status drop_mino_auto(t_tetris *tetris, t_mino *mino)
{
	if (is_time_to_drop(&tetris->time)) {
		update_next_drop_time(&tetris->time);
		return try_drop(tetris, mino);
	}
	return TETRIS_PLAY;
}

static void run_tetris(t_tetris *tetris)
{
	t_status status = TETRIS_PLAY;
	t_mino mino = generate_random_mino();

	while (status != TETRIS_GAME_OVER) {
		status = handle_key_input(tetris, &mino);
		if (status == TETRIS_PLAY) {
			status = drop_mino_auto(tetris, &mino);
		}
		if (tetris->is_moved) {
			refresh_screen(tetris, &mino);
			tetris->is_moved = false;
		}
		if (status == TETRIS_LOCK_DOWN) {
			status = handle_locked_down(tetris, &mino);
		}
	}
}

static void init_ncurses()
{
	Initscr();
	timeout(1);
}

static void end_ncurses()
{
	Endwin();
}

int main()
{
	t_tetris tetris = create_tetris();

	srand(time(NULL));
	init_keyhook_funcp();
	init_ncurses();
	run_tetris(&tetris);
	end_ncurses();
	end_tetris(&tetris);
}
