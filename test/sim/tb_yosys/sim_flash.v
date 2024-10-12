 /*
 Copyright 2024 Blue Liang, liangkangnan@163.com
                                                                         
 Licensed under the Apache License, Version 2.0 (the "License");         
 you may not use this file except in compliance with the License.        
 You may obtain a copy of the License at                                 
                                                                         
     http://www.apache.org/licenses/LICENSE-2.0                          
                                                                         
 Unless required by applicable law or agreed to in writing, software    
 distributed under the License is distributed on an "AS IS" BASIS,       
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and     
 limitations under the License.
 */

// norflash simulation model
// CPHA = 0, CPOL = 0

module sim_flash #(
	parameter WIDTH = 8,
	parameter DEPTH = 1 << 15
) (
	input  wire        clk,
	input  wire        rst_n,

	input  wire        spi_cs_n,
	input  wire        spi_sck,
	input  wire        spi_mosi,
	output wire        spi_miso
);

localparam ADDR_WIDTH = $clog2(DEPTH);

// support cmd
localparam CMD_READ_STATUS     = 8'h05;
localparam CMD_READ_BYTE       = 8'h03;
localparam CMD_PAGE_PROG       = 8'h02;
localparam CMD_SECTOR_ERASE    = 8'h20;
localparam CMD_32K_ERASE       = 8'h52;
localparam CMD_64K_ERASE       = 8'hD8;
localparam CMD_CHIP_ERASE      = 8'h60;
localparam CMD_CHIP_ERASE2     = 8'hC7;
localparam CMD_WRITE_ENABLE    = 8'h06;
localparam CMD_WRITE_DISABLE   = 8'h04;

localparam S_IDLE              = 4'd0;
localparam S_CMD               = 4'd1;
localparam S_ADDR              = 4'd2;
localparam S_READ_DATA         = 4'd3;
localparam S_READ_STATUS       = 4'd4;
localparam S_PROG_DATA         = 4'd5;
localparam S_WRITE_ENABLE      = 4'd6;
localparam S_WRITE_DISABLE     = 4'd7;
localparam S_ERASE             = 4'd8;
localparam S_UNKNOWN           = 4'd15;

reg [ 3:0] state_d                , state_q;
reg [31:0] shift_reg_d            , shift_reg_q;
reg [ 7:0] counter_d              , counter_q;
reg [ 7:0] cmd_d                  , cmd_q;
reg [23:0] addr_d                 , addr_q;
reg        spi_miso_d             , spi_miso_q;
reg        read_en_d              , read_en_q;
reg [ 7:0] status_reg_d           , status_reg_q;
reg        reset_fifo_d           , reset_fifo_q;
reg        write_fifo_en_d        , write_fifo_en_q;
reg [ 7:0] write_fifo_data_d      , write_fifo_data_q;
reg        flash_write_enable_d   , flash_write_enable_q;

reg                  spi_sck_q;
reg                  programming;
reg                  write_en;
reg                  fifo_read_en;
reg [7:0]            prog_byte;
reg [ADDR_WIDTH-1:0] prog_addr;
reg [ADDR_WIDTH-1:0] prog_start_addr;

wire       fifo_empty;
wire [7:0] fifo_read_data;

wire sck_pos = (~spi_sck_q) && spi_sck;
wire sck_neg = spi_sck_q && (~spi_sck);

wire [WIDTH-1:0]      sram_rdata;
wire [WIDTH-1:0]      sram_wdata;
wire [ADDR_WIDTH-1:0] sram_addr;

always @ (*) begin
	state_d                = state_q;
	counter_d              = counter_q;
	shift_reg_d            = shift_reg_q;
	cmd_d                  = cmd_q;
	addr_d                 = addr_q;
	spi_miso_d             = spi_miso_q;
	read_en_d              = 1'b0;
	status_reg_d           = {7'h0, programming};
	reset_fifo_d           = 1'b0;
	write_fifo_en_d        = 1'b0;
	write_fifo_data_d      = write_fifo_data_q;
	flash_write_enable_d   = flash_write_enable_q;

	if (spi_cs_n) begin
		state_d = S_IDLE;
	end

	case (state_q)
		S_IDLE: begin
			if (~spi_cs_n) begin
				state_d = S_CMD;
				counter_d = 8'h0;
			end
		end

		S_CMD: begin
			if (sck_pos) begin
				counter_d = counter_q + 1'b1;
				shift_reg_d = {shift_reg_q[30:0], spi_mosi};
				if (counter_q == 7) begin
					counter_d = 8'h0;
					cmd_d = shift_reg_d[7:0];
					if (cmd_d == CMD_READ_STATUS) begin
						state_d = S_READ_STATUS;
					end else if (cmd_d == CMD_READ_BYTE ||
								 cmd_d == CMD_PAGE_PROG) begin
						state_d = S_ADDR;
					end else if (cmd_d == CMD_SECTOR_ERASE ||
								 cmd_d == CMD_32K_ERASE ||
								 cmd_d == CMD_64K_ERASE ||
								 cmd_d == CMD_CHIP_ERASE ||
								 cmd_d == CMD_CHIP_ERASE2) begin
						state_d = S_ERASE;
					end else if (cmd_d == CMD_WRITE_ENABLE) begin
						state_d = S_WRITE_ENABLE;
						flash_write_enable_d = 1'b1;
					end else if (cmd_d == CMD_WRITE_DISABLE) begin
						state_d = S_WRITE_DISABLE;
						flash_write_enable_d = 1'b0;
					end else begin
						state_d = S_UNKNOWN;
						$display("sim_flash: unknown cmd!!!");
					end
				end
			end
		end

		S_ADDR: begin
			if (sck_pos) begin
				counter_d = counter_q + 1'b1;
				shift_reg_d = {shift_reg_q[30:0], spi_mosi};
				if (counter_q == 23) begin
					counter_d = 8'h0;
					addr_d = {shift_reg_q[22:0], spi_mosi};
					if (cmd_q == 8'h03) begin
						state_d = S_READ_DATA;
						read_en_d = 1'b1;
					end else if (cmd_q == 8'h02) begin
						state_d = S_PROG_DATA;
						reset_fifo_d = 1'b1;
					end
				end
			end
		end

		S_READ_DATA: begin
			if (sck_neg) begin
				counter_d = counter_q + 1'b1;
				spi_miso_d = sram_rdata[7 - counter_q];
				if (counter_q == 7) begin
					counter_d = 8'h0;
					read_en_d = 1'b1;
					addr_d = addr_q + 24'h1;
				end
			end
		end

		S_READ_STATUS: begin
			if (sck_neg) begin
				counter_d = counter_q + 1'b1;
				spi_miso_d = status_reg_q[7 - counter_q];
				if (counter_q == 7) begin
					counter_d = 8'h0;
				end
			end
		end

		S_PROG_DATA: begin
			if (sck_pos) begin
				counter_d = counter_q + 1'b1;
				write_fifo_data_d = {write_fifo_data_q[6:0], spi_mosi};
				if (counter_q == 7) begin
					counter_d = 8'h0;
					write_fifo_en_d = 1'b1;
				end
			end
		end

		S_ERASE, S_WRITE_ENABLE, S_WRITE_DISABLE: begin

		end

		S_UNKNOWN: begin
			counter_d = 8'h0;
		end

	endcase
end

assign spi_miso = spi_miso_d;

always @ (posedge clk or negedge rst_n) begin
	if (!rst_n) begin
		state_q                <= S_IDLE;
		spi_sck_q              <= 1'b0;
		counter_q              <= 8'h0;
		shift_reg_q            <= 32'h0;
		cmd_q                  <= 8'h0;
		addr_q                 <= 24'h0;
		spi_miso_q             <= 1'b0;
		read_en_q              <= 1'b0;
		status_reg_q           <= 8'h0;
		reset_fifo_q           <= 1'b0;
		write_fifo_en_q        <= 1'b0;
		write_fifo_data_q      <= 8'h0;
		flash_write_enable_q   <= 1'b0;
	end else begin
		state_q                <= state_d;
		spi_sck_q              <= spi_sck;
		counter_q              <= counter_d;
		shift_reg_q            <= shift_reg_d;
		cmd_q                  <= cmd_d;
		addr_q                 <= addr_d;
		spi_miso_q             <= spi_miso_d;
		read_en_q              <= read_en_d;
		status_reg_q           <= status_reg_d;
		reset_fifo_q           <= reset_fifo_d;
		write_fifo_en_q        <= write_fifo_en_d;
		write_fifo_data_q      <= write_fifo_data_d;
		flash_write_enable_q   <= flash_write_enable_d;
	end
end

// flash program logic
always @ (posedge clk or negedge rst_n) begin
	if (!rst_n) begin
		programming       <= 1'b0;
		write_en          <= 1'b0;
		prog_byte         <= 8'h0;
		prog_addr         <= {ADDR_WIDTH{1'b0}};
		fifo_read_en      <= 1'b0;
		prog_start_addr   <= {ADDR_WIDTH{1'b0}};
	end else begin
		if (spi_cs_n && (state_q == S_PROG_DATA) && (~programming) && flash_write_enable_q) begin
			programming <= 1'b1;
			fifo_read_en <= 1'b1;
			prog_start_addr <= addr_q[ADDR_WIDTH-1:0];
		end
		if (programming) begin
			prog_byte <= fifo_read_data;
			prog_addr <= prog_start_addr;
			prog_start_addr <= prog_start_addr + 1'b1;
			if (fifo_empty) begin
				write_en <= 1'b0;
				programming <= 1'b0;
				fifo_read_en <= 1'b0;
			end else begin
				write_en <= 1'b1;
			end
		end
	end
end

// program datas FIFO
sync_fifo #(
	.DEPTH(256),
	.WIDTH(8)
) prog_data_fifo (
	.clk    (clk),
	.rst_n  (rst_n & ~reset_fifo_q),
	.w_data (write_fifo_data_q),
	.w_en   (write_fifo_en_q),
	.r_data (fifo_read_data),
	.r_en   (fifo_read_en),
	.empty  (fifo_empty)
);

assign sram_wdata = prog_byte;
assign sram_addr  = read_en_d ? addr_d[ADDR_WIDTH-1:0] : prog_addr;

sram_sync #(
	.WIDTH(WIDTH),
	.DEPTH(DEPTH),
	.BYTE_ENABLE(1),
	.PRELOAD_FILE("preload_file.mem")
) sram (
	.clk   (clk),
	.wen   (write_en),
	.ren   (read_en_d),
	.addr  (sram_addr),
	.wdata (sram_wdata),
	.rdata (sram_rdata)
);

endmodule
