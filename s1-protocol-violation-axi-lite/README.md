# S1 - Protocol Violation - AXI-Lite

Source: https://zipcpu.com/formal/2018/12/28/axilite.html

This bug is from Xilinx's official demostrative AXI-Lite implementation provided in Vivado. It's first found by [ZipCPU](https://zipcpu.com), who provides a detailed description and analysis in the link above.

`sources_bug_s1b` and `sources_bug_s1r` contain buggy source files.
`source_oracle_tb0/1` contain the oracle source files and the corresponding testbench.

```bash
make
./source_oracle_tb0_test
./source_oracle_tb1_test
```