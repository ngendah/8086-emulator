#include "SDL.h"
#include "address.h"
#include "cpu.h"
#include "peripheral.h"
#include "ram.h"
#include <streambuf>

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

  struct KeyBoard final : BUS, Peripheral {
    std::streambuf *_buf{};
    InterruptHandler *_interrupt_handler{};

    void process_input(uint8_t *key_state) {
      (void)key_state; // process the input, write ...
      _interrupt_handler->interrupt(0x21);
    }

  protected:
    uint16_t write(UNUSED_PARAM Address const *,
                   UNUSED_PARAM const Bytes &) override {
      return 0;
    };
    Bytes read(UNUSED_PARAM Address const *,
               UNUSED_PARAM uint16_t size) override {
      return Bytes();
    };
    void bootstrap(std::streambuf *buf, InterruptHandler *handler) override {
      _buf = buf; // receive a region in memory
      _interrupt_handler = handler;
    };
  };

  struct Display final : BUS, Peripheral {
    std::streambuf *_buf{};

    uint16_t write(UNUSED_PARAM Address const *,
                   UNUSED_PARAM const Bytes &) override {
      return 0;
    };
    Bytes read(UNUSED_PARAM Address const *,
               UNUSED_PARAM uint16_t size) override {
      return Bytes();
    };
    void bootstrap(std::streambuf *buf,
                   UNUSED_PARAM InterruptHandler *) override {
      _buf = buf; // receive a region in memory
    };
  };

  struct Pointer final : BUS, Peripheral {
    std::streambuf *_buf{};
    InterruptHandler *_interrupt_handler{};

    uint16_t write(UNUSED_PARAM Address const *,
                   UNUSED_PARAM const Bytes &) override {
      return 0;
    };
    Bytes read(UNUSED_PARAM Address const *,
               UNUSED_PARAM uint16_t size) override {
      return Bytes();
    };
    void bootstrap(std::streambuf *buf, InterruptHandler *handler) override {
      _buf = buf; // receive a region in memory
      _interrupt_handler = handler;
    };
  };

  SDL_Event evt;
  // uint8_t *key_state = nullptr;
  RAM ram(125);
  CPU cpu(&ram);
  Display display;
  KeyBoard keyboard;
  Pointer pointer;
  cpu.bootstrap("./dos.com",
                std::vector<Peripheral *>{&keyboard, &display, &pointer});
  while (!cpu.halt()) {
    while (SDL_PollEvent(&evt)) {
      switch (evt.type) {
      case SDL_QUIT:
        cpu.power_off();
        break;
      }
    }
    // cpu.execute();
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
