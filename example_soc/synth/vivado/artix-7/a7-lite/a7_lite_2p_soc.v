/*****************************************************************************\
|                      Copyright (C) 2021-2022 Luke Wren                      |
|                     SPDX-License-Identifier: Apache-2.0                     |
\*****************************************************************************/

// Example file integrating a Hazard3 processor, processor JTAG + debug
// components, some memory and a UART.

`default_nettype none

module a7_lite_2p_soc #(
	parameter IRAM_DEPTH = 1 << 15, // Default 32 kwords -> 128 kB
	parameter DRAM_DEPTH = 1 << 13, // Default 8 kwords -> 32 kB
	parameter CLK_MHZ    = 12       // For timer timebase
) (
	// System clock + reset
	input wire               clk,
	input wire               rst_n,

	// JTAG port to RISC-V JTAG-DTM
	input  wire              tck,
	input  wire              trst_n,
	input  wire              tms,
	input  wire              tdi,
	output wire              tdo,

`ifdef SIMULATION
	output wire              dump_wave_en,
	output wire              sim_finish,
`endif

	input  wire [31:0]       reset_offset,

	// XIP interface
	output wire              xip_cs_n,
	output wire              xip_sck,
	output wire [3:0]        xip_dout,
	output wire [3:0]        xip_douten,
	input  wire [3:0]        xip_din,

	// PIO
	input  wire [31:0]       pio_in,
	output wire [31:0]       pio_out,
	output wire [31:0]       pio_out_en,

	// SD ADC
	input  wire              adc_cmp_in,
	output wire              adc_pwm_out,

	// UART
	output wire              uart_tx,
	input  wire              uart_rx
);

localparam W_ADDR = 32;
localparam W_DATA = 32;

// ----------------------------------------------------------------------------
// Processor debug

wire              dmi_psel;
wire              dmi_penable;
wire              dmi_pwrite;
wire [8:0]        dmi_paddr;
wire [31:0]       dmi_pwdata;
wire [31:0]       dmi_prdata;
wire              dmi_pready;
wire              dmi_pslverr;


// TCK-domain DTM logic can force a hard reset
wire dmihardreset_req;
wire assert_dmi_reset = !rst_n || dmihardreset_req;
wire rst_n_dmi;

reset_sync dmi_reset_sync_u (
	.clk       (clk),
	.rst_n_in  (!assert_dmi_reset),
	.rst_n_out (rst_n_dmi)
);

// Standard RISC-V JTAG-DTM connected to external IOs.
// JTAG-DTM IDCODE should be a JEP106-compliant ID:
localparam IDCODE = 32'hdeadbeef;

hazard3_jtag_dtm #(
    .IDCODE (IDCODE)
) dtm_u (
    .tck              (tck),
    .trst_n           (trst_n),
    .tms              (tms),
    .tdi              (tdi),
    .tdo              (tdo),

    .dmihardreset_req (dmihardreset_req),

    .clk_dmi          (clk),
    .rst_n_dmi        (rst_n_dmi),

    .dmi_psel         (dmi_psel),
    .dmi_penable      (dmi_penable),
    .dmi_pwrite       (dmi_pwrite),
    .dmi_paddr        (dmi_paddr),
    .dmi_pwdata       (dmi_pwdata),
    .dmi_prdata       (dmi_prdata),
    .dmi_pready       (dmi_pready),
    .dmi_pslverr      (dmi_pslverr)
);

localparam N_HARTS = 1;
localparam XLEN = 32;

wire                      sys_reset_req;
wire                      sys_reset_done;
wire [N_HARTS-1:0]        hart_reset_req;
wire [N_HARTS-1:0]        hart_reset_done;

wire [N_HARTS-1:0]        hart_req_halt;
wire [N_HARTS-1:0]        hart_req_halt_on_reset;
wire [N_HARTS-1:0]        hart_req_resume;
wire [N_HARTS-1:0]        hart_halted;
wire [N_HARTS-1:0]        hart_running;

wire [N_HARTS*XLEN-1:0]   hart_data0_rdata;
wire [N_HARTS*XLEN-1:0]   hart_data0_wdata;
wire [N_HARTS-1:0]        hart_data0_wen;

wire [N_HARTS*XLEN-1:0]   hart_instr_data;
wire [N_HARTS-1:0]        hart_instr_data_vld;
wire [N_HARTS-1:0]        hart_instr_data_rdy;
wire [N_HARTS-1:0]        hart_instr_caught_exception;
wire [N_HARTS-1:0]        hart_instr_caught_ebreak;

wire [31:0]               sbus_addr;
wire                      sbus_write;
wire [1:0]                sbus_size;
wire                      sbus_vld;
wire                      sbus_rdy;
wire                      sbus_err;
wire [31:0]               sbus_wdata;
wire [31:0]               sbus_rdata;

hazard3_dm #(
	.N_HARTS      (N_HARTS),
	.HAVE_SBA     (1),
	.NEXT_DM_ADDR (0)
) dm (
	.clk                         (clk),
	.rst_n                       (rst_n),

	.dmi_psel                    (dmi_psel),
	.dmi_penable                 (dmi_penable),
	.dmi_pwrite                  (dmi_pwrite),
	.dmi_paddr                   (dmi_paddr),
	.dmi_pwdata                  (dmi_pwdata),
	.dmi_prdata                  (dmi_prdata),
	.dmi_pready                  (dmi_pready),
	.dmi_pslverr                 (dmi_pslverr),

	.sys_reset_req               (sys_reset_req),
	.sys_reset_done              (sys_reset_done),
	.hart_reset_req              (hart_reset_req),
	.hart_reset_done             (hart_reset_done),

	.hart_req_halt               (hart_req_halt),
	.hart_req_halt_on_reset      (hart_req_halt_on_reset),
	.hart_req_resume             (hart_req_resume),
	.hart_halted                 (hart_halted),
	.hart_running                (hart_running),

	.hart_data0_rdata            (hart_data0_rdata),
	.hart_data0_wdata            (hart_data0_wdata),
	.hart_data0_wen              (hart_data0_wen),

	.hart_instr_data             (hart_instr_data),
	.hart_instr_data_vld         (hart_instr_data_vld),
	.hart_instr_data_rdy         (hart_instr_data_rdy),
	.hart_instr_caught_exception (hart_instr_caught_exception),
	.hart_instr_caught_ebreak    (hart_instr_caught_ebreak),

	.sbus_addr                   (sbus_addr),
	.sbus_write                  (sbus_write),
	.sbus_size                   (sbus_size),
	.sbus_vld                    (sbus_vld),
	.sbus_rdy                    (sbus_rdy),
	.sbus_err                    (sbus_err),
	.sbus_wdata                  (sbus_wdata),
	.sbus_rdata                  (sbus_rdata)
);


// Generate resynchronised reset for CPU based on upstream system reset and on
// system/hart reset requests from DM.

wire assert_cpu_reset = !rst_n || sys_reset_req || hart_reset_req[0];
wire rst_n_cpu;

reset_sync cpu_reset_sync (
	.clk       (clk),
	.rst_n_in  (!assert_cpu_reset),
	.rst_n_out (rst_n_cpu)
);

// Still some work to be done on the reset handshake -- this ought to be
// resynchronised to DM's reset domain here, and the DM should wait for a
// rising edge after it has asserted the reset pulse, to make sure the tail
// of the previous "done" is not passed on.
assign sys_reset_done = rst_n_cpu;
assign hart_reset_done = rst_n_cpu;

// ----------------------------------------------------------------------------

// Instruction fetch port
wire [W_ADDR-1:0] i_haddr;
wire              i_hwrite;
wire [1:0]        i_htrans;
wire              i_hexcl;
wire [2:0]        i_hsize;
wire [2:0]        i_hburst;
wire [3:0]        i_hprot;
wire              i_hmastlock;
wire [7:0]        i_hmaster;
wire              i_hready;
wire              i_hresp;
wire              i_hexokay;
wire [W_DATA-1:0] i_hwdata;
wire [W_DATA-1:0] i_hrdata;

// Load/store port
wire [W_ADDR-1:0] d_haddr;
wire              d_hwrite;
wire [1:0]        d_htrans;
wire              d_hexcl;
wire [2:0]        d_hsize;
wire [2:0]        d_hburst;
wire [3:0]        d_hprot;
wire              d_hmastlock;
wire [7:0]        d_hmaster;
wire              d_hready;
wire              d_hresp;
wire              d_hexokay;
wire [W_DATA-1:0] d_hwdata;
wire [W_DATA-1:0] d_hrdata;

// DMA port
wire [W_ADDR-1:0] dma0_haddr;
wire              dma0_hwrite;
wire [1:0]        dma0_htrans;
wire              dma0_hexcl;
wire [2:0]        dma0_hsize;
wire [2:0]        dma0_hburst;
wire [3:0]        dma0_hprot;
wire              dma0_hmastlock;
wire [7:0]        dma0_hmaster;
wire              dma0_hready;
wire              dma0_hresp;
wire              dma0_hexokay;
wire [W_DATA-1:0] dma0_hwdata;
wire [W_DATA-1:0] dma0_hrdata;

wire [W_ADDR-1:0] dma1_haddr;
wire              dma1_hwrite;
wire [1:0]        dma1_htrans;
wire              dma1_hexcl;
wire [2:0]        dma1_hsize;
wire [2:0]        dma1_hburst;
wire [3:0]        dma1_hprot;
wire              dma1_hmastlock;
wire [7:0]        dma1_hmaster;
wire              dma1_hready;
wire              dma1_hresp;
wire              dma1_hexokay;
wire [W_DATA-1:0] dma1_hwdata;
wire [W_DATA-1:0] dma1_hrdata;

wire              pwrup_req;
wire              unblock_out;

wire              uart_irq;
wire              timer_irq;
wire              pio0_irq;
wire              pio1_irq;
wire              timer0_irq;
wire              dma0_irq;
wire              dma1_irq;
wire              adc_irq;

wire [15:0]       peri_reset_n_req;

`ifdef SIMULATION
wire [NUM_IRQS-1:0] irq;
wire                soft_irq;
`endif

`include "a7_lite_soc_config.vh"

hazard3_cpu_2port #(
`include "hazard3_config_inst.vh"
) cpu (
	.clk                        (clk),
	.clk_always_on              (clk),
	.rst_n                      (rst_n_cpu),

	.pwrup_req                  (pwrup_req),
	.pwrup_ack                  (pwrup_req),   // Tied back
	.clk_en                     (/* unused */),
	.unblock_out                (unblock_out),
	.unblock_in                 (unblock_out), // Tied back

	.reset_offset               (reset_offset),

	.i_haddr                    (i_haddr),
	.i_hwrite                   (i_hwrite),
	.i_htrans                   (i_htrans),
	.i_hsize                    (i_hsize),
	.i_hburst                   (i_hburst),
	.i_hprot                    (i_hprot),
	.i_hmastlock                (i_hmastlock),
	.i_hmaster                  (i_hmaster),
	.i_hready                   (i_hready),
	.i_hresp                    (i_hresp),
	.i_hwdata                   (i_hwdata),
	.i_hrdata                   (i_hrdata),

	.d_haddr                    (d_haddr),
	.d_hexcl                    (d_hexcl),
	.d_hwrite                   (d_hwrite),
	.d_htrans                   (d_htrans),
	.d_hsize                    (d_hsize),
	.d_hburst                   (d_hburst),
	.d_hprot                    (d_hprot),
	.d_hmastlock                (d_hmastlock),
	.d_hmaster                  (d_hmaster),
	.d_hready                   (d_hready),
	.d_hresp                    (d_hresp),
	.d_hexokay                  (d_hexokay),
	.d_hwdata                   (d_hwdata),
	.d_hrdata                   (d_hrdata),

	.dbg_req_halt               (hart_req_halt),
	.dbg_req_halt_on_reset      (hart_req_halt_on_reset),
	.dbg_req_resume             (hart_req_resume),
	.dbg_halted                 (hart_halted),
	.dbg_running                (hart_running),

	.dbg_data0_rdata            (hart_data0_rdata),
	.dbg_data0_wdata            (hart_data0_wdata),
	.dbg_data0_wen              (hart_data0_wen),

	.dbg_instr_data             (hart_instr_data),
	.dbg_instr_data_vld         (hart_instr_data_vld),
	.dbg_instr_data_rdy         (hart_instr_data_rdy),
	.dbg_instr_caught_exception (hart_instr_caught_exception),
	.dbg_instr_caught_ebreak    (hart_instr_caught_ebreak),

	.dbg_sbus_addr              (sbus_addr),
	.dbg_sbus_write             (sbus_write),
	.dbg_sbus_size              (sbus_size),
	.dbg_sbus_vld               (sbus_vld),
	.dbg_sbus_rdy               (sbus_rdy),
	.dbg_sbus_err               (sbus_err),
	.dbg_sbus_wdata             (sbus_wdata),
	.dbg_sbus_rdata             (sbus_rdata),
`ifdef SIMULATION
	.irq                        ({irq[NUM_IRQS-1:7], adc_irq | irq[6], dma1_irq | irq[5], dma0_irq | irq[4], timer0_irq | irq[3], pio1_irq | irq[2], pio0_irq | irq[1], uart_irq | irq[0]}),
	.soft_irq                   (soft_irq),
`else
	.irq                        ({{NUM_IRQS-7{1'b0}}, adc_irq, dma1_irq, dma0_irq, timer0_irq, pio1_irq, pio0_irq, uart_irq}),
	.soft_irq                   (1'b0),
`endif
	.timer_irq                  (timer_irq)
);

// We do not implement global exclusive monitoring.
assign d_hexokay = 1'b1;

// ----------------------------------------------------------------------------
// Bus fabric

// - nor flash at...... 0x0000_0000
// - 128 kB IRAM at.... 0x1000_0000
// - 32 kB DRAM at..... 0x2000_0000
// - System timer at... 0x4000_0000
// - UART at........... 0x4000_4000
// - XIP ctrl at....... 0x4000_8000
// - SIM ctrl at....... 0x8000_0000

// AHBL layer

wire               flash_hready_resp;
wire               flash_hready;
wire               flash_hresp;
wire [W_ADDR-1:0]  flash_haddr;
wire               flash_hwrite;
wire [1:0]         flash_htrans;
wire [2:0]         flash_hsize;
wire [2:0]         flash_hburst;
wire [3:0]         flash_hprot;
wire               flash_hmastlock;
wire [W_DATA-1:0]  flash_hwdata;
wire [W_DATA-1:0]  flash_hrdata;

wire               dram_hready_resp;
wire               dram_hready;
wire               dram_hresp;
wire [W_ADDR-1:0]  dram_haddr;
wire               dram_hwrite;
wire [1:0]         dram_htrans;
wire [2:0]         dram_hsize;
wire [2:0]         dram_hburst;
wire [3:0]         dram_hprot;
wire               dram_hmastlock;
wire [W_DATA-1:0]  dram_hwdata;
wire [W_DATA-1:0]  dram_hrdata;

wire               bridge_hready_resp;
wire               bridge_hready;
wire               bridge_hresp;
wire [W_ADDR-1:0]  bridge_haddr;
wire               bridge_hwrite;
wire [1:0]         bridge_htrans;
wire [2:0]         bridge_hsize;
wire [2:0]         bridge_hburst;
wire [3:0]         bridge_hprot;
wire               bridge_hmastlock;
wire [W_DATA-1:0]  bridge_hwdata;
wire [W_DATA-1:0]  bridge_hrdata;

`ifdef SIMULATION

wire               sim_ctrl_hready_resp;
wire               sim_ctrl_hready;
wire               sim_ctrl_hresp;
wire [W_ADDR-1:0]  sim_ctrl_haddr;
wire               sim_ctrl_hwrite;
wire [1:0]         sim_ctrl_htrans;
wire [2:0]         sim_ctrl_hsize;
wire [2:0]         sim_ctrl_hburst;
wire [3:0]         sim_ctrl_hprot;
wire               sim_ctrl_hmastlock;
wire [W_DATA-1:0]  sim_ctrl_hwdata;
wire [W_DATA-1:0]  sim_ctrl_hrdata;

wire               iram_hready_resp;
wire               iram_hready;
wire               iram_hresp;
wire [W_ADDR-1:0]  iram_haddr;
wire               iram_hwrite;
wire [1:0]         iram_htrans;
wire [2:0]         iram_hsize;
wire [2:0]         iram_hburst;
wire [3:0]         iram_hprot;
wire               iram_hmastlock;
wire [W_DATA-1:0]  iram_hwdata;
wire [W_DATA-1:0]  iram_hrdata;

ahbl_crossbar #(
    .N_MASTERS  (4),
    .N_SLAVES   (5),
    .W_ADDR     (W_ADDR),
    .W_DATA     (W_DATA),
    .ADDR_MAP   (160'h80000000_40000000_20000000_10000000_00000000),
    .ADDR_MASK  (160'hf0000000_f0000000_f0000000_f0000000_f0000000)
) crossbar (
	.clk             (clk),
	.rst_n           (rst_n),

    .src_hready_resp ({i_hready     , dma1_hready     , dma0_hready     , d_hready}),
    .src_hresp       ({i_hresp      , dma1_hresp      , dma0_hresp      , d_hresp}),
    .src_haddr       ({i_haddr      , dma1_haddr      , dma0_haddr      , d_haddr}),
    .src_hwrite      ({i_hwrite     , dma1_hwrite     , dma0_hwrite     , d_hwrite}),
    .src_htrans      ({i_htrans     , dma1_htrans     , dma0_htrans     , d_htrans}),
    .src_hsize       ({i_hsize      , dma1_hsize      , dma0_hsize      , d_hsize}),
    .src_hburst      ({i_hburst     , dma1_hburst     , dma0_hburst     , d_hburst}),
    .src_hprot       ({i_hprot      , dma1_hprot      , dma0_hprot      , d_hprot}),
    .src_hmastlock   ({i_hmastlock  , dma1_hmastlock  , dma0_hmastlock  , d_hmastlock}),
    .src_hwdata      ({i_hwdata     , dma1_hwdata     , dma0_hwdata     , d_hwdata}),
    .src_hrdata      ({i_hrdata     , dma1_hrdata     , dma0_hrdata     , d_hrdata}),

    .dst_hready_resp ({sim_ctrl_hready_resp , bridge_hready_resp , dram_hready_resp , iram_hready_resp , flash_hready_resp}),
    .dst_hready      ({sim_ctrl_hready      , bridge_hready      , dram_hready      , iram_hready      , flash_hready}),
    .dst_hresp       ({sim_ctrl_hresp       , bridge_hresp       , dram_hresp       , iram_hresp       , flash_hresp}),
    .dst_haddr       ({sim_ctrl_haddr       , bridge_haddr       , dram_haddr       , iram_haddr       , flash_haddr}),
    .dst_hwrite      ({sim_ctrl_hwrite      , bridge_hwrite      , dram_hwrite      , iram_hwrite      , flash_hwrite}),
    .dst_htrans      ({sim_ctrl_htrans      , bridge_htrans      , dram_htrans      , iram_htrans      , flash_htrans}),
    .dst_hsize       ({sim_ctrl_hsize       , bridge_hsize       , dram_hsize       , iram_hsize       , flash_hsize}),
    .dst_hburst      ({sim_ctrl_hburst      , bridge_hburst      , dram_hburst      , iram_hburst      , flash_hburst}),
    .dst_hprot       ({sim_ctrl_hprot       , bridge_hprot       , dram_hprot       , iram_hprot       , flash_hprot}),
    .dst_hmastlock   ({sim_ctrl_hmastlock   , bridge_hmastlock   , dram_hmastlock   , iram_hmastlock   , flash_hmastlock}),
    .dst_hwdata      ({sim_ctrl_hwdata      , bridge_hwdata      , dram_hwdata      , iram_hwdata      , flash_hwdata}),
    .dst_hrdata      ({sim_ctrl_hrdata      , bridge_hrdata      , dram_hrdata      , iram_hrdata      , flash_hrdata})
);

`else

ahbl_crossbar #(
    .N_MASTERS  (4),
    .N_SLAVES   (3),
    .W_ADDR     (W_ADDR),
    .W_DATA     (W_DATA),
    .ADDR_MAP   (96'h40000000_20000000_00000000),
    .ADDR_MASK  (96'hf0000000_f0000000_f0000000)
) crossbar (
	.clk             (clk),
	.rst_n           (rst_n),

    .src_hready_resp ({i_hready     , dma1_hready     , dma0_hready     , d_hready}),
    .src_hresp       ({i_hresp      , dma1_hresp      , dma0_hresp      , d_hresp}),
    .src_haddr       ({i_haddr      , dma1_haddr      , dma0_haddr      , d_haddr}),
    .src_hwrite      ({i_hwrite     , dma1_hwrite     , dma0_hwrite     , d_hwrite}),
    .src_htrans      ({i_htrans     , dma1_htrans     , dma0_htrans     , d_htrans}),
    .src_hsize       ({i_hsize      , dma1_hsize      , dma0_hsize      , d_hsize}),
    .src_hburst      ({i_hburst     , dma1_hburst     , dma0_hburst     , d_hburst}),
    .src_hprot       ({i_hprot      , dma1_hprot      , dma0_hprot      , d_hprot}),
    .src_hmastlock   ({i_hmastlock  , dma1_hmastlock  , dma0_hmastlock  , d_hmastlock}),
    .src_hwdata      ({i_hwdata     , dma1_hwdata     , dma0_hwdata     , d_hwdata}),
    .src_hrdata      ({i_hrdata     , dma1_hrdata     , dma0_hrdata     , d_hrdata}),

    .dst_hready_resp ({bridge_hready_resp , dram_hready_resp , flash_hready_resp}),
    .dst_hready      ({bridge_hready      , dram_hready      , flash_hready}),
    .dst_hresp       ({bridge_hresp       , dram_hresp       , flash_hresp}),
    .dst_haddr       ({bridge_haddr       , dram_haddr       , flash_haddr}),
    .dst_hwrite      ({bridge_hwrite      , dram_hwrite      , flash_hwrite}),
    .dst_htrans      ({bridge_htrans      , dram_htrans      , flash_htrans}),
    .dst_hsize       ({bridge_hsize       , dram_hsize       , flash_hsize}),
    .dst_hburst      ({bridge_hburst      , dram_hburst      , flash_hburst}),
    .dst_hprot       ({bridge_hprot       , dram_hprot       , flash_hprot}),
    .dst_hmastlock   ({bridge_hmastlock   , dram_hmastlock   , flash_hmastlock}),
    .dst_hwdata      ({bridge_hwdata      , dram_hwdata      , flash_hwdata}),
    .dst_hrdata      ({bridge_hrdata      , dram_hrdata      , flash_hrdata})
);

`endif

// APB layer

wire        bridge_psel;
wire        bridge_penable;
wire        bridge_pwrite;
wire [15:0] bridge_paddr;
wire [31:0] bridge_pwdata;
wire [31:0] bridge_prdata;
wire        bridge_pready;
wire        bridge_pslverr;

wire        sysinfo_psel;
wire        sysinfo_penable;
wire        sysinfo_pwrite;
wire [15:0] sysinfo_paddr;
wire [31:0] sysinfo_pwdata;
wire [31:0] sysinfo_prdata;
wire        sysinfo_pready;
wire        sysinfo_pslverr;

wire        perireset_psel;
wire        perireset_penable;
wire        perireset_pwrite;
wire [15:0] perireset_paddr;
wire [31:0] perireset_pwdata;
wire [31:0] perireset_prdata;
wire        perireset_pready;
wire        perireset_pslverr;

wire        uart_psel;
wire        uart_penable;
wire        uart_pwrite;
wire [15:0] uart_paddr;
wire [31:0] uart_pwdata;
wire [31:0] uart_prdata;
wire        uart_pready;
wire        uart_pslverr;

wire        mach_timer_psel;
wire        mach_timer_penable;
wire        mach_timer_pwrite;
wire [15:0] mach_timer_paddr;
wire [31:0] mach_timer_pwdata;
wire [31:0] mach_timer_prdata;
wire        mach_timer_pready;
wire        mach_timer_pslverr;

wire        timer0_psel;
wire        timer0_penable;
wire        timer0_pwrite;
wire [15:0] timer0_paddr;
wire [31:0] timer0_pwdata;
wire [31:0] timer0_prdata;
wire        timer0_pready;
wire        timer0_pslverr;

wire        xip_psel;
wire        xip_penable;
wire        xip_pwrite;
wire [15:0] xip_paddr;
wire [31:0] xip_pwdata;
wire [31:0] xip_prdata;
wire        xip_pready;
wire        xip_pslverr;

wire        pio0_psel;
wire        pio0_penable;
wire        pio0_pwrite;
wire [15:0] pio0_paddr;
wire [31:0] pio0_pwdata;
wire [31:0] pio0_prdata;
wire        pio0_pready;
wire        pio0_pslverr;

wire        pio1_psel;
wire        pio1_penable;
wire        pio1_pwrite;
wire [15:0] pio1_paddr;
wire [31:0] pio1_pwdata;
wire [31:0] pio1_prdata;
wire        pio1_pready;
wire        pio1_pslverr;

wire        dma0_psel;
wire        dma0_penable;
wire        dma0_pwrite;
wire [15:0] dma0_paddr;
wire [31:0] dma0_pwdata;
wire [31:0] dma0_prdata;
wire        dma0_pready;
wire        dma0_pslverr;

wire        dma1_psel;
wire        dma1_penable;
wire        dma1_pwrite;
wire [15:0] dma1_paddr;
wire [31:0] dma1_pwdata;
wire [31:0] dma1_prdata;
wire        dma1_pready;
wire        dma1_pslverr;

wire        crc_psel;
wire        crc_penable;
wire        crc_pwrite;
wire [15:0] crc_paddr;
wire [31:0] crc_pwdata;
wire [31:0] crc_prdata;
wire        crc_pready;
wire        crc_pslverr;

wire        tfpu_psel;
wire        tfpu_penable;
wire        tfpu_pwrite;
wire [15:0] tfpu_paddr;
wire [31:0] tfpu_pwdata;
wire [31:0] tfpu_prdata;
wire        tfpu_pready;
wire        tfpu_pslverr;

wire        sdadc_psel;
wire        sdadc_penable;
wire        sdadc_pwrite;
wire [15:0] sdadc_paddr;
wire [31:0] sdadc_pwdata;
wire [31:0] sdadc_prdata;
wire        sdadc_pready;
wire        sdadc_pslverr;

ahbl_to_apb apb_bridge_u (
	.clk               (clk),
	.rst_n             (rst_n),

	.ahbls_hready      (bridge_hready),
	.ahbls_hready_resp (bridge_hready_resp),
	.ahbls_hresp       (bridge_hresp),
	.ahbls_haddr       (bridge_haddr),
	.ahbls_hwrite      (bridge_hwrite),
	.ahbls_htrans      (bridge_htrans),
	.ahbls_hsize       (bridge_hsize),
	.ahbls_hburst      (bridge_hburst),
	.ahbls_hprot       (bridge_hprot),
	.ahbls_hmastlock   (bridge_hmastlock),
	.ahbls_hwdata      (bridge_hwdata),
	.ahbls_hrdata      (bridge_hrdata),

	.apbm_paddr        (bridge_paddr),
	.apbm_psel         (bridge_psel),
	.apbm_penable      (bridge_penable),
	.apbm_pwrite       (bridge_pwrite),
	.apbm_pwdata       (bridge_pwdata),
	.apbm_pready       (bridge_pready),
	.apbm_prdata       (bridge_prdata),
	.apbm_pslverr      (bridge_pslverr)
);

generate
if (TFPU == 1) begin: has_tfpu

apb_splitter #(
	.N_SLAVES   (13),
	.ADDR_MAP   (208'he000_d000_c000_b000_a000_9000_8000_7000_4000_3000_2000_1000_0000),
	.ADDR_MASK  (208'hf000_f000_f000_f000_f000_f000_f000_f000_f000_f000_f000_f000_f000)
) inst_apb_splitter (
	.apbs_paddr   (bridge_paddr),
	.apbs_psel    (bridge_psel),
	.apbs_penable (bridge_penable),
	.apbs_pwrite  (bridge_pwrite),
	.apbs_pwdata  (bridge_pwdata),
	.apbs_pready  (bridge_pready),
	.apbs_prdata  (bridge_prdata),
	.apbs_pslverr (bridge_pslverr),

	.apbm_paddr   ({crc_paddr   , sdadc_paddr   , dma1_paddr   , dma0_paddr   , pio1_paddr   , pio0_paddr   , xip_paddr   , tfpu_paddr   , uart_paddr   , timer0_paddr   , perireset_paddr   , sysinfo_paddr   , mach_timer_paddr  }),
	.apbm_psel    ({crc_psel    , sdadc_psel    , dma1_psel    , dma0_psel    , pio1_psel    , pio0_psel    , xip_psel    , tfpu_psel    , uart_psel    , timer0_psel    , perireset_psel    , sysinfo_psel    , mach_timer_psel   }),
	.apbm_penable ({crc_penable , sdadc_penable , dma1_penable , dma0_penable , pio1_penable , pio0_penable , xip_penable , tfpu_penable , uart_penable , timer0_penable , perireset_penable , sysinfo_penable , mach_timer_penable}),
	.apbm_pwrite  ({crc_pwrite  , sdadc_pwrite  , dma1_pwrite  , dma0_pwrite  , pio1_pwrite  , pio0_pwrite  , xip_pwrite  , tfpu_pwrite  , uart_pwrite  , timer0_pwrite  , perireset_pwrite  , sysinfo_pwrite  , mach_timer_pwrite }),
	.apbm_pwdata  ({crc_pwdata  , sdadc_pwdata  , dma1_pwdata  , dma0_pwdata  , pio1_pwdata  , pio0_pwdata  , xip_pwdata  , tfpu_pwdata  , uart_pwdata  , timer0_pwdata  , perireset_pwdata  , sysinfo_pwdata  , mach_timer_pwdata }),
	.apbm_pready  ({crc_pready  , sdadc_pready  , dma1_pready  , dma0_pready  , pio1_pready  , pio0_pready  , xip_pready  , tfpu_pready  , uart_pready  , timer0_pready  , perireset_pready  , sysinfo_pready  , mach_timer_pready }),
	.apbm_prdata  ({crc_prdata  , sdadc_prdata  , dma1_prdata  , dma0_prdata  , pio1_prdata  , pio0_prdata  , xip_prdata  , tfpu_prdata  , uart_prdata  , timer0_prdata  , perireset_prdata  , sysinfo_prdata  , mach_timer_prdata }),
	.apbm_pslverr ({crc_pslverr , sdadc_pslverr , dma1_pslverr , dma0_pslverr , pio1_pslverr , pio0_pslverr , xip_pslverr , tfpu_pslverr , uart_pslverr , timer0_pslverr , perireset_pslverr , sysinfo_pslverr , mach_timer_pslverr})
);

end else begin: no_tfpu

apb_splitter #(
	.N_SLAVES   (12),
	.ADDR_MAP   (192'he000_d000_c000_b000_a000_9000_8000_4000_3000_2000_1000_0000),
	.ADDR_MASK  (192'hf000_f000_f000_f000_f000_f000_f000_f000_f000_f000_f000_f000)
) inst_apb_splitter (
	.apbs_paddr   (bridge_paddr),
	.apbs_psel    (bridge_psel),
	.apbs_penable (bridge_penable),
	.apbs_pwrite  (bridge_pwrite),
	.apbs_pwdata  (bridge_pwdata),
	.apbs_pready  (bridge_pready),
	.apbs_prdata  (bridge_prdata),
	.apbs_pslverr (bridge_pslverr),

	.apbm_paddr   ({crc_paddr   , sdadc_paddr   , dma1_paddr   , dma0_paddr   , pio1_paddr   , pio0_paddr   , xip_paddr   , uart_paddr   , timer0_paddr   , perireset_paddr   , sysinfo_paddr   , mach_timer_paddr  }),
	.apbm_psel    ({crc_psel    , sdadc_psel    , dma1_psel    , dma0_psel    , pio1_psel    , pio0_psel    , xip_psel    , uart_psel    , timer0_psel    , perireset_psel    , sysinfo_psel    , mach_timer_psel   }),
	.apbm_penable ({crc_penable , sdadc_penable , dma1_penable , dma0_penable , pio1_penable , pio0_penable , xip_penable , uart_penable , timer0_penable , perireset_penable , sysinfo_penable , mach_timer_penable}),
	.apbm_pwrite  ({crc_pwrite  , sdadc_pwrite  , dma1_pwrite  , dma0_pwrite  , pio1_pwrite  , pio0_pwrite  , xip_pwrite  , uart_pwrite  , timer0_pwrite  , perireset_pwrite  , sysinfo_pwrite  , mach_timer_pwrite }),
	.apbm_pwdata  ({crc_pwdata  , sdadc_pwdata  , dma1_pwdata  , dma0_pwdata  , pio1_pwdata  , pio0_pwdata  , xip_pwdata  , uart_pwdata  , timer0_pwdata  , perireset_pwdata  , sysinfo_pwdata  , mach_timer_pwdata }),
	.apbm_pready  ({crc_pready  , sdadc_pready  , dma1_pready  , dma0_pready  , pio1_pready  , pio0_pready  , xip_pready  , uart_pready  , timer0_pready  , perireset_pready  , sysinfo_pready  , mach_timer_pready }),
	.apbm_prdata  ({crc_prdata  , sdadc_prdata  , dma1_prdata  , dma0_prdata  , pio1_prdata  , pio0_prdata  , xip_prdata  , uart_prdata  , timer0_prdata  , perireset_prdata  , sysinfo_prdata  , mach_timer_prdata }),
	.apbm_pslverr ({crc_pslverr , sdadc_pslverr , dma1_pslverr , dma0_pslverr , pio1_pslverr , pio0_pslverr , xip_pslverr , uart_pslverr , timer0_pslverr , perireset_pslverr , sysinfo_pslverr , mach_timer_pslverr})
);

end
endgenerate

// ----------------------------------------------------------------------------
// Memory and peripherals

// No preloaded bootloader -- just use the debugger! (the processor will
// actually enter an infinite crash loop after reset if memory is
// zero-initialised so don't leave the little guy hanging too long)

generate
if (ICACHE == 1) begin: has_icache

wire               cache_src_hready_resp;
wire               cache_src_hready;
wire               cache_src_hresp;
wire [W_ADDR-1:0]  cache_src_haddr;
wire               cache_src_hwrite;
wire [1:0]         cache_src_htrans;
wire [2:0]         cache_src_hsize;
wire [2:0]         cache_src_hburst;
wire [3:0]         cache_src_hprot;
wire               cache_src_hmastlock;
wire [W_DATA-1:0]  cache_src_hwdata;
wire [W_DATA-1:0]  cache_src_hrdata;

wire               cache_dst_hready_resp;
wire               cache_dst_hready;
wire               cache_dst_hresp;
wire [W_ADDR-1:0]  cache_dst_haddr;
wire               cache_dst_hwrite;
wire [1:0]         cache_dst_htrans;
wire [2:0]         cache_dst_hsize;
wire [2:0]         cache_dst_hburst;
wire [3:0]         cache_dst_hprot;
wire               cache_dst_hmastlock;
wire [W_DATA-1:0]  cache_dst_hwdata;
wire [W_DATA-1:0]  cache_dst_hrdata;

wire               xip_hready_resp;
wire               xip_hready;
wire               xip_hresp;
wire [W_ADDR-1:0]  xip_haddr;
wire               xip_hwrite;
wire [1:0]         xip_htrans;
wire [2:0]         xip_hsize;
wire [2:0]         xip_hburst;
wire [3:0]         xip_hprot;
wire               xip_hmastlock;
wire [W_DATA-1:0]  xip_hwdata;
wire [W_DATA-1:0]  xip_hrdata;

wire               icache_en;
wire               icache_flush;
wire               icache_flush_busy;

assign xip_hready          = icache_en ? cache_dst_hready      : flash_hready;
assign xip_haddr           = icache_en ? cache_dst_haddr       : flash_haddr;
assign xip_hwrite          = icache_en ? cache_dst_hwrite      : flash_hwrite;
assign xip_htrans          = icache_en ? cache_dst_htrans      : flash_htrans;
assign xip_hsize           = icache_en ? cache_dst_hsize       : flash_hsize;
assign xip_hburst          = icache_en ? cache_dst_hburst      : flash_hburst;
assign xip_hprot           = icache_en ? cache_dst_hprot       : flash_hprot;
assign xip_hmastlock       = icache_en ? cache_dst_hmastlock   : flash_hmastlock;
assign xip_hwdata          = icache_en ? cache_dst_hwdata      : flash_hwdata;

assign cache_src_hready    = icache_en ? flash_hready          : 1'b0;
assign cache_src_haddr     = icache_en ? flash_haddr           : {W_DATA{1'b0}};
assign cache_src_hwrite    = icache_en ? flash_hwrite          : 1'b0;
assign cache_src_htrans    = icache_en ? flash_htrans          : 2'b0;
assign cache_src_hsize     = icache_en ? flash_hsize           : 3'b0;
assign cache_src_hburst    = icache_en ? flash_hburst          : 1'b0;
assign cache_src_hprot     = icache_en ? flash_hprot           : 4'b0;
assign cache_src_hmastlock = icache_en ? flash_hmastlock       : 1'b0;
assign cache_src_hwdata    = icache_en ? flash_hwdata          : {W_DATA{1'b0}};

assign flash_hready_resp     = icache_en ? cache_src_hready_resp : xip_hready_resp;
assign flash_hresp           = icache_en ? cache_src_hresp       : xip_hresp;
assign flash_hrdata          = icache_en ? cache_src_hrdata      : xip_hrdata;

assign cache_dst_hready_resp = icache_en ? xip_hready_resp       : 1'b0;
assign cache_dst_hresp       = icache_en ? xip_hresp             : 1'b0;
assign cache_dst_hrdata      = icache_en ? xip_hrdata            : {W_DATA{1'b0}};

ahb_cache_readonly #(
	.N_WAYS(2),
	.W_LINE(128),
	.DEPTH(128) // Capacity in bits = DEPTH * W_LINE * N_WAYS.
) icache (
	.clk               (clk),
	.rst_n             (rst_n),

	.flush             (icache_flush),
	.flush_busy        (icache_flush_busy),

	.src_hready_resp   (cache_src_hready_resp),
	.src_hready        (cache_src_hready),
	.src_hresp         (cache_src_hresp),
	.src_haddr         (cache_src_haddr),
	.src_hwrite        (cache_src_hwrite),
	.src_htrans        (cache_src_htrans),
	.src_hsize         (cache_src_hsize),
	.src_hburst        (cache_src_hburst),
	.src_hprot         (cache_src_hprot),
	.src_hmastlock     (cache_src_hmastlock),
	.src_hwdata        (cache_src_hwdata),
	.src_hrdata        (cache_src_hrdata),

	.dst_hready_resp   (cache_dst_hready_resp),
	.dst_hready        (cache_dst_hready),
	.dst_hresp         (cache_dst_hresp),
	.dst_haddr         (cache_dst_haddr),
	.dst_hwrite        (cache_dst_hwrite),
	.dst_htrans        (cache_dst_htrans),
	.dst_hsize         (cache_dst_hsize),
	.dst_hburst        (cache_dst_hburst),
	.dst_hprot         (cache_dst_hprot),
	.dst_hmastlock     (cache_dst_hmastlock),
	.dst_hwdata        (cache_dst_hwdata),
	.dst_hrdata        (cache_dst_hrdata)
);

spi_qspi_xip xip_u (
	.clk               (clk),
	.rst_n             (rst_n && peri_reset_n_req[XIP_RESET_BIT]),

	.apbs_psel         (xip_psel),
	.apbs_penable      (xip_penable),
	.apbs_pwrite       (xip_pwrite),
	.apbs_paddr        (xip_paddr),
	.apbs_pwdata       (xip_pwdata),
	.apbs_prdata       (xip_prdata),
	.apbs_pready       (xip_pready),
	.apbs_pslverr      (xip_pslverr),

	.ahbls_hready_resp (xip_hready_resp),
	.ahbls_hready      (xip_hready),
	.ahbls_hresp       (xip_hresp),
	.ahbls_haddr       (xip_haddr),
	.ahbls_hwrite      (xip_hwrite),
	.ahbls_htrans      (xip_htrans),
	.ahbls_hsize       (xip_hsize),
	.ahbls_hburst      (xip_hburst),
	.ahbls_hprot       (xip_hprot),
	.ahbls_hmastlock   (xip_hmastlock),
	.ahbls_hwdata      (xip_hwdata),
	.ahbls_hrdata      (xip_hrdata),

	.icache_en         (icache_en),
	.icache_flush      (icache_flush),
	.icache_flush_busy (icache_flush_busy),

	.spi_cs_n          (xip_cs_n),
	.spi_sck           (xip_sck),
	.spi_dout          (xip_dout),
	.spi_douten        (xip_douten),
	.spi_din           (xip_din)
);

end else begin: no_icache

spi_qspi_xip xip_u (
	.clk               (clk),
	.rst_n             (rst_n && peri_reset_n_req[XIP_RESET_BIT]),

	.apbs_psel         (xip_psel),
	.apbs_penable      (xip_penable),
	.apbs_pwrite       (xip_pwrite),
	.apbs_paddr        (xip_paddr),
	.apbs_pwdata       (xip_pwdata),
	.apbs_prdata       (xip_prdata),
	.apbs_pready       (xip_pready),
	.apbs_pslverr      (xip_pslverr),

	.ahbls_hready_resp (flash_hready_resp),
	.ahbls_hready      (flash_hready),
	.ahbls_hresp       (flash_hresp),
	.ahbls_haddr       (flash_haddr),
	.ahbls_hwrite      (flash_hwrite),
	.ahbls_htrans      (flash_htrans),
	.ahbls_hsize       (flash_hsize),
	.ahbls_hburst      (flash_hburst),
	.ahbls_hprot       (flash_hprot),
	.ahbls_hmastlock   (flash_hmastlock),
	.ahbls_hwdata      (flash_hwdata),
	.ahbls_hrdata      (flash_hrdata),

	.spi_cs_n          (xip_cs_n),
	.spi_sck           (xip_sck),
	.spi_dout          (xip_dout),
	.spi_douten        (xip_douten),
	.spi_din           (xip_din)
);

end
endgenerate

ahb_sync_sram #(
	.DEPTH (DRAM_DEPTH)
) dram (
	.clk               (clk),
	.rst_n             (rst_n),

	.ahbls_hready_resp (dram_hready_resp),
	.ahbls_hready      (dram_hready),
	.ahbls_hresp       (dram_hresp),
	.ahbls_haddr       (dram_haddr),
	.ahbls_hwrite      (dram_hwrite),
	.ahbls_htrans      (dram_htrans),
	.ahbls_hsize       (dram_hsize),
	.ahbls_hburst      (dram_hburst),
	.ahbls_hprot       (dram_hprot),
	.ahbls_hmastlock   (dram_hmastlock),
	.ahbls_hwdata      (dram_hwdata),
	.ahbls_hrdata      (dram_hrdata)
);

`ifdef SIMULATION
sim_ctrl #(
	.NUM_IRQS(NUM_IRQS)
) sim_ctrl_u (
	.clk               (clk),
	.rst_n             (rst_n),

	.dump_wave_en      (dump_wave_en),
	.sim_finish        (sim_finish),
	.irq               (irq),
	.soft_irq          (soft_irq),

	.ahbls_hready_resp (sim_ctrl_hready_resp),
	.ahbls_hready      (sim_ctrl_hready),
	.ahbls_hresp       (sim_ctrl_hresp),
	.ahbls_haddr       (sim_ctrl_haddr),
	.ahbls_hwrite      (sim_ctrl_hwrite),
	.ahbls_htrans      (sim_ctrl_htrans),
	.ahbls_hsize       (sim_ctrl_hsize),
	.ahbls_hburst      (sim_ctrl_hburst),
	.ahbls_hprot       (sim_ctrl_hprot),
	.ahbls_hmastlock   (sim_ctrl_hmastlock),
	.ahbls_hwdata      (sim_ctrl_hwdata),
	.ahbls_hrdata      (sim_ctrl_hrdata)
);

ahb_sync_sram #(
	.DEPTH (IRAM_DEPTH)
) iram (
	.clk               (clk),
	.rst_n             (rst_n),

	.ahbls_hready_resp (iram_hready_resp),
	.ahbls_hready      (iram_hready),
	.ahbls_hresp       (iram_hresp),
	.ahbls_haddr       (iram_haddr),
	.ahbls_hwrite      (iram_hwrite),
	.ahbls_htrans      (iram_htrans),
	.ahbls_hsize       (iram_hsize),
	.ahbls_hburst      (iram_hburst),
	.ahbls_hprot       (iram_hprot),
	.ahbls_hmastlock   (iram_hmastlock),
	.ahbls_hwdata      (iram_hwdata),
	.ahbls_hrdata      (iram_hrdata)
);
`endif

sysinfo_regs sysinfo_u (
	.clk           (clk),
	.rst_n         (rst_n),

	.apbs_psel     (sysinfo_psel),
	.apbs_penable  (sysinfo_penable),
	.apbs_pwrite   (sysinfo_pwrite),
	.apbs_paddr    (sysinfo_paddr),
	.apbs_pwdata   (sysinfo_pwdata),
	.apbs_prdata   (sysinfo_prdata),
	.apbs_pready   (sysinfo_pready),
	.apbs_pslverr  (sysinfo_pslverr)
);

peri_reset reset_u (
	.clk           (clk),
	.rst_n         (rst_n),

	.apbs_psel     (perireset_psel),
	.apbs_penable  (perireset_penable),
	.apbs_pwrite   (perireset_pwrite),
	.apbs_paddr    (perireset_paddr),
	.apbs_pwdata   (perireset_pwdata),
	.apbs_prdata   (perireset_prdata),
	.apbs_pready   (perireset_pready),
	.apbs_pslverr  (perireset_pslverr),
	.reset_n_req   (peri_reset_n_req)
);

uart_mini uart_u (
	.clk          (clk),
	.rst_n        (rst_n && peri_reset_n_req[UART_RESET_BIT]),

	.apbs_psel    (uart_psel),
	.apbs_penable (uart_penable),
	.apbs_pwrite  (uart_pwrite),
	.apbs_paddr   (uart_paddr),
	.apbs_pwdata  (uart_pwdata),
	.apbs_prdata  (uart_prdata),
	.apbs_pready  (uart_pready),
	.apbs_pslverr (uart_pslverr),

	.rx           (uart_rx),
	.tx           (uart_tx),
	.cts          (1'b0),
	.rts          (/* unused */),
	.irq          (uart_irq),
	.dreq         (/* unused */)
);

timer timer0 (
	.clk          (clk),
	.rst_n        (rst_n && peri_reset_n_req[TIMER0_RESET_BIT]),

	.apbs_psel    (timer0_psel),
	.apbs_penable (timer0_penable),
	.apbs_pwrite  (timer0_pwrite),
	.apbs_paddr   (timer0_paddr),
	.apbs_pwdata  (timer0_pwdata),
	.apbs_prdata  (timer0_prdata),
	.apbs_pready  (timer0_pready),
	.apbs_pslverr (timer0_pslverr),
	.irq          (timer0_irq)
);

// Microsecond timebase for timer

reg [$clog2(CLK_MHZ)-1:0] timer_tick_ctr;
reg                       timer_tick;

always @ (posedge clk or negedge rst_n) begin
	if (!rst_n) begin
		timer_tick_ctr <= {$clog2(CLK_MHZ){1'b0}};
		timer_tick <= 1'b0;
	end else begin
		if (|timer_tick_ctr) begin
			timer_tick_ctr <= timer_tick_ctr - 1'b1;
		end else begin
			timer_tick_ctr <= CLK_MHZ - 1;
		end
		timer_tick <= ~|timer_tick_ctr;
	end
end

hazard3_riscv_timer riscv_timer_u (
	.clk       (clk),
	.rst_n     (rst_n && peri_reset_n_req[MACH_TIMER_RESET_BIT]),

	.psel      (mach_timer_psel),
	.penable   (mach_timer_penable),
	.pwrite    (mach_timer_pwrite),
	.paddr     (mach_timer_paddr),
	.pwdata    (mach_timer_pwdata),
	.prdata    (mach_timer_prdata),
	.pready    (mach_timer_pready),
	.pslverr   (mach_timer_pslverr),

	.dbg_halt  (hart_halted),

	.tick      (timer_tick),

	.timer_irq (timer_irq)
);

`ifdef SIMULATION
// for gtkwave dump
wire pio0_out    = pio_out[0];
wire pio0_out_en = pio_out_en[0];
wire pio1_out    = pio_out[1];
wire pio1_out_en = pio_out_en[1];
wire pio2_out    = pio_out[2];
wire pio2_out_en = pio_out_en[2];
wire pio3_out    = pio_out[3];
wire pio3_out_en = pio_out_en[3];
wire pio4_out    = pio_out[4];
wire pio4_out_en = pio_out_en[4];
wire pio5_out    = pio_out[5];
wire pio5_out_en = pio_out_en[5];
wire pio6_out    = pio_out[6];
wire pio6_out_en = pio_out_en[6];
wire pio7_out    = pio_out[7];
wire pio7_out_en = pio_out_en[7];
wire pio8_out    = pio_out[8];
wire pio8_out_en = pio_out_en[8];
wire pio9_out    = pio_out[9];
wire pio9_out_en = pio_out_en[9];
wire pio10_out    = pio_out[10];
wire pio10_out_en = pio_out_en[10];
wire pio11_out    = pio_out[11];
wire pio11_out_en = pio_out_en[11];
wire pio12_out    = pio_out[12];
wire pio12_out_en = pio_out_en[12];
wire pio13_out    = pio_out[13];
wire pio13_out_en = pio_out_en[13];
wire pio14_out    = pio_out[14];
wire pio14_out_en = pio_out_en[14];
wire pio15_out    = pio_out[15];
wire pio15_out_en = pio_out_en[15];
wire pio16_out    = pio_out[16];
wire pio16_out_en = pio_out_en[16];
wire pio17_out    = pio_out[17];
wire pio17_out_en = pio_out_en[17];
wire pio18_out    = pio_out[18];
wire pio18_out_en = pio_out_en[18];
wire pio19_out    = pio_out[19];
wire pio19_out_en = pio_out_en[19];
wire pio20_out    = pio_out[20];
wire pio20_out_en = pio_out_en[20];
wire pio21_out    = pio_out[21];
wire pio21_out_en = pio_out_en[21];
wire pio22_out    = pio_out[22];
wire pio22_out_en = pio_out_en[22];
wire pio23_out    = pio_out[23];
wire pio23_out_en = pio_out_en[23];
wire pio24_out    = pio_out[24];
wire pio24_out_en = pio_out_en[24];
wire pio25_out    = pio_out[25];
wire pio25_out_en = pio_out_en[25];
wire pio26_out    = pio_out[26];
wire pio26_out_en = pio_out_en[26];
wire pio27_out    = pio_out[27];
wire pio27_out_en = pio_out_en[27];
wire pio28_out    = pio_out[28];
wire pio28_out_en = pio_out_en[28];
wire pio29_out    = pio_out[29];
wire pio29_out_en = pio_out_en[29];
wire pio30_out    = pio_out[30];
wire pio30_out_en = pio_out_en[30];
wire pio31_out    = pio_out[31];
wire pio31_out_en = pio_out_en[31];
`endif

pio_multi #(
	.NUM_PIO(2)
) pio (
	.clk           (clk),
	.rst_n         (rst_n && peri_reset_n_req[PIO_RESET_BIT]),

	.apbs_psel     ({pio1_psel   , pio0_psel}),
	.apbs_penable  ({pio1_penable, pio0_penable}),
	.apbs_pwrite   ({pio1_pwrite , pio0_pwrite}),
	.apbs_paddr    ({pio1_paddr  , pio0_paddr}),
	.apbs_pwdata   ({pio1_pwdata , pio0_pwdata}),
	.apbs_prdata   ({pio1_prdata , pio0_prdata}),
	.apbs_pready   ({pio1_pready , pio0_pready}),
	.apbs_pslverr  ({pio1_pslverr, pio0_pslverr}),

	.gpio_in       (pio_in),
	.gpio_out      (pio_out),
	.gpio_dir      (pio_out_en),
	.irq           ({pio1_irq, pio0_irq})
);

dma dma0 (
	// AHB-Lite Master Interface
	.ahbm_clk       (clk),
	.ahbm_rst_n     (rst_n),

	.ahbm_haddr     (dma0_haddr),
	.ahbm_hwrite    (dma0_hwrite),
	.ahbm_htrans    (dma0_htrans),
	.ahbm_hsize     (dma0_hsize),
	.ahbm_hburst    (dma0_hburst),
	.ahbm_hprot     (dma0_hprot),
	.ahbm_hmastlock (dma0_hmastlock),
	.ahbm_hmaster   (dma0_hmaster),
	.ahbm_hexcl     (dma0_hexcl),
	.ahbm_hready    (dma0_hready),
	.ahbm_hresp     (dma0_hresp),
	.ahbm_hexokay   (1'b1),
	.ahbm_hwdata    (dma0_hwdata),
	.ahbm_hrdata    (dma0_hrdata),

	// APB Slave Interface
	.apbs_clk       (clk),
	.apbs_rst_n     (rst_n),

	.apbs_psel      (dma0_psel),
	.apbs_penable   (dma0_penable),
	.apbs_pwrite    (dma0_pwrite),
	.apbs_paddr     (dma0_paddr),
	.apbs_pwdata    (dma0_pwdata),
	.apbs_prdata    (dma0_prdata),
	.apbs_pready    (dma0_pready),
	.apbs_pslverr   (dma0_pslverr),

	// Interrupt
	.irq            (dma0_irq)
);

dma dma1 (
	// AHB-Lite Master Interface
	.ahbm_clk       (clk),
	.ahbm_rst_n     (rst_n),

	.ahbm_haddr     (dma1_haddr),
	.ahbm_hwrite    (dma1_hwrite),
	.ahbm_htrans    (dma1_htrans),
	.ahbm_hsize     (dma1_hsize),
	.ahbm_hburst    (dma1_hburst),
	.ahbm_hprot     (dma1_hprot),
	.ahbm_hmastlock (dma1_hmastlock),
	.ahbm_hmaster   (dma1_hmaster),
	.ahbm_hexcl     (dma1_hexcl),
	.ahbm_hready    (dma1_hready),
	.ahbm_hresp     (dma1_hresp),
	.ahbm_hexokay   (1'b1),
	.ahbm_hwdata    (dma1_hwdata),
	.ahbm_hrdata    (dma1_hrdata),

	// APB Slave Interface
	.apbs_clk       (clk),
	.apbs_rst_n     (rst_n),

	.apbs_psel      (dma1_psel),
	.apbs_penable   (dma1_penable),
	.apbs_pwrite    (dma1_pwrite),
	.apbs_paddr     (dma1_paddr),
	.apbs_pwdata    (dma1_pwdata),
	.apbs_prdata    (dma1_prdata),
	.apbs_pready    (dma1_pready),
	.apbs_pslverr   (dma1_pslverr),

	// Interrupt
	.irq            (dma1_irq)
);

crc #(
	.FIFO_DEPTH(8)
) crc_u (
	.clk            (clk),
	.rst_n          (rst_n),

	.apbs_psel      (crc_psel),
	.apbs_penable   (crc_penable),
	.apbs_pwrite    (crc_pwrite),
	.apbs_paddr     (crc_paddr),
	.apbs_pwdata    (crc_pwdata),
	.apbs_prdata    (crc_prdata),
	.apbs_pready    (crc_pready),
	.apbs_pslverr   (crc_pslverr)
);

generate
if (TFPU == 1) begin: has_tfpu
tfpu_top tfpu (
	.clk            (clk),
	.rst_n          (rst_n),

	.apbs_psel      (tfpu_psel),
	.apbs_penable   (tfpu_penable),
	.apbs_pwrite    (tfpu_pwrite),
	.apbs_paddr     (tfpu_paddr),
	.apbs_pwdata    (tfpu_pwdata),
	.apbs_prdata    (tfpu_prdata),
	.apbs_pready    (tfpu_pready),
	.apbs_pslverr   (tfpu_pslverr)
);
end
endgenerate

sdadc_top #(
	.ADC_WIDTH(10),
	.ACCUM_BITS(12)
) sdadc (
	.clk         (clk),
	.rst_n       (rst_n),

	.apbs_psel   (sdadc_psel),
	.apbs_penable(sdadc_penable),
	.apbs_pwrite (sdadc_pwrite),
	.apbs_paddr  (sdadc_paddr),
	.apbs_pwdata (sdadc_pwdata),
	.apbs_prdata (sdadc_prdata),
	.apbs_pready (sdadc_pready),
	.apbs_pslverr(sdadc_pslverr),

	.cmp_in      (adc_cmp_in),
	.pwm_out     (adc_pwm_out),
	.irq         (adc_irq)
);

endmodule
