#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct Data 
{
    char *data;
    int size;
};

struct Cell
{
    bool lock;
    int value;
};

struct Puzzle 
{
    struct Cell *cells[9][9];
};

void initPuzzle(struct Puzzle *p, struct Data *d);
void bruteForceSolve(struct Puzzle *p);
bool isSolved(char *p);

int main(int argc, char *argv[])
{
    struct Data d = {0};
    struct Puzzle p = {0};

    if (argc != 2) 
    { 
        fprintf(stderr, "Error, bad argc. Expected 1, got %d.\n", argc-1);
        fprintf(stderr, "Usage: solve <filename>\n");
        exit(1);
    } 
    else 
    {
        FILE *file = fopen(argv[1], "rb");

        if (file) 
        {
            fseek(file, 0, SEEK_END);
            d.size = ftell(file);
            fseek(file, 0, SEEK_SET);

            d.data = malloc(d.size+1);
            fread(d.data, 1, d.size, file);
            d.data[d.size] = 0;

            fclose(file);
        }
    }

    initPuzzle(&p, &d);
    bruteForceSolve(&p);

    return 0;
}

void initPuzzle(struct Puzzle *p, struct Data *d)
{
    if (d->size != 90) 
    {
        fprintf(stderr, "Error, bad file.\n");
        fprintf(stderr, "Expected size 90, got %d.\n", d->size);
        exit(1);
    } 
    else 
    {
        for (int i = 0; i < 9; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                if (j != 10) // 10th byte is \n, skip.
                {
                    p->cells[i][j] = (struct Cell *) malloc(sizeof(struct Cell));
                    p->cells[i][j]->value = d->data[i*10+j] - 48;
                    p->cells[i][j]->lock = (p->cells[i][j]->value == 0) ? false : true;
                }
            }
        }
    }
}

void bruteForceSolve(struct Puzzle *p)
{
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            printf("%d", p->cells[i][j]->value);
        }

        printf("\n");
    }
}
