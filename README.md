# MCU_mylibs

#### 介绍
一个MCU C++跨平台驱动库，有一些片上资源和外部设备的驱动
项目即将改用陈述性名称, 跨平台嵌入式集成库CEILib(Cross-platform Embedded Integration Library)

### 支持列表
目前支持: STM32F103C8T6, STM32F407ZET6, ESP8266, CH32V103C8T6


#### 软件架构
软件架构说明
大部分使用C++编写，STM32版调用了HAL库，使用了FreeRTOS的CMSIS OS2接口
ESP8266使用了ESP-IDF框架


#### 硬件驱动库
##### 用户输出设备
- SSD1306 128x64液晶
- 数码管
##### 用户输入设备
- 按钮
##### 接口/通信
- I2C
- GPIO
##### 传感器
- 片上ADC
- MCU内置温度传感器
- DHT11
- HTU2x
- AHT2x
- BMP280
- C8S二氧化碳传感器
- 超声波模块
##### 执行器
- 28BYJ-48步进电机
- 直流电机
- 舵机
##### 控制器/数据处理
- 抽象数据Buffer
##### 其他
- RTC时钟


#### 软件库
- printf_xxx库
- 3x5, 5x7 ASCII字体
未来计划支持列表请查看`docs/future.md`


#### 使用说明
###### 准备
1.  克隆此仓库到本地，不要放在某个项目目录内，以后用软连接引用
2.  按造`docs/create_project.md`里的步骤创建工程

#### 非MCU代码
PC端工具，大多由Python编写，在ubuntu下测试过:  
    RTC时间同步，点阵/数码管字体生成，二值图动画生成
Android端工具（计划）  
自动配置工具（计划）  

#### 目的
创建一个跨平台的, 综合性的, 容易使用的嵌入式库, 目标是取代Arduino,
尽可能的使用自由软件, 允许适当使用免费的非自由开源软件, 不使用收费的专有软件.

#### 版权说明
本软件库使用LGPLv3或更新授权, 见`LICENSE`文件,
包含的第三方库按照第三方库的许可证授权, 如有侵权, 请及时通知删除.
本README.md文件及docs文件夹下的文档使用GFDLv1.3或更新授权.

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
