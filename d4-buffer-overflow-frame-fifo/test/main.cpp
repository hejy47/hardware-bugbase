#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
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

#include "Vaxis_fifo_wrapper.h"

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

void write_to_output(std::ofstream &outfile, Vaxis_fifo_wrapper *tb) {
    outfile << static_cast<int>(tb->rst) << ",";
    outfile << static_cast<int>(tb->s_axis_tdata) << ",";
    outfile << static_cast<int>(tb->s_axis_tvalid) << ",";
    outfile << static_cast<int>(tb->s_axis_tready) << ",";
    outfile << static_cast<int>(tb->s_axis_tlast) << ",";
    outfile << static_cast<int>(tb->s_axis_tuser) << ",";
    outfile << static_cast<int>(tb->m_axis_tdata) << ",";
    outfile << static_cast<int>(tb->m_axis_tvalid) << ",";
    outfile << static_cast<int>(tb->m_axis_tready) << ",";
    outfile << static_cast<int>(tb->m_axis_tlast) << ",";
    outfile << static_cast<int>(tb->m_axis_tuser) << ",";
    outfile << static_cast<int>(tb->status_overflow) << ",";
    outfile << static_cast<int>(tb->status_bad_frame) << ",";
    outfile << static_cast<int>(tb->status_good_frame) << "\n";
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vaxis_fifo_wrapper *tb = new Vaxis_fifo_wrapper;

    // init file
    std::ofstream signal_file("output_d4_frame_fifo.txt");
    std::string signal_names = "rst, s_axis_tdata, s_axis_tvalid, s_axis_tready, s_axis_tlast, s_axis_tuser, m_axis_tdata, m_axis_tvalid, m_axis_tready, m_axis_tlast, m_axis_tuser, status_overflow, status_bad_frame, status_good_frame";
    signal_file << signal_names << std::endl;

    Verilated::traceEverOn(true);
    VerilatedVcdC *trace = new VerilatedVcdC;
    tb->trace(trace, 99);
    trace->open("axis_fifo.vcd");

    if (setjmp(jmp_env) == 0) {
        signal(SIGABRT, &sig_handler);
        signal(SIGINT, &sig_handler);
    } else {
        goto save_trace_and_exit;
    }

    tb->rst = 1;
    tb->s_axis_tdata = 0;
    tb->s_axis_tvalid = 0;
    tb->s_axis_tlast = 0;
    tb->s_axis_tuser = 0;
    tb->m_axis_tready = 0;
    tb->eval();

    for (int i = 0; i < 10; i++) {
        tb->clk = 1;
        tb->eval();
        write_to_output(signal_file, tb);
        trace->dump(timestamp);
        sc_time_step();
        tb->clk = 0;
        tb->eval();
        trace->dump(timestamp);
        sc_time_step();
    }

    tb->rst = 0;
    tb->eval();
    for (int i = 0; i < 5; i++) {
        tb->clk = 1;
        tb->eval();
        write_to_output(signal_file, tb);
        trace->dump(timestamp);
        sc_time_step();
        tb->clk = 0;
        tb->eval();
        trace->dump(timestamp);
        sc_time_step();
    }

    for (int k = 0; k < 10; k++) {
        for (int i = 0; i < 10; i++) {
            tb->s_axis_tdata = i;
            tb->s_axis_tvalid = 1;
            tb->s_axis_tlast = 0;
            tb->s_axis_tuser = 0;
            tb->eval();

            tb->clk = 1;
            tb->eval();
            write_to_output(signal_file, tb);
            trace->dump(timestamp);
            sc_time_step();
            tb->clk = 0;
            tb->eval();
            trace->dump(timestamp);
            sc_time_step();
        }

        tb->s_axis_tdata = 233;
        tb->s_axis_tvalid = 1;
        tb->s_axis_tlast = 1;
        tb->s_axis_tuser = 0;
        tb->eval();

        tb->clk = 1;
        tb->eval();
        write_to_output(signal_file, tb);
        trace->dump(timestamp);
        sc_time_step();
        tb->clk = 0;
        tb->eval();
        trace->dump(timestamp);
        sc_time_step();

        tb->s_axis_tdata = 0;
        tb->s_axis_tvalid = 0;
        tb->s_axis_tlast = 0;
        tb->s_axis_tuser = 0;
        tb->eval();
        for (int i = 0; i < 5; i++) {
            tb->clk = 1;
            tb->eval();
            write_to_output(signal_file, tb);
            trace->dump(timestamp);
            sc_time_step();
            tb->clk = 0;
            tb->eval();
            trace->dump(timestamp);
            sc_time_step();
        }
    }

save_trace_and_exit:

    trace->flush();
    trace->close();

    signal_file.flush();
    signal_file.close();

    exit(EXIT_SUCCESS);
}
