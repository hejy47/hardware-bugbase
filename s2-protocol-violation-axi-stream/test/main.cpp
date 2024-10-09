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
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_M_AXIS_ARESETN) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__PI_M_AXIS_TREADY) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axis_tvalid) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__stream_data_out) << ", ";
  outfile << static_cast<uint64_t>(15) << ", ";
  outfile << static_cast<uint64_t>(tb->testbench__DOT__UUT__DOT__axis_tlast_delay) << ",\n";
}


int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vtestbench *tb = new Vtestbench;

    std::ofstream signal_file("output_s2_axi_stream.txt");
    std::string signal_names = "M_AXIS_ARESETN, M_AXIS_TREADY, M_AXIS_TVALID, M_AXIS_TDATA, M_AXIS_TSTRB, M_AXIS_TLAST";
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
