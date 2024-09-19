这是Digilent的Cmod A7开发板（见下图）的FPGA工程，使用的是Vivado开发环境（非工程模式）。

![board](./docs/board.png)

- constrs：包含约束文件
- dosc：主要包括板子的原理图
- scripts：tcl脚本

fpga_cmod_a7_top.v是整个工程的顶层文件。

在本目下执行make即可在out目录下生成fpga_cmod_a7_top.bit文件。



