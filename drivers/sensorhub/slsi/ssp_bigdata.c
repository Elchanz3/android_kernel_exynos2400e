/*
 *  Copyright (C) 2024, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
#include <linux/init.h>
#include "ssp_bigdata.h"
#include "ssp_type_define.h"
#include "ssp_iio.h"

#define BIGDATA_LENGTH_SIZE		(2)
#define BIGDATA_TYPE_SIZE		(1)

#ifdef CONFIG_SENSORS_SSP_BIGDATA_V2 
int ssp_big_queue_work(struct ssp_data *data, u8 event, bool isNofity)
{
	bool ret;

	if (event == SSP_BIG_CRASH_MINIDUMP) {
		data->bigdata_event = event;
		ret = queue_work(data->bigdata_wq, &data->work_bigdata);	
	} else if (event == SSP_BIG_SENSOR_INIT_STATE) {
		if (!isNofity) {
			nanohub_info("[SSP] change reset type %d -> none\n", data->reset_type);			
			data->reset_type = RESET_TYPE_NONE;
		}
		ret = queue_work(data->bigdata_wq, &data->work_bigdata_info);		
	} else {
		return 0;
	}
	nanohub_info("[SSP]: call ssp_big_queue_work (%d) : %d\n", event, ret);

	return 1;
}

void report_wakeup_info(struct ssp_data *data, char lib_num, char inst)	
{
	char buffer[5] = {0,};
	u16 size = 2;

	if (lib_num == SH_LIBRARY_WRIST_UP)
		return;

	memcpy(&buffer[0], &size, 2);
	buffer[2] = SSP_BIG_WAKEUP_INFO;

	buffer[3] = lib_num;

	// replace lib num when lib_num = 0xFF
	if (lib_num == 0xff) {
		buffer[3] = 0xf0;

		switch (inst) {
		case MSG2AP_INST_BYPASS_DATA:
			break;			
		case MSG2AP_INST_DEBUG_DATA:
		case MSG2AP_INST_LIBRARY_DATA:
		case MSG2AP_INST_BIG_DATA:
		case MSG2AP_INST_META_DATA:
		case MSG2AP_INST_TIME_SYNC:
		case MSG2AP_INST_RESET:
		case MSG2AP_INST_DUMP_DATA:
		case MSG2AP_INST_GYRO_CAL:
#ifdef CONFIG_SENSORS_SSP_MAGNETIC_SENSOR
		case MSG2AP_INST_MAG_CAL:
#endif
#ifdef CONFIG_SENSORS_SSP_HOTKET
		case MSG2AP_INST_FUNC_KEY_STATUS:
#endif
		case MSG2AP_INST_ACC_ESN:
			buffer[3] |= ((u8)inst & 0x0f);			
			break;
		default:
			buffer[3] = 0xff;			
			break;
		}
		ssp_infof("inst=%d(%d)", buffer[0], inst);
	}
	report_scontext_big_data(data, buffer, size + 2);

	//ssp_infof("size=%d", sizeof(buffer));
}

static void bigdata_task(struct work_struct *work)
{
	struct ssp_data *data = container_of((struct work_struct *)work,
							struct ssp_data, work_bigdata);
	int size = 0;
	char *buf = NULL;
	u8 errType = (u8)data->chub_err_type;

	buf = kzalloc(1024, GFP_KERNEL);
	memset(buf, 0, 1024);

	buf[2] = data->bigdata_event;

	switch(data->bigdata_event) {
		case SSP_BIG_CRASH_MINIDUMP:
			size = BIGDATA_TYPE_SIZE +
				sizeof(data->crash_mini_dump.gpr_dump) + sizeof(data->crash_mini_dump.hardfault_info) + 
				sizeof(errType);
			memcpy(buf, &size, BIGDATA_LENGTH_SIZE);
			buf[3] = errType;
			memcpy(&buf[4], (void *)&data->crash_mini_dump.gpr_dump, size - BIGDATA_TYPE_SIZE - sizeof(errType));		
			break;
		case SSP_BIG_ESN:
			size = BIGDATA_TYPE_SIZE + sizeof(data->acc_esn);
			memcpy(buf, &size, BIGDATA_LENGTH_SIZE);
			memcpy(&buf[3], (void *)data->acc_esn, size - BIGDATA_TYPE_SIZE);	
			break;
		default:
			break;
	}
	report_scontext_big_data(data, buf, size + BIGDATA_LENGTH_SIZE);

	ssp_infof("size=%d event=%u)", size, buf[2]);

	kfree(buf);

	return;
}

static void bigdata_info_task(struct work_struct *work)
{
	struct ssp_data *data = container_of((struct work_struct *)work,
							struct ssp_data, work_bigdata_info);
	int size = 0;
	char buf[20] = {0, };

	size = BIGDATA_TYPE_SIZE + 1 + sizeof(data->uSensorState) + 1;
	memcpy(&buf[0], &size, BIGDATA_LENGTH_SIZE);
	buf[2] = SSP_BIG_SENSOR_INIT_STATE;
	buf[3] = convert_reset_type_to_reason(data->reset_type);
	memcpy(&buf[4], (void *)&data->uSensorState, sizeof(data->uSensorState));		
	buf[4 + sizeof(data->uSensorState)] = SSP_BIG_VERSION;

	report_scontext_big_data(data, buf, size + BIGDATA_LENGTH_SIZE);

	ssp_infof("size=%d event=%u)", size, buf[2]);

	return;
}

int initialize_bigdata_work(struct ssp_data *data)
{
	data->bigdata_wq = create_singlethread_workqueue("bigdata_wq");
	if (!data->bigdata_wq)
		return ERROR;

	INIT_WORK(&data->work_bigdata, bigdata_task);
	INIT_WORK(&data->work_bigdata_info, bigdata_info_task);

	return SUCCESS;
}
#endif

