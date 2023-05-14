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

  struct KeyBoard final : Peripheral {
    struct _Port : Port {
      void write_hi(uint8_t val) override { Port::write_hi(val); }
      void write_lo(uint8_t val) override { Port::write_lo(val); }
      void write(uint8_t val) override { Port::write(val); }
      void write(uint16_t val) override { Port::write(val); }
      uint16_t read() const override { return Port::read(); }
      uint8_t read_byte() const override { return Port::read_byte(); }
      uint8_t read_hi() const override { return Port::read_hi(); }
      uint8_t read_lo() const override { return Port::read_lo(); }
      friend struct KeyBoard;

    private:
      _Port(KeyBoard *keyboard) : _keyboard(keyboard) {}
      uint8_t _control_register{};
      KeyBoard *_keyboard;
    };

    uint16_t _port_number = 0x61;
    _Port _port;
    InterruptHandler *_interrupt_handler{};

    KeyBoard() : _port(this) {}

    void process_input(uint8_t *key_state) {
      (void)key_state; // process the input, write ...
      _interrupt_handler->interrupt(0x21);
    }

  protected:
    void bootstrap(Ports *port, InterruptHandler *handler) override {
      port->add(_port_number, &_port);
      _interrupt_handler = handler;
    };
  };

  struct Display final : Peripheral {
    void bootstrap(Ports *ports, UNUSED_PARAM InterruptHandler *) override {
      (void)ports;
    };
  };

  struct Pointer final : Peripheral {
    InterruptHandler *_interrupt_handler{};

    void bootstrap(Ports *ports, InterruptHandler *handler) override {
      (void)ports;
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
