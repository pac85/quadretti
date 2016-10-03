/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <thread>
#include <stdlib.h>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

int IMAGE_HEIGHT = 7;
int IMAGE_WIDTH = 7;

vector<string> program;


SDL_Window* window;
SDL_Renderer* renderer;

struct icolor
{
    int r, g, b;
    icolor(int _r, int _g, int _b)
    {
        r = _r, g = _g, b = _b;
    }
};

struct ivec2
{
    int x, y;
    ivec2(int _x, int _y)
    {
        x = _x, y = _y;
    }
};


void init()
{
    // Initialize SDL.
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
                return ;

        // Create the window where we will draw.
        window = SDL_CreateWindow("SDL_RenderClear",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        SCREEN_WIDTH, SCREEN_HEIGHT,
                        0);

        // We must call SDL_CreateRenderer in order for draw calls to affect this window.
        renderer = SDL_CreateRenderer(window, -1, 0);
}

#define NUM_COLORS 2

int current_index = 0;

icolor colors[NUM_COLORS] = {icolor(0, 0, 0),icolor(30, 30, 30)};

void next_color()
{
    current_index++;
    if(current_index >= NUM_COLORS)
    {
        current_index = 0;
    }

    SDL_SetRenderDrawColor(renderer, colors[current_index].r, colors[current_index].g, colors[current_index].b, 255);

}

void DrawTile(ivec2 pos)
{
    ivec2 tile_middle = ivec2(
                              pos.x * (SCREEN_WIDTH / IMAGE_WIDTH),
                              pos.y * (SCREEN_HEIGHT / IMAGE_HEIGHT)
                              );

    for(int x = 0; x < (SCREEN_WIDTH / IMAGE_WIDTH); x++)
    {
        for(int y = 0; y < (SCREEN_HEIGHT / IMAGE_HEIGHT); y++)
        {
            SDL_RenderDrawPoint(renderer, x + tile_middle.x, y + tile_middle.y);
        }
    }
    SDL_RenderPresent(renderer);
}

ivec2 current_pos(0, 0);

void parse_program_line(string line)
{
    if(!line.compare("^"))
    {
        current_pos.y --;
        return;
    }
    if(!line.compare("v"))
    {
        current_pos.y ++;
        return;
    }
    if(!line.compare("<"))
    {
        current_pos.x --;
        return;
    }
    if(!line.compare(">"))
    {
        current_pos.x ++;
        return;
    }
    if(!line.compare("nc"))
    {
        next_color();
        return;
    }
    if(!line.compare("f"))
    {
        DrawTile(current_pos);
        return;
    }
}

void run()
{
    for(unsigned int i = 0;i < program.size(); i++)
    {
        if(!program[i].compare(0, 1, "("))
        {
            char * c_times = new char;
            int times;
            char* p_text = new char[program[i].length() - 3];

            program[i].copy(p_text, program[i].length() - 3, 1);

            program[i].copy(c_times, 1, program[i].length() - 2);

            times = atoi(c_times);

            for(int t = 0;t < times;t++)
            {
                for(int ic = 0; ic < 1 + (sizeof(p_text)/sizeof(char)); ic++)
                {
                    string instruction = string(&p_text[ic]);
                    instruction.resize(1);
                    parse_program_line(instruction);
                }
            }
        }
        else
        {
            parse_program_line(program[i]);
        }
    }
}

void parse_command(string cmd)
{
    if(!cmd.compare("list"))
    {
        for(unsigned int i = 0;i < program.size(); i++)
        {
            cout << program[i] << endl;
        }
        return;
    }
    if(!cmd.compare("clear"))
    {
        program.clear();
        // Select the color for drawing. It is set to red here.
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // Clear the entire screen to our selected color.
        SDL_RenderClear(renderer);

        next_color();
        next_color();


        current_pos = ivec2(0, 0);
        SDL_RenderPresent(renderer);
        return;
    }
    if(!cmd.compare("run"))
    {
        run();
        return;
    }

    program.push_back(cmd);
}

void whait_command()
{
    string in_cmd;

    while(in_cmd.compare("exit"))
    {
        cout << "$:" ;
        cin >> in_cmd;
        parse_command(in_cmd);
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char* argv[])
{
        cout << "inserisci larghezza e lunghezza:";
        cin >> IMAGE_WIDTH >> IMAGE_HEIGHT;


        init();

        // Select the color for drawing. It is set to red here.
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Clear the entire screen to our selected color.
        SDL_RenderClear(renderer);

        next_color();
        next_color();

        /*string in_cmd;

        while(in_cmd.compare("exit"))
        {
            cout << "$:" ;
            cin >> in_cmd;
            parse_command(in_cmd);
            SDL_RenderPresent(renderer);
        }*/

        thread input_thread(whait_command);


        SDL_Event evt;
        bool programrunning = true;
        while(programrunning)
        {
              SDL_WaitEvent(&evt);
              if(evt.type == SDL_QUIT)
              programrunning = false;
        }

        input_thread.join();

        //DrawTile(ivec2(0, 3));


        // Up until now everything was drawn behind the scenes.
        // This will show the new, red contents of the window.
        SDL_RenderPresent(renderer);

        // Give us time to see the window.
        //SDL_Delay(5000);

        // Always be sure to clean up
        SDL_Quit();
        return 0;
}
