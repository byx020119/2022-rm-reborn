# 2022-rm-c-test

<!-- PROJECT SHIELDS -->



<h3  align="center">2022-rm-c-test</h3>

<p  align="center">

此为全国大学生机器人大赛 Robomaster 2022 机甲大师系列，Reborn战队“哨兵”机器人嵌入式组代码及“空中”机器人嵌入式组代码，包括2022-rm-a-down，2022-rm-a-up，2022-rm-a-plane，2022-rm-c-test，2022-rm-self-plane几部分代码，2022赛季Reborn战队获得超级对抗赛[东部赛区二等奖](https://www.robomaster.com/zh-CN/resource/pages/announcement/1463)及[全国三等奖](https://www.robomaster.com/zh-CN/resource/pages/announcement/1477)。

其中分支名称`2022-rm-DEV-SERVICE(TYPE)`

- `2022` 指robomaster 2022赛季。
- `rm` 指robomaster比赛。
- `DEV` 指开发板类型，本赛季所使用过的有a（DJI robomaster a型板）、c（DJI robomaster c型板）、与self（reborn队内自研板）。
- `SERVICE` 指兵种，down（双枪哨兵下枪），up（双枪哨兵上枪），plane（空中机器人）。
- `TYPE` 指代码类型，server为服务器版本，因为训练时候方便全自动操作，server版本中，一旦服务器开始战斗，哨兵机器人自动进入巡逻状态，全程无人操作。
![](https://rm-static.djicdn.com/tem/55708/428eaaef4b6ba1632310756119837520.png)
<br  />

<a  href="https://github.com/byx020119/2022-rm-reborn.git"><strong>探索本项目的文档 »</strong></a>

<br  />

<br  />

<a  href="https://github.com/byx020119/2022-rm-reborn.git">查看</a>

·

<a  href="https://github.com/byx020119/2022-rm-reborn/issues">报告Bug</a>

·

<a  href="https://github.com/byx020119/2022-rm-reborn/issues">提出新特性</a>

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

git clone https://github.com/byx020119/2022-rm-reborn.git

```
2. checkout branch

```sh
git checkout 2022-rm-DEV-SERVICE(TYPE)
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

  

github: http://github.com/byx020119 &ensp; email: byx020119@gmail.com

  

*您也可以在贡献者名单中参看所有参与该项目的开发者。*
  
<br>
</br>

## 鸣谢


- [Y1Yukina (赵玥玥) (github.com)](https://github.com/Y1Yukina)


<!-- links -->


<!--stackedit_data:
eyJoaXN0b3J5IjpbLTI4OTkwNTc0LDQ1OTYzMTgwOCwtODI5OT
UzMTYxLC03MTAyOTU5NTMsODEwMjAwNzAzLDg2Nzk5MjUwNywt
MTE1MzYxMjIyMiwxNjQwODkwMTgsLTY5MjYwMDExMiwtMTQ1ND
Y2NDUwNCwxMTkzNjIxNTE5LC05NjMxODY0NzcsLTEyNTQ2NTM4
NThdfQ==
-->