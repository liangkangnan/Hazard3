#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "nec_receive.h"

#define IN_PIN  30

PIO pio = pio0;
uint32_t sm = 0;

// Validate a 32-bit frame and store the address and data at the locations
// provided.
//
// Returns: `true` if the frame was valid, otherwise `false`
bool nec_decode_frame(uint32_t frame, uint8_t *p_address, uint8_t *p_data)
{
    // access the frame data as four 8-bit fields
    //
    union {
        uint32_t raw;
        struct {
            uint8_t address;
            uint8_t inverted_address;
            uint8_t data;
            uint8_t inverted_data;
        };
    } f;

    f.raw = frame;

    // a valid (non-extended) 'NEC' frame should contain 8 bit
    // address, inverted address, data and inverted data
    if (f.address != (f.inverted_address ^ 0xff) ||
        f.data != (f.inverted_data ^ 0xff)) {
        return false;
    }

    // store the validated address and data
    *p_address = f.address;
    *p_data = f.data;

    return true;
}

int main()
{
    uart_init(115200);

    printf("hello pio nec_receive!!!\n");

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, 0, nec_receive_program.length - 1);
    pio_sm_config_set_clkdiv(&config, get_core_clock_hz() / (10.0 / 562.5e-6), 0);
    pio_add_program_at_offset(pio, &nec_receive_program, 0);

    pio_sm_config_set_in_pins(&config, IN_PIN);
    pio_sm_config_set_in_shift(&config, true, true, 32);
    pio_sm_config_set_jmp_pin(&config, IN_PIN);

    pio_sm_set_consecutive_pindirs(pio, sm, IN_PIN, 1, false);

    pio_sm_exec(pio, sm, pio_encode_set(pio_y, 0));

    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);

    printf("pio nec_receive started\n");

    uint8_t addr, data;

    while (1) {
        // display any frames in the receive FIFO
        uint32_t rx_frame = pio_sm_get_blocking(pio, sm);
        if (rx_frame == 0) {
            printf("repeat\n");
        } else {
            if (nec_decode_frame(rx_frame, &addr, &data))
                printf("received, addr: 0x%02x, data: 0x%02x\n", addr, data);
            else
                printf("received: %08x\n", rx_frame);
        }
    }
}
