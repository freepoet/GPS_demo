## 最近的修改
* 2020/11/07 </br>
排查1101版本的程序没有问题，尝试将显示屏型号为9481的板子的P4引脚两个跳线帽拔掉，1101版本的程序可以正常运行，但是9486的板子不需要拔，原因未知。 -.-| </br>
GPS_demo_final定位时间较长。
* 2020/11/06  </br>
继承GPS_demo_1031，再写一个仅适用于9481（显示屏型号）的程序，failed
* 2020/11/01 </br>
修复TFTLCD(型号：9481，之前的程序使用的LCD型号为9486）无法显示的问题，继承1031的程序，将lcd.c改为了tftlcd.c。 </br>
bug（似乎存在）:没办法定位。 </br>
debug:设置断点发现程序总是卡在定位初始化的位置。 </br>
原因：tftlcd.c中的程序与定位存在冲突。 </br>
* 2020/10/31 </br>
修改GPS_demo_1020 GPS_demo_1024中的bugs 增加GPS_demo_1031演示程序
* 2020/10/21 </br>
添加定时中断 检测按键
* 2020/10/20 </br>
实现定位


## 使用说明
### 一 引脚的连接
       GPS模块<-------------->STM32开发板
        VCC                    电源模块5V
        GND                    电源模块GND
        TXD                     PB11
        RXD                     PB10
        PPS(悬空)
--------------------
### 二 使用
#### 1、开发板接上电源和仿真器，烧录程序。
#### 2、按一下按键模块的"复位"按键，程序开始运行，按"K_DOWN"进行功能切换，
##### 功能1：显示时间、经纬度、海拔等信息；
在空旷位置，首次定位时间较长，可能需要几分钟的时间。<br>
fix mode:3D 代表定位成功。 定位成功以后才能切换到功能2。

![image](https://github.com/freepoet/GPS_demo/blob/main/demo_images/test.jpg)
##### 功能2：轨迹绘制（*有待完善*）；




  
