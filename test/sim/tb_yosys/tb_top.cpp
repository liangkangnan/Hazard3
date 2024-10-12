#include <fstream>
#include <cstdint>
#include <string>
#include <stdio.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Device-under-test model generated by CXXRTL:
#include "dut.cpp"
#include <cxxrtl/cxxrtl_vcd.h>

#define FLASH_START 0x00000000
#define FLASH_SIZE  (128 * 1024)
#define FLASH_END   (FLASH_START + FLASH_SIZE - 1)

#define IRAM_START  0x10000000
#define IRAM_SIZE   (128 * 1024)
#define IRAM_END    (IRAM_START + IRAM_SIZE - 1)

#define DRAM_START  0x20000000
#define DRAM_SIZE   (32 * 1024)
#define DRAM_END    (DRAM_START + DRAM_SIZE - 1)

// MEM_KB define in Makefile
#define MEM_SIZE (MEM_KB * 1024)

uint8_t bin_memory[MEM_SIZE];
static const int TCP_BUF_SIZE = 256;

static int wait_for_connection(int server_fd, uint16_t port, struct sockaddr *sock_addr, socklen_t *sock_addr_len)
{
	int sock_fd;
	printf("Waiting for connection on port %u\n", port);
	if (listen(server_fd, 3) < 0) {
		fprintf(stderr, "listen failed\n");
		exit(-1);
	}
	sock_fd = accept(server_fd, sock_addr, sock_addr_len);
	if (sock_fd < 0) {
		fprintf(stderr, "accept failed\n");
		exit(-1);
	}
	printf("Connected\n");
	return sock_fd;
}

// -----------------------------------------------------------------------------

const char *help_str =
"Usage: testbench_verilator [--bin x.bin] [--binaddr addr] [--port n] [--vcd x.vcd]\\\n"
"                           [--cycles n] [--retcode]\n"
"\n"
"    --bin x.bin      : Flat binary file to be loaded\n"
"    --binaddr addr   : Flat binary file loaded to address [addr]\n"
"    --vcd x.vcd      : Path to dump waveforms to\n"
"    --cycles n       : Maximum number of cycles to run before exiting.\n"
"                       Default is 0 (no maximum).\n"
"    --port n         : Port number to listen for openocd remote bitbang. Sim\n"
"                       runs in lockstep with JTAG bitbang, not free-running.\n"
"    --dumpall        : Dump wave all the time.\n"
"    --retcode        : Testbench's return code is the return code written to\n"
"                       IO_EXIT by the CPU, or -1 if timed out.\n"
;

void exit_help(std::string errtext = "")
{
	std::cerr << errtext << help_str;
	exit(-1);
}

int main(int argc, char **argv, char **env)
{
    bool dump_waves = false;
    bool load_bin = false;
    bool dump_all = false;
	std::string waves_path;
    std::string bin_path;
    int64_t max_cycles = 0;
    uint32_t binaddr = 0;
    uint16_t port = 0;
    bool propagate_return_code = false;
    uint32_t i, j;

    // 参数解析
    for (int i = 1; i < argc; ++i) {
        //printf("argv[%d]=%s\n", i, argv[i]);
        std::string s(argv[i]);
        if ((s.rfind("--", 0) != 0) && (s.rfind("+", 0) != 0)) {
            std::cerr << "Unexpected positional argument " << s << "\n";
            exit_help("");
        } else if (s == "--bin") {
            if (argc - i < 2)
                exit_help("Option --bin requires an argument\n");
            load_bin = true;
            bin_path = argv[i + 1];
            i += 1;
        } else if (s == "--binaddr") {
            if (argc - i < 2)
                exit_help("Option --binaddr requires an argument\n");
            binaddr = std::stol(argv[i + 1], 0, 0);
            i += 1;
        } else if (s == "--vcd") {
            if (argc - i < 2)
                exit_help("Option --vcd requires an argument\n");
            dump_waves = true;
            waves_path = argv[i + 1];
            i += 1;
        } else if (s == "--cycles") {
            if (argc - i < 2)
                exit_help("Option --cycles requires an argument\n");
            max_cycles = std::stol(argv[i + 1], 0, 0);
            i += 1;
		} else if (s == "--port") {
            if (argc - i < 2)
                exit_help("Option --port requires an argument\n");
            port = std::stol(argv[i + 1], 0, 0);
            i += 1;
		} else if (s == "--retcode") {
            propagate_return_code = true;
		} else if (s == "--dumpall") {
            dump_all = true;
        } else {
            std::cerr << "Unrecognised argument " << s << "\n";
            exit_help("");
        }
    }

    // 检查bin addr范围
    if (binaddr >= FLASH_START && binaddr <= FLASH_END) {
        ;
    } else if (binaddr >= IRAM_START && binaddr <= IRAM_END) {
        ;
    } else if (binaddr >= DRAM_START && binaddr <= DRAM_END) {
        ;
    } else {
        fprintf(stderr, "bin addr error!!!\n");
        exit(-1);
    }

	cxxrtl_design::p_tb__top top;

	std::ofstream waves_fd;
	cxxrtl::vcd_writer vcd;
    // 记录波形文件
    if (dump_waves) {
		waves_fd.open(waves_path);
		cxxrtl::debug_items all_debug_items;
		top.debug_info(&all_debug_items, /*scopes=*/nullptr, "");
		vcd.timescale(1, "us");
		vcd.add(all_debug_items);
    }

    // 加载固件
    if (load_bin) {
        std::ifstream fd(bin_path, std::ios::binary | std::ios::ate);
        if (!fd){
            std::cerr << "Failed to open \"" << bin_path << "\"\n";
            return -1;
        }
        std::streamsize bin_size = fd.tellg();
        if (bin_size > MEM_SIZE) {
            std::cerr << "Binary file (" << bin_size << " bytes) is larger than memory (" << MEM_SIZE << " bytes)\n";
            return -1;
        }
        fd.seekg(0, std::ios::beg);
        fd.read((char*)bin_memory, bin_size);
        printf("load bin: %s, size = %ld\n", bin_path.c_str(), bin_size);
        uint32_t word;
        for (i = 0, j = 0; i < bin_size; i += 4, j++) {
            word = bin_memory[i + 0] << 0;
            word += bin_memory[i + 1] << 8;
            word += bin_memory[i + 2] << 16;
            word += bin_memory[i + 3] << 24;
            #ifdef HAS_FLASH
                if (binaddr >= FLASH_START && binaddr <= FLASH_END)
                    top.memory_p_flash_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
                else if (binaddr >= IRAM_START && binaddr <= IRAM_END)
                    top.memory_p_u__fpga_2e_soc__u_2e_iram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
                else if (binaddr >= DRAM_START && binaddr <= DRAM_END)
                    top.memory_p_u__fpga_2e_soc__u_2e_dram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
                else
                    fprintf(stderr, "bin addr error!!!\n");
            #else
                if (binaddr >= IRAM_START && binaddr <= IRAM_END)
                    top.memory_p_u__fpga_2e_soc__u_2e_iram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
                else if (binaddr >= DRAM_START && binaddr <= DRAM_END)
                    top.memory_p_u__fpga_2e_soc__u_2e_dram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
                else
                    fprintf(stderr, "bin addr error!!!\n");
            #endif
        }
        word = 0;
        for (i = 0; i < bin_size % 4; i++) {
            word += bin_memory[(bin_size / 4) * 4 + i] << (i * 8);
        }
        #ifdef HAS_FLASH
            if (binaddr >= FLASH_START && binaddr <= FLASH_END)
                top.memory_p_flash_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
            else if (binaddr >= IRAM_START && binaddr <= IRAM_END)
                top.memory_p_u__fpga_2e_soc__u_2e_iram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
            else if (binaddr >= DRAM_START && binaddr <= DRAM_END)
                top.memory_p_u__fpga_2e_soc__u_2e_dram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
            else
                fprintf(stderr, "bin addr error!!!\n");
        #else
            if (binaddr >= IRAM_START && binaddr <= IRAM_END)
                top.memory_p_u__fpga_2e_soc__u_2e_iram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
            else if (binaddr >= DRAM_START && binaddr <= DRAM_END)
                top.memory_p_u__fpga_2e_soc__u_2e_dram_2e_sram_2e_behav__mem_2e_mem[j].set<uint32_t>(word);
            else
                fprintf(stderr, "bin addr error!!!\n");
        #endif
    }

	int server_fd, sock_fd;
	struct sockaddr_in sock_addr;
	int sock_opt = 1;
	socklen_t sock_addr_len = sizeof(sock_addr);
	char txbuf[TCP_BUF_SIZE], rxbuf[TCP_BUF_SIZE];
	int rx_ptr = 0, rx_remaining = 0, tx_ptr = 0;
    // openocd端号
	if (port != 0) {
		server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd == 0) {
			fprintf(stderr, "socket creation failed\n");
			exit(-1);
		}

		int setsockopt_rc = setsockopt(
			server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
			&sock_opt, sizeof(sock_opt)
		);

		if (setsockopt_rc) {
			fprintf(stderr, "setsockopt failed\n");
			exit(-1);
		}

		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = INADDR_ANY;
		sock_addr.sin_port = htons(port);
		if (bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
			fprintf(stderr, "bind failed\n");
			exit(-1);
		}

		sock_fd = wait_for_connection(server_fd, port, (struct sockaddr *)&sock_addr, &sock_addr_len);
	}

	top.step();
    top.p_rst__n.set<bool>(false);
	top.p_clk.set<bool>(true);
	top.p_tck.set<bool>(true);
	top.step();
	top.p_clk.set<bool>(false);
	top.p_tck.set<bool>(false);
    top.p_rst__n.set<bool>(true);
	top.step();
	top.step(); // workaround for github.com/YosysHQ/yosys/issues/2780

    printf("Sim started...\n");

	bool timed_out = false;
	for (int64_t cycle = 0; cycle < max_cycles || max_cycles == 0; ++cycle) {
		top.p_clk.set<bool>(false);
		top.step();
		if (dump_waves && (dump_all || top.p_dump__wave__en.get<uint8_t>()))
			vcd.sample(cycle * 2);
		top.p_clk.set<bool>(true);
		top.step();
		top.step(); // workaround for github.com/YosysHQ/yosys/issues/2780

		// If --port is specified, we run the simulator in lockstep with the
		// remote bitbang commands, to get more consistent simulation traces.
		// This slows down simulation quite a bit compared with normal
		// free-running.
		//
		// Most bitbang commands complete in one cycle (e.g. TCK/TMS/TDI
		// writes) but reads take 0 cycles, step=false.
		bool got_exit_cmd = false;
		bool step = false;
		if (port != 0) {
			while (!step) {
				if (rx_remaining > 0) {
					char c = rxbuf[rx_ptr++];
					--rx_remaining;

					if (c == 'r' || c == 's') {
						//top.p_trst__n.set<bool>(true);
						step = true;
                        printf("trst = 1\n");
					}
					else if (c == 't' || c == 'u') {
						//top.p_trst__n.set<bool>(false);
                        printf("trst = 0\n");
					}
					else if (c >= '0' && c <= '7') {
						int mask = c - '0';
						top.p_tck.set<bool>(mask & 0x4);
						top.p_tms.set<bool>(mask & 0x2);
						top.p_tdi.set<bool>(mask & 0x1);
						step = true;
					}
					else if (c == 'R') {
						txbuf[tx_ptr++] = top.p_tdo.get<bool>() ? '1' : '0';
						if (tx_ptr >= TCP_BUF_SIZE || rx_remaining == 0) {
							send(sock_fd, txbuf, tx_ptr, 0);
							tx_ptr = 0;
						}
					}
					else if (c == 'Q') {
						printf("OpenOCD sent quit command\n");
						got_exit_cmd = true;
						step = true;
					}
				}
				else {
					// Potentially the last command was not a read command, but
					// OpenOCD is still waiting for a last response from its
					// last command packet before it sends us any more, so now is
					// the time to flush TX.
					if (tx_ptr > 0) {
						send(sock_fd, txbuf, tx_ptr, 0);
						tx_ptr = 0;
					}	
					rx_ptr = 0;
					rx_remaining = read(sock_fd, &rxbuf, TCP_BUF_SIZE);
					if (rx_remaining == 0) {
						if (port == 0) {
							// Presumably EOF, so quit.
							got_exit_cmd = true;
						}
						else {
							// The socket is closed. Wait for another connection.
							sock_fd = wait_for_connection(server_fd, port, (struct sockaddr *)&sock_addr, &sock_addr_len);
						}
					}
				}
			}
		}

		if (dump_waves && (dump_all || top.p_dump__wave__en.get<uint8_t>())) {
			// The extra step() is just here to get the bus responses to line up nicely
			// in the VCD (hopefully is a quick update)
			top.step();
			vcd.sample(cycle * 2 + 1);
			waves_fd << vcd.buffer;
			vcd.buffer.clear();
		}

		if (cycle + 1 == max_cycles) {
			printf("Max cycles reached\n");
			timed_out = true;
		}
		if (got_exit_cmd)
			break;
    }

    if (sock_fd > 0)
        close(sock_fd);

/*
    std::ofstream mem_fd;
    mem_fd.open("mem.dump");
    if (!mem_fd.is_open()) {
        std::cerr << "Failed to open \"" << "mem.dump" << "\"\n";
        return -1;
    } else {
        for (i = 0; i < 5120 / 4; i++) {
            uint32_t tmp = top.memory_p_u__fpga_2e_soc__u_2e_sram0_2e_sram_2e_behav__mem_2e_mem[i].get<uint32_t>();
            mem_fd.write((char *)&tmp, 4);
        }
    }
*/

    return 0;
}
