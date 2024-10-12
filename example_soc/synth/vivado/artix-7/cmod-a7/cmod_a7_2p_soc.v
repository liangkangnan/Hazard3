/*****************************************************************************\
|                      Copyright (C) 2021-2022 Luke Wren                      |
|                     SPDX-License-Identifier: Apache-2.0                     |
\*****************************************************************************/

// Example file integrating a Hazard3 processor, processor JTAG + debug
// components, some memory and a UART.

`default_nettype none

module cmod_a7_2p_soc #(
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
`endif

	// SPI interface
	output wire              spi_cs_n,
	output wire              spi_sck,
	output wire              spi_mosi,
	input  wire              spi_miso,

	// IO
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

wire              pwrup_req;
wire              unblock_out;

wire              uart_irq;
wire              timer_irq;

`include "cmod_a7_soc_config.vh"

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

	.irq                        ({3'h0, uart_irq}),

	.soft_irq                   (1'b0),
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

ahbl_crossbar #(
    .N_MASTERS  (2),
    .N_SLAVES   (5),
    .W_ADDR     (W_ADDR),
    .W_DATA     (W_DATA),
    .ADDR_MAP   (160'h80000000_40000000_20000000_10000000_00000000),
    .ADDR_MASK  (160'hf0000000_f0000000_f0000000_f0000000_f0000000)
) crossbar (
	.clk             (clk),
	.rst_n           (rst_n),

    .src_hready_resp ({i_hready     , d_hready}),
    .src_hresp       ({i_hresp      , d_hresp}),
    .src_haddr       ({i_haddr      , d_haddr}),
    .src_hwrite      ({i_hwrite     , d_hwrite}),
    .src_htrans      ({i_htrans     , d_htrans}),
    .src_hsize       ({i_hsize      , d_hsize}),
    .src_hburst      ({i_hburst     , d_hburst}),
    .src_hprot       ({i_hprot      , d_hprot}),
    .src_hmastlock   ({i_hmastlock  , d_hmastlock}),
    .src_hwdata      ({i_hwdata     , d_hwdata}),
    .src_hrdata      ({i_hrdata     , d_hrdata}),

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
    .N_MASTERS  (2),
    .N_SLAVES   (4),
    .W_ADDR     (W_ADDR),
    .W_DATA     (W_DATA),
    .ADDR_MAP   (128'h40000000_20000000_10000000_00000000),
    .ADDR_MASK  (128'hf0000000_f0000000_f0000000_f0000000)
) crossbar (
	.clk             (clk),
	.rst_n           (rst_n),

    .src_hready_resp ({i_hready     , d_hready}),
    .src_hresp       ({i_hresp      , d_hresp}),
    .src_haddr       ({i_haddr      , d_haddr}),
    .src_hwrite      ({i_hwrite     , d_hwrite}),
    .src_htrans      ({i_htrans     , d_htrans}),
    .src_hsize       ({i_hsize      , d_hsize}),
    .src_hburst      ({i_hburst     , d_hburst}),
    .src_hprot       ({i_hprot      , d_hprot}),
    .src_hmastlock   ({i_hmastlock  , d_hmastlock}),
    .src_hwdata      ({i_hwdata     , d_hwdata}),
    .src_hrdata      ({i_hrdata     , d_hrdata}),

    .dst_hready_resp ({bridge_hready_resp , dram_hready_resp , iram_hready_resp , flash_hready_resp}),
    .dst_hready      ({bridge_hready      , dram_hready      , iram_hready      , flash_hready}),
    .dst_hresp       ({bridge_hresp       , dram_hresp       , iram_hresp       , flash_hresp}),
    .dst_haddr       ({bridge_haddr       , dram_haddr       , iram_haddr       , flash_haddr}),
    .dst_hwrite      ({bridge_hwrite      , dram_hwrite      , iram_hwrite      , flash_hwrite}),
    .dst_htrans      ({bridge_htrans      , dram_htrans      , iram_htrans      , flash_htrans}),
    .dst_hsize       ({bridge_hsize       , dram_hsize       , iram_hsize       , flash_hsize}),
    .dst_hburst      ({bridge_hburst      , dram_hburst      , iram_hburst      , flash_hburst}),
    .dst_hprot       ({bridge_hprot       , dram_hprot       , iram_hprot       , flash_hprot}),
    .dst_hmastlock   ({bridge_hmastlock   , dram_hmastlock   , iram_hmastlock   , flash_hmastlock}),
    .dst_hwdata      ({bridge_hwdata      , dram_hwdata      , iram_hwdata      , flash_hwdata}),
    .dst_hrdata      ({bridge_hrdata      , dram_hrdata      , iram_hrdata      , flash_hrdata})
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

wire        uart_psel;
wire        uart_penable;
wire        uart_pwrite;
wire [15:0] uart_paddr;
wire [31:0] uart_pwdata;
wire [31:0] uart_prdata;
wire        uart_pready;
wire        uart_pslverr;

wire        timer_psel;
wire        timer_penable;
wire        timer_pwrite;
wire [15:0] timer_paddr;
wire [31:0] timer_pwdata;
wire [31:0] timer_prdata;
wire        timer_pready;
wire        timer_pslverr;

wire        xip_psel;
wire        xip_penable;
wire        xip_pwrite;
wire [15:0] xip_paddr;
wire [31:0] xip_pwdata;
wire [31:0] xip_prdata;
wire        xip_pready;
wire        xip_pslverr;

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

apb_splitter #(
	.N_SLAVES   (3),
	.ADDR_MAP   (48'h8000_4000_0000),
	.ADDR_MASK  (48'hf000_f000_f000)
) inst_apb_splitter (
	.apbs_paddr   (bridge_paddr),
	.apbs_psel    (bridge_psel),
	.apbs_penable (bridge_penable),
	.apbs_pwrite  (bridge_pwrite),
	.apbs_pwdata  (bridge_pwdata),
	.apbs_pready  (bridge_pready),
	.apbs_prdata  (bridge_prdata),
	.apbs_pslverr (bridge_pslverr),

	.apbm_paddr   ({xip_paddr   , uart_paddr   , timer_paddr  }),
	.apbm_psel    ({xip_psel    , uart_psel    , timer_psel   }),
	.apbm_penable ({xip_penable , uart_penable , timer_penable}),
	.apbm_pwrite  ({xip_pwrite  , uart_pwrite  , timer_pwrite }),
	.apbm_pwdata  ({xip_pwdata  , uart_pwdata  , timer_pwdata }),
	.apbm_pready  ({xip_pready  , uart_pready  , timer_pready }),
	.apbm_prdata  ({xip_prdata  , uart_prdata  , timer_prdata }),
	.apbm_pslverr ({xip_pslverr , uart_pslverr , timer_pslverr})
);

// ----------------------------------------------------------------------------
// Memory and peripherals

// No preloaded bootloader -- just use the debugger! (the processor will
// actually enter an infinite crash loop after reset if memory is
// zero-initialised so don't leave the little guy hanging too long)

spi_03h_xip xip_u (
	.clk               (clk),
	.rst_n             (rst_n),

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

	.spi_cs_n          (spi_cs_n),
	.spi_sck           (spi_sck),
	.spi_mosi          (spi_mosi),
	.spi_miso          (spi_miso)
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
sim_ctrl sim_ctrl_u (
	.clk               (clk),
	.rst_n             (rst_n),

	.dump_wave_en      (dump_wave_en),

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
`endif

uart_mini uart_u (
	.clk          (clk),
	.rst_n        (rst_n),

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
	.rst_n     (rst_n),

	.psel      (timer_psel),
	.penable   (timer_penable),
	.pwrite    (timer_pwrite),
	.paddr     (timer_paddr),
	.pwdata    (timer_pwdata),
	.prdata    (timer_prdata),
	.pready    (timer_pready),
	.pslverr   (timer_pslverr),

	.dbg_halt  (hart_halted),

	.tick      (timer_tick),

	.timer_irq (timer_irq)
);

endmodule
