#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "mailbox.h"
#include "printf.h"

#define GPU_NOCACHE 0x40000000

typedef struct {
    unsigned int width;       // width of the display
    unsigned int height;      // height of the display
    unsigned int virtual_width;  // width of the virtual framebuffer
    unsigned int virtual_height; // height of the virtual framebuffer
    unsigned int pitch;       // number of bytes per row
    unsigned int depth;       // number of bits per pixel
    unsigned int x_offset;    // x of the upper left corner of the virtual fb
    unsigned int y_offset;    // y of the upper left corner of the virtual fb
    unsigned int framebuffer; // pointer to the start of the framebuffer
    unsigned int size;        // number of bytes in the framebuffer
} fb_config_t;

static volatile fb_config_t fb __attribute__((aligned(16)));

const int WIDTH = 640;
const int HEIGHT = 480;
const int DEPTH = 32;

int fb_init(void) {
    fb.width = WIDTH;
    fb.height = HEIGHT;
    fb.virtual_width = WIDTH;
    fb.virtual_height = WIDTH;
    fb.depth = DEPTH;
    fb.x_offset = 0;
    fb.y_offset = 0;

    // these must be 0
    fb.framebuffer = 0;
    fb.size = 0;
    fb.pitch = 0;

    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
    int err =  mailbox_read(MAILBOX_FRAMEBUFFER);
    return err;
}

void main(void) {
    gpio_init();
    uart_init();

    // DANGER - ignoring error
    (void) fb_init();

    printf("fb.height: %d\n",fb.height);
    printf("fb.pitch: %d\n",fb.pitch);

    unsigned int *im = (unsigned int *)fb.framebuffer;
    // compute total number of unsigned ints in the framebuffer
    int n = (fb.pitch / 4)* fb.height; 
    for (int i = 0; i < n; i++) { 
        *im++ = 0xff0000ff; // blue!!
    };
}
