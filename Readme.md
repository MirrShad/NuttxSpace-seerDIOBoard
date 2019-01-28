# 项目情况：
    已经能在短时间内跑任务链，但是因为没有添加陀螺仪自动标零逻辑，所以长时间跑任务链会造成陀螺仪漂移。

# 编译方法：
    mkdir nuttxspace
    git clone https://github.com/MirrShad/NuttxSpace-seerDIOBoard.git seerDIOBoard
    git clone https://github.com/MirrShad/NuttxSpace-nuttx.git nuttx
    git clone https://bitbucket.org/nuttx/uclibc.git uclibc
    cd uclibc
    ./install.sh ../nuttx
    cd ..
    cd nuttx
    make distclean
    cd tools
    ./configure.sh seerdioboard/cxxtest
    cd ..
    make menuconfig
        Build Setup
        Build Configuration
        Application directory
            ../seerDIOBoard
    make

# 对应rbk：
    https://www.teambition.com/project/5a279095601d190f35e3a4be/works/5c4e7a5a11870f0017d6c01f/work/5c4e7a8d61b2020017a68b29

# dspchassis：
    去掉了每2s心跳包查询 
    往上报的陀螺仪和里程计的报文里面添加时间戳

# 使用硬件：
    SRC2000 1.2.6

# 使用车体：
    井智顶升车

# 调试手段：
    将COM2接到电脑上，然后以115200的波特率运行友善串口调试助手输出字符串。

# 框架结构：
    |-nuttx
        |-drivers硬件资源的接口的实现，会去调用相应的board.h来实现接口
            |-can
                |-seer-can.c
            |-leds
                |-heartled.c
        |-include上面drivers的接口就放在这个文件夹下
            |-nuttx
                |-can
                    seer-can.h
                |-leds
                    heartled.h
        |-configs
            |-seerdioboard
                |-src
                    |-stm32_bringup.c这里是自定义硬件接口初始化的地方
                    |-hal_can.c 这里是调用标准库的地方
                    |-hal_gpio.c 我只实现了can和gpio来熟悉nuttx的架构，剩下的为了赶时间都用的是nuttx自带的hal库
    |-seerdioboard
        

