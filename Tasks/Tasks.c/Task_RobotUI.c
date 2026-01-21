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
char stringWarning[]={"Warning"};
uint16_t UI_Hurt_Show_Time_Counter = 0;
uint16_t UI_PushUp_Counter_Dynamic = 0;
uint16_t UI_PushUp_Counter_Static = 0;

#define UI_Dynamic_Max_Num 		10
#define UI_Static_Max_Num 		100

#define  UI_Hurt_Show_Time_MS 	3000

#define UI_Init_X_1				30
#define UI_Init_Y_1				850
#define UI_Line_Spacing_1 		30
#define UI_Font_Size_1 			18
#define UI_Line_Width_1 		2

void UI_Draw_Arrow(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, 
				   int32_t arrow_head_length, int32_t arrow_head_width, 
				   int8_t operate, int8_t layer, int16_t line_width, int8_t color, 
				   char *imgname1, char *imgname2, char *imgname3) {
	UI_FUN.Line_Draw(&UI_Graph5.imageData[0], imgname1, operate, layer, color, line_width, start_x, start_y, end_x, end_y);
	int32_t arrow_length = sqrt((end_x - start_x) * (end_x - start_x) + (end_y - start_y) * (end_y - start_y));
	int32_t head_lenth_vector_x = (start_x - end_x) * arrow_head_length / arrow_length;
	int32_t head_lenth_vector_y = (start_y - end_y) * arrow_head_length / arrow_length;
	int32_t width_vector_x = (end_y - start_y) / 2 * arrow_head_width / arrow_length;
	int32_t width_vector_y = (start_x - end_x) / 2 * arrow_head_width / arrow_length;
	UI_FUN.Line_Draw(&UI_Graph5.imageData[1], imgname2, operate, layer, color, line_width, 
					 end_x, end_y, end_x + head_lenth_vector_x + width_vector_x, end_y + head_lenth_vector_y + width_vector_y);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[2], imgname3, operate, layer, color, line_width, 
					 end_x, end_y, end_x + head_lenth_vector_x - width_vector_x, end_y + head_lenth_vector_y - width_vector_y);
	UI_Graph5.imageData[3].operate_tpye = 0;
	UI_Graph5.imageData[4].operate_tpye = 0;
	UI_FUN.UI_PushUp_Graphs(5, &UI_Graph5);
}

//静态UI------------------------------------------------------------------
//弹道
void UI_Draw_Init_1() {
	UI_FUN.Line_Draw(&UI_Graph5.imageData[0], "001", UI_Graph_Add, 0, UI_Color_Green, 2,  840,  520,  1080,  520);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[1], "002", UI_Graph_Add, 0, UI_Color_Green, 2,  855,  480,  1065,  480);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[2], "003", UI_Graph_Add, 0, UI_Color_Green, 2,  870,  440,  1050,  440);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[3], "004", UI_Graph_Add, 0, UI_Color_Green, 2,  895,  400,  1035,  400);
	UI_FUN.Line_Draw(&UI_Graph5.imageData[4], "005", UI_Graph_Add, 0, UI_Color_Green, 2,  960,  680,  960,  320);
	UI_FUN.UI_PushUp_Graphs(5, &UI_Graph5);
}
void UI_Draw_Init_2() {
	UI_FUN.Circle_Draw(&UI_Graph1.imageData[0],"007",UI_Graph_Add, 1, UI_Color_White, 3, 1600, 800, 100);
	UI_FUN.UI_PushUp_Graphs(1, &UI_Graph1);
}
//动态初始UI---------------------------------------------------------
void UI_Draw_Init_3() {
	//方向
	UI_FUN.Line_Draw(&UI_Graph1.imageData[0], "107", UI_Graph_Add, 1, UI_Color_Green, 5, 1600, 800, 1600+cos(0)*100, 800+sin(0)*100);
	UI_FUN.UI_PushUp_Graphs(1, &UI_Graph1);
}
//字符
void UI_Draw_Init_4() {
	if (ControlMes.tnndcolor == 1) {
		UI_FUN.Char_Draw(&UI_String1.String,"108",UI_Graph_Add, 3, UI_Color_Main, UI_Font_Size_1, 10, 
					  	 UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1, "Team: Red");
	} else {
		UI_FUN.Char_Draw(&UI_String1.String,"108",UI_Graph_Add, 3, UI_Color_Main, UI_Font_Size_1, 11, 
					  	 UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1, "Team: Blue");
	}
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_5() {
	if (ControlMes.fric_Flag == 0) {
		UI_FUN.Char_Draw(&UI_String1.String,"109",UI_Graph_Add, 3, UI_Color_Purplish_red, UI_Font_Size_1, 20, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1, "Friction Wheel: Off");
	} else {
		UI_FUN.Char_Draw(&UI_String1.String,"109",UI_Graph_Add, 3, UI_Color_Green, UI_Font_Size_1, 19, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1, "Friction Wheel: On");
	}
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_6() {
	if (ControlMes.AutoAimFlag == 0) {
		UI_FUN.Char_Draw(&UI_String1.String,"110",UI_Graph_Add, 3, UI_Color_Purplish_red, UI_Font_Size_1, 14, 
						 UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 2, "Auto Aim: Off");
		} else {
		UI_FUN.Char_Draw(&UI_String1.String,"110",UI_Graph_Add, 3, UI_Color_Green, UI_Font_Size_1, 13, 
						 UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 2, "Auto Aim: On");
	}
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_7() {
	if (ControlMes.change_Flag == 0) {
		UI_FUN.Char_Draw(&UI_String1.String,"111",UI_Graph_Add, 3, UI_Color_Purplish_red, UI_Font_Size_1, 18, 
						 UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 3, "Change Speed: Off");
	} else {
		UI_FUN.Char_Draw(&UI_String1.String,"111",UI_Graph_Add, 3, UI_Color_Green, UI_Font_Size_1, 17, 
						 UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 3, "Change Speed: On");
	}
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_8() {
	char temp_string[30];
	memset(temp_string, 0, sizeof(temp_string));
	sprintf(temp_string, "Shoot Speed: %dHz", ControlMes.shoot_Speed);
	UI_FUN.Char_Draw(&UI_String1.String,"112",UI_Graph_Add, 3, UI_Color_Orange, UI_Font_Size_1, sizeof(temp_string), 
					  UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 4, temp_string);
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_9() {
	char temp_string[30];
	memset(temp_string, 0, sizeof(temp_string));
	sprintf(temp_string, "Rotate Speed: %.1f Degree/s", Saber_Angle.Z_Vel);
	UI_FUN.Char_Draw(&UI_String1.String,"113",UI_Graph_Add, 3, UI_Color_Orange, UI_Font_Size_1, sizeof(temp_string), 
					  UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 5, temp_string);
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_10() {
	CAP_Ratio = ((float)PowerRxData.capEnergy) * 1.0f/255.0f;
	char temp_string[30];
	memset(temp_string, 0, sizeof(temp_string));
	sprintf(temp_string, "Super Capacitor: %.2f%%", CAP_Ratio * 100.0);
	UI_FUN.Char_Draw(&UI_String1.String,"114",UI_Graph_Add, 3, UI_Color_Orange, UI_Font_Size_1, sizeof(temp_string), 
					  UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 6, temp_string);
	UI_FUN.UI_PushUp_String(&UI_String1);
}
void UI_Draw_Init_11() {
	CAP_Ratio = ((float)PowerRxData.capEnergy) * 1.0f/255.0f;
	UI_FUN.Rectangle_Draw(&UI_Graph2.imageData[0], "115", UI_Graph_Add, 1, UI_Color_Orange, 2, 
						  UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 7, UI_Init_X_1 + 500, UI_Init_Y_1 - UI_Line_Spacing_1 * 8);
	UI_FUN.Line_Draw(&UI_Graph2.imageData[1], "116", UI_Graph_Add, 1, UI_Color_Orange, 20,
					 UI_Init_X_1 + 6 , UI_Init_Y_1 - (int)((float)UI_Line_Spacing_1 * 7.5), UI_Init_X_1 + (int)(6.0 + (494.0 - 6.0) * CAP_Ratio), UI_Init_Y_1 - (int)((float)UI_Line_Spacing_1 * 7.5));
	UI_FUN.UI_PushUp_Graphs(2, &UI_Graph2);
}
void UI_Draw_Init_12() {
	UI_Hurt_Show_Time_Counter = 65535;
	UI_Draw_Arrow(0,0,0,0,0,0, UI_Graph_Add, 4, 0, UI_Color_Black, "117", "118", "119");
}

//动态更新UI-----------------------------------------------------------------------------
void UI_Draw_Update_1() {
	//方向+超电
	static uint32_t changeable_value_1_last = 0;
	static uint32_t changeable_value_2_last = 0;
	static uint32_t changeable_value_3_last = 0;

	CAP_Ratio = ((float)PowerRxData.capEnergy) * 1.0f/255.0f;
	uint32_t changeable_value_1 = 1600+cos(2*PI*M6020s_Yaw.realAngle/8192 - PI/4)*100;
	uint32_t changeable_value_2 = 800+sin(2*PI*M6020s_Yaw.realAngle/8192 - PI/4)*100;
	uint32_t changeable_value_3 = UI_Init_X_1 + (int)(6.0 + (494.0 - 6.0) * CAP_Ratio);
	if (
		changeable_value_1 == changeable_value_1_last &&
		changeable_value_2 == changeable_value_2_last &&
		changeable_value_3 == changeable_value_3_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		changeable_value_2_last = changeable_value_2;
		changeable_value_3_last = changeable_value_3;
		UI_FUN.Line_Draw(&UI_Graph2.imageData[0], "107", UI_Graph_Change, 1, UI_Color_Green, 5, 1600, 800, changeable_value_1, changeable_value_2);
		UI_FUN.Line_Draw(&UI_Graph2.imageData[1], "116", UI_Graph_Change, 1, UI_Color_Orange, 20,
						UI_Init_X_1 + 6 , UI_Init_Y_1 - (int)((float)UI_Line_Spacing_1 * 7.5), changeable_value_3, UI_Init_Y_1 - (int)((float)UI_Line_Spacing_1 * 7.5));
		UI_FUN.UI_PushUp_Graphs(2, &UI_Graph2);
	}
}
void UI_Draw_Update_2() {
	static float changeable_value_1_last = 0;

	CAP_Ratio = ((float)PowerRxData.capEnergy) * 1.0f/255.0f;
	float changeable_value_1 = CAP_Ratio;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		if(changeable_value_1 <= 0.5)
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

}
void UI_Draw_Update_3() {
	static uint8_t changeable_value_1_last = 0;

	uint8_t changeable_value_1 = ControlMes.tnndcolor;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		if (changeable_value_1 == 1) {
			UI_FUN.Char_Draw(&UI_String1.String,"108",UI_Graph_Change, 3, UI_Color_Main, UI_Font_Size_1, 10, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1, "Team: Red");
		} else {
			UI_FUN.Char_Draw(&UI_String1.String,"108",UI_Graph_Change, 3, UI_Color_Main, UI_Font_Size_1, 11, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1, "Team: Blue");
		}
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_4() {
	static uint8_t changeable_value_1_last = 0;

	uint8_t changeable_value_1 = ControlMes.fric_Flag;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		if (changeable_value_1 == 0) {
			UI_FUN.Char_Draw(&UI_String1.String,"109",UI_Graph_Change, 3, UI_Color_Purplish_red, UI_Font_Size_1, 20, 
								UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1, "Friction Wheel: Off");
		} else {
			UI_FUN.Char_Draw(&UI_String1.String,"109",UI_Graph_Change, 3, UI_Color_Green, UI_Font_Size_1, 19, 
								UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1, "Friction Wheel: On");
		}
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_5() {
	static uint8_t changeable_value_1_last = 0;

	uint8_t changeable_value_1 = ControlMes.AutoAimFlag;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		if (changeable_value_1 == 0) {
			UI_FUN.Char_Draw(&UI_String1.String,"110",UI_Graph_Change, 3, UI_Color_Purplish_red, UI_Font_Size_1, 14, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 2, "Auto Aim: Off");
			} else {
			UI_FUN.Char_Draw(&UI_String1.String,"110",UI_Graph_Change, 3, UI_Color_Green, UI_Font_Size_1, 13, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 2, "Auto Aim: On");
		}
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_6() {
	static uint8_t changeable_value_1_last = 0;

	uint8_t changeable_value_1 = ControlMes.change_Flag;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		if (changeable_value_1 == 0) {
			UI_FUN.Char_Draw(&UI_String1.String,"111",UI_Graph_Change, 3, UI_Color_Purplish_red, UI_Font_Size_1, 18, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 3, "Change Speed: Off");
		} else {
			UI_FUN.Char_Draw(&UI_String1.String,"111",UI_Graph_Change, 3, UI_Color_Green, UI_Font_Size_1, 17, 
							UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 3, "Change Speed: On");
		}
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_7() {
	static uint8_t changeable_value_1_last = 0;

	uint8_t changeable_value_1 = ControlMes.shoot_Speed;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		char temp_string[30];
		memset(temp_string, 0, sizeof(temp_string));
		sprintf(temp_string, "Shoot Speed: %dHz", changeable_value_1);
		UI_FUN.Char_Draw(&UI_String1.String,"112",UI_Graph_Change, 3, UI_Color_Orange, UI_Font_Size_1, sizeof(temp_string), 
						UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 4, temp_string);
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_8() {
	static float32_t changeable_value_1_last = 0;

	float32_t changeable_value_1 = Saber_Angle.Z_Vel;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		char temp_string[30];
		memset(temp_string, 0, sizeof(temp_string));
		sprintf(temp_string, "Rotate Speed: %.1f Degree/s", changeable_value_1);
		UI_FUN.Char_Draw(&UI_String1.String,"113",UI_Graph_Change, 3, UI_Color_Orange, UI_Font_Size_1, sizeof(temp_string), 
						UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 5, temp_string);
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_9() {
	static float32_t changeable_value_1_last = 0;

	CAP_Ratio = ((float)PowerRxData.capEnergy) * 1.0f/255.0f;
	float32_t changeable_value_1 = CAP_Ratio;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		char temp_string[30];
		memset(temp_string, 0, sizeof(temp_string));
		sprintf(temp_string, "Super Capacitor: %.2f%%", changeable_value_1 * 100.0);
		UI_FUN.Char_Draw(&UI_String1.String,"114",UI_Graph_Change, 3, UI_Color_Orange, UI_Font_Size_1, sizeof(temp_string), 
						UI_Line_Width_1, UI_Init_X_1, UI_Init_Y_1 - UI_Line_Spacing_1 * 6, temp_string);
		UI_FUN.UI_PushUp_String(&UI_String1);
	}
}
void UI_Draw_Update_10() {
	static float32_t changeable_value_1_last = 0;

	float UI_Hurt_Angle = 2.0*PI*(float)M6020s_Yaw.realAngle/8192.0 + PI/2.0*(float)g_referee.hurt_data_.armor_id;
	float32_t changeable_value_1 = UI_Hurt_Angle;
	if (
		changeable_value_1 == changeable_value_1_last
	) {
		if ((UI_PushUp_Counter_Dynamic + 1) % UI_Dynamic_Max_Num != 0) {
			UI_PushUp_Counter_Dynamic += 1;
		}
	} else {
		changeable_value_1_last = changeable_value_1;
		if (UI_Hurt_Show_Time_Counter * 35 > UI_Hurt_Show_Time_MS) {
			UI_Hurt_Show_Time_Counter = UI_Hurt_Show_Time_MS / 35 + 1;
		} else {

		}
		{
			UI_Draw_Arrow(
				960 +cos(changeable_value_1)*200, 
				540 +sin(changeable_value_1)*200,
				960 +cos(changeable_value_1)*400, 
				540 +sin(changeable_value_1)*400,
				60,60, 
				UI_Graph_Change, 4, 2, UI_Color_Black, "117", "118", "119");
		}
	}
}


void Robot_UI(void const *argument) {
	vTaskDelay(300);
	TickType_t last_time;
    for (;;)
    {
		UI_FUN.ID_Judge();
		vTaskDelay(pdMS_TO_TICKS(35));
//静态UI及动态初始UI---------------------------------------------------------------------------------------------------------------------------
		int8_t extra_delay_flag = 1;
		switch (UI_PushUp_Counter_Static % UI_Static_Max_Num)
		{
			case 0:
				UI_Draw_Init_1();
				break;
			case 1:
				UI_Draw_Init_2();
				break;
			case 2:
				UI_Draw_Init_3();
				break;
			case 3:
				UI_Draw_Init_4();
				break;
			case 4:
				UI_Draw_Init_5();
				break;
			case 5:
				UI_Draw_Init_6();
				break;
			case 6:
				UI_Draw_Init_7();
				break;
			case 7:
				UI_Draw_Init_8();
				break;
			case 8:
				UI_Draw_Init_9();
				break;
			case 9:
				UI_Draw_Init_10();
				break;
			case 10:
				UI_Draw_Init_11();
				break;
			case 11:
				UI_Draw_Init_12();
				break;
		
			default:
				extra_delay_flag = 0;
				break;
		}
		if (extra_delay_flag) {
			vTaskDelay(pdMS_TO_TICKS(35));
			UI_Hurt_Show_Time_Counter += 1;
		}
//动态更新UI-----------------------------------------------------------------------------
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 0) {
			UI_Draw_Update_1();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 1) {
			UI_Draw_Update_2();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 2) {
			UI_Draw_Update_3();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 3) {
			UI_Draw_Update_4();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 4) {
			UI_Draw_Update_5();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 5) {
			UI_Draw_Update_6();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 6) {
			UI_Draw_Update_7();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 7) {
			UI_Draw_Update_8();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 8) {
			UI_Draw_Update_9();
		}
		if (UI_PushUp_Counter_Dynamic % UI_Dynamic_Max_Num == 9) {
			UI_Draw_Update_10();
		}
		UI_PushUp_Counter_Dynamic++;
		UI_PushUp_Counter_Static++;
		UI_Hurt_Show_Time_Counter += 1;
	}
}
