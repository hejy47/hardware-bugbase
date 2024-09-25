#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cassert>
#include <verilated_vcd_c.h>
#include <verilated.h>
#include <queue>
#include <map>
#include <tuple>
#include <exception>
#include <csetjmp>
#include <csignal>

#include "Vtestbench.h"

using namespace std;


jmp_buf jmp_env;
void sig_handler(int signum) {
    longjmp(jmp_env, 1);
}

const uint64_t psc = 2500; // 2500 ps per cycle
const uint64_t pst = 1250; // 1250 ps per tick (half cycle)
uint64_t timestamp = 0;
double sc_time_stamp() {
    return timestamp;
}
void sc_time_step() {
    timestamp += pst;
}

void write_to_output(std::ofstream &outfile, Vtestbench *tb) {
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_AWVALID) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_ARESETN) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_WSTRB) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_WVALID) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_ARPROT) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_AWPROT) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_RREADY) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_ARADDR) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_ARVALID) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_AWADDR) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_WDATA) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_S_AXI_BREADY) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_awready) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_wready) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_rvalid) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_arready) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_bresp) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_bvalid) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_rdata) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axi_rresp) << ",\n";
}


int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vtestbench *tb = new Vtestbench;

    std::ofstream signal_file("output_s1_axi_lite.txt");
    std::string signal_names = "S_AXI_AWVALID, S_AXI_ARESETN, S_AXI_WSTRB, S_AXI_WVALID, S_AXI_ARPROT, S_AXI_AWPROT, S_AXI_RREADY, S_AXI_ARADDR, S_AXI_ARVALID, S_AXI_AWADDR, S_AXI_WDATA, S_AXI_BREADY, S_AXI_AWREADY, S_AXI_WREADY, S_AXI_RVALID, S_AXI_ARREADY, S_AXI_BRESP, S_AXI_BVALID, S_AXI_RDATA, S_AXI_RRESP";
    signal_file << signal_names << std::endl;

    Verilated::traceEverOn(true);
    VerilatedVcdC *trace = new VerilatedVcdC;
    tb->trace(trace, 99);
    string waveform = string(argv[0])+".vcd";
    trace->open(waveform.c_str());

    if (setjmp(jmp_env) == 0) {
        signal(SIGABRT, &sig_handler);
        signal(SIGINT, &sig_handler);
    } else {
        goto save_trace_and_exit;
    }

    while (!Verilated::gotFinish()) {
        tb->clock = 1;
        tb->eval();
        write_to_output(signal_file, tb);
        trace->dump(timestamp);
        sc_time_step();

        tb->clock = 0;
        tb->eval();
        trace->dump(timestamp);
        sc_time_step();
    }

save_trace_and_exit:

    trace->flush();
    trace->close();

    signal_file.flush();
    signal_file.close();

    exit(EXIT_SUCCESS);
}
