#ifndef JudgingSystemTask_H
#define JudgingSystemTask_H
#include "main.h"

#define SolveFloatValueStr_Init \
{\
	{0,0,0,0},\
	0,\
	0,\
	0,\
	0,\
	0.0,\
	&FloatValueSolveFunction,\
	&solve_chengfang,\
} \

typedef __packed struct
{
 uint8_t game_type : 4;
 uint8_t game_progress : 4;
 uint16_t stage_remain_time;
 uint64_t SyncTimeStamp;
} ext_game_status_t;

//定义一种类型为ext_event_data_t
//数据32位，第11位为己方前哨战的状态
typedef __packed struct    
{
	uint16_t qianshao : 11 ; //当前己方前哨战的状态 
}ext_event_data_t;

typedef __packed struct
{
	uint8_t sof;             //数据帧头，固定0xA5
	uint16_t data_length;    //每帧内数据data的长度 
	uint8_t seq;             //包序号
	uint8_t crc8;            //帧头的crc校验结果
}frame_header_t;

typedef struct
{
	uint8_t robot_id;
	uint16_t stageRemainTime; //当前阶段剩余时间
	uint8_t gameProgress;     //当前比赛处于哪个阶段
	uint8_t robotLevel;       //机器人当前等级
	uint16_t remainHP;        //机器人当前血量
	uint16_t maxHP;           //机器人满血量
}extGameRobotState_t;  //比赛机器人状态

typedef __packed struct
{
	uint8_t armorType; //因装甲被击打掉血时，标示受到伤害的装甲ID
	uint8_t hurtType;  //血量变化类型
}extRobotHurt_t; //伤害数据

typedef __packed struct
{
	uint8_t bulletType; //弹丸类型  1：17mm弹丸   2：42mm弹丸
	uint8_t bulletFreq; //弹丸射频
	float bulletSpeed;  //弹丸射速
}extShootData_t;  //实时射击信息

typedef __packed struct
{
	float ChassisVolt;        //底盘输出电压
	float ChassisCurrent;     //底盘输出电流
	float ChassisPower;       //底盘输出功率
	float ChassisPowerBuffer; //底盘功率缓冲
	uint16_t shooter_17_Heat;   //17mm枪口热量
	uint16_t shooter_42_Heat;   //42mm枪口热量
}extPowerHeatData_t;  //实时功率热量数据

typedef __packed struct
{
	uint8_t cardType;  //卡类型
	uint8_t cardIdx;   //卡索引号
}extRfidDetect_t; //场地交互数据

typedef __packed struct
{
	uint8_t winner;
}extGameResult_t;  //比赛胜负数据

typedef __packed struct
{
	uint8_t buffType;     //buff类型
	uint8_t buffAddition; //buff加成比
}extGetBuff_t;  //Buff获取数据

typedef __packed struct
{
	float x;
	float y;
	float z;
	float yaw;        //枪口朝向角度值
}extGameRobotPos_t; //机器人位置角度信息

typedef __packed struct
{
	float data1;
	float data2;
	float data3;
	uint8_t mask;
}extShowData_t;  //参赛队自定义数据

typedef struct GetFloatValueStrcut
{
	int Temp_Array[4];
	long int Combination_Array;
	int JieMa;
	long int WeiShu;
	int WeiShu_EveryBit;
	float Float_Value;
	float (*FloatValueSolve)(struct GetFloatValueStrcut *date);
	float (*ChengFang_Solution)(float X,int N);
}GetFloatValueStrcut;

#define BUFFER_MAX   100

typedef __packed struct
{
	int headPosition;
	int tailPosition;
	uint8_t ringBuf[BUFFER_MAX];
}ringBuffer_t;

extern ext_game_status_t   gameState;
extern ext_event_data_t    eventState;  //ext_event_data_t的句柄为eventState
extern extGameRobotState_t robotState;
extern extRobotHurt_t      robotHurt;
extern extPowerHeatData_t  robotPowerHeat;
extern extShootData_t      robotShootData;
extern ringBuffer_t buffer;
extern  float Yaw_encoder;
extern  int Yaw_encoder_s;

float FloatValueSolveFunction(struct GetFloatValueStrcut *date);
float solve_chengfang(float x,int n);
int Transform_Hex_To_Oct(int data,int len);

void getGameState(uint8_t *stateData);
void getEventData(uint8_t *eventData);//
void getRobotState(uint8_t *stateData);
void getRobotHurt(uint8_t *hurtData);
void getRobotPowerHeat(uint8_t *powerHeartData);
void getRobotShootData(uint8_t *shootData);

void RingBuffer_Write(uint8_t data);
uint8_t   RingBuffer_Read(uint8_t *pdata);
#endif
