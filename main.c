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
    SDL_Surface *window_surface;
};

void clear_context(struct Context *context)
{
    if (context->window_surface != NULL)
    {
        SDL_FreeSurface(context->window_surface);
    }
    if (context->window != NULL)
    {
        SDL_DestroyWindow(context->window);
    }
}

void exit_with_failure(char *error_message, struct Context *context)
{
    printf("%s\n", error_message);
    printf("Last SDL error %s \n", SDL_GetError());
    clear_context(context);
    exit(FAILED_STATUS);
}

void draw_rect(struct Context *context, SDL_Rect *rect, Uint32 color)
{
    int status = SDL_FillRect(context->window_surface, rect, color);
    if (status < 0)
    {
        printf("ERROR: Failed to draw rect: %s \n", SDL_GetError());
        clear_context(context);
        exit(status);
    }
}

#define NEIGHBORS_COUNT 3
#define RULE_SIZE 8
int choosen_rule = 0;

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
    decode_rule(rule, 90);

    return rule[rule_index];
}

// selecting 3 neighbors
// don't modify edges
void next_state(int *cells_current_state)
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

Uint32 rgb(struct Context *context, Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_MapRGB(context->window_surface->format, r, g, b);
}

Uint32 cell_derive_color(struct Context *context, int cell_state)
{
    if (cell_state == 0)
    {
        // black
        return rgb(context, 0, 0, 0);
    }
    else if (cell_state == 1)
    {
        // white
        return rgb(context, 255, 255, 255);
    }
    exit_with_failure("cell state should be 0 or 1", context);

    return -1;
}

void draw_cell(struct Context *context, int row, int column, int cell_state)
{
    struct SDL_Rect rect;

    rect.w = CELL_W;
    rect.h = CELL_H;

    rect.x = column * CELL_W;
    rect.y = row * CELL_H;

    Uint32 color = cell_derive_color(context, cell_state);

    draw_rect(context, &rect, color);
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
    context.window_surface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        exit_with_failure("ERROR: Failed to initialize the SDL2 library: ", &context);
    }

    context.window = SDL_CreateWindow("Cell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);

    if (context.window == NULL)
    {
        exit_with_failure("ERROR: Failed to create window: ", &context);
    }

    context.window_surface = SDL_GetWindowSurface(context.window);

    if (context.window_surface == NULL)
    {
        exit_with_failure("ERROR: Failed to get surface of window: ", &context);
    }

    if (SDL_UpdateWindowSurface(context.window) < 0)
    {
        exit_with_failure("ERROR: Failed to update window surface: ", &context);
    }

    int cells[CELLS_IN_ROW] = {0};
    cells[CELLS_IN_ROW / 2] = 1;

    // draw CA

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
                // case SDL_KEYDOWN:
                //     Uint8 *keys = SDL_GetKeyboardState(NULL);

                //     if (keys[SDL_SCANCODE_UP] == 1)
                //     {
                //         choosen_rule++;
                //     }
                //     //      else if (keys[SDL_SCANCODE_S] == 1)
                //     //         m_direction = Direction::DOWN;
                //     // else if (keys[SDL_SCANCODE_A] == 1)
                //     //     m_direction = Direction::LEFT;
                //     // else if (keys[SDL_SCANCODE_D] == 1)
                //     //     m_direction = Direction::RIGHT;
                //     break;
            }

            for (int row = 0; row < CELLS_IN_COLUMN; row++)
            {
                draw_cells(&context, row, cells);
                SDL_UpdateWindowSurface(context.window);
                SDL_Delay(1);
                next_state(cells);
            }
        }
    }

    clear_context(&context);
}