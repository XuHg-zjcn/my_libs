###### STM32配置方法
1.  打开STM32CubeIDE
2.  创建项目，适当配置外设，生成代码
3.  右键项目，点击Convert to C++
4.  创建所需的目录(common, interface)软链接到项目目录 例子:
    ```shell
        resp_dir='~/elec/mcu_mylibs'                   #请替换本仓库克隆的目录
        cd ~/STM32CubeIDE/workspace_1.6.1/MyF1Board    #请替换成你的项目目录
        mkdir mcu_mylibs
        cd mcu_mylibs
        ln -s $resp_dir$/common
        ln -s $resp_dir$/stm32
        ln -s $resp_dir$/stm32f1xx                     #请替换成芯片型号对应的库
    ```
5.  右键项目 -> Properties -> C/C++ General -> Paths and Symbols -> Includes -> 添加所需的头文件目录
    mcu_mylibs/commom/Inc
    mcu_mylibs/stm32/Inc
    mcu_mylibs/stm32f1/Inc (f4项目添加对应目录)
6.  编辑配置文件 mcu_mylibs/common/Inc/mylibs_config.hpp
