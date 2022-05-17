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

//����һ������Ϊext_event_data_t
//����32λ����11λΪ����ǰ��ս��״̬
typedef __packed struct    
{
	uint16_t qianshao : 11 ; //��ǰ����ǰ��ս��״̬ 
}ext_event_data_t;

typedef __packed struct
{
	uint8_t sof;             //����֡ͷ���̶�0xA5
	uint16_t data_length;    //ÿ֡������data�ĳ��� 
	uint8_t seq;             //�����
	uint8_t crc8;            //֡ͷ��crcУ����
}frame_header_t;

typedef struct
{
	uint8_t robot_id;
	uint16_t stageRemainTime; //��ǰ�׶�ʣ��ʱ��
	uint8_t gameProgress;     //��ǰ���������ĸ��׶�
	uint8_t robotLevel;       //�����˵�ǰ�ȼ�
	uint16_t remainHP;        //�����˵�ǰѪ��
	uint16_t maxHP;           //��������Ѫ��
}extGameRobotState_t;  //����������״̬

typedef __packed struct
{
	uint8_t armorType; //��װ�ױ������Ѫʱ����ʾ�ܵ��˺���װ��ID
	uint8_t hurtType;  //Ѫ���仯����
}extRobotHurt_t; //�˺�����

typedef __packed struct
{
	uint8_t bulletType; //��������  1��17mm����   2��42mm����
	uint8_t bulletFreq; //������Ƶ
	float bulletSpeed;  //��������
}extShootData_t;  //ʵʱ�����Ϣ

typedef __packed struct
{
	float ChassisVolt;        //���������ѹ
	float ChassisCurrent;     //�����������
	float ChassisPower;       //�����������
	float ChassisPowerBuffer; //���̹��ʻ���
	uint16_t shooter_17_Heat;   //17mmǹ������
	uint16_t shooter_42_Heat;   //42mmǹ������
}extPowerHeatData_t;  //ʵʱ������������

typedef __packed struct
{
	uint8_t cardType;  //������
	uint8_t cardIdx;   //��������
}extRfidDetect_t; //���ؽ�������

typedef __packed struct
{
	uint8_t winner;
}extGameResult_t;  //����ʤ������

typedef __packed struct
{
	uint8_t buffType;     //buff����
	uint8_t buffAddition; //buff�ӳɱ�
}extGetBuff_t;  //Buff��ȡ����

typedef __packed struct
{
	float x;
	float y;
	float z;
	float yaw;        //ǹ�ڳ���Ƕ�ֵ
}extGameRobotPos_t; //������λ�ýǶ���Ϣ

typedef __packed struct
{
	float data1;
	float data2;
	float data3;
	uint8_t mask;
}extShowData_t;  //�������Զ�������

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
extern ext_event_data_t    eventState;  //ext_event_data_t�ľ��ΪeventState
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
