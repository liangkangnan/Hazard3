本目录是一些FPGA上运行的例程（example_soc/synth/vivado/artix-7工程），同时也可以用于仿真，

仿真时使用test/sim/tb_yosys仿真器，比如要对cmoda7-2p工程仿真，则使用命令：

```
make -f Makefile.cmod_a7_2p
```

生成。

然后，如果要对hello_world程序进行仿真，则在hello_world目录下编译程序：

```
make RUN=IRAM
```

RUN=IRAM表示在IRAM中运行。

接着运行如下命令开始仿真：

```
../../sim/tb_yosys/tb_cmod_a7_2p_top --bin hello_world.bin --binaddr 0x10000000
```

其中bootrom、common、drivers目录是公共代码。