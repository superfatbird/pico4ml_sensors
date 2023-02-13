# 初识ArduCAM的pico4ml开发板
## 概述
ArduCAM pico4ml是arduCAM团队为机器学习相关的应用提供的一个开发板。关于机器学习的部分可以参见ArduCAM团队的原始fork，这个fork的目的是学习和使用开发板上已经预先装配好的各种附属器件（相机、屏幕、IMU、mic）。

开发板上一个主要的附属器件就是图像传感器，型号HM01B0。这个传感器主打的特点是低功耗，工作时的功率可以降低到4mw以下。 根据数据手册上的描述，这个传感器集成了硬件的运动探测单元（motion detection circuit），通过设置region of interest 和探测阈值可以实现运动检测并提供一个信号输出（为其他的控制提供中断用）。 为了控制相机的行为，需要对相机的寄存器进行访问。这个访问是通过I2C进行的。传感器本身提供了几种图像数据输出的方式， 1bit，4bits，8bits，在这个开发板上用的是1bit的方式进行的，图像数据传输的pin是CAM_D0。相机的数据手册可以在sparkfun上下载到，是2019版的，不是很全面，缺少了对寄存器描述的章节。 在Ralph Yamamoto分享的关于arducam hm01b0的帖子的回复里面看到了一个链接，找到了一个内容比较全面的2016版本的数据手册。

传感器和RP2040之间的接线情况参考开发板的电路原理图，原理图中间有一个叫做PICO_GPIO的部分，描述了板上的附属器件和单片机的连接情况。Pico4ml这个开发板和Raspberry Pi的pico开发板的pinout是相同的，用的flash也是相同的，可以把这个开发板理解成在pico的基础上添加了相机、屏幕、IMU和麦克风四个配件。配件的datasheet以及开发板的原理图见archive目录。 
ArduCAM在github上提供的例程主要在https://github.com/ArduCAM/pico-tflmicro 这个repo里面。其中tfl应该是tensor flow的缩写。这个repo当中的include文件夹是各个附属器件的驱动库，每一个驱动库里面的example文件夹下面有一个.ino文件，是可以在arduino IDE下编译使用的。这个.ino的例程不一定能直接跑通，可以看懂了代码以后进行调整，进而用来测试使用。想要在Arduino的IDE下使用这些库的话，需要把repo里面的include里面的内容放到本地的arduino的libraries的文件夹下面，为编译提供必要的驱动文件。

用Arduino的IDE给pico烧录程序，需要添加其IDE支持的开发板，这里用rpi-pico的开发板就可以。 添加这个开发板到IDE有两个方式：一个是可以添加arduino官方的Arduino Mbed OS PR2040，另一个是添加Earl Philhower的社区core，添加完成后，就可以在Arduio IDE的tool里面选择对应的board和port了， 具体的烧录方法见最后面一节。
## 文件描述
目前在arduino_firmware_example文件夹中有三个固件，两个用来演示相机，一个用来演示LCD的显示。
### 固件和PC端软件配合
我在arduino_firmware_example/py_hm01b_camera中的逻辑是让该固件盯着USB串口上的buffer，如果buffer里面有内容，就把buffer读空，然后调动相机传感器抓一张图像，再通过usb串口发回去。software_on_pc/serial.py可以与这个固件配合使用来读取串口上穿回来的每个pixel上的灰度值，再绘制成一幅图像。这个简单的python脚本，使用了pyserial的库和PIL的库（pillow）。这个脚本会打开RP2040模拟的usb串口，发一个字符到串口，然后再读usb串口上的数据，关闭串口，把读回来的数据绘制成图片。读回来的数据是一串8位的灰度数值，传感器每一个pixel上的灰度数值，一个一个的发回来，因为知道有多少个像素，也知道像素是96x96的方式排列的，就直接调用Image.frombytes()的函数绘制图像。 脚本既可以再Spyder里面运行，也可以直接在shell里面运行。

另外在arduino_firmware_example/processing_preview_firmware中测试了Ralph Yamamoto的固件和processing代码，做了简单的修改，修改了应该包含的库的名字和串口的名字，程序可以正常运行。 与PC端的processing脚本software_on_pc/preview_hm01b0配合使用。

### 关于相机库函数的一些说明：
相机配置结构的声明在arducam_hm01b0.h里面，具体配置的参数在例程.ino里面
相机接线的pin的定义在arducam_hm01b0.c里面; 
具体函数的实现也在这个.c文件里面，其中有一个要注意的是arducam_init()函数用的输入是结构指针，如何引用结构里面的成员或者如何用结构指针来引用成员参见K&R的131页。

Pico4ml上硬件配置情况：
* 主控芯片RP2040
* I2C控制的（SCCB的）相机， HiMax HM01B0, Up to QVGA (320 x 240 @60fps)， SCCB就是一种I2C, serial camera control bus….GPIO3到6、GPIO19到21是和相机模块连接的，也是I2C0的默认GPIO
* I2C控制的IMU，型号ICM20948，  在电路图上看不出IMU是和那个连接的， 在ArduCAM的github的repo （ pico-tflmicro/include/ICM20948/ICM20948.cpp）里面，可以看到定义的I2C连接是连接是I2C0上的。所以IMU和camera应该是不能同时通过I2C0去控制的。 
* SPI的LCD屏幕，0.96 inch LCD SPI Display (160 x 80, ST7735)，GPIO9到13是和LCD连接的
* I2S的麦克， GPIO31到34是和MIC连接的。 Mic的部分有两个IC组成，一个是UCTRONICS-MIC，输出PDM（pulse density modulation），另一个是ADI的ADAU7002，把PDM的信号转成I2S，这个I2S信号连接在RP2040上。
* QSPI的2M的flash， W25Q16JVUXIQ （这个和RPI-pico相同）

## 用Arduino IDE 烧录固件到raspberry pi pico
条件一：
烧录固件的时候要让board进入bootload的状态（具体怎么进入这个状态和每个board的设定相关，需要参考board生产商的资料，pico的话，就是在给board上电之前按住boot的按钮，上电后松开按钮），这时候会在电脑上显示成为一个u盘的状态，u盘的名字是RPI-RP2。

条件二：
在Arduino IDE里面选正确的tool->board里面选正确的board：pico
在tool->port里面选UF2_Board.
两个条件都满足的情况下，点IDE上烧录按钮就可以实现编译和烧录了。

额外的话：
用PICO的时候是不是有USB串口（ubuntu上的ttyACM0）是由pico里面的固件决定的，如果固件控制RP2040模拟了一个USB串口，就可以在电脑上看到一个串口，否则就看不到。pico不是用usb串口来烧录固件的，是通过uf2的bootloader来烧录固件的，和uno不同。Uno是用串口烧录固件的，UNO的usb串口是用FTDI或者ATmeg16u模拟出来的。

RPI-2040 用的是兼容uf2固件的bootloader
关于条件一，如何添加pico board到Arduino IDE里面可以看看社区的工具（https://www.tomshardware.com/how-to/program-raspberry-pi-pico-with-arduino-ide ）或者用arduino官方的Mbed OS 来添加pico board的支持

## 参考资料：

1. 开发版的电路原理图
https://www.arducam.com/downloads/Arducam-Pico4ML-board-Shematics-UC-798_SCH.pdf
2. ArduCAM的pico4ml的repo
https://github.com/ArduCAM/pico-tflmicro 
3. Rp2040开发参考文档都可以通过这个入口找到
https://raspberrypi.github.io/pico-sdk-doxygen/index.html 
4. HM01B0的数据手册
https://www.uctronics.com/download/Datasheet/HM01B0-MWA-image-sensor-datasheet.pdf 
5. Yamamoto的帖子
https://community.element14.com/members-area/personalblogs/b/ralph-yamamoto-s-blog/posts/camera-module-for-raspberry-pi-pico 
6.Python的pyserial的库
https://pyserial.readthedocs.io/en/latest/pyserial_api.html 
7. Python的PIL(pillow)的库
https://pillow.readthedocs.io/en/stable/reference/Image.html#PIL.Image.Image.frombytes 
8. Arduino pico core的serial相关的文档，哪个是usb serial就是从这里知道的
https://arduino-pico.readthedocs.io/en/latest/serial.html 
9. Arduion RP2040 社区core
https://github.com/earlephilhower/arduino-pico
10. pico4ml板上集成的flash芯片
https://www.digikey.com/en/products/detail/winbond-electronics/W25Q16JVUXIQ-TR/15182017 
11. HM01B0的一个驱动，可以参考一下
https://github.com/mjs513/TMM-HB01B0-Camera/blob/main/TMM-HM01B0/src/HM01B0.cpp 
12. 关于HM01B0的报道：台湾的半导体设计商Himax（奇景光电旗下恒景科技）新产品
http://www.mems.me/mems/product_201601/2748.html 
