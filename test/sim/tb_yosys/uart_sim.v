

module uart_sim (
	input wire clk,
	input wire rst_n,

	// APB Port
	input wire apbs_psel,
	input wire apbs_penable,
	input wire apbs_pwrite,
	input wire [15:0] apbs_paddr,
	input wire [31:0] apbs_pwdata,
	output wire [31:0] apbs_prdata,
	output wire apbs_pready,
	output wire apbs_pslverr,

	input wire rx,
	output reg tx,
	input wire cts,
	output reg rts,

	output wire irq,
	output wire dreq
);

    localparam ADDR_CSR = 0;
    localparam ADDR_DIV = 4;
    localparam ADDR_FSTAT = 8;
    localparam ADDR_TX = 12;
    localparam ADDR_RX = 16;

    wire [31:0] wdata = apbs_pwdata;
    wire wen = apbs_psel && apbs_penable && apbs_pwrite;
    wire [15:0] addr = apbs_paddr & 16'h1c;
    wire __tx_wen = wen && addr == ADDR_TX;
    wire [7:0] tx_wdata = wdata[7:0];

    always @(posedge clk) begin
        if (__tx_wen) begin
            $write("%c", tx_wdata);
        end
    end

    assign irq = 1'b0;
    assign tx = 1'b0;
    assign rts = 1'b0;
    assign dreq = 1'b0;
    assign apbs_pready = 1'b1;
    assign apbs_pslverr = 1'b0;
    assign apbs_prdata = 32'h0;

endmodule
