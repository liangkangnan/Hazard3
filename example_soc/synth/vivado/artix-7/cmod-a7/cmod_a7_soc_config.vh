localparam RESET_VECTOR        = 32'h0000_0040;
localparam MTVEC_INIT          = 32'h0000_0000;
localparam EXTENSION_A         = 1;
localparam EXTENSION_C         = 1;
localparam EXTENSION_M         = 1;
localparam EXTENSION_ZBA       = 1;
localparam EXTENSION_ZBB       = 1;
localparam EXTENSION_ZBC       = 0;
localparam EXTENSION_ZBS       = 1;
localparam EXTENSION_ZBKB      = 1;
localparam EXTENSION_ZCB       = 1;
localparam EXTENSION_ZCMP      = 1;
localparam EXTENSION_ZIFENCEI  = 1;
localparam EXTENSION_XH3BEXTM  = 1;
localparam EXTENSION_XH3IRQ    = 1;
localparam EXTENSION_XH3PMPM   = 0;
localparam EXTENSION_XH3POWER  = 0;
localparam CSR_M_MANDATORY     = 1;
localparam CSR_M_TRAP          = 1;
localparam CSR_COUNTER         = 1;
localparam U_MODE              = 0;
localparam PMP_REGIONS         = 0;
localparam PMP_GRAIN           = 0;
localparam PMP_HARDWIRED       = {(PMP_REGIONS > 0 ? PMP_REGIONS : 1){1'b0}};
localparam PMP_HARDWIRED_ADDR  = {(PMP_REGIONS > 0 ? PMP_REGIONS : 1){32'h0}};
localparam PMP_HARDWIRED_CFG   = {(PMP_REGIONS > 0 ? PMP_REGIONS : 1){8'h00}};
localparam DEBUG_SUPPORT       = 1;
localparam BREAKPOINT_TRIGGERS = 4;
localparam NUM_IRQS            = 4;
localparam IRQ_PRIORITY_BITS   = 4;
localparam IRQ_INPUT_BYPASS    = {(NUM_IRQS > 0 ? NUM_IRQS : 1){1'b0}};
localparam MVENDORID_VAL       = 32'hdeadbeef;
localparam MIMPID_VAL          = 32'h12345678;
localparam MHARTID_VAL         = 32'h0;
localparam MCONFIGPTR_VAL      = 32'h0;
localparam REDUCED_BYPASS      = 0;
localparam MULDIV_UNROLL       = 2;
localparam MUL_FAST            = 1;
localparam MUL_FASTER          = 1;
localparam MULH_FAST           = 1;
localparam FAST_BRANCHCMP      = 1;
localparam RESET_REGFILE       = 0;
localparam BRANCH_PREDICTOR    = 1;
localparam MTVEC_WMASK         = 32'hfffffffd;
