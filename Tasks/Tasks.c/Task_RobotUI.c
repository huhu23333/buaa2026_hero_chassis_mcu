/**
 * @file Task_RobotUI.c
 * @author Cyx(1696143358@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Task_RobotUI.h"
#include "PowerControl.h"
float CAP_Ratio =0.00;
char stringKeep[] = {"Shoot:\nRotate:\nAuto:\nCtrl:\n"};
char stringWarning[]={"Warning"};

//静态UI------------------------------------------------------------------
//弹道
void UI_Draw_Static_1() {
	UI_FUN.Line_Draw(&UI_Graph5.imageData[0], "001", UI_Graph_Add, 0, UI_Color_Green, 2,  840,  520,  1080,  520);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[1], "002", UI_Graph_Add, 0, UI_Color_Green, 2,  855,  480,  1065,  480);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[2], "003", UI_Graph_Add, 0, UI_Color_Green, 2,  870,  440,  1050,  440);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[3], "004", UI_Graph_Add, 0, UI_Color_Green, 2,  895,  400,  1035,  400);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[4], "005", UI_Graph_Add, 0, UI_Color_Green, 2,  960,  680,  960,  320);
	UI_FUN.UI_PushUp_Graphs(5, &UI_Graph5);
}
//字符
void UI_Draw_Static_2() {
	UI_FUN.Char_Draw(&UI_String1.String,"006",UI_Graph_Add, 3, UI_Color_Main, 24, sizeof(stringKeep) , 5,30 ,850 , stringKeep);
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Static_3() {
	UI_FUN.Circle_Draw(&UI_Graph1.imageData[0],"007",UI_Graph_Add, 1, UI_Color_White, 3, 1600, 800, 100);
	UI_FUN.UI_PushUp_Graphs(1, &UI_Graph1);
}
//动态初始UI---------------------------------------------------------
void UI_Draw_Dynamic_Init_1() {
	//电容电量 条：line 量：float
	UI_FUN.Line_Draw(&UI_Graph7.imageData[0], "101", UI_Graph_Add, 1, UI_Color_Green, 30, 560, 150, 560, 150);
	UI_FUN.Float_Draw(&UI_Graph7.imageData[1],"102", UI_Graph_Add, 1, UI_Color_White, 24, 2, 5, 1200, 200, 1.00);
	//射击速度
	UI_FUN.Float_Draw(&UI_Graph7.imageData[2],"103", UI_Graph_Add, 1, UI_Color_White, 20, 1, 5, 200, 850, ControlMes.shoot_Speed);
	//小陀螺速度
	UI_FUN.Float_Draw(&UI_Graph7.imageData[3],"104", UI_Graph_Add, 1, UI_Color_White, 20, 1, 5, 200, 810, ControlMes.z_rotation_velocity);
	//自瞄
	UI_FUN.Circle_Draw(&UI_Graph7.imageData[4],"105",UI_Graph_Add, 1, UI_Color_White, 15, 200, 765, 7);
	//变速
	UI_FUN.Circle_Draw(&UI_Graph7.imageData[5],"106",UI_Graph_Add, 1, UI_Color_White, 15, 200, 730, 7);
	//方向
	UI_FUN.Line_Draw(&UI_Graph7.imageData[6], "107", UI_Graph_Add, 1, UI_Color_Green, 5, 1600, 800, 1600+cos(0)*100, 800+sin(0)*100);
	UI_FUN.UI_PushUp_Graphs(7, &UI_Graph7);
}
//动态更新UI-----------------------------------------------------------------------------
void UI_Draw_Dynamic_1() {
	CAP_Ratio =  PowerRxData.capEnergy * 100.0f/255.0f;
	//超电
	UI_FUN.Line_Draw(&UI_Graph7.imageData[0], "101", UI_Graph_Change, 1, UI_Color_Green, 30, 560, 150, (int)(560 + 8*CAP_Ratio), 150);
	UI_FUN.Float_Draw(&UI_Graph7.imageData[1],"102", UI_Graph_Change, 1, UI_Color_White, 24, 1, 5, 1200, 200, CAP_Ratio);
	//射击速度
	if(ControlMes.fric_Flag == 0)
	{
		UI_FUN.Float_Draw(&UI_Graph7.imageData[2],"103", UI_Graph_Change, 1, UI_Color_White, 20, 1, 5, 200, 850, 1.0 * ControlMes.shoot_Speed);
	}
	else if( Dial_2006.realTorque >= 2000 ||Dial_2006.realTorque <= -2000)
	{
		UI_FUN.Float_Draw(&UI_Graph7.imageData[2],"103", UI_Graph_Change, 1, UI_Color_Yellow, 20, 1, 5, 200, 850, 1.0 * ControlMes.shoot_Speed);
	}
	else if(ControlMes.fric_Flag == 1)
	{
		UI_FUN.Float_Draw(&UI_Graph7.imageData[2],"103", UI_Graph_Change, 1, UI_Color_Green, 20, 1, 5, 200, 850, 1.0 * ControlMes.shoot_Speed);
	}
	//小陀螺速度
	UI_FUN.Float_Draw(&UI_Graph7.imageData[3],"104", UI_Graph_Change, 1, UI_Color_White, 20, 1, 5, 200, 810, Saber_Angle.Z_Vel);
	//自瞄
	if(ControlMes.AutoAimFlag == 0x01)
	{
		UI_FUN.Circle_Draw(&UI_Graph7.imageData[4],"105",UI_Graph_Change, 1, UI_Color_Green, 15, 200, 765, 7);
	}
	else
	{
		UI_FUN.Circle_Draw(&UI_Graph7.imageData[4],"105",UI_Graph_Change, 1, UI_Color_White, 15, 200, 765, 7);
	}
	//变速
	if(ControlMes.change_Flag == 0x01)
	{
		UI_FUN.Circle_Draw(&UI_Graph7.imageData[5],"106",UI_Graph_Change, 1, UI_Color_Green, 15, 200, 730, 7);
	}
	else
	{
		UI_FUN.Circle_Draw(&UI_Graph7.imageData[5],"106",UI_Graph_Change, 1, UI_Color_White, 15, 200, 730, 7);
	}
	//方向
	UI_FUN.Line_Draw(&UI_Graph7.imageData[6], "107", UI_Graph_Change, 1, UI_Color_Green, 5, 1600, 800, 1600+cos(2*PI*M6020s_Yaw.realAngle/8192 - PI/4)*100, 800+sin(2*PI*M6020s_Yaw.realAngle/8192 - PI/4)*100);
	UI_FUN.UI_PushUp_Graphs(7, &UI_Graph7);
}
void UI_Draw_Dynamic_2() {
	if(CAP_Ratio <= 50)
	{
		UI_FUN.Char_Draw(&UI_String2.String,"404",UI_Graph_Add, 3,UI_Color_Main ,24, sizeof(stringWarning) , 4 ,900 , 700 , stringWarning);
		UI_FUN.UI_PushUp_String(&UI_String2);
	}
	else
	{
		UI_FUN.Char_Draw(&UI_String2.String,"404",UI_Graph_Del, 3,UI_Color_Main ,24, sizeof(stringWarning) , 4 ,900 , 700 , stringWarning);
		UI_FUN.UI_PushUp_String(&UI_String2);
	}
}

void Robot_UI(void const *argument) {
	static u16 UI_PushUp_Counter = 0;
	vTaskDelay(300);
	TickType_t last_time;
    for (;;)
    {
		UI_FUN.ID_Judge();
		vTaskDelay(pdMS_TO_TICKS(35));
//静态UI及动态初始UI---------------------------------------------------------------------------------------------------------------------------
    	if(UI_PushUp_Counter % 100 == 0)
		{
			UI_Draw_Static_1();
			vTaskDelay(pdMS_TO_TICKS(35));
		}
    	if(UI_PushUp_Counter % 100 == 1)
		{
			UI_Draw_Static_2();
			vTaskDelay(pdMS_TO_TICKS(35));
		}
    	if(UI_PushUp_Counter % 100 == 2)
		{
			UI_Draw_Static_3();
			vTaskDelay(pdMS_TO_TICKS(35));
		}
    	if(UI_PushUp_Counter % 100 == 3)
		{
			UI_Draw_Dynamic_Init_1();
			vTaskDelay(pdMS_TO_TICKS(35));
		}
//动态更新UI-----------------------------------------------------------------------------
		if (UI_PushUp_Counter % 10 == 0) {
			UI_Draw_Dynamic_1();
		}
		if (UI_PushUp_Counter % 10 == 1) {
			UI_Draw_Dynamic_2();
		}
		UI_PushUp_Counter++;
	}
}
