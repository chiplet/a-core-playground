// DESCRIPTION: Verilator: Verilog example module
//
// This file ONLY is placed under the Creative Commons Public Domain, for
// any use, without warranty, 2017 by Wilson Snyder.
// SPDX-License-Identifier: CC0-1.0
//======================================================================

#include <memory>
#include <fstream>
#include <cstdint>
#include <string>
#include <sstream>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vacorechip.h"
#include "Vacorechip___024root.h"
// #include "vga.hpp"
#include "sim_display.hpp"
#include "remote_bitbang.hpp"
#include "SDL2/SDL.h"

// #define PRINT_EXECUTION_TRACE

int asd = 0;

// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }


// AI slop RISC-V disassembler, let's see if anything useful comes out :)
std::string disassemble(uint32_t inst) {
    static const char* regNames[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
        "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    uint32_t opcode = inst & 0x7F;
    uint32_t rd = (inst >> 7) & 0x1F;
    uint32_t funct3 = (inst >> 12) & 0x7;
    uint32_t rs1 = (inst >> 15) & 0x1F;
    uint32_t rs2 = (inst >> 20) & 0x1F;
    uint32_t funct7 = (inst >> 25) & 0x7F;
    int32_t imm = static_cast<int32_t>(inst) >> 20; // Sign-extended immediate
    std::ostringstream result;

    if (opcode == 0x33) { // R-type
        if (funct7 == 0x00) {
            if (funct3 == 0x00) result << "add    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x01) result << "sll    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x02) result << "slt    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x03) result << "sltu   " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x04) result << "xor    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x05) result << "srl    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x06) result << "or     " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x07) result << "and    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
        } else if (funct7 == 0x20) {
            if (funct3 == 0x00) result << "sub    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
            else if (funct3 == 0x05) result << "sra    " << regNames[rd] << "," << regNames[rs1] << "," << regNames[rs2];
        }
    } else if (opcode == 0x13) { // I-type
        if (funct3 == 0x00) result << "addi   " << regNames[rd] << "," << regNames[rs1] << "," << imm;
        else if (funct3 == 0x02) result << "slti   " << regNames[rd] << "," << regNames[rs1] << "," << imm;
        else if (funct3 == 0x03) result << "sltiu  " << regNames[rd] << "," << regNames[rs1] << "," << imm;
        else if (funct3 == 0x04) result << "xori   " << regNames[rd] << "," << regNames[rs1] << "," << imm;
        else if (funct3 == 0x06) result << "ori    " << regNames[rd] << "," << regNames[rs1] << "," << imm;
        else if (funct3 == 0x07) result << "andi   " << regNames[rd] << "," << regNames[rs1] << "," << imm;
        else if (funct3 == 0x01) result << "slli   " << regNames[rd] << "," << regNames[rs1] << "," << (imm & 0x1F);
        else if (funct3 == 0x05) {
            if ((imm & 0x400) == 0) result << "srli   " << regNames[rd] << "," << regNames[rs1] << "," << (imm & 0x1F);
            else result << "srai   " << regNames[rd] << "," << regNames[rs1] << "," << (imm & 0x1F);
        }
    } else if (opcode == 0x03) { // Load
        if (funct3 == 0x00) result << "lb     " << regNames[rd] << "," << imm << "(" << regNames[rs1] << ")";
        else if (funct3 == 0x01) result << "lh     " << regNames[rd] << "," << imm << "(" << regNames[rs1] << ")";
        else if (funct3 == 0x02) result << "lw     " << regNames[rd] << "," << imm << "(" << regNames[rs1] << ")";
        else if (funct3 == 0x04) result << "lbu    " << regNames[rd] << "," << imm << "(" << regNames[rs1] << ")";
        else if (funct3 == 0x05) result << "lhu    " << regNames[rd] << "," << imm << "(" << regNames[rs1] << ")";
    } else if (opcode == 0x23) { // Store
        int32_t imm_s = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
        if (funct3 == 0x00) result << "sb     " << regNames[rs2] << "," << imm_s << "(" << regNames[rs1] << ")";
        else if (funct3 == 0x01) result << "sh     " << regNames[rs2] << "," << imm_s << "(" << regNames[rs1] << ")";
        else if (funct3 == 0x02) result << "sw     " << regNames[rs2] << "," << imm_s << "(" << regNames[rs1] << ")";
    } else if (opcode == 0x63) { // Branch
        int32_t imm_b = ((inst >> 31) << 12) | (((inst >> 7) & 0x1) << 11) |
                        (((inst >> 25) & 0x3F) << 5) | (((inst >> 8) & 0xF) << 1);
        imm_b = (imm_b << 19) >> 19; // Sign-extend to 32 bits
        if (funct3 == 0x00) result << "beq    " << regNames[rs1] << "," << regNames[rs2] << "," << imm_b;
        else if (funct3 == 0x01) result << "bne    " << regNames[rs1] << "," << regNames[rs2] << "," << imm_b;
        else if (funct3 == 0x04) result << "blt    " << regNames[rs1] << "," << regNames[rs2] << "," << imm_b;
        else if (funct3 == 0x05) result << "bge    " << regNames[rs1] << "," << regNames[rs2] << "," << imm_b;
        else if (funct3 == 0x06) result << "bltu   " << regNames[rs1] << "," << regNames[rs2] << "," << imm_b;
        else if (funct3 == 0x07) result << "bgeu   " << regNames[rs1] << "," << regNames[rs2] << "," << imm_b;
    } else if (opcode == 0x6F) { // JAL
        int32_t imm_j = ((inst >> 31) << 20) | (((inst >> 12) & 0xFF) << 12) |
                        (((inst >> 20) & 0x1) << 11) | (((inst >> 21) & 0x3FF) << 1);
        result << "jal    " << regNames[rd] << "," << imm_j;
    } else if (opcode == 0x67) { // JALR
        result << "jalr   " << regNames[rd] << "," << imm << "(" << regNames[rs1] << ")";
    } else if (opcode == 0x37) { // LUI
        result << "lui    " << regNames[rd] << "," << (inst & 0xFFFFF000);
    } else if (opcode == 0x17) { // AUIPC
        result << "auipc  " << regNames[rd] << "," << (inst & 0xFFFFF000);
    } else {
        result << "unknown";
    }
    return result.str();
}

// more AI slop, this one looks pretty reasonable tbh
const char* getExceptionCause(uint8_t mcause) {
    switch (mcause) {
        case 0:  return "INSTR_ADDR_MISALIG";
        case 1:  return "INSTR_ACCESS_FAULT";
        case 2:  return "ILLEGAL_INSTR";
        case 3:  return "BREAKPOINT";
        case 4:  return "LOAD_ADDR_MISALIG";
        case 5:  return "LOAD_ACCESS_FAULT";
        case 6:  return "STORE_ADDR_MISALIG";
        case 7:  return "STORE_ACCESS_FAULT";
        case 11: return "ENV_CALL_FROM_M";
        default: return "UNKNOWN_CAUSE";
    }
}

// this is organic human-made slop!
void printRegs(const std::unique_ptr<Vacorechip>& top) {
    uint32_t* regfile[32] = {
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_0),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_1),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_2),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_3),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_4),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_5),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_6),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_7),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_8),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_9),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_10),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_11),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_12),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_13),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_14),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_15),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_16),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_17),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_18),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_19),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_20),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_21),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_22),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_23),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_24),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_25),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_26),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_27),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_28),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_29),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_30),
        (uint32_t*)(&top->rootp->acorechip__DOT__core__DOT__regfile_block__DOT__regfile__DOT__x_31)
    };
    printf("REGISTER DUMP\n");
    for (int i = 0; i < 32; i+=4) {
        printf("x%d%s = 0x%08x    ", i+0, i+0<10 ? " " : "", *(regfile[i+0]));
        printf("x%d%s = 0x%08x    ", i+1, i+1<10 ? " " : "", *(regfile[i+1]));
        printf("x%d%s = 0x%08x    ", i+2, i+2<10 ? " " : "", *(regfile[i+2]));
        printf("x%d%s = 0x%08x\n",   i+3, i+3<10 ? " " : "", *(regfile[i+3]));
    }
}

int main(int argc, char** argv, char** env) {

    // Prevent unused variable warnings
    if (false && argc && argv && env) {}

    // FIXME: properly parse and validate that a valid filepath is passed in
    // instead of assuming first cli arg is a valid filepath
    char* program_filepath = argv[1];
    printf("loading program: %s\n", program_filepath);

    // read file bytes to vector
    std::ifstream file(program_filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Could not open file " << program_filepath << "\n";
        return 1;
    }
    std::streamsize fileSize = file.tellg();
    std::vector<uint8_t> program_bytes(fileSize);
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(program_bytes.data()), fileSize)) {
        std::cerr << "Error: Could not read file " << program_filepath << "\n";
        return 1;
    }
    file.close();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");

    // Construct a VerilatedContext to hold simulation time, etc.
    // Multiple modules (made later below with Vtop) may share the same
    // context to share time, or modules may have different contexts if
    // they should be independent from each other.

    // Using unique_ptr is similar to
    // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    // Do not instead make Vtop as a file-scope static variable, as the
    // "C++ static initialization order fiasco" may cause a crash

    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs argument parsing
    contextp->randReset(2);

    // Verilator must compute traced signals
    contextp->traceEverOn(true);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v".
    // Using unique_ptr is similar to "Vtop* top = new Vtop" then deleting at end.
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vacorechip> top{new Vacorechip{contextp.get(), "TOP"}};

    // write program binary to program memory banks
    uint8_t* banks[4] = {
        (uint8_t*)&top->rootp->acorechip__DOT__progmem__DOT__mem_array_0,
        (uint8_t*)&top->rootp->acorechip__DOT__progmem__DOT__mem_array_1,
        (uint8_t*)&top->rootp->acorechip__DOT__progmem__DOT__mem_array_2,
        (uint8_t*)&top->rootp->acorechip__DOT__progmem__DOT__mem_array_3
    };

    for (int i = 0; i < program_bytes.size(); i++) {
        uint32_t bank_addr = i >> 2;
        uint32_t bank_idx = i & 0b11;
        banks[bank_idx][bank_addr] = program_bytes[i];
        printf("pm[%d] = %d\n", i, program_bytes[i]); 
    }

    // Set Vtop's input signals
    top->reset = 0;
    top->clock = 0;

    SimDisplay display(320, 200, (uint8_t*)&top->rootp->acorechip__DOT__ram__DOT__mem_array_0);

    // const uint16_t port = 1234;
    // JtagRemoteBitbang jtag(
    //     port,
    //     &top->io_jtag_TCK,
    //     &top->io_jtag_TMS,
    //     &top->io_jtag_TDI,
    //     nullptr, // tdo
    //     &top->io_jtag_TRSTn
    // );

    uint32_t pc = 0x1000;
    uint32_t last_pc = 0x1000;
    uint64_t cnt = 0;

    // Simulate until $finish
    while (!contextp->gotFinish()) {
    // for (int i = 0; i < 10000000; i++) {
        // Historical note, before Verilator 4.200 Verilated::gotFinish()
        // was used above in place of contextp->gotFinish().
        // Most of the contextp-> calls can use Verilated:: calls instead;
        // the Verilated:: versions just assume there's a single context
        // being used (per thread).  It's faster and clearer to use the
        // newer contextp-> versions.

        contextp->timeInc(1);  // 1 timeprecision period passes...
        // Historical note, before Verilator 4.200 a sc_time_stamp()
        // function was required instead of using timeInc.  Once timeInc()
        // is called (with non-zero), the Verilated libraries assume the
        // new API, and sc_time_stamp() will no longer work.

        // Toggle a fast (time/2 period) clock
        top->clock = !top->clock;

        // Toggle control signals on an edge that doesn't correspond
        // to where the controls are sampled; in this example we do
        // this only on a negedge of clock, because we know
        // reset is not sampled there.
        if (!top->clock) {
            if (contextp->time() > 1 && contextp->time() < 10) {
                top->reset = 1;  // Assert reset
            } else {
                top->reset = 0;  // Deassert reset
                top->io_core_en = 1;
            }
        }

        // Evaluate model
        // (If you have multiple models being simulated in the same
        // timestep then instead of eval(), call eval_step() on each, then
        // eval_end_step() on each. See the manual.)
        top->eval();

        if (cnt++ % 10000 == 0) {
#ifdef PRINT_EXECUTION_TRACE            
            printf("----------RENDER FRAME----------\n");
#endif // PRINT_EXECUTION_TRACE
            display.render();
        }
        // if (contextp->time() % 4 == 0)
        //     jtag.tick();

#ifdef PRINT_EXECUTION_TRACE            
        last_pc = pc;
        pc = top->rootp->acorechip__DOT__core__DOT__ifd_id_reg_pc;
        uint32_t instr = top->rootp->acorechip__DOT__core__DOT__ifd_id_reg_ifetch_rdata;
        if (instr)
            printf("pc=0x%08x : instr=0x%08x : %s\n", pc, instr, disassemble(instr).c_str()
        );
#endif // PRINT_EXECUTION_TRACE

        // access screenbuffer array directly
        // top->ACoreChip->screenbuffer.data()[640*y+x]);

        // handle SDL2 events
        SDL_Event e;
        if (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

        // exit simulation if core runs into some issue
        if (cnt > 10 && top->io_core_fault) {
            printf("EXCEPTION\n");
            printf("mepc = 0x%08x\n", top->rootp->acorechip__DOT__core__DOT__memd_wb_reg_mepc);
            printf("mcause = 0x%08x : %s\n", top->rootp->acorechip__DOT__core__DOT__memd_wb_reg_mcause,
                getExceptionCause(top->rootp->acorechip__DOT__core__DOT__memd_wb_reg_mcause));
            break;
        }
    }
    printRegs(top);

    // Final model cleanup
    top->final();

    // Return good completion status
    // Don't use exit() or destructor won't get called
    return 0;
}
