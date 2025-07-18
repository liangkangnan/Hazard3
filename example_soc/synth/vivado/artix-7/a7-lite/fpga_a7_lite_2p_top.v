`default_nettype none

module fpga_a7_lite_2p_top (
	input wire        clk_50m,
	input wire        rst_n,

	input  wire       tck,
	input  wire       tms,
	input  wire       tdi,
	output wire       tdo,

	output wire       led,

`ifdef SIMULATION
	output wire       dump_wave_en,
	output wire       sim_finish,
	input  wire [31:0]reset_offset,
	output wire       xip_cs_n,
	output wire       xip_sck,
	output wire [3:0] xip_dout,
	output wire [3:0] xip_douten,
	input  wire [3:0] xip_din,
	input  wire [31:0]pio_in,
	output wire [31:0]pio_out_en,
	output wire [31:0]pio_out,
`else
	output wire       xip_cs_n,
	output wire       xip_sck,
	inout  wire [3:0] xip_io,

	inout  wire [31:0] pio,
`endif

	input  wire       adc_cmp_in,
	output wire       adc_pwm_out,

	output wire       uart_tx,
	input  wire       uart_rx
);

`include "a7_lite_soc_config.vh"

    wire clk_sys;
    wire rst_n_sys;
    wire trst_n;
    wire pll_locked;

`ifdef SIMULATION
    assign clk_sys = clk_50m;
    assign pll_locked = 1'b1;
`else
    // 使用xilinx vivado中的mmcm IP进行分频
    // 输入为50MHZ，输出为12MHZ
    mmcm_main_clk u_mmcm_main_clk (
        // Clock out ports
        .clk_out_12m(clk_sys),          // output clk_out_12m
        // Status and control signals
        .resetn(rst_n),                 // input resetn
        .locked(pll_locked),            // output locked
        // Clock in ports
        .clk_in1(clk_50m)               // input clk_in1
    );
`endif

    fpga_reset #(
        .SHIFT (5)
    ) rstgen (
        .clk         (clk_sys),
        .force_rst_n (rst_n & pll_locked),
        .rst_n       (rst_n_sys)
    );

    reset_sync trst_sync_u (
        .clk       (tck),
        .rst_n_in  (rst_n_sys),
        .rst_n_out (trst_n)
    );

    activity_led #(
        .WIDTH (1 << 6),
        .ACTIVE_LEVEL (1'b1)
    ) tck_led_u (
        .clk   (clk_sys),
        .rst_n (rst_n_sys),
        .i     (tck),
        .o     (led)
    );

`ifdef SIMULATION

`else
	wire [3:0] xip_din;
	wire [3:0] xip_douten;
	wire [3:0] xip_dout;

	for (genvar j = 0; j < 4; j = j + 1) begin : xip_pin_data
		assign xip_io[j] = xip_douten[j] ? xip_dout[j] : 1'bz;
		assign xip_din[j] = xip_io[j];
	end

	wire [31:0] pio_in;
	wire [31:0] pio_out_en;
	wire [31:0] pio_out;

	for (genvar j = 0; j < 32; j = j + 1) begin : pio_pin_data
		assign pio[j] = pio_out_en[j] ? pio_out[j] : 1'bz;
		assign pio_in[j] = pio[j];
	end
`endif

    a7_lite_2p_soc #(
        .CLK_MHZ        (12),
        .IRAM_DEPTH     (1 << 15),
        .DRAM_DEPTH     (1 << 13)
    ) soc_u (
        .clk            (clk_sys),
        .rst_n          (rst_n_sys),

        .tck            (tck),
        .trst_n         (trst_n),
        .tms            (tms),
        .tdi            (tdi),
        .tdo            (tdo),

`ifdef SIMULATION
        .dump_wave_en   (dump_wave_en),
        .sim_finish     (sim_finish),
        .reset_offset   (reset_offset),
`else
        .reset_offset   (RESET_VECTOR),
`endif

		.pio_in         (pio_in),
		.pio_out        (pio_out),
		.pio_out_en     (pio_out_en),

		.xip_cs_n       (xip_cs_n),
		.xip_sck        (xip_sck),
		.xip_dout       (xip_dout),
		.xip_douten     (xip_douten),
		.xip_din        (xip_din),

		.adc_cmp_in     (adc_cmp_in),
		.adc_pwm_out    (adc_pwm_out),

        .uart_tx        (uart_tx),
        .uart_rx        (uart_rx)
    );

endmodule
