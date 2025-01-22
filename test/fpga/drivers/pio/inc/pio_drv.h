#ifndef _PIO_DRV_H_
#define _PIO_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "pio_regs.h"

#define PIO0_BASE_ADDR  (0x40009000)

typedef struct {
    volatile uint32_t clkdiv;
    volatile uint32_t pinctrl;
    volatile uint32_t execctrl;
    volatile uint32_t shiftctrl;
    volatile uint32_t instr;
    volatile uint32_t push;
    volatile uint32_t pull;
    volatile uint32_t fstat;
} pio_sm_hw_t;

typedef struct {
    volatile uint32_t ctrl;
    volatile uint32_t irq_inte;
    volatile uint32_t irq_intp;
    volatile uint32_t pins_data;
    volatile uint32_t pins_dir;
    volatile uint32_t pins_data_set;
    volatile uint32_t pins_data_clr;
    pio_sm_hw_t sm[4];
    volatile uint32_t instr_mem[32];
} pio_hw_t;

#define pio0_hw ((pio_hw_t *)PIO0_BASE_ADDR)
//#define pio1_hw ((pio_hw_t *)PIO1_BASE_ADDR)

typedef pio_hw_t *PIO;

#define pio0 pio0_hw

typedef struct pio_program {
    const uint16_t *instructions;
    uint8_t length;
} pio_program_t;

/** \brief PIO Configuration structure
 *  \ingroup sm_config
 *
 * This structure is an in-memory representation of the configuration that can be applied to a PIO
 * state machine later using pio_sm_set_config() or pio_sm_init().
 */
typedef struct {
    uint32_t clkdiv;
    uint32_t execctrl;
    uint32_t shiftctrl;
    uint32_t pinctrl;
} pio_sm_config;

/*! \brief Set the base of the 'out' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'out' pins can overlap with the 'in', 'set' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param out_base 0-31 First pin to set as output
 */
static inline void pio_sm_config_set_out_pin_base(pio_sm_config *c, uint32_t out_base) {
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_OUT_BASE_MASK) |
                 ((out_base & 31) << PIO_PINCTRL0_OUT_BASE_LSB);
}

/*! \brief Set the number of 'out' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'out' pins can overlap with the 'in', 'set' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param out_count 0-32 Number of pins to set.
 */
static inline void pio_sm_config_set_out_pin_count(pio_sm_config *c, uint32_t out_count) {
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_OUT_COUNT_MASK) |
                 (out_count << PIO_PINCTRL0_OUT_COUNT_LSB);
}

/*! \brief Set the 'out' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'out' pins can overlap with the 'in', 'set' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param out_base 0-31 First pin to set as output
 * \param out_count 0-32 Number of pins to set.
 */
static inline void pio_sm_config_set_out_pins(pio_sm_config *c, uint32_t out_base, uint32_t out_count) {
    pio_sm_config_set_out_pin_base(c, out_base);
    pio_sm_config_set_out_pin_count(c, out_count);
}

/*! \brief Set the base of the 'set' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'set' pins can overlap with the 'in', 'out' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param set_base 0-31 First pin to set as
 */
static inline void pio_sm_config_set_set_pin_base(pio_sm_config *c, uint32_t set_base) {
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_SET_BASE_MASK) |
                 ((set_base & 31) << PIO_PINCTRL0_SET_BASE_LSB);
}

/*! \brief Set the count of 'set' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'set' pins can overlap with the 'in', 'out' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param set_count 0-5 Number of pins to set.
 */
static inline void pio_sm_config_set_set_pin_count(pio_sm_config *c, uint32_t set_count) {
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_SET_COUNT_MASK) |
                 (set_count << PIO_PINCTRL0_SET_COUNT_LSB);
}

/*! \brief Set the 'set' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'set' pins can overlap with the 'in', 'out' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param set_base 0-31 First pin to set as
 * \param set_count 0-5 Number of pins to set.
 */
static inline void pio_sm_config_set_set_pins(pio_sm_config *c, uint32_t set_base, uint32_t set_count) {
    pio_sm_config_set_set_pin_base(c, set_base);
    pio_sm_config_set_set_pin_count(c, set_count);
}

/*! \brief Set the base of the 'in' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'in' pins can overlap with the 'out', 'set' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param in_base 0-31 First pin to use as input
 */
static inline void pio_sm_config_set_in_pin_base(pio_sm_config *c, uint32_t in_base) {
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_IN_BASE_MASK) |
                 ((in_base & 31) << PIO_PINCTRL0_IN_BASE_LSB);
}

/*! \brief Set the base fpr the 'in' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'in' pins can overlap with the 'out', 'set' and 'sideset' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param in_base 0-31 First pin to use as input
 */
static inline void pio_sm_config_set_in_pins(pio_sm_config *c, uint32_t in_base) {
    pio_sm_config_set_in_pin_base(c, in_base);
}

/*! \brief Set the base of the 'sideset' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * 'sideset' pins can overlap with the 'in', 'out' and 'set' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param sideset_base 0-31 base pin for 'side set'
 */
static inline void pio_sm_config_set_sideset_pin_base(pio_sm_config *c, uint32_t sideset_base) {
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_SIDE_BASE_MASK) |
                 ((sideset_base & 31) << PIO_PINCTRL0_SIDE_BASE_LSB);
}

/*! \brief Set the 'sideset' pins in a state machine configuration
 *  \ingroup sm_config
 *
 * This method is identical to \ref sm_config_set_sideset_pin_base, and is provided
 * for backwards compatibility
 *
 * 'sideset' pins can overlap with the 'in', 'out' and 'set' pins
 *
 * \param c Pointer to the configuration structure to modify
 * \param sideset_base 0-31 base pin for 'side set'
 */
static inline void pio_sm_config_set_sideset_pins(pio_sm_config *c, uint32_t sideset_base) {
    pio_sm_config_set_sideset_pin_base(c, sideset_base);
}

/*! \brief Set the 'sideset' options in a state machine configuration
 *  \ingroup sm_config
 *
 * \param c Pointer to the configuration structure to modify
 * \param bit_count Number of bits to steal from delay field in the instruction for use of side set (max 5)
 * \param en True if the topmost side set bit is used as a flag for whether to apply side set on that instruction
 * \param pindirs True if the side set affects pin directions rather than values
 */
static inline void pio_sm_config_set_sideset(pio_sm_config *c, uint32_t sideset_base, uint32_t bit_count, bool en, bool pindirs) {
    pio_sm_config_set_sideset_pin_base(c, sideset_base);
    c->pinctrl = (c->pinctrl & ~PIO_PINCTRL0_SIDE_COUNT_MASK) |
                 (bit_count << PIO_PINCTRL0_SIDE_COUNT_LSB);
    c->execctrl = (c->execctrl & ~(PIO_EXECCTRL0_SIDESET_EN_MASK | PIO_EXECCTRL0_SIDE_PINDIR_MASK)) |
                  ((uint8_t)(en) << PIO_EXECCTRL0_SIDESET_EN_LSB) |
                  ((uint8_t)(pindirs) << PIO_EXECCTRL0_SIDE_PINDIR_LSB);
}

/*! \brief Set the wrap addresses in a state machine configuration
 *  \ingroup sm_config
 *
 * \param c Pointer to the configuration structure to modify
 * \param wrap_target the instruction memory address to wrap to
 * \param wrap        the instruction memory address after which to set the program counter to wrap_target
 *                    if the instruction does not itself update the program_counter
 */
static inline void pio_sm_config_set_wrap(pio_sm_config *c, uint32_t wrap_bottom, uint32_t wrap_top) {
    c->execctrl = (c->execctrl & ~(PIO_EXECCTRL0_WRAP_TOP_MASK | PIO_EXECCTRL0_WRAP_BOTTOM_MASK)) |
                  (wrap_top << PIO_EXECCTRL0_WRAP_TOP_LSB) |
                  (wrap_bottom << PIO_EXECCTRL0_WRAP_BOTTOM_LSB);
}

/*! \brief Set the 'jmp' pin in a state machine configuration
 *  \ingroup sm_config
 *
 * \param c Pointer to the configuration structure to modify
 * \param pin The raw GPIO pin number to use as the source for a `jmp pin` instruction
 */
static inline void pio_sm_config_set_jmp_pin(pio_sm_config *c, uint32_t pin) {
    c->execctrl = (c->execctrl & ~PIO_EXECCTRL0_JMP_PIN_MASK) |
                  ((pin & 31) << PIO_EXECCTRL0_JMP_PIN_LSB);
}

/*! \brief Setup 'in' shifting parameters in a state machine configuration
 *  \ingroup sm_config
 *
 * \param c Pointer to the configuration structure to modify
 * \param shift_right true to shift ISR to right, false to shift ISR to left
 * \param autopush whether autopush is enabled
 * \param push_threshold threshold in bits to shift in before auto/conditional re-pushing of the ISR
 */
static inline void pio_sm_config_set_in_shift(pio_sm_config *c, bool shift_right, bool autopush, uint32_t push_threshold) {
    c->shiftctrl = (c->shiftctrl &
                    ~(PIO_SHIFTCTRL0_IN_SHIFT_DIR_MASK |
                      PIO_SHIFTCTRL0_AUTO_PUSH_MASK |
                      PIO_SHIFTCTRL0_ISR_THRESHOLD_MASK)) |
                   ((uint8_t)(shift_right) << PIO_SHIFTCTRL0_IN_SHIFT_DIR_LSB) |
                   ((uint8_t)(autopush) << PIO_SHIFTCTRL0_AUTO_PUSH_LSB) |
                   ((push_threshold << PIO_SHIFTCTRL0_ISR_THRESHOLD_LSB) & PIO_SHIFTCTRL0_ISR_THRESHOLD_MASK);
}

/*! \brief Setup 'out' shifting parameters in a state machine configuration
 *  \ingroup sm_config
 *
 * \param c Pointer to the configuration structure to modify
 * \param shift_right true to shift OSR to right, false to shift OSR to left
 * \param autopull whether autopull is enabled
 * \param pull_threshold threshold in bits to shift out before auto/conditional re-pulling of the OSR
 */
static inline void pio_sm_config_set_out_shift(pio_sm_config *c, bool shift_right, bool autopull, uint32_t pull_threshold) {
    c->shiftctrl = (c->shiftctrl &
                    ~(PIO_SHIFTCTRL0_OUT_SHIFT_DIR_MASK |
                      PIO_SHIFTCTRL0_AUTO_PULL_MASK |
                      PIO_SHIFTCTRL0_OSR_THRESHOLD_MASK)) |
                   ((uint8_t)(shift_right) << PIO_SHIFTCTRL0_OUT_SHIFT_DIR_LSB) |
                   ((uint8_t)(autopull) << PIO_SHIFTCTRL0_AUTO_PULL_LSB) |
                   ((pull_threshold << PIO_SHIFTCTRL0_OSR_THRESHOLD_LSB) & PIO_SHIFTCTRL0_OSR_THRESHOLD_MASK);
}

/*! \brief Use a state machine to set the same pin direction for multiple consecutive pins for the PIO instance
 *  \ingroup hardware_pio
 *
 * This method repeatedly reconfigures the target state machine's pin configuration and executes 'set' instructions to set the pin direction on consecutive pins,
 * before restoring the state machine's pin configuration to what it was.
 *
 * This method is provided as a convenience to set initial pin directions, and should not be used against a state machine that is enabled.
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3) to use
 * \param pins_base the first pin to set a direction for
 * \param pin_count the count of consecutive pins to set the direction for
 * \param is_out the direction to set; true = out, false = in
 * \return PICO_OK (0) on success, error code otherwise
 */
int pio_sm_set_consecutive_pindirs(PIO pio, uint32_t sm, uint32_t pins_base, uint32_t pin_count, bool is_out);

static inline void pio_sm_config_set_clkdiv(pio_sm_config *c, uint32_t int_div, uint32_t frac_div) {
    c->clkdiv = (int_div << 8) | frac_div;
}

/*! \brief Enable or disable a PIO state machine
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param enabled true to enable the state machine; false to disable
 */
static inline void pio_sm_set_enabled(PIO pio, uint32_t sm, bool enabled) {
    pio->ctrl = (pio->ctrl & ~(1u << sm)) | ((uint8_t)(enabled) << sm);
}

int pio_sm_init(PIO pio, uint32_t sm, uint32_t initial_pc, const pio_sm_config *config);

/*! \brief Restart a state machine with a known state
 *  \ingroup hardware_pio
 *
 * This method clears the ISR, shift counters, clock divider counter
 * pin write flags, delay counter, latched EXEC instruction, and IRQ wait condition.
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 */
static inline void pio_sm_restart(PIO pio, uint32_t sm) {
    pio->ctrl |= 1u << (PIO_CTRL_SM_RESTART_LSB + sm);
}

static inline void pio_sm_clkdiv_restart(PIO pio, uint32_t sm) {
    pio->ctrl |= 1u << (PIO_CTRL_CLKDIV_RESTART_LSB + sm);
}

/*! \brief Immediately execute an instruction on a state machine
 *  \ingroup hardware_pio
 *
 * This instruction is executed instead of the next instruction in the normal control flow on the state machine.
 * Subsequent calls to this method replace the previous executed
 * instruction if it is still running. \see pio_sm_is_exec_stalled() to see if an executed instruction
 * is still running (i.e. it is stalled on some condition)
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param instr the encoded PIO instruction
 */
inline static void pio_sm_exec(PIO pio, uint32_t sm, uint32_t instr) {
    pio->sm[sm].instr = instr;
}

/*! \brief Write a word of data to a state machine's TX FIFO
 *  \ingroup hardware_pio
 *
 * This is a raw FIFO access that does not check for fullness. If the FIFO is
 * full, the FIFO contents and state are not affected by the write attempt.
 * Hardware sets the TXOVER sticky flag for this FIFO in FDEBUG, to indicate
 * that the system attempted to write to a full FIFO.
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param data the 32 bit data value
 *
 * \sa pio_sm_put_blocking()
 */
static inline void pio_sm_put(PIO pio, uint32_t sm, uint32_t data) {
    pio->sm[sm].push = data;
}

/*! \brief Read a word of data from a state machine's RX FIFO
 *  \ingroup hardware_pio
 *
 * This is a raw FIFO access that does not check for emptiness. If the FIFO is
 * empty, the hardware ignores the attempt to read from the FIFO (the FIFO
 * remains in an empty state following the read) and the sticky RXUNDER flag
 * for this FIFO is set in FDEBUG to indicate that the system tried to read
 * from this FIFO when empty. The data returned by this function is undefined
 * when the FIFO is empty.
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 *
 * \sa pio_sm_get_blocking()
 */
static inline uint32_t pio_sm_get(PIO pio, uint32_t sm) {
    return pio->sm[sm].pull;
}

/*! \brief Determine if a state machine's RX FIFO is full
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return true if the RX FIFO is full
 */
static inline bool pio_sm_is_rx_fifo_full(PIO pio, uint32_t sm) {
    return (pio->sm[sm].fstat & (1u << (PIO_FSTAT0_RXFULL_LSB))) != 0;
}

/*! \brief Determine if a state machine's RX FIFO is empty
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return true if the RX FIFO is empty
 */
static inline bool pio_sm_is_rx_fifo_empty(PIO pio, uint32_t sm) {
    return (pio->sm[sm].fstat & (1u << (PIO_FSTAT0_RXEMPTY_LSB))) != 0;
}

/*! \brief Return the number of elements currently in a state machine's RX FIFO
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return the number of elements in the RX FIFO
 */
static inline uint32_t pio_sm_get_rx_fifo_level(PIO pio, uint32_t sm) {
    return ((pio->sm[sm].fstat & PIO_FSTAT0_RXLEVEL_MASK) >> PIO_FSTAT0_RXLEVEL_LSB);
}

/*! \brief Determine if a state machine's TX FIFO is full
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return true if the TX FIFO is full
 */
static inline bool pio_sm_is_tx_fifo_full(PIO pio, uint32_t sm) {
    return (pio->sm[sm].fstat & (1u << (PIO_FSTAT0_TXFULL_LSB))) != 0;
}

/*! \brief Determine if a state machine's TX FIFO is empty
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return true if the TX FIFO is empty
 */
static inline bool pio_sm_is_tx_fifo_empty(PIO pio, uint32_t sm) {
    return (pio->sm[sm].fstat & (1u << (PIO_FSTAT0_TXEMPTY_LSB))) != 0;
}

/*! \brief Return the number of elements currently in a state machine's TX FIFO
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return the number of elements in the TX FIFO
 */
static inline uint32_t pio_sm_get_tx_fifo_level(PIO pio, uint32_t sm) {
    return ((pio->sm[sm].fstat & PIO_FSTAT0_TXLEVEL_MASK) >> PIO_FSTAT0_TXLEVEL_LSB);
}

/*! \brief Write a word of data to a state machine's TX FIFO, blocking if the FIFO is full
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param data the 32 bit data value
 */
static inline void pio_sm_put_blocking(PIO pio, uint32_t sm, uint32_t data) {
    while (pio_sm_is_tx_fifo_full(pio, sm));
    pio_sm_put(pio, sm, data);
}

/*! \brief Read a word of data from a state machine's RX FIFO, blocking if the FIFO is empty
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 */
static inline uint32_t pio_sm_get_blocking(PIO pio, uint32_t sm) {
    while (pio_sm_is_rx_fifo_empty(pio, sm));
    return pio_sm_get(pio, sm);
}

/*! \brief set the current clock divider for a state machine
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param div the clock divider
 */
static inline void pio_sm_set_clkdiv(PIO pio, uint32_t sm, uint32_t div) {
    pio->sm[sm].clkdiv = div << 8;
}

/*! \brief Attempt to load the program
 *  \ingroup hardware_pio
 *
 * \see pio_can_add_program() if you need to check whether the program can be loaded
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param program the program definition
 * \return the instruction memory offset the program is loaded at, or negative for error (for
 * backwards compatibility with prior SDK the error value is -1 i.e. PICO_ERROR_GENERIC)
 */
int pio_add_program(PIO pio, const pio_program_t *program);

/*! \brief Attempt to load the program at the specified instruction memory offset
 *  \ingroup hardware_pio
 *
 * \see pio_can_add_program_at_offset() if you need to check whether the program can be loaded
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param program the program definition
 * \param offset the instruction memory offset wanted for the start of the program
 * \return the instruction memory offset the program is loaded at, or negative for error (for
 * backwards compatibility with prior SDK the error value is -1 i.e. PICO_ERROR_GENERIC)
 */
int pio_add_program_at_offset(PIO pio, const pio_program_t *program, uint32_t offset);

/*! \brief Apply a state machine configuration to a state machine
 *  \ingroup hardware_pio
 *
 * \param pio Handle to PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param config the configuration to apply
 * \return PICO_OK (0) on success, negative error code otherwise
*/
static inline int pio_sm_set_config(PIO pio, uint32_t sm, const pio_sm_config *config) {
    pio->sm[sm].clkdiv = config->clkdiv;
    pio->sm[sm].execctrl = config->execctrl;
    pio->sm[sm].shiftctrl = config->shiftctrl;
    pio->sm[sm].pinctrl = config->pinctrl;
    return 0;
}

/*! \brief Clear a state machine's TX FIFOs
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 */
static inline void pio_sm_clear_tx_fifo(PIO pio, uint32_t sm) {
    pio->sm[sm].shiftctrl |= (1 << PIO_SHIFTCTRL0_CLEAR_TXFIFO_LSB);
}

/*! \brief Clear a state machine's RX FIFOs
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 */
static inline void pio_sm_clear_rx_fifo(PIO pio, uint32_t sm) {
    pio->sm[sm].shiftctrl |= (1 << PIO_SHIFTCTRL0_CLEAR_RXFIFO_LSB);
}

/*! \brief Clear a state machine's TX and RX FIFOs
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 */
static inline void pio_sm_clear_fifos(PIO pio, uint32_t sm) {
    pio->sm[sm].shiftctrl |= (1 << PIO_SHIFTCTRL0_CLEAR_RXFIFO_LSB) |
                             (1 << PIO_SHIFTCTRL0_CLEAR_TXFIFO_LSB);
}

/*! \brief PIO interrupt source numbers for pio related IRQs
 * \ingroup hardware_pio
 */
typedef enum pio_interrupt_source {
    pio_sm3_tx_fifo_not_full = 15,      ///< State machine 3 TX FIFO is not full
    pio_sm2_tx_fifo_not_full = 14,      ///< State machine 2 TX FIFO is not full
    pio_sm1_tx_fifo_not_full = 13,      ///< State machine 1 TX FIFO is not full
    pio_sm0_tx_fifo_not_full = 12,      ///< State machine 0 TX FIFO is not full
    pio_sm3_rx_fifo_not_empty = 11,     ///< State machine 3 RX FIFO is not empty
    pio_sm2_rx_fifo_not_empty = 10,     ///< State machine 2 RX FIFO is not empty
    pio_sm1_rx_fifo_not_empty = 9,      ///< State machine 1 RX FIFO is not empty
    pio_sm0_rx_fifo_not_empty = 8,      ///< State machine 0 RX FIFO is not empty
    pio_sm_int7               = 7,      ///< State machine interrupt 7
    pio_sm_int6               = 6,      ///< State machine interrupt 6
    pio_sm_int5               = 5,      ///< State machine interrupt 5
    pio_sm_int4               = 4,      ///< State machine interrupt 4
    pio_sm_int3               = 3,      ///< State machine interrupt 3
    pio_sm_int2               = 2,      ///< State machine interrupt 2
    pio_sm_int1               = 1,      ///< State machine interrupt 1
    pio_sm_int0               = 0,      ///< State machine interrupt 0
} pio_interrupt_source_t;

/*! \brief  Determine if a particular PIO interrupt is set
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param source the PIO interrupt number 0-7
 * \return true if corresponding PIO interrupt is currently set
 */
static inline bool pio_interrupt_get(PIO pio, pio_interrupt_source_t source) {
    return (pio->irq_intp & (1u << source));
}

/*! \brief  Clear a particular PIO interrupt
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param source the PIO interrupt number 0-7
 */
static inline void pio_interrupt_clear(PIO pio, pio_interrupt_source_t source) {
    pio->irq_intp = (1u << source);
}

/*! \brief  Enable/Disable a single source on a PIO's IRQ 0
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param source the source number (see \ref pio_interrupt_source)
 * \param enabled true to enable IRQ 0 for the source, false to disable.
 */
static inline void pio_set_irq_source_enabled(PIO pio, pio_interrupt_source_t source, bool enabled) {
    if (enabled)
        pio->irq_inte |= 1u << source;
    else
        pio->irq_inte &= ~(1u << source);
}

/*!
 * return 0 if put succ.
 */
int pio_sm_try_put(PIO pio, uint32_t sm, uint32_t data);

/*!
 * return 0 if get succ.
 */
int pio_sm_try_get(PIO pio, uint32_t sm, uint32_t *data);

/*! \brief Determine if an instruction set by pio_sm_exec() is stalled executing
 *  \ingroup hardware_pio
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \return true if the executed instruction is still running (stalled)
 */
static inline bool pio_sm_is_exec_stalled(PIO pio, uint32_t sm) {
    return pio->sm[sm].execctrl & PIO_EXECCTRL0_EXEC_STALLED_MASK;
}

/*! \brief Immediately execute an instruction on a state machine and wait for it to complete
 *  \ingroup hardware_pio
 *
 * This instruction is executed instead of the next instruction in the normal control flow on the state machine.
 * Subsequent calls to this method replace the previous executed
 * instruction if it is still running. \see pio_sm_is_exec_stalled() to see if an executed instruction
 * is still running (i.e. it is stalled on some condition)
 *
 * \param pio The PIO instance; e.g. \ref pio0 or \ref pio1
 * \param sm State machine index (0..3)
 * \param instr the encoded PIO instruction
 */
static inline void pio_sm_exec_wait_blocking(PIO pio, uint32_t sm, uint32_t instr) {
    pio_sm_exec(pio, sm, instr);
    while (pio_sm_is_exec_stalled(pio, sm));
}

static inline void pio_gpio_data_put(PIO pio, uint32_t data) {
    pio->pins_data = data;
}

static inline uint32_t pio_gpio_data_get(PIO pio) {
    return pio->pins_data;
}

static inline void pio_gpio_dir_put(PIO pio, uint32_t data) {
    pio->pins_dir = data;
}

static inline uint32_t pio_gpio_dir_get(PIO pio) {
    return pio->pins_dir;
}

static inline void pio_gpio_data_bits_set(PIO pio, uint32_t set_bits) {
    pio->pins_data_set = set_bits;
}

static inline void pio_gpio_data_bits_clr(PIO pio, uint32_t clr_bits) {
    pio->pins_data_clr = clr_bits;
}

static inline void pio_sm_set_tx_fifo_peek_mode_enabled(PIO pio, uint32_t sm, uint8_t en) {
    if (en)
        pio->sm[sm].shiftctrl |= 1 << PIO_SHIFTCTRL0_TXFIFO_PEEK_MODE_LSB;
    else
        pio->sm[sm].shiftctrl &= ~(1 << PIO_SHIFTCTRL0_TXFIFO_PEEK_MODE_LSB);
}

static inline void pio_sm_set_tx_fifo_shadow_mode_enabled(PIO pio, uint32_t sm, uint8_t en) {
    if (en)
        pio->sm[sm].shiftctrl |= 1 << PIO_SHIFTCTRL0_TXFIFO_SHADOW_MODE_LSB;
    else
        pio->sm[sm].shiftctrl &= ~(1 << PIO_SHIFTCTRL0_TXFIFO_SHADOW_MODE_LSB);
}

static inline void pio_sm_set_tx_fifo_shadow_update(PIO pio, uint32_t sm) {
    pio->sm[sm].shiftctrl |= 1 << PIO_SHIFTCTRL0_TXFIFO_SHADOW_UPDATE_LSB;
}

static inline uint32_t pio_sm_get_tx_fifo_shadow_update_state(PIO pio, uint32_t sm) {
    return (pio->sm[sm].shiftctrl & PIO_SHIFTCTRL0_TXFIFO_SHADOW_UPDATE_MASK);
}

#endif
