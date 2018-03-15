# RISCV-Simulator

[![Build Status](https://travis-ci.org/LC-John/RISCV-Simulator.svg?branch=master)](https://travis-ci.org/LC-John/RISCV-Simulator)

This is a project of Computer Organization and Achitecture, PKU. LAB2 and LAB3.2 are included. I believe my solution is quite elegant and may sparks you.

The simulator simulates on RV64I and RV64F ISA. (There are still some bugs in RV64F, yet I'm not going to debug them.)

The specification of RISC-V ISA is available in this repo. Or you may retrieve the latest version from https://riscv.org/specifications/.

Find more details at https://github.com/LC-John/Notes-and-homeworks/tree/master/体系实习.

## License

MIT License

## Usage

### How to build

1. Simply download this repo by clicking the `DOWNLOAD` button or call `git git@github.com:LC-John/RISCV-Simulator.git` in command line.

2. Use QtCreator or other IDEs to open the `.pro` project file.

3. Build and run.

4. Have fun playing with this simulator.

### How to run

Use `sim -h` to get the help menu. Everything you need is in it.

## Architecture design

There are 5 main units (actually there exsists 4 units) in this simulator.

### Instruction Fetcher (IF) unit

This unit fetch an instruction from the instruction memory, and update PC. Then the unit split the instruction into several domains, such as `rd`, `rs1`, `rs2`, *etc*.

![image](https://github.com/LC-John/RISCV-Simulator/blob/master/images/1.png)

### Instruction Decoder (ID) unit

This unit takes the domains of instructions as input, and performs some decoding -- read the register file and generate an immediate number if necessary.

![image](https://github.com/LC-John/RISCV-Simulator/blob/master/images/2.png)

### Excutor (EX) unit

This unit perform some logical/arithmetic calculation, such as `logical and`, `add`, *etc*. Also this unit perform a comparison between two numbers, and the result is passed to the controller. Such results will be used by instructions such as `BEQ`, *etc*.

![image](https://github.com/LC-John/RISCV-Simulator/blob/master/images/3.png)

There is another extra Excutor unit, called **EX2**. During the process of multiplication and division, the time cost is usually more than one cycle. EX2 is activated when EX excutes such a time-costing instruction.

### Memory Reader/Writor (MEM) unit

This unit reads/writes a byte/half word/word/double word/*etc* to the data memory (main memory). It cannot read and write in the same time.

![image](https://github.com/LC-John/RISCV-Simulator/blob/master/images/4.png)

### Register File Writor (WB) unit

This unit write one register in the register file. The content has multiple sources.

![image](https://github.com/LC-John/RISCV-Simulator/blob/master/images/5.png)

### Cache/Memory unit

This unit is lately added. It simulates the data cache and the instruction cache.

### Pipeline architecture

This simulator has 3 modes -- single-cycle, multi-cycle and pipeline. single-cycle and multi-cycle mode are easy. The architecture of the pipelie mode is shown below.

![image](https://github.com/LC-John/RISCV-Simulator/blob/master/images/6.png)

## Examples

See the PDF reports at https://github.com/LC-John/Notes-and-homeworks/tree/master/体系实习.

## Notice

Last but the most important thing is that you can reference them, but **PLEASE DO NOT COPY**! If you copy this project and get caught, no one can help you, and you deserve nothing but a zero score. 

Heads up! I've inserted something into my code. It will somehow show the identification information of the true author, which is me. So anaing, **DO NOT COPY**.

Good luck and have fun. :)
