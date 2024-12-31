#ifndef _PIO_INSTRUCTIONS_H_
#define _PIO_INSTRUCTIONS_H_

#include <stdint.h>
#include <stdbool.h>

enum pio_instr_bits {
    pio_instr_bits_jmp = 0x0000,
    pio_instr_bits_wait = 0x2000,
    pio_instr_bits_in = 0x4000,
    pio_instr_bits_out = 0x6000,
    pio_instr_bits_push = 0x8000,
    pio_instr_bits_pull = 0x8080,
    pio_instr_bits_mov = 0xa000,
    pio_instr_bits_irq = 0xc000,
    pio_instr_bits_set = 0xe000,
};

enum pio_src_dest {
    pio_pins = 0u,
    pio_x = 1u,
    pio_y = 2u,
    pio_null = 3u,
    pio_pindirs = 4u,
    pio_exec_mov = 4u,
    pio_status = 5u,
    pio_pc = 5u,
    pio_isr = 6u,
    pio_osr = 7u,
    pio_exec_out = 7u,
};

static inline uint32_t _pio_encode_instr_and_args(enum pio_instr_bits instr_bits, uint32_t arg1, uint32_t arg2) {
    return instr_bits | (arg1 << 5u) | (arg2 & 0x1fu);
}

static inline uint32_t _pio_encode_instr_and_src_dest(enum pio_instr_bits instr_bits, enum pio_src_dest dest, uint32_t value) {
    return _pio_encode_instr_and_args(instr_bits, dest & 7u, value);
}

/*! \brief Encode just the delay slot bits of an instruction
 *  \ingroup pio_instructions
 *
 * \note This function does not return a valid instruction encoding; instead it returns an encoding of the delay
 * slot suitable for `OR`ing with the result of an encoding function for an actual instruction. Care should be taken when
 * combining the results of this function with the results of \ref pio_encode_sideset and \ref pio_encode_sideset_opt
 * as they share the same bits within the instruction encoding.
 *
 * \param cycles the number of cycles 0-31 (or less if side set is being used)
 * \return the delay slot bits to be ORed with an instruction encoding
 */
static inline uint32_t pio_encode_delay(uint32_t cycles) {
    return cycles << 8u;
}

/*! \brief Encode just the side set bits of an instruction (in optional -`opt` side set mode)
 *  \ingroup pio_instructions
 *
 * \note This function does not return a valid instruction encoding; instead it returns an encoding of the side set bits
 * suitable for `OR`ing with the result of an encoding function for an actual instruction. Care should be taken when
 * combining the results of this function with the results of \ref pio_encode_delay as they share the same bits
 * within the instruction encoding.
 *
 * \param sideset_bit_count number of side set bits as would be specified via `.sideset <n> opt` in pioasm
 * \param value the value to sideset on the pins
 * \return the side set bits to be ORed with an instruction encoding
 */
static inline uint32_t pio_encode_sideset(uint32_t sideset_bit_count, uint32_t value) {
    return (0x1000u | value << (12u - sideset_bit_count));
}

/*! \brief Encode an unconditional JMP instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `JMP <addr>`
 *
 * \param addr The target address 0-31 (an absolute address within the PIO instruction memory)
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_jmp(uint32_t addr) {
    return _pio_encode_instr_and_args(pio_instr_bits_jmp, 0, addr);
}

/*! \brief Encode an IN instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `IN <src>, <count>`
 *
 * \param src The source to take data from
 * \param count The number of bits 1-32
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_in(enum pio_src_dest src, uint32_t count) {
    return _pio_encode_instr_and_src_dest(pio_instr_bits_in, src, count);
}

/*! \brief Encode an OUT instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `OUT <src>, <count>`
 *
 * \param dest The destination to write data to
 * \param count The number of bits 1-32
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_out(enum pio_src_dest dest, uint32_t count) {
    return _pio_encode_instr_and_src_dest(pio_instr_bits_out, dest, count);
}

/*! \brief Encode a PUSH instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `PUSH <if_full>, <block>`
 *
 * \param if_full true for `PUSH IF_FULL ...`, false for `PUSH ...`
 * \param block true for `PUSH ... BLOCK`, false for `PUSH ...`
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_push(bool if_full, bool block) {
    return _pio_encode_instr_and_args(pio_instr_bits_push, (if_full ? 2u : 0u) | (block ? 1u : 0u), 0);
}

/*! \brief Encode a PULL instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `PULL <if_empty>, <block>`
 *
 * \param if_empty true for `PULL IF_EMPTY ...`, false for `PULL ...`
 * \param block true for `PULL ... BLOCK`, false for `PULL ...`
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_pull(bool if_empty, bool block) {
    return _pio_encode_instr_and_args(pio_instr_bits_pull, (if_empty ? 2u : 0u) | (block ? 1u : 0u), 0);
}

/*! \brief Encode a MOV instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `MOV <dest>, <src>`
 *
 * \param dest The destination to write data to
 * \param src The source to take data from
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_mov(enum pio_src_dest dest, enum pio_src_dest src) {
    return _pio_encode_instr_and_src_dest(pio_instr_bits_mov, dest, src & 7u);
}

/*! \brief Encode a SET instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `SET <dest>, <value>`
 *
 * \param dest The destination to apply the value to
 * \param value The value 0-31
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_set(enum pio_src_dest dest, uint32_t value) {
    return _pio_encode_instr_and_src_dest(pio_instr_bits_set, dest, value);
}

/*! \brief Encode a NOP instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `NOP` which is itself encoded as `MOV y, y`
 *
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_nop(void) {
    return pio_encode_mov(pio_y, pio_y);
}

static inline uint32_t _pio_encode_irq(bool relative, uint32_t irq) {
    return (relative ? 0x10u : 0x0u) | irq;
}

/*! \brief Encode a WAIT for GPIO pin instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `WAIT <polarity> GPIO <gpio>`
 *
 * \param polarity true for `WAIT 1`, false for `WAIT 0`
 * \param gpio The real GPIO number 0-31
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_wait_gpio(bool polarity, uint32_t gpio) {
    return _pio_encode_instr_and_args(pio_instr_bits_wait, 0u | (polarity ? 4u : 0u), gpio);
}

/*! \brief Encode a WAIT for pin instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `WAIT <polarity> PIN <pin>`
 *
 * \param polarity true for `WAIT 1`, false for `WAIT 0`
 * \param pin The pin number 0-31 relative to the executing SM's input pin mapping
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_wait_pin(bool polarity, uint32_t pin) {
    return _pio_encode_instr_and_args(pio_instr_bits_wait, 1u | (polarity ? 4u : 0u), pin);
}

/*! \brief Encode a WAIT for IRQ instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `WAIT <polarity> IRQ <irq> <relative>`
 *
 * \param polarity true for `WAIT 1`, false for `WAIT 0`
 * \param relative true for a `WAIT IRQ <irq> REL`, false for regular `WAIT IRQ <irq>`
 * \param irq the irq number 0-7
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_wait_irq(bool polarity, bool relative, uint32_t irq) {
    return _pio_encode_instr_and_args(pio_instr_bits_wait, 2u | (polarity ? 4u : 0u), _pio_encode_irq(relative, irq));
}

/*! \brief Encode a IRQ SET instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `IRQ SET <irq> <relative>`
 *
 * \param relative true for a `IRQ SET <irq> REL`, false for regular `IRQ SET <irq>`
 * \param irq the irq number 0-7
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_irq_set(bool relative, uint32_t irq) {
    return _pio_encode_instr_and_args(pio_instr_bits_irq, 0, _pio_encode_irq(relative, irq));
}

/*! \brief Encode a IRQ WAIT instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `IRQ WAIT <irq> <relative>`
 *
 * \param relative true for a `IRQ WAIT <irq> REL`, false for regular `IRQ WAIT <irq>`
 * \param irq the irq number 0-7
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_irq_wait(bool relative, uint32_t irq) {
    return _pio_encode_instr_and_args(pio_instr_bits_irq, 1, _pio_encode_irq(relative, irq));
}

/*! \brief Encode a IRQ CLEAR instruction
 *  \ingroup pio_instructions
 *
 * This is the equivalent of `IRQ CLEAR <irq> <relative>`
 *
 * \param relative true for a `IRQ CLEAR <irq> REL`, false for regular `IRQ CLEAR <irq>`
 * \param irq the irq number 0-7
 * \return The instruction encoding with 0 delay and no side set value
 * \see pio_encode_delay, pio_encode_sideset, pio_encode_sideset_opt
 */
static inline uint32_t pio_encode_irq_clear(bool relative, uint32_t irq) {
    return _pio_encode_instr_and_args(pio_instr_bits_irq, 2, _pio_encode_irq(relative, irq));
}


#endif
