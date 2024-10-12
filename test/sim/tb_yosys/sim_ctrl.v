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

// AHB slave for simulation control.
module sim_ctrl #(
	parameter W_DATA = 32,
	parameter W_ADDR = 32
) (
	input wire                clk,
	input wire                rst_n,

	output wire               dump_wave_en,

	output wire               ahbls_hready_resp,
	input  wire               ahbls_hready,
	output wire               ahbls_hresp,
	input  wire [W_ADDR-1:0]  ahbls_haddr,
	input  wire               ahbls_hwrite,
	input  wire [1:0]         ahbls_htrans,
	input  wire [2:0]         ahbls_hsize,
	input  wire [2:0]         ahbls_hburst,
	input  wire [3:0]         ahbls_hprot,
	input  wire               ahbls_hmastlock,
	input  wire [W_DATA-1:0]  ahbls_hwdata,
	output wire [W_DATA-1:0]  ahbls_hrdata
);

    localparam ADDR_PUTCHAR     = 8'h0;
    localparam ADDR_PUTUINT32   = 8'h4;
    localparam ADDR_EXIT        = 8'h8;
	localparam ADDR_SET_SOFTIRQ = 8'h10;
	localparam ADDR_CLR_SOFTIRQ = 8'h14;
	localparam ADDR_DUMP_WAVE   = 8'h18;

    wire ahb_write_aphase_d = ahbls_htrans[1] && ahbls_hready && ahbls_hwrite;
    reg ahb_write_aphase_q;
    reg [W_ADDR-1:0] addr_q;
    reg dump_wave_en_q;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ahb_write_aphase_q <= 1'b0;
            addr_q <= {W_ADDR{1'b0}};
            dump_wave_en_q <= 1'b0;
        end else begin
            ahb_write_aphase_q <= ahb_write_aphase_d;
            if (ahb_write_aphase_d)
                addr_q <= ahbls_haddr;
            if (ahb_write_aphase_q) begin
                if (addr_q[7:0] == ADDR_PUTCHAR) begin
                    $write("%c", ahbls_hwdata[7:0]);
                end else if (addr_q[7:0] == ADDR_PUTUINT32) begin
                    $write("%d", ahbls_hwdata);
                end else if (addr_q[7:0] == ADDR_DUMP_WAVE) begin
                    dump_wave_en_q <= ahbls_hwdata[0];
                end else if (addr_q[7:0] == ADDR_EXIT) begin
                    $display("APP req exit, code = %d", ahbls_hwdata);
                    //$finish;
                end
            end
        end
    end

    assign dump_wave_en = dump_wave_en_q;

    assign ahbls_hresp = 1'b0;
    assign ahbls_hready_resp = 1'b1;
    assign ahbls_hrdata = {W_DATA{1'b0}};

endmodule
