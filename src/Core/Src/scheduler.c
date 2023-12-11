///*
// * scheduler.c
// *
// *  Created on: Dec 2, 2023
// *      Author: Lenovo
// */
//
//
//#include "scheduler.h"
//sTasks SCH_tasks_G[SCH_MAX_TASKS];
//
//uint32_t current_index_task = 0;
//static uint32_t count_SCH_Update
//void SCH_Init(void){
////	unsigned char i;
////	for(i = 0 ; i < SCH_MAX_TASKS; i++){
////		SCH_Delete_Task(i);
////	}
//	current_index_task = 0;
//}
//
//void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
//	if(current_index_task < SCH_MAX_TASKS){
//		SCH_tasks_G[current_index_task].pTask = pFunction;
//		SCH_tasks_G[current_index_task].Delay = DELAY;
//		SCH_tasks_G[current_index_task].Period = PERIOD;
//		SCH_tasks_G[current_index_task].RunMe = 0;
//		SCH_tasks_G[current_index_task].TaskID = current_index_task;
//		current_index_task += 1;
//		//id--;
//	}
//}
//
//void SCH_Delete_Task(const uint32_t TASK_ID){
//	if(current_index_task <= TASK_ID) return;
//	else{
//		for(uint32_t i = TASK_ID; i < current_index_task - 1; i++){
////			SCH_tasks_G[current_index_task].pTask = pFunction;
////			SCH_tasks_G[current_index_task].Delay = DELAY;
////			SCH_tasks_G[current_index_task].Period = PERIOD;
////			SCH_tasks_G[current_index_task].RunMe = 0;
//
//			SCH_tasks_G[i].pTask = SCH_tasks_G[i+1].pTask;
//			SCH_tasks_G[i].Delay = SCH_tasks_G[i+1].Period;
//			SCH_tasks_G[i].Period = SCH_tasks_G[i+1].RunMe;
//			SCH_tasks_G[i].RunMe = SCH_tasks_G[i+1].Delay;
//		}
//		current_index_task -= 1;
//		//id--;
//	}
//}
//
//void SCH_Update(void){
//	for(int i = 0; i < current_index_task; i++){
//		if(SCH_tasks_G[i].Delay > 0){
//			SCH_tasks_G[i].Delay--;
//		}
//		else{
//			SCH_tasks_G[i].Delay = SCH_tasks_G[i].Period;
//			SCH_tasks_G[i].RunMe = 1;
//		}
//	}
//}
//
//void SCH_Dispatch_Tasks(void){
//	for(int i = 0; i < current_index_task; i++) {
//		if(SCH_tasks_G[i].RunMe > 0) {
//			(*SCH_tasks_G[i].pTask)();
//			SCH_tasks_G[i].RunMe--;
//			if(SCH_tasks_G[i].Period == 0){
//				SCH_Delete_Task(i);
//			}
//		}
//	}
//	// Enter low-power mode (Sleep mode). The MCU will wake up on the next interrupt
//	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//}





/*
 * scheduler.c
 *
 *  Created on: Nov 28, 2023
 *      Author: DELL
 */


#include "main.h"
#include "scheduler.h"


typedef struct {
	void (*pTask) (void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	uint32_t TaskID;
} sTask;

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint32_t newTaskID = 0;
static uint32_t count_SCH_Update = 0;

void SCH_Init(void){
    for (int i=0; i<SCH_MAX_TASKS; i++){
        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;

        SCH_tasks_G[i].TaskID = -1;
    }
}
static uint32_t Get_New_Task_ID(void){
	newTaskID++;
	if(newTaskID == NO_TASK_ID){
		newTaskID++;
	}
	return newTaskID;
}



uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t newTaskIndex = 0;
	uint32_t sumDelay = 0;
	uint32_t newDelay = 0;

	for (newTaskIndex = 0; newTaskIndex < SCH_MAX_TASKS; newTaskIndex++) {
		sumDelay = sumDelay + SCH_tasks_G[newTaskIndex].Delay;
		if (sumDelay > DELAY) {
			newDelay = DELAY - (sumDelay - SCH_tasks_G[newTaskIndex].Delay);
			SCH_tasks_G[newTaskIndex].Delay = sumDelay - DELAY;
			for(uint8_t i = SCH_MAX_TASKS - 1; i > newTaskIndex; i --){
				SCH_tasks_G[i].pTask = SCH_tasks_G[i - 1].pTask;
				SCH_tasks_G[i].Period = SCH_tasks_G[i - 1].Period;
				SCH_tasks_G[i].Delay = SCH_tasks_G[i - 1].Delay;
				SCH_tasks_G[i].TaskID = SCH_tasks_G[i - 1].TaskID;
			}
			SCH_tasks_G[newTaskIndex].pTask = pFunction;
			SCH_tasks_G[newTaskIndex].Delay = newDelay;
			SCH_tasks_G[newTaskIndex].Period = PERIOD;
			if (SCH_tasks_G[newTaskIndex].Delay == 0) {
				SCH_tasks_G[newTaskIndex].RunMe = 1;
			} else {
				SCH_tasks_G[newTaskIndex].RunMe = 0;
			}
			SCH_tasks_G[newTaskIndex].TaskID = Get_New_Task_ID();
			return SCH_tasks_G[newTaskIndex].TaskID;
		} else {
			if(SCH_tasks_G[newTaskIndex].pTask == 0x0000){
				SCH_tasks_G[newTaskIndex].pTask = pFunction;
				SCH_tasks_G[newTaskIndex].Delay = DELAY - sumDelay;
				SCH_tasks_G[newTaskIndex].Period = PERIOD;
				if(SCH_tasks_G[newTaskIndex].Delay == 0){
					SCH_tasks_G[newTaskIndex].RunMe = 1;
				} else {
					SCH_tasks_G[newTaskIndex].RunMe = 0;
				}
				SCH_tasks_G[newTaskIndex].TaskID = Get_New_Task_ID();
				return SCH_tasks_G[newTaskIndex].TaskID;
			}
		}
	}
	return SCH_tasks_G[newTaskIndex].TaskID;
}


uint8_t SCH_Delete_Task(uint32_t taskID){
	uint8_t Return_code  = 0;
	uint8_t taskIndex;
	uint8_t j;
	if(taskID != NO_TASK_ID){
		for(taskIndex = 0; taskIndex < SCH_MAX_TASKS; taskIndex ++){
			if(SCH_tasks_G[taskIndex].TaskID == taskID){
				Return_code = 1;
				if(taskIndex != 0 && taskIndex < SCH_MAX_TASKS - 1){
					if(SCH_tasks_G[taskIndex+1].pTask != 0x0000){
						SCH_tasks_G[taskIndex+1].Delay += SCH_tasks_G[taskIndex].Delay;
					}
				}

				for( j = taskIndex; j < SCH_MAX_TASKS - 1; j ++){
					SCH_tasks_G[j].pTask = SCH_tasks_G[j+1].pTask;
					SCH_tasks_G[j].Period = SCH_tasks_G[j+1].Period;
					SCH_tasks_G[j].Delay = SCH_tasks_G[j+1].Delay;
					SCH_tasks_G[j].RunMe = SCH_tasks_G[j+1].RunMe;
					SCH_tasks_G[j].TaskID = SCH_tasks_G[j+1].TaskID;
				}
				SCH_tasks_G[j].pTask = 0;
				SCH_tasks_G[j].Period = 0;
				SCH_tasks_G[j].Delay = 0;
				SCH_tasks_G[j].RunMe = 0;
				SCH_tasks_G[j].TaskID = 0;
				return Return_code;
			}
		}
	}
	return Return_code;
}


void SCH_Update(void) {
	count_SCH_Update++;
	// O (1)
	if (SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0) {
		if(SCH_tasks_G[0].Delay > 0){
			SCH_tasks_G[0].Delay -= 1;
		}
		if (SCH_tasks_G[0].Delay == 0) {
			SCH_tasks_G[0].RunMe = 1;
		}
	}
}


void SCH_Dispatch_Tasks(void){
	if(SCH_tasks_G[0].RunMe > 0) {
		(*SCH_tasks_G[0].pTask)();
		SCH_tasks_G[0].RunMe = 0;
		sTask temtask = SCH_tasks_G[0];
		SCH_Delete_Task(temtask.TaskID);
		if (temtask.Period != 0) {
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period);
		}
	}
}



