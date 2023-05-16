#include "SDL.h"
#include "cpu.h"
#include "display.h"
#include "keyboard.h"
#include "pointer.h"
#include "ram.h"

int main(int, char **) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
      SDL_CreateWindow("SDL2 Test Window", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_Event evt;
  // uint8_t *key_state = nullptr;
  RAM ram(125);
  CPU cpu(&ram);
  Display display;
  KeyBoard keyboard;
  Pointer pointer;
  cpu.bootstrap("./dos.com",
                std::vector<Device *>{&keyboard, &display, &pointer});
  while (!cpu.power_off()) {
    while (SDL_PollEvent(&evt)) {
      switch (evt.type) {
      case SDL_QUIT:
        keyboard.process_input(nullptr);
        break;
      }
    }
    // cpu.execute();
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
