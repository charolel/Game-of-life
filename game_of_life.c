#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#define COLS 80
#define ROWS 25

int **CreateMatrix(int y, int x);
int **ReadMatrix(int **matrix_old, int x, int y);
int MatrixAnalysis(int **matrix_old, int y, int x);
int **UpdateMatrix(int **matrix_old, int **matrix_new, int y, int x);
int **UploadMatrix(int **matrix_old, int **matrix_new, int y, int x);
void Output(int **matrix_old, int y, int x, int ywindow, int speed);
void RunGame(int **matrix_old, int **matrix_new, int **matrix_check, int y, int x);
int CheckLoop(int **matrix_new, int **matrix_check, int y, int x);

int main()
{
    int **matrix_check = CreateMatrix(ROWS, COLS);
    int **matrix_old = CreateMatrix(ROWS, COLS);
    int **matrix_new = CreateMatrix(ROWS, COLS);

    RunGame(matrix_old, matrix_new, matrix_check, ROWS, COLS);

    free(matrix_check);
    free(matrix_old);
    free(matrix_new);

    printf("end of the game");
    return 0;
}

int **CreateMatrix(int y, int x)
{
    int **data = (int **)malloc(y * sizeof(int *) + x * y * sizeof(int));
    int *start = (int *)(data + y);
    for (int i = 0; i < y; i++)
    {
        data[i] = start + i * x;
    }
    return data;
}

int **ReadMatrix(int **matrix_old, int y, int x)
{
    printf("enter filename in in this format: [number].\nyou can choose file \"1\", \"2\", \"3\", \"4\" or \"5\".\n");
    char filename[10];
    scanf("%s", filename);
    strcat(filename, ".txt");

    FILE *f;
    f = fopen(filename, "r");
    if (f == NULL)
        printf("error reading file.");
    else
    {
        for (int i = 0; i < y; i++)
        {
            for (int j = 0; j < x; j++)
            {
                fscanf(f, "%d", &matrix_old[i][j]);
            }
        }
    }
    fclose(f);

    return matrix_old;
}

int MatrixAnalysis(int **matrix_old, int y, int x)
{
    int count = 0;
    int new_state = 0;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int shift_y = ((y + i) + ROWS) % ROWS;
            int shift_x = ((x + j) + COLS) % COLS;

            if (!(i == 0 && j == 0) && matrix_old[shift_y][shift_x] == 1)
            {
                count++;
            }
        }
    }
    if (matrix_old[y][x] == 0 && count == 3)
    {
        new_state = 1;
    }

    if (matrix_old[y][x] == 1 && (count == 2 || count == 3))
    {
        new_state = 1;
    }
    return new_state;
}

int **UpdateMatrix(int **matrix_old, int **matrix_new, int y, int x)
{
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            matrix_new[i][j] = MatrixAnalysis(matrix_old, i, j);
        }
    }
    return matrix_new;
}

int **UploadMatrix(int **matrix_old, int **matrix_new, int y, int x)
{
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            matrix_old[i][j] = matrix_new[i][j];
        }
    }
    return matrix_old;
}

void Output(int **matrix_old, int y, int x, int ywindow, int speed)
{
    clear();

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            if (matrix_old[i][j] == 0)
            {
                printw(" ");
            }
            else if (matrix_old[i][j] == 1)
            {
                printw("*");
            }
        }
        printw("\n");
    }

    // в левом нижнем углу пишем скорость игры:
    mvwprintw(stdscr, ywindow - 1, 0, "w or key up: speed+;\ts or key down: speed-;\tspeed = %d", speed / 10000);
    refresh();
}

void RunGame(int **matrix_old, int **matrix_new, int **matrix_check, int y, int x)
{
    matrix_old = ReadMatrix(matrix_old, y, x);
    matrix_new = UpdateMatrix(matrix_old, matrix_new, y, x);
    int ywindow, xwindow; // кол-во строк и столбцов на экране терминала
    int flag = 1, speed = 100000, speed_max = 400000, speed_change = 50000, speed_min = 0, key;

    // hello, ncurses!
    initscr();

    noecho();              // не отображаем вводимые символы
    curs_set(0);           // 0 - курсор невидимый, 1 - нормальный, 2 - очень заметный
    nodelay(stdscr, true); // отменяет ожидание ввода у getch
    keypad(stdscr, true);  // разблокировали спец. клавиши (стрелки)

    // first iteration
    getmaxyx(stdscr, ywindow, xwindow); // определили размер окна терминала
    Output(matrix_old, y, x, ywindow, speed);

    // cycle
    while (flag != 0)
    {
        matrix_check = UploadMatrix(matrix_check, matrix_old, y, x);
        matrix_old = UploadMatrix(matrix_old, matrix_new, y, x);
        matrix_new = UpdateMatrix(matrix_old, matrix_new, y, x);

        getmaxyx(stdscr, ywindow, xwindow);
        Output(matrix_old, y, x, ywindow, speed);

        key = getch();
        if (speed >= speed_min && speed <= speed_max)
        {
            if (key == 'w' || key == 259)
                speed -= speed_change;
            else if (key == 's' || key == 258)
                speed += speed_change;
            else if (key == 'q' || key == 'Q')
                flag = 0;
        }
        if (speed == speed_min)
            speed += speed_change;
        else if (speed == speed_max)
            speed -= speed_change;

        usleep(speed);

        if (flag != 0)
            flag = CheckLoop(matrix_new, matrix_check, y, x);
    }

    // last iteration
    Output(matrix_new, y, x, ywindow, speed);
    endwin();
}

int CheckLoop(int **matrix_new, int **matrix_check, int y, int x)
{
    int flag = 1;
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            if (matrix_new[i][j] == matrix_check[i][j])
            {
                flag++;
            }
        }
    }

    if (flag == (x * y + 1))
    {
        flag = 0;
    }

    return flag;
}
