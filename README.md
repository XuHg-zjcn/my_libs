# MCU_mylibs

#### 介绍
一个MCU C++跨平台驱动库，有一些片上资源和外部设备的驱动

### 目前支持列表
STM32F103C8T6
STM32F407ZET6
#### 计划支持
ESP8266
ESP32
CC2640
CC2652P
STM8S103F3P6

#### 软件架构
软件架构说明
大部分使用C++编写，调用了HAL库，使用了FreeRTOS

#### 功能
- 片上ADC
- 抽象数据Buffer
- I2C
- SSD1306 128x64液晶
- 28BYJ-48步进电机
- 超声波模块
- DHT11
- 数码管
- RTC时钟, USB连接电脑同步时间

#### 功能-计划
- 电源(电池)管理
- 步进电机PWM细分
- 舵机
- PID控制
- 加热器温度控制
- 4Pin风扇
- W25Qxx SPI Flash
- SPI/SDIO SD卡
- LED点阵
- BMP280
- MPU6050六轴/MPU9250九轴
- 38kHz红外
- SX1276/1278 LoRa
- 声波通信, FSO光通信, OLED传输数据


#### 使用说明

1.  打开STM32CubeIDE
2.  创建项目，适当配置外设，生成代码
3.  右键项目，点击Convert to C++
4.  复制本仓库到项目目录
5.  右键项目 -> Properties -> C/C++ General -> Paths and Symbols -> Includes -> 添加所需的头文件目录
    mcu_mylibs/commom/Inc
    mcu_mylibs/stm32/Inc
    mcu_mylibs/stm32f1/Inc (f4项目添加对应目录)
6.  右键项目 -> Properties -> C/C++ General -> Paths and Symbols -> Source Location -> 展开当前项目 -> Filter -> Edit Filter
    添加example, F1项目需添加*f4*, F4项目需添加*f1*
7.  编辑配置文件 mcu_mylibs/common/Inc/mylibs_config.hpp

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
