#include <stdio.h>

int isAvailable(int puzzle[][9], int row, int col, int num)
{
	int rowStart = (row / 3) * 3;
	int colStart = (col / 3) * 3;
	int i;

	for (i = 0; i<9; ++i)
	{
		if (puzzle[row][i] == num) return 0;
		if (puzzle[i][col] == num) return 0;
		if (puzzle[rowStart + (i % 3)][colStart + (i / 3)] == num) return 0;
	}
	return 1;
}

int fillSudoku(int puzzle[][9], int row, int col)
{
	int i;
	if (row<9 && col<9)
	{
		if (puzzle[row][col] != 0)
		{
			if ((col + 1)<9) return fillSudoku(puzzle, row, col + 1);
			else if ((row + 1)<9) return fillSudoku(puzzle, row + 1, 0);
			else return 1;
		}
		else
		{
			for (i = 0; i<9; ++i)
			{
				if (isAvailable(puzzle, row, col, i + 1))
				{
					puzzle[row][col] = i + 1;
					if ((col + 1)<9)
					{
						if (fillSudoku(puzzle, row, col + 1)) return 1;
						else puzzle[row][col] = 0;
					}
					else if ((row + 1)<9)
					{
						if (fillSudoku(puzzle, row + 1, 0)) return 1;
						else puzzle[row][col] = 0;
					}
					else return 1;
				}
			}
		}
		return 0;
	}
	else return 1;
}

int main()
{
	FILE *fp = fopen("input.txt", "r");
	int puzzle[9][9] = { 0, };
	int i, j, T;
	for (fscanf(fp, "%d", &T); T--;)
	{
		for (i = 0; i < 9; i++)
		{
			fscanf(fp, "%d %d %d %d %d %d %d %d %d\n",
				&puzzle[i][0], &puzzle[i][1], &puzzle[i][2],
				&puzzle[i][3], &puzzle[i][4], &puzzle[i][5],
				&puzzle[i][6], &puzzle[i][7], &puzzle[i][8]);
		}

		if (fillSudoku(puzzle, 0, 0))
		{
			for (i = 1; i < 10; ++i)
			{
				for (j = 1; j < 10; ++j)
				{
					printf("%d ", puzzle[i - 1][j - 1]);
				}
				printf("\n");
			}
		}
	}
	fclose(fp);
	return 0;
}