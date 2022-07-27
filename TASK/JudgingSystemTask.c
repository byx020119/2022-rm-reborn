#include "main.h"

extGameRobotState_t robotState;
ext_game_status_t   gameState;
extRobotHurt_t      robotHurt;
extPowerHeatData_t  robotPowerHeat;
extShootData_t      robotShootData;
ext_event_data_t  eventState;  //前哨战是否被击毁类型和句柄声明

ringBuffer_t buffer;

uint32_t ChassisPower_temp;
uint32_t ChassisPower_buffer;
float chassisPowerBuffer = 0;//功率缓冲
float Parameter_Transformation(int32_t data);
float  Yaw_encoder=0.0f;
int  Yaw_encoder_s=0;
float test_power=0;
	
	
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
函数：void getEventData(uint8_t *eventData)  数据未读
功能：读取己方前哨站的状态 0为被击毁，1为存活
备注：共32个数据，第11位(下标为10为所需数据）
***/
void getEventData(uint8_t *eventData)
{
	int tempGameData[12],i=0,Event_state;
    //读11位，第11位	 1：己方前哨战存活，0：己方前哨站被击毁 
	for(i=0;i<12;i++)
	{
		tempGameData[i]=eventData[i];   //存储前11位的数据
	}
	Event_state = tempGameData[11];  //Event_state为中间变量
	eventState.qianshao =  (Event_state & 0x01);   //得到第11位数据  eventState为定义的结构体的句柄  

}

/***
函数：void getRobotState(uint8_t *stateData) 数据可读已使用
功能：从裁判系统读取机器人状态(当前血量)
备注：ID：0x0201
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
函数：void getRobotHurt(uint8_t *hurtData) 数据可读已使用
功能：从裁判系统读取伤害数据
备注：ID：0x0206
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
	
	Attacked_Monitor();

}

/***
函数：void getRobotPowerHeat(uint8_t *powerHeartData) 数据可读未使用
功能：从裁判系统读取实时功率热量数据
备注：ID：0x0202   50Hz频率周期发送
      共29个数据，下标23，24为17mm弹丸枪口热量数据
***/
void getRobotPowerHeat(uint8_t *powerHeartData)
{
	int tempPowerHeatData[21],i,temp_17_heat;
	for(i=0;i<21;i++)
	{
		tempPowerHeatData[i]=powerHeartData[i];
	}
	temp_17_heat=tempPowerHeatData[17]|tempPowerHeatData[18]<<8;
	robotPowerHeat.shooter_17_Heat = Transform_Hex_To_Oct(temp_17_heat,16);
  Shooter_17_Heat =robotPowerHeat.shooter_17_Heat;
	

	ChassisPower_temp = powerHeartData[11]|(powerHeartData[12]<<8)|(powerHeartData[13]<<16)|(powerHeartData[14]<<24);
	robotPowerHeat.ChassisPower = Parameter_Transformation(ChassisPower_temp);//功率
	test_power = robotPowerHeat.ChassisPower;
	
	ChassisPower_buffer = powerHeartData[15]|(powerHeartData[16]<<8);//|(powerHeartData[21]<<16)|(powerHeartData[22]<<24);
//	chassisPowerBuffer = Parameter_Transformation(ChassisPower_buffer); //缓冲功率
	robotPowerHeat.ChassisPowerBuffer = Parameter_Transformation(ChassisPower_buffer); //缓冲功率
	
//	robotPowerHeat.shooter_17_Heat= powerHeartData[23]|(powerHeartData[24]<<8);
//	chassisPowerError = robotPowerHeat.ChassisPower - lastChassisPower;
	
if(ChassisPower_buffer<=100)
{
	Speed_Offset.kp = 8;
	Speed_Offset.kd = 2;
	
	Speed_Offset.ref = 100;
	Speed_Offset.fdb = ChassisPower_buffer;
	Speed_Offset.Calc(&Speed_Offset);
	Speed_Offset.output = -Speed_Offset.output;      //应该是负的关系
}
if(ChassisPower_buffer>100)
{
  Speed_Offset.output =0.00000001;
}
	
}




/***
函数：void getRobotShootData(uint8_t *shootData) 数据可读已使用
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
	
	BulletSpeed_Monitor();
	
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
