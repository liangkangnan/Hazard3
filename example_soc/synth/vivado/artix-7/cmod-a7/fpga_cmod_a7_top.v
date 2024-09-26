`default_nettype none

module fpga_cmod_a7_top (
	input wire        clk_12m,

	input  wire       tck,
	input  wire       tms,
	input  wire       tdi,
	output wire       tdo,

	output wire       led,

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
        .force_rst_n (1'b1),
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

    cmod_a7_soc #(
        .DTM_TYPE       ("JTAG"),
        .CLK_MHZ        (12),
        .FLASH_DEPTH    (1 << 15),
        .SRAM_DEPTH     (1 << 13)
    ) soc_u (
        .clk            (clk_sys),
        .rst_n          (rst_n_sys),

        .tck            (tck),
        .trst_n         (trst_n),
        .tms            (tms),
        .tdi            (tdi),
        .tdo            (tdo),

        .uart_tx        (uart_tx),
        .uart_rx        (uart_rx)
    );

endmodule
