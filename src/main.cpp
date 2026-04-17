#include <GL/glew.h>
#include "window.hpp"

int main() {
    window window(800, 600, "Advanced CG Lab");

    while (!window.should_close()) {
        window.clear_screen();
        window.update();

        window.swap_buffers();
    }

    return 0;
}