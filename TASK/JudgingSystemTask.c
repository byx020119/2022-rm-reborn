#include "main.h"

extGameRobotState_t robotState;
ext_game_status_t   gameState;
extRobotHurt_t      robotHurt;
extPowerHeatData_t  robotPowerHeat;
extShootData_t      robotShootData;
ext_event_data_t  eventState;  //ǰ��ս�Ƿ񱻻������ͺ;������

ringBuffer_t buffer;

uint32_t ChassisPower_temp;
uint32_t ChassisPower_buffer;
float chassisPowerBuffer = 0;//���ʻ���
float Parameter_Transformation(int32_t data);
float  Yaw_encoder=0.0f;
int  Yaw_encoder_s=0;
float test_power=0;
	
	
/***
������void getGameState(uint8_t *stateData)
���ܣ��Ӳ���ϵͳ��ȡ��Ϸ״̬
��ע��ID��0x0001
      ��11�����ݣ��±�11��12Ϊ��ǰѪ������
***/
void getGameState(uint8_t *stateData)
{
	int tempGameData[12],i,tempTime=0,Game_state;
	for(i=0;i<11;i++)
	{
		tempGameData[i]=stateData[i];
	}
  /***����״̬***/
	Game_state=tempGameData[7];
	gameState.game_type=(Game_state & 0x0F);
	gameState.game_progress=(Game_state & 0xF0)>>4;
	/***ʣ��ʱ��***/
	tempTime=tempGameData[8]|tempGameData[9]<<8;
	gameState.stage_remain_time = Transform_Hex_To_Oct(tempTime,16);
}
/***
������void getEventData(uint8_t *eventData)
���ܣ���ȡ����ǰ��վ��״̬ 0Ϊ�����٣�1Ϊ���
��ע����32�����ݣ���11λ(�±�Ϊ10Ϊ�������ݣ�
***/
void getEventData(uint8_t *eventData)
{
	int tempGameData[12],i=0,Event_state;
    //��11λ����11λ	 1������ǰ��ս��0������ǰ��վ������ 
	for(i=0;i<12;i++)
	{
		tempGameData[i]=eventData[i];   //�洢ǰ11λ������
	}
	Event_state = tempGameData[11];  //Event_stateΪ�м����
	eventState.qianshao =  (Event_state & 0x01);   //�õ���11λ����  eventStateΪ����Ľṹ��ľ��  

}

/***
������void getRobotState(uint8_t *stateData)
���ܣ��Ӳ���ϵͳ��ȡ������״̬(��ǰѪ��)
��ע��ID��0x0001
      ��17�����ݣ��±�11��12Ϊ��ǰѪ������
***/
void getRobotState(uint8_t *stateData)
{
	int tempStateData[22],i,tempBlood=0,Robot_numbel;
	for(i=0;i<22;i++)
	{
		tempStateData[i]=stateData[i];
	}
  /***��ȡ��ǰ������ID***/
	Robot_numbel=tempStateData[7];
	robotState.robot_id=Robot_numbel;
	/***��ȡ��ǰѪ��ֵ***/
	tempBlood=tempStateData[9]|tempStateData[10]<<8;
	robotState.remainHP = Transform_Hex_To_Oct(tempBlood,16);
	
	Bleed_Monitor();

}

/***
������void getRobotHurt(uint8_t *hurtData)
���ܣ��Ӳ���ϵͳ��ȡ�˺�����
��ע��ID��0x0002
      ��10�����ݣ��±�7Ϊ�˺�����
***/
void getRobotHurt(uint8_t *hurtData)
{
	int tempHurtData[8],i,hurtInfo;
	for(i=0;i<8;i++)
	{
		tempHurtData[i]=hurtData[i];
	}
	hurtInfo=tempHurtData[7];
	robotHurt.armorType = hurtInfo & 0x0F;      //��������װ��ID
	robotHurt.hurtType  = (hurtInfo & 0xF0)>>4; //�˺�����
	
	Attacked_Monitor();

}

/***
������void getRobotPowerHeat(uint8_t *powerHeartData)
���ܣ��Ӳ���ϵͳ��ȡʵʱ������������
��ע��ID��0x0004   50HzƵ�����ڷ���
      ��29�����ݣ��±�23��24Ϊ17mm����ǹ����������
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
	robotPowerHeat.ChassisPower = Parameter_Transformation(ChassisPower_temp);//����
	test_power = robotPowerHeat.ChassisPower;
	
	ChassisPower_buffer = powerHeartData[15]|(powerHeartData[16]<<8);//|(powerHeartData[21]<<16)|(powerHeartData[22]<<24);
//	chassisPowerBuffer = Parameter_Transformation(ChassisPower_buffer); //���幦��
	robotPowerHeat.ChassisPowerBuffer = Parameter_Transformation(ChassisPower_buffer); //���幦��
	
//	robotPowerHeat.shooter_17_Heat= powerHeartData[23]|(powerHeartData[24]<<8);
//	chassisPowerError = robotPowerHeat.ChassisPower - lastChassisPower;
	
if(ChassisPower_buffer<=100)
{
	Speed_Offset.kp = 8;
	Speed_Offset.kd = 2;
	
	Speed_Offset.ref = 100;
	Speed_Offset.fdb = ChassisPower_buffer;
	Speed_Offset.Calc(&Speed_Offset);
	Speed_Offset.output = -Speed_Offset.output;      //Ӧ���Ǹ��Ĺ�ϵ
}
if(ChassisPower_buffer>100)
{
  Speed_Offset.output =0.00000001;
}
	
}




/***
������void getRobotShootData(uint8_t *shootData)
���ܣ��Ӳ���ϵͳ��ȡʵʱ�����Ϣ
��ע��ID��0x0003
      ��15�����ݣ��±�9��10��11��12Ϊ������������
***/
void getRobotShootData(uint8_t *shootData)
{
	int shoot_17_frequency = 0;
	int tempShootData[15],i;
	GetFloatValueStrcut shoot_17_Speed = SolveFloatValueStr_Init;//4�ֽڸ���������10����С���Ľṹ��
	for(i=0;i<15;i++)
	{
		tempShootData[i] = shootData[i];
	}
//	shoot_17_frequency = tempShootData[8];
	for(i=0;i<4;i++)
	{
		shoot_17_Speed.Temp_Array[i] = tempShootData[i+9];
	}
	robotShootData.bulletType = tempShootData[7];
	if (robotShootData.bulletType==1)
	{
	robotShootData.bulletFreq = shootData[8];
	}
	robotShootData.bulletSpeed = shoot_17_Speed.FloatValueSolve(&shoot_17_Speed);//��ȡ17mm��������
//	BulletSpeed_Transmit((int)robotShootData.bulletSpeed);
	
	/***�Լ�����ǹ������***/
	GunHeart_CalcProcess((int)robotShootData.bulletSpeed);
	
	Bullet_17_real_Speed = robotShootData.bulletSpeed;
	
	BulletSpeed_Monitor();
	
}




/*********************************************
������Parameter_Transformation
���ܣ����ֽڶ�������װ��Ϊ������
**********************************************/
float Parameter_Transformation(int32_t data)
{
  int temp1,temp4;
	long temp2;
	float temp3;
	//temp1�ǽ���
	//temp2��β��
	//temp3�������õ���
	//temp4�ǵ���β����ÿһλ
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
���ܣ�4�ֽڱ�ʾ�ĸ�����תΪ10����С��
��ڲ�����GetFloatValueStrcut���ͽṹ��
���ڲ�����10����С��ֵ
��ע����
**************************************/
float FloatValueSolveFunction(struct GetFloatValueStrcut *date)
{
  int i;
	date->Combination_Array = (date->Temp_Array[0]<<0)|(date->Temp_Array[1]<<8)\
	                                     |(date->Temp_Array[2]<<16)|(date->Temp_Array[3]<<24);//������϶�������ʽ��ʾ�ĸ�����
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
���ܣ��˷����㺯��
��ڲ�����x--����
          n--ָ��
����ֵ���˷�����ֵ
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
������int Transform_Hex_To_Oct(int data,int len)
���ܣ�16������ת����10������
��ע��data:16���Ƹ�ʽ  len��data��2����λ��
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
������void RingBuffer_Write(uint8_t data)
���ܣ�������dataд�뻷�ζ���buffer.ringBuf��
��ע����
***/
void RingBuffer_Write(uint8_t data)
{
	buffer.ringBuf[buffer.tailPosition] = data;     //��β��׷��
	if(++buffer.tailPosition>=BUFFER_MAX)           //β�ڵ�ƫ��
		buffer.tailPosition = 0;                      //����������󳤶ȣ����㣬�γɻ��ζ���
	if(buffer.tailPosition == buffer.headPosition)  //���β���ڵ�׷��ͷ���ڵ㣬���޸�ͷ���ƫ��λ�ö�����������
		if(++buffer.headPosition>=BUFFER_MAX)
			buffer.headPosition = 0;
}

/***
������u8 RingBuffer_Read(uint8_t *pdata)
���ܣ��ӻ��ζ���buffer.ringBuf�ж�ȡ���ݵ���ַpdata��
��ע����
***/
u8 RingBuffer_Read(uint8_t *pdata)
{
	if(buffer.headPosition == buffer.tailPosition)  //���ͷβ�Ӵ���ʾ������Ϊ��
	{
		return 1;  //����1�����λ������ǿյ�
	}
	else
	{
		*pdata = buffer.ringBuf[buffer.headPosition];  //����������ǿ���ȡͷ�ڵ�ֵ��ƫ��ͷ�ڵ�
		if(++buffer.headPosition>=BUFFER_MAX)
			buffer.headPosition = 0;
		return 0;   //����0����ʾ��ȡ���ݳɹ�
	}
	
}
