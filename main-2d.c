#include <stdio.h>

#include <SDL2/SDL.h>

#define FAILED_STATUS 1

#define WINDOW_W 1300
#define WINDOW_H 800
#define CELL_W 10
#define CELL_H 10

// has to divide by two
#define CELLS_IN_ROW WINDOW_H / CELL_H
#define CELLS_IN_COLUMN WINDOW_W / CELL_W

struct Context
{
    SDL_Window *window;
    SDL_Renderer *window_renderer;
};

void clear_context(struct Context *context)
{
    if (context->window_renderer != NULL)
    {
        SDL_RenderClear(context->window_renderer);
    }
    if (context->window != NULL)
    {
        SDL_DestroyWindow(context->window);
    }
    SDL_Quit();
}

void exit_with_failure(char *error_message, struct Context *context)
{
    printf("%s\n", error_message);
    printf("Last SDL error %s \n", SDL_GetError());
    clear_context(context);
    exit(FAILED_STATUS);
}

int apply_rule(int cell_matrix[CELLS_IN_ROW][CELLS_IN_COLUMN], int row, int column)
{
    int current_cell = cell_matrix[row][column];

    int neighbors_alive = 0;
    // look at square 3x3
    for (int i = row - 1; i < row + 2; i++)
    {
        for (int j = column - 1; j < column + 2; j++)
        {
            if (!(i == row && j == column))
            {
                neighbors_alive += cell_matrix[i][j];
            }
        }
    }

    // if cell is alive
    if (current_cell)
    {
        if (neighbors_alive >= 4 || neighbors_alive <= 1)
        {
            return 0; // cell dies
        }
        else
        {
            return cell_matrix[row][column]; // nothing happens
        }
    }
    else
    { // curent cell is dead
        if (neighbors_alive == 3)
        {
            return 1; // cell birth
        }
        else
        {
            return cell_matrix[row][column]; // nothing happens
        }
    }
}

// selecting 9 neighbors
// ignore edges for simplicity
void next_state(int cell_matrix[CELLS_IN_ROW][CELLS_IN_COLUMN])
{
    int new_state[CELLS_IN_ROW][CELLS_IN_COLUMN];
    memset(new_state, 0, sizeof(new_state));

    for (int i = 1; i < CELLS_IN_ROW - 1; i++)
    {
        for (int j = 1; j < CELLS_IN_COLUMN - 1; j++)
        {
            new_state[i][j] = apply_rule(cell_matrix, i, j);
        }
    }

    // copy values
    for (int i = 0; i < CELLS_IN_ROW; i++)
    {
        for (int j = 0; j < CELLS_IN_COLUMN; j++)
        {
            cell_matrix[i][j] = new_state[i][j];
        }
    }
}

void draw_cell(struct Context *context, int row, int column, int cell_state)
{
    struct SDL_Rect rect;

    rect.w = CELL_W;
    rect.h = CELL_H;

    rect.x = column * CELL_W;
    rect.y = row * CELL_H;

    if (cell_state == 1)
    {
        // black
        if (SDL_SetRenderDrawColor(context->window_renderer, 255, 255, 255, 255) < 0)
        {
            exit_with_failure("ERROR: failed to StRenderDrawColor", context);
        };
    }
    else if (cell_state == 0)
    {
        // white
        if (SDL_SetRenderDrawColor(context->window_renderer, 0, 0, 0, 255) < 0)
        {
            exit_with_failure("ERROR: failed to StRenderDrawColor", context);
        };
    }
    else
    {
        printf("cell_state[%d][%d]: %d\n", row, column, cell_state);
        exit_with_failure("cell state should be 0 or 1", context);
    }

    if (SDL_RenderFillRect(context->window_renderer, &rect) < 0)
    {
        exit_with_failure("ERROR: failed to RenderFillRect", context);
    };
}

void draw_cells(struct Context *context, int cells_matrix[CELLS_IN_ROW][CELLS_IN_COLUMN])
{
    for (int i = 0; i < CELLS_IN_ROW; i++)
    {
        for (int j = 0; j < CELLS_IN_COLUMN; j++)
        {
            draw_cell(context, i, j, cells_matrix[i][j]);
        }
    }
}

void init_cells(int cells_matrix[CELLS_IN_ROW][CELLS_IN_COLUMN])
{
    for (int i = 0; i < CELLS_IN_ROW; i++)
    {
        for (int j = 0; j < CELLS_IN_COLUMN; j++)
        {
            if ((rand() % 5) == 1)
            {
                cells_matrix[i][j] = 1;
            }
            else
            {
                cells_matrix[i][j] = 0;
            }
        }
    }
}

int main()
{
    struct Context context;
    context.window = NULL;
    context.window_renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        exit_with_failure("ERROR: Failed to initialize the SDL2 library: ", &context);
    }

    context.window = SDL_CreateWindow("Cell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);

    if (context.window == NULL)
    {
        exit_with_failure("ERROR: Failed to create window: ", &context);
    }

    context.window_renderer = SDL_CreateRenderer(context.window, -1, SDL_RENDERER_ACCELERATED);

    if (context.window_renderer == NULL)
    {
        exit_with_failure("ERROR: Failed to get renderer: ", &context);
    }

    if (SDL_RenderClear(context.window_renderer) < 0)
    {
        exit_with_failure("ERROR: Failed to clear renderer: ", &context);
    }

    // initialize state
    int cells_matrix[CELLS_IN_ROW][CELLS_IN_COLUMN];
    init_cells(cells_matrix);

    SDL_RenderClear(context.window_renderer);
    draw_cells(&context, cells_matrix);
    SDL_RenderPresent(context.window_renderer);

    char window_open = 1;
    while (window_open)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) > 0)
        {
            switch (e.type)
            {
            case SDL_QUIT:
                window_open = 0;
                break;
            case SDL_KEYDOWN:
            {
                const Uint8 *keys = SDL_GetKeyboardState(NULL);
                if (keys[SDL_SCANCODE_DOWN] == 1)
                {
                    SDL_RenderClear(context.window_renderer);
                    init_cells(cells_matrix);
                    draw_cells(&context, cells_matrix);
                    SDL_RenderPresent(context.window_renderer);
                }
                else if (keys[SDL_SCANCODE_RIGHT] == 1)
                {
                    SDL_RenderClear(context.window_renderer);
                    draw_cells(&context, cells_matrix);
                    next_state(cells_matrix);
                    SDL_RenderPresent(context.window_renderer);
                    SDL_Delay(50);
                }
                break;
            }
            }
        }
    }

    clear_context(&context);
}