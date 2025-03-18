#include "pio_drv.h"
#include "pio_instructions.h"


static uint8_t _sm_claimed[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static uint32_t _used_instruction_space[NUM_PIOS];

static int find_offset_for_program(PIO pio, const pio_program_t *program)
{
    uint32_t used_mask = _used_instruction_space[pio_get_index(pio)];
    uint32_t program_mask = (1u << program->length) - 1;

    // work down from the top
    for (int i = PIO_INSTRUCTION_COUNT - program->length; i >= 0; i--) {
        if (!(used_mask & (program_mask << (uint32_t) i))) {
            return i;
        }
    }

    return -1;
}

int pio_add_program_at_offset(PIO pio, const pio_program_t *program, uint32_t offset)
{
    for (uint32_t i = 0; i < program->length; ++i) {
        pio->instr_mem[offset + i] = program->instructions[i];
    }

    uint32_t program_mask = (1u << program->length) - 1;
    _used_instruction_space[pio_get_index(pio)] |= program_mask << offset;

    return (int)offset;
}

int pio_add_program(PIO *pio, uint32_t *sm, const pio_program_t *program)
{
    uint8_t i, j;
    int ret;

    for (i = 0; i < NUM_PIOS; i++) {
        for (j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
            if (_sm_claimed[i][j] == 0) {
                ret = find_offset_for_program(pio_get_instance(i), program);
                if (ret > 0) {
                    *pio = pio_get_instance(i);
                    *sm = j;
                    pio_add_program_at_offset(pio_get_instance(i), program, ret);
                    _sm_claimed[i][j] = 1;
                    return ret;
                }
            }
        }
    }

    return -1;
}

void pio_remove_program(PIO pio, uint32_t sm, const pio_program_t *program, uint32_t loaded_offset)
{
    uint32_t program_mask = (1u << program->length) - 1;
    program_mask <<= loaded_offset;
    _used_instruction_space[pio_get_index(pio)] &= ~program_mask;
    _sm_claimed[pio_get_index(pio)][sm] = 0;
}

int pio_sm_init(PIO pio, uint32_t sm, uint32_t initial_pc, const pio_sm_config *config)
{
    // Halt the machine, set some sensible defaults
    pio_sm_set_enabled(pio, sm, false);

    pio_sm_set_config(pio, sm, config);

    pio_sm_clear_fifos(pio, sm);

    // Finally, clear some internal SM state
    //pio_sm_restart(pio, sm);
    pio_sm_clkdiv_restart(pio, sm);
    // JMP always
    pio_sm_exec(pio, sm, initial_pc & 0x1fu);

    return 0;
}

int pio_sm_set_consecutive_pindirs(PIO pio, uint32_t sm, uint32_t pin_base, uint32_t count, bool is_out)
{
    uint32_t pinctrl_saved = pio->sm[sm].pinctrl;
    uint32_t pindir_val = is_out ? 0x1f : 0;

    while (count > 5) {
        pio->sm[sm].pinctrl = (5u << PIO_PINCTRL0_SET_COUNT_LSB) | (pin_base << PIO_PINCTRL0_SET_BASE_LSB);
        pio_sm_exec(pio, sm, pio_encode_set(pio_pindirs, pindir_val));
        count -= 5;
        pin_base = (pin_base + 5) & 0x1f;
    }

    pio->sm[sm].pinctrl = (count << PIO_PINCTRL0_SET_COUNT_LSB) | (pin_base << PIO_PINCTRL0_SET_BASE_LSB);
    pio_sm_exec(pio, sm, pio_encode_set(pio_pindirs, pindir_val));
    pio->sm[sm].pinctrl = pinctrl_saved;

    return 0;
}

int pio_sm_try_put(PIO pio, uint32_t sm, uint32_t data)
{
    if (pio_sm_is_tx_fifo_full(pio, sm))
        return -1;
    else
        pio_sm_put(pio, sm, data);

    return 0;
}

int pio_sm_try_get(PIO pio, uint32_t sm, uint32_t *data)
{
    if (pio_sm_is_rx_fifo_empty(pio, sm))
        return -1;
    else
        *data = pio_sm_get(pio, sm);

    return 0;
}
