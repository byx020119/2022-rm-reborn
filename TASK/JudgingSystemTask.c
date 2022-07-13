#include "main.h"
#include "JudgingSystemTask.h"
#include "GunHeartAndBleed.h"
#include "ShootingTask.h"

extGameRobotState_t robotState;
ext_game_status_t   gameState;
extRobotHurt_t      robotHurt;
extPowerHeatData_t  robotPowerHeat;
extShootData_t      robotShootData;
ext_event_data_t    eventState;  //前哨战是否被击毁类型和句柄声明
ext_bullet_remaining_t remainBullet;
ringBuffer_t buffer;

uint32_t ChassisPower_temp;
uint32_t ChassisPower_buffer;
float chassisPowerBuffer = 0;//功率缓冲
float Parameter_Transformation(int32_t data);
//int  Yaw_encoder=0;
float Yaw_encoder=0.0f;
int  Yaw_encoder_s=0;
float test_power=0;

uint8_t Qianshao_state = 0;
uint8_t Shooter_17_Heat1 = 0;
uint8_t Shooter_17_Heat2 = 0; 


/***
函数：void getGameState(uint8_t *stateData)
功能：从裁判系统读取游戏状态
备注：ID：0x0001
      共11个数据，下标11，12为当前血量数据
***/
void getGameState(uint8_t *stateData)
{
	int tempGameData[12],i,tempTime=0,Game_state;
	for(i=0;i<11;i++)
	{
		tempGameData[i]=stateData[i];
	}
  /***比赛状态***/
	Game_state=tempGameData[7];
	gameState.game_type=(Game_state & 0x0F);
	gameState.game_progress=(Game_state & 0xF0)>>4;
	/***剩余时间***/
	tempTime=tempGameData[8]|tempGameData[9]<<8;
	gameState.stage_remain_time = Transform_Hex_To_Oct(tempTime,16);
}

/***
函数：void getEventData(uint8_t *eventData)
功能：读取己方前哨站的状态 0为被击毁，1为存活
备注：共32个数据，第11位(下标为10为所需数据）
***/
void getEventData(uint8_t *eventData)
{
	int tempGameData[32],i=0,Event_state;  //不用管检验CRC有几位，直接在文档里看有几位bit
    //读11位，第11位	 1：己方前哨战存活，0：己方前哨站被击毁 
	for(i=0;i<32;i++)
	{
		tempGameData[i]=eventData[i];   //存储前11位的数据
	}
	Event_state = tempGameData[8]; //第二个八位数，显示形式为16进制
	//Event_state为中间变量，tempGameData[0]是0x00A5的数，函数以八进制为一个大小标准，【7】是文档里第一个有效的八位bit，即第二栏写的大小为1
	Qianshao_state = (Event_state & 0x04);   
	//与二进制大小为0000 0100，即十六进制为0x04按位与得到第11bit的数据
	//传输顺序是从低位到高位(例如剩余血量是一个2大小，即16位bit的数，那么要将【9】先左移8位，再和【9】按位与)
}

/***
函数：void getRobotState(uint8_t *stateData)
功能：从裁判系统读取机器人状态(当前血量)
备注：ID：0x0001
      共17个数据，下标11，12为当前血量数据
***/
void getRobotState(uint8_t *stateData)
{
	int tempStateData[22],i,tempBlood=0,Robot_numbel;
	for(i=0;i<22;i++)
	{
		tempStateData[i]=stateData[i];
	}
  /***读取当前机器人ID***/
	Robot_numbel=tempStateData[7];
	robotState.robot_id=Robot_numbel;
	/***读取当前血量值***/
	tempBlood=tempStateData[9]|tempStateData[10]<<8;
	robotState.remainHP = Transform_Hex_To_Oct(tempBlood,16);
	
	Bleed_Monitor();

}

/***
函数：void getRobotHurt(uint8_t *hurtData)
功能：从裁判系统读取伤害数据
备注：ID：0x0002
      共10个数据，下标7为伤害数据
***/
void getRobotHurt(uint8_t *hurtData)
{
	int tempHurtData[8],i,hurtInfo;
	for(i=0;i<8;i++)
	{
		tempHurtData[i]=hurtData[i];
	}
	hurtInfo=tempHurtData[7];
	robotHurt.armorType = hurtInfo & 0x0F;      //被攻击的装甲ID
	robotHurt.hurtType  = (hurtInfo & 0xF0)>>4; //伤害类型
	
	Attacked_Monitor();//5/9   这个是裁判系统传输数据之后才会进入这个函数，如果有延迟的话，应该会对被攻击状态有影响

}

/***
函数：void getRobotPowerHeat(uint8_t *powerHeartData)
功能：从裁判系统读取实时功率热量数据
备注：ID：0x0004   50Hz频率周期发送
      共29个数据，下标23，24为17mm弹丸枪口热量数据
***/
void getRobotPowerHeat(uint8_t *powerHeartData)
{
	int tempPowerHeatData[21],i,temp_17_heat_1,temp_17_heat_2;
	for(i=0;i<21;i++)
	{
	  tempPowerHeatData[i]=powerHeartData[i];
	}
	temp_17_heat_1=tempPowerHeatData[17]|tempPowerHeatData[18]<<8;  //1号
	temp_17_heat_2=tempPowerHeatData[19]|tempPowerHeatData[20]<<8;  //2号
	
	robotPowerHeat.shooter_17_Heat_1 = Transform_Hex_To_Oct(temp_17_heat_1,16);
	robotPowerHeat.shooter_17_Heat_2 = Transform_Hex_To_Oct(temp_17_heat_2,16);
	
  Shooter_17_Heat1 =robotPowerHeat.shooter_17_Heat_1;
  Shooter_17_Heat2 =robotPowerHeat.shooter_17_Heat_2;
}

/***
函数：void getRobotShootData(uint8_t *shootData)
功能：从裁判系统读取实时射击信息
备注：ID：0x0003
      共15个数据，下标9，10，11，12为弹丸射速数据
***/
void getRobotShootData(uint8_t *shootData)
{
	int shoot_17_frequency = 0;
	int tempShootData[15],i;
	GetFloatValueStrcut shoot_17_Speed = SolveFloatValueStr_Init;//4字节浮点数换算10进制小数的结构体
	for(i=0;i<15;i++)
	{
		tempShootData[i] = shootData[i];
	}
//	shoot_17_frequency = tempShootData[8];
	for(i=0;i<4;i++)
	{
		shoot_17_Speed.Temp_Array[i] = tempShootData[i+10];
	}
	robotShootData.bulletType = tempShootData[7];
	if (robotShootData.bulletType==1)
	{
	robotShootData.bulletFreq = shootData[8];
	}
	robotShootData.bulletSpeed = shoot_17_Speed.FloatValueSolve(&shoot_17_Speed);//读取17mm弹丸射速
//	BulletSpeed_Transmit((int)robotShootData.bulletSpeed);
	
	/***自己计算枪口热量***/
	GunHeart_CalcProcess((int)robotShootData.bulletSpeed);
	
	Bullet_17_real_Speed = robotShootData.bulletSpeed;
	
	//BulletSpeed_Monitor();
	
}

/***
函数：void getRobotState(uint8_t *stateData)
功能：从裁判系统读取机器人剩余弹量
备注：ID：0x0208
      共个数据，下标为当前数据
***/
void getRemainBulletData(uint8_t *bulletData)
{
	int tempBulletData[12],i,tempBullet=0,remain_Bullet;
	for(i=0;i<11;i++)  //多取几位也没事
	{
		tempBulletData[i]=bulletData[i];
	}
  /***读取当前机器人剩余弹量***/
	remain_Bullet= tempBulletData[7] | (tempBulletData[8]<<8);//tempBulletData[7]是第七位，是第一个有效位，为8bit，一个大小
	remainBullet.bullet_remaining_num_17mm=remain_Bullet;  //remainBullet是句柄名字，bullet_remaining_num_17mm是结构体里的一个参量
//	remainBullet.bullet_remaining_num_17mm = Transform_Hex_To_Oct(remain_Bullet,16);	//转换成10进制，只在这里debug检验是否正确而已，板间通讯仍用16进制取传输
    
}



/*********************************************
函数：Parameter_Transformation
功能：四字节二进制数装换为浮点数
**********************************************/
float Parameter_Transformation(int32_t data)
{
  int temp1,temp4;
	long temp2;
	float temp3;
	//temp1是阶码
	//temp2是尾数
	//temp3是最后算好的数
	//temp4是电流尾数的每一位
	temp1=((data&0X7F800000)>>23)-127; 
	temp2= data&0X007FFFFF;
	for(int j=0;j<24;j++)
	{
		if(j==0)
		{ 
			temp3=(float)ldexp(1.0,temp1);
		}
		else
		{
		temp4=(temp2&(0x00400000>>(j-1)))>>(23-j);
		temp3=temp3+temp4*(float)ldexp(1.0,temp1-j);
		}
	}
	return temp3;
}



/*************************************
功能：4字节表示的浮点数转为10进制小数
入口参数：GetFloatValueStrcut类型结构体
出口参数：10进制小数值
备注：无
**************************************/
float FloatValueSolveFunction(struct GetFloatValueStrcut *date)
{
  int i;
	date->Combination_Array = (date->Temp_Array[0]<<0)|(date->Temp_Array[1]<<8)\
	                                     |(date->Temp_Array[2]<<16)|(date->Temp_Array[3]<<24);//倒序组合二进制形式表示的浮点数
	date->JieMa = ((date->Combination_Array & 0X7F800000)>>23)-127;
	date->WeiShu = date->Combination_Array & 0X007FFFFF;
	for(i=0;i<24;i++)
	{
		if(i==0) {date->Float_Value = 1*date->ChengFang_Solution(2,date->JieMa);}
		else
		{
			date->WeiShu_EveryBit = (date->WeiShu & (0X00400000>>(i-1)))>>(23-i);
			date->Float_Value = date->Float_Value \
			                              + date->WeiShu_EveryBit*date->ChengFang_Solution(2,date->JieMa-i);
		}
	}
	return date->Float_Value;
}


/*******************
功能：乘方计算函数
入口参数：x--底数
          n--指数
返回值：乘方计算值
*********************/
float solve_chengfang(float x,int n)         
{
	int i;
	int a;
	float s=1.0;
	if(n>=0) a=n;
	if(n<0) a=-n;
	for(i=1;i<=a;i++)
		s*=x;
	if(n>=0)
	   return s;
	if(n<0)
	  return 1/s;
	return 0;
}

/***
函数：int Transform_Hex_To_Oct(int data,int len)
功能：16进制数转换成10进制数
备注：data:16进制格式  len：data中2进制位数
***/
int Transform_Hex_To_Oct(int data,int len)
{
	int a=0x0001,temp=0;
	int ans=0;
	for(int i=0;i<len;i++)
	{
		temp=(data&(a<<i))>>i;
		ans = ans + temp*solve_chengfang(2,i);
	}
	return ans;
}

/***
函数：void RingBuffer_Write(uint8_t data)
功能：将数据data写入环形队列buffer.ringBuf中
备注：无
***/
void RingBuffer_Write(uint8_t data)
{
	buffer.ringBuf[buffer.tailPosition] = data;     //从尾部追加
	if(++buffer.tailPosition>=BUFFER_MAX)           //尾节点偏移
		buffer.tailPosition = 0;                      //大于数组最大长度，归零，形成环形队列
	if(buffer.tailPosition == buffer.headPosition)  //如果尾部节点追到头部节点，则修改头结点偏移位置丢弃早期数据
		if(++buffer.headPosition>=BUFFER_MAX)
			buffer.headPosition = 0;
}

/***
函数：u8 RingBuffer_Read(uint8_t *pdata)
功能：从环形队列buffer.ringBuf中读取数据到地址pdata中
备注：无
***/
u8 RingBuffer_Read(uint8_t *pdata)
{
	if(buffer.headPosition == buffer.tailPosition)  //如果头尾接触表示缓冲区为空
	{
		return 1;  //返回1，环形缓冲区是空的
	}
	else
	{
		*pdata = buffer.ringBuf[buffer.headPosition];  //如果缓冲区非空则取头节点值并偏移头节点
		if(++buffer.headPosition>=BUFFER_MAX)
			buffer.headPosition = 0;
		return 0;   //返回0，表示读取数据成功
	}
	
}
