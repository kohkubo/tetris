#include <string.h>

#include "ttrs_display.h"
#include "ttrs_matrix.h"

static bool is_in_matrix(int row, int col, int offset_row, int offset_col)
{
	return (col + offset_col >= 0 && col + offset_col < MATRIX_COL && row + offset_row < MATRIX_ROW);
}

#define EMPTY_CELL 0
#define FILLED_CELL 1

bool can_place_on_matrix(const t_matrix matrix, const t_mino *mino, t_position pos)
{
	for (int i = 0; i < mino->len; i++) {
		for (int j = 0; j < mino->len; j++) {
			if (mino->shape[i][j] == EMPTY_CELL) {
				continue;
			}
			if (!is_in_matrix(pos.row, pos.col, i, j) || matrix[pos.row + i][pos.col + j] == FILLED_CELL) {
				return false;
			}
		}
	}
	return true;
}

void place_mino_on_matrix(t_matrix matrix, t_current_mino *current)
{
	t_position pos = current->pos;

	for (int i = 0; i < current->mino.len; i++) {
		for (int j = 0; j < current->mino.len; j++) {
			if (current->mino.shape[i][j] == FILLED_CELL) {
				matrix[pos.row + i][pos.col + j] = current->mino.shape[i][j];
			}
		}
	}
}

static void clear_filled_line(t_matrix matrix, int row)
{
	for (int i = row; i > 0; i--) {
		memcpy(matrix[i], matrix[i - 1], MATRIX_COL);
	}
	memset(matrix[0], 0, MATRIX_COL);
}

static bool is_filled_line(t_line line)
{
	for (int i = 0; i < MATRIX_COL; i++) {
		if (line[i] != FILLED_CELL) {
			return false;
		}
	}
	return true;
}

static int clear_filled_lines(t_matrix matrix)
{
	int num_of_cleared_lines = 0;

	for (int i = 0; i < MATRIX_ROW; i++) {
		if (is_filled_line(matrix[i])) {
			num_of_cleared_lines++;
			clear_filled_line(matrix, i);
		}
	}
	return num_of_cleared_lines;
}

int lock_down_current_mino(t_tetris *tetris)
{
	place_mino_on_matrix(tetris->matrix, &tetris->current);
	const int num_of_cleared_lines = clear_filled_lines(tetris->matrix);
	update_drop_speed(&tetris->time, num_of_cleared_lines);
	return num_of_cleared_lines;
}
