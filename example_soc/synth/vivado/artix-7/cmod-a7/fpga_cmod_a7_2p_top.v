`default_nettype none

module fpga_cmod_a7_2p_top (
	input wire        clk_12m,
	input wire        rst_n,

	input  wire       tck,
	input  wire       tms,
	input  wire       tdi,
	output wire       tdo,

	output wire       led,

`ifdef SIMULATION
	output wire       dump_wave_en,
	input  wire [31:0]reset_offset,
`endif

	output wire       spi_cs_n,
	output wire       spi_sck,
	output wire       spi_mosi,
	input  wire       spi_miso,

	output wire       uart_tx,
	input  wire       uart_rx
);

    wire clk_sys;
    wire rst_n_sys;
    wire trst_n;

    assign clk_sys = clk_12m;

/*
    // 使用xilinx vivado中的mmcm IP进行倍频
    // 输入为12MHZ，输出为24MHZ
    mmcm_main_clk u_mmcm_main_clk (
        .clk_out1(clk_sys),
        .resetn  (gen_reset_n),
        .clk_in1 (clk_12m),
        .locked  (main_clk_locked)
    );
*/

    fpga_reset #(
        .SHIFT (5)
    ) rstgen (
        .clk         (clk_sys),
        .force_rst_n (rst_n),
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

    cmod_a7_2p_soc #(
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
        .reset_offset   (reset_offset),
`else
        .reset_offset   (32'h0000_0040),
`endif

        .spi_cs_n       (spi_cs_n),
        .spi_sck        (spi_sck),
        .spi_mosi       (spi_mosi),
        .spi_miso       (spi_miso),

        .uart_tx        (uart_tx),
        .uart_rx        (uart_rx)
    );

endmodule
