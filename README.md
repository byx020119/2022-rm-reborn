# -2022 Robomaster备赛测试代码-


<!-- PROJECT SHIELDS -->



<h3  align="center">2022-rm-c-test</h3>

<p  align="center">

此为全国大学生机器人大赛 Robomaster 2022 机甲大师系列，Reborn战队“哨兵”机器人嵌入式组代码，包括2022-rm-a-down，2022-rm-a-up两部分A型板代码，分别控制机器人的上枪组及下枪组，另有2022-rm-a-test为C型板代码。2022赛季Reborn战队获得超级对抗赛[东部赛区二等奖](https://www.robomaster.com/zh-CN/resource/pages/announcement/1463)及[全国三等奖](https://www.robomaster.com/zh-CN/resource/pages/announcement/1477)。

本工程为2022年robomaster比赛备赛过程中老版本车代码更改can口适配于2021年哨兵的代码。
![](https://rm-static.djicdn.com/tem/55708/428eaaef4b6ba1632310756119837520.png)
<br  />

<a  href="https://github.com/byx020119/2022-rm-c-test"><strong>探索本项目的文档 »</strong></a>

<br  />

<br  />

<a  href="https://github.com/byx020119/2022-rm-c-test">查看</a>

·

<a  href="https://github.com/byx020119/2022-rm-c-test/issues">报告Bug</a>

·

<a  href="https://github.com/byx020119/2022-rm-c-test/issues">提出新特性</a>

</p>  

</p>


## 目录

  

- [开发环境](#开发环境)

- [文件目录说明](#文件目录说明)

- [如何参与开源项目](#如何参与开源项目)

- [版本控制](#版本控制)

- [作者](#作者)

- [鸣谢](#鸣谢)

  <br>
</br>

## 开发环境

windows系统下的keil平台，keil5
  

###### **安装步骤**


1. Clone the repo

```sh

git clone https://github.com/byx20119/2022-rm-c-test.git

```
<br>
</br>
  

## 文件目录说明

eg:

  

```

2022-rm-c-test

|--README.md（说明文档）
|--/BSP/（基本配置文档）
|  |--adc.c
|  |--adc.h
|--/Common/（基本控制算法文档）
|  |--pid.c
|  |--pid.h
|--/CanBusTask/（can通信设置）
|  |--CanBusTask.c
|  |--CanBusTask.h
|--/ControlTask/（控制任务文档）
|  |--ControlTask.c
|  |--ControlTask.h
|--/USER/（用户文件）
|  |--shaobing c.uvprojx（启动文件）
|  |--标准库同hal库哨兵区别.doc
...
 

```


### 如何参与开源项目

  

贡献使开源社区成为一个学习、激励和创造的绝佳场所。你所作的任何贡献都是**非常感谢**的。

  
  

1. Fork the Project

2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)

3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)

4. Push to the Branch (`git push origin feature/AmazingFeature`)

5. Open a Pull Request

  <br>
</br>
  
  

## 版本控制

  

该项目使用Git进行版本管理。您可以在repository参看当前可用版本。

  <br>
</br>

## 作者

  

byx020119
![](https://avatars.githubusercontent.com/u/92295993?s=48&v=4)

  

github:http://github.com/byx020119 &ensp; qq:2237478533

  

*您也可以在贡献者名单中参看所有参与该项目的开发者。*
  
  <br>
</br>

## 鸣谢


- [Y1Yukina (赵玥玥) (github.com)](https://github.com/Y1Yukina)


<!-- links -->


<!--stackedit_data:
eyJoaXN0b3J5IjpbLTIwMjIxNzYzNzksMTIyMjgyNjk2OCwyMT
EwMDgzNzY0XX0=
-->