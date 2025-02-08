#ifndef _PIO_SPI_H_
#define _PIO_SPI_H_

#define spi_wrap_target 0
#define spi_wrap        5

#define spi_wrap_bottom spi_wrap_target
#define spi_wrap_top    spi_wrap

static const uint16_t spi_program_instructions[] = {
    0x98a0,
    0xe047,
    0x7201,
    0x5901,
    0x0082,
    0x8020,
};

static const struct pio_program spi_program = {
    .instructions = spi_program_instructions,
    .length = sizeof(spi_program_instructions) / sizeof(spi_program_instructions[0]),
};

#endif
