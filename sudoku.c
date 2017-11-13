#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// #define DEBUG

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
void printPuzzle(struct Puzzle *p);
bool bruteForceSolve(struct Puzzle *p);
bool violatesRule(struct Puzzle *p);
bool noBlanks(struct Puzzle *p);

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

    fprintf(stdout, "Puzzle:\n");
    printPuzzle(&p);

    if (!bruteForceSolve(&p))
    {
        fprintf(stderr, "Error: unsolvable.\n");
        exit(1);
    }

    fprintf(stdout, "---------\n");
    fprintf(stdout, "Solution:\n");
    printPuzzle(&p);

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

void printPuzzle(struct Puzzle *p)
{
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            if (p->cells[i][j]->value != 0)
            {
                printf("%d", p->cells[i][j]->value);
            }
            else
            {
                printf(" ");
            }
        }

        printf("\n");
    }
}

bool bruteForceSolve(struct Puzzle *p)
{
#ifdef DEBUG
    printPuzzle(p);
#endif

    if (violatesRule(p))
    {
        return false;
    }
    else
    {
        if (noBlanks(p))
        {
            return true;
        }
    }
    
    bool stop = false;
    int pi, pj;

    for (int i = 0; (i < 9) && !stop; ++i)
    {
        for (int j = 0; (j < 9) && !stop; ++j)
        {
            if (p->cells[i][j]->value == 0)
            {
                pi = i; 
                pj = j;
                stop = true;
            }
        }
    }

    bool solved = false;

#ifdef DEBUG
    fprintf(stdout, "Working on (%d, %d)\n", pi, pj);
#endif
    do
    {
        if (p->cells[pi][pj]->value == 9)
        {
            p->cells[pi][pj]->value = 0;
            return false;
        }
        else
        {
            p->cells[pi][pj]->value++;
        }

        solved = bruteForceSolve(p);
    } while (!solved);

    return solved;
}

bool violatesRule(struct Puzzle *p)
{
    // Rows
    for (int i = 0; i < 9; ++i)
    {
        int nums[10] = {0};
        for (int j = 0; j < 9; ++j)
        {
            ++nums[p->cells[i][j]->value];
        }

        // Check for duplicates.
        for (int n = 1; n < 10; ++n)
        {
            if (nums[n] > 1)
            {
#ifdef DEBUG
                fprintf(stdout, "Violation: row %d\n", i);
#endif
                return true;
            }
        }
    }

    // Columns
    for (int j = 0; j < 9; ++j)
    {
        int nums[10] = {0};
        for (int i = 0; i < 9; ++i)
        {
            ++nums[p->cells[i][j]->value];
        }

        // Check for duplicates.
        for (int n = 1; n < 10; ++n)
        {
            if (nums[n] > 1)
            {
#ifdef DEBUG
                fprintf(stdout, "Violation: col %d\n", j);
#endif
                return true;
            }
        }
    }

    // Boxes
    /*
        111222333
        111222333
        111222333
        444555666
        444555666
        444555666
        777888999
        777888999
        777888999

           m,n
        at 0,0, 0<=i<3, j<3
        at 0,1, 3<=i<6, j<3
        at 0,2, 6<=i<9, j<3

        at 1,0, 0<=i<3, 3=<j<6
        at 1,1, 3<=i<6, 3=<j<6
        at 1,2, 6<=i<9, 3=<j<6

        at 2,0, 0<=i<3, 6=<j<9
        at 2,1, 3<=i<6, 6=<j<9
        at 2,2, 6<=i<9, 6=<j<9
    */
    for (int n = 0; n < 3; ++n)
    {
        for (int m = 0; m < 3; ++m)
        {
            int nums[10] = {0};
            for (int i = (3*n); i < (3*n + 3); ++i)
            {
                for (int j = (3*m); j < (3*m + 3); ++j)
                {
                    ++nums[p->cells[i][j]->value];
                }
            }

            // Check for duplicates.
            for (int n = 1; n < 10; ++n)
            {
                if (nums[n] > 1)
                {
#ifdef DEBUG
                    fprintf(stdout, "Violation: box (%d,%d)\n", n, m);
#endif
                    return true;
                }
            }
        }
    }

    return false;
}

bool noBlanks(struct Puzzle *p)
{
    bool filled = true;
    bool stop = false;

    for (int i = 0; (i < 9) && !stop; ++i)
    {
        for (int j = 0; (j < 9) && !stop; ++j)
        {
            if (p->cells[i][j]->value == 0)
            {
                filled = false;
                stop = true;
            }
        }
    }

    return filled;
}
