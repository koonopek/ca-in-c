#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>

#define FAILED_STATUS 1

#define WINDOW_W 1300
#define WINDOW_H 800
#define CELL_W 5
#define CELL_H 5

// has to divide by two
#define CELLS_IN_ROW WINDOW_W / CELL_W
#define CELLS_IN_COLUMN WINDOW_H / CELL_H

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

#define NEIGHBORS_COUNT 3
#define RULE_SIZE 8
int choosen_rule = 90;

void decode_rule(int *rule, int rule_id)
{
    for (int i = 0; i < RULE_SIZE; i++)
    {
        rule[i] = rule_id % 2;
        rule_id = rule_id >> 1;
    }
}

int state_transition(int *neighbors)
{
    int rule_index = 0;
    int bit = 1;
    for (int i = 0; i < NEIGHBORS_COUNT; i++)
    {
        rule_index += neighbors[i] * bit;
        bit = bit << 1;
    }

    int rule[RULE_SIZE];
    decode_rule(rule, choosen_rule);

    return rule[rule_index];
}

// selecting 3 neighbors
// don't modify edges
void next_state_1d(int *cells_current_state)
{
    int new_state[CELLS_IN_ROW];

    for (int i = 1; i < CELLS_IN_ROW - 1; i++)
    {
        int neighbors[] = {cells_current_state[i - 1], cells_current_state[i], cells_current_state[i + 1]};

        new_state[i] = state_transition(neighbors);
    }

    // copy values
    for (int i = 0; i < CELLS_IN_ROW; i++)
    {
        cells_current_state[i] = new_state[i];
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
        exit_with_failure("cell state should be 0 or 1", context);
    }

    if (SDL_RenderFillRect(context->window_renderer, &rect) < 0)
    {
        exit_with_failure("ERROR: failed to RenderFillRect", context);
    };
}

void draw_cells(struct Context *context, int row, int *cells)
{
    for (int i = 0; i < CELLS_IN_ROW; i++)
    {
        draw_cell(context, row, i, cells[i]);
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

    // init cells - 1D
    int cells[CELLS_IN_ROW] = {0};
    cells[CELLS_IN_ROW / 2] = 1;

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
                if (keys[SDL_SCANCODE_UP] == 1)
                {
                    choosen_rule = (choosen_rule + 1) % 256;
                    for (int i = 0; i < CELLS_IN_ROW; i++)
                    {
                        cells[i] = 0;
                    }
                    cells[CELLS_IN_ROW / 2] = 1;
                    printf("choosen rule: %d\n", choosen_rule);
                }
                else if (keys[SDL_SCANCODE_DOWN] == 1)
                {
                    choosen_rule = (choosen_rule - 1) % 256;
                    for (int i = 0; i < CELLS_IN_ROW; i++)
                    {
                        cells[i] = 0;
                    }
                    cells[CELLS_IN_ROW / 2] = 1;
                    printf("choosen rule: %d\n", choosen_rule);
                }
                else if (keys[SDL_SCANCODE_RIGHT] == 1)
                {

                    SDL_RenderClear(context.window_renderer);
                    for (int row = 0; row < CELLS_IN_COLUMN; row++)
                    {
                        draw_cells(&context, row, cells);
                        next_state_1d(cells);
                    }
                    SDL_RenderPresent(context.window_renderer);
                }
                break;
            }
            }
        }
    }

    clear_context(&context);
}