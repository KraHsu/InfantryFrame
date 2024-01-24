/*
 *                                                     __----~~~~~~~~~~~------___
 *                                    .  .   ~~//====......          __--~ ~~
 *                    -.            \_|//     |||\\  ~~~~~~::::... /~
 *                 ___-==_       _-~o~  \/    |||  \\            _/~~-
 *         __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 *     _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 *   .~       .~       |   \\ -_    /  /-   /   ||      \   /
 *  /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 *  |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 *           '         ~-|      /|    |-~\~~       __--~~
 *                       |-~~-_/ |    |   ~\_   _-~            /\
 *                            /  \     \__   \/~                \__
 *                        _--~ _/ | .-~~____--~-/                  ~~==.
 *                       ((->/~   '.|||' -_|    ~~-/ ,              . _||
 *                                  -_     ~\      ~~---l__i__i__i--~~_/
 *                                  _-~-__   ~)  \--______________--~~
 *                                //.-~~~-~_--~- |-------~~~~~~~~
 *                                       //.-~~~--\
 *                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *                               神兽保佑            永无BUG
 *
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-0123 15:54:08
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */

#include "referee.h"
#include "stm32g4xx.h"

namespace Referee {
    referee_interface refereeRxData{&huart2, 1};

    infantry::UsartDataType *referee_interface::rxCallback( ) {
        Referee_RXCallback(&huart2);
        return this;
    }

    void registerRxCallBack( ) {
        if (Const_Referee_UART_HANDLER == nullptr) {
            return;
        }
        infantry::UsartDeviceRxDataMap[
            Const_Referee_UART_HANDLER == &huart1 ? 0
                                                  : Const_Referee_UART_HANDLER == &huart2 ? 1 : 2
        ] = &refereeRxData;
    }
}

Referee::Referee_RefereeDataTypeDef Referee_RefereeData;

namespace Referee {
    UART_HandleTypeDef *Const_Referee_UART_HANDLER;
#define Const_Referee_TX_BUFF_LEN             300
#define Const_Referee_RX_BUFF_LEN             300
#define Const_Referee_REMOTE_OFFLINE_TIME     1000
    uint8_t Referee_TxData[Const_Referee_TX_BUFF_LEN];
    uint8_t Referee_RxData[Const_Referee_RX_BUFF_LEN];
// Device header
    const uint8_t PARSE_FAILED = 0, PARSE_SUCCEEDED = 1;
    Summoner_Data_t Summoner_Data;

/********** REFEREE CMD PARSER FUNCTION **********/
    uint8_t P_ext_game_status(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_game_status_t *) data_ptr;

        referee->game_type = struct_ptr->game_type;
        referee->game_progress = struct_ptr->game_progress;
        referee->stage_remain_time = struct_ptr->stage_remain_time;
        referee->SyncTimeStamp = struct_ptr->SyncTimeStamp;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_game_result(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        //ext_game_result_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_game_robot_HP(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_game_robot_HP_t *) data_ptr;

        referee->red_1_robot_HP = struct_ptr->red_1_robot_HP;
        referee->red_2_robot_HP = struct_ptr->red_2_robot_HP;
        referee->red_3_robot_HP = struct_ptr->red_3_robot_HP;
        referee->red_4_robot_HP = struct_ptr->red_4_robot_HP;
        referee->red_5_robot_HP = struct_ptr->red_5_robot_HP;
        referee->red_7_robot_HP = struct_ptr->red_7_robot_HP;
        referee->red_outpost_HP = struct_ptr->red_outpost_HP;
        referee->red_base_HP = struct_ptr->red_base_HP;

        referee->blue_1_robot_HP = struct_ptr->blue_1_robot_HP;
        referee->blue_2_robot_HP = struct_ptr->blue_2_robot_HP;
        referee->blue_3_robot_HP = struct_ptr->blue_3_robot_HP;
        referee->blue_4_robot_HP = struct_ptr->blue_4_robot_HP;
        referee->blue_5_robot_HP = struct_ptr->blue_5_robot_HP;
        referee->blue_7_robot_HP = struct_ptr->blue_7_robot_HP;
        referee->blue_outpost_HP = struct_ptr->blue_outpost_HP;
        referee->blue_base_HP = struct_ptr->blue_base_HP;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_event_data(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_event_data_t *) data_ptr;

        referee->event_type = struct_ptr->event_type;

        referee->SelfCharge_1_OccupyStatus = (struct_ptr->event_type >> 31) & 0x01;
        referee->SelfCharge_2_OccupyStatus = (struct_ptr->event_type >> 30) & 0x01;
        referee->SelfCharge_3_OccupyStatus = (struct_ptr->event_type >> 29) & 0x01;
        referee->SelfEnergyBuff_ShootingPoint_OccupyStatus = (struct_ptr->event_type >> 28) & 0x01;
        referee->SelfSmallEnergyBuff_ActivateStatus = (struct_ptr->event_type >> 27) & 0x01;
        referee->SelfBigEnergyBuff_ActivateStatus = (struct_ptr->event_type >> 26) & 0x01;
        referee->SelfR2_B2_CircularHighland_OccupypStatus = (struct_ptr->event_type >> 25) & 0x01;
        referee->SelfR3_B3_Trapezoidal_OccupypStatus = (struct_ptr->event_type >> 24) & 0x01;
        referee->SelfR4_B4_Trapezoidal_OccupypStatus = (struct_ptr->event_type >> 23) & 0x01;
        referee->SelfBase_Shield_Status = (struct_ptr->event_type >> 22) & 0x01;
        referee->SelfOutpost_Status = (struct_ptr->event_type >> 21) & 0x01;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_supply_projectile_action(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        //ext_supply_projectile_action_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_referee_warning(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        //ext_referee_warning_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_dart_remaining_time(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
//    ext_dart_remaining_time_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_game_robot_status(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_game_robot_status_t *) data_ptr;

        referee->robot_level = struct_ptr->robot_level;
        referee->remain_HP = struct_ptr->remain_HP;
        referee->max_HP = struct_ptr->max_HP;
        referee->chassis_power_limit = struct_ptr->chassis_power_limit;

        referee->mains_power_gimbal_output = struct_ptr->mains_power_gimbal_output;
        referee->mains_power_chassis_output = struct_ptr->mains_power_chassis_output;
        referee->mains_power_shooter_output = struct_ptr->mains_power_shooter_output;

        referee->shooter_id1_17mm_cooling_rate = struct_ptr->shooter_id1_17mm_cooling_rate;
        referee->shooter_id2_17mm_cooling_rate = struct_ptr->shooter_id2_17mm_cooling_rate;
        referee->shooter_id1_17mm_cooling_limit = struct_ptr->shooter_id1_17mm_cooling_limit;
        referee->shooter_id2_17mm_cooling_limit = struct_ptr->shooter_id2_17mm_cooling_limit;
        referee->shooter_id1_17mm_speed_limit = struct_ptr->shooter_id1_17mm_speed_limit;
        referee->shooter_id2_17mm_speed_limit = struct_ptr->shooter_id2_17mm_speed_limit;

        if (referee->robot_id != struct_ptr->robot_id) {
            referee->robot_id = struct_ptr->robot_id;
            referee->client_id = Referee_GetClientIDByRobotID(referee->robot_id);
        }

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_power_heat_data(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_power_heat_data_t *) data_ptr;

        referee->chassis_power = struct_ptr->chassis_power;
        referee->chassis_power_buffer = struct_ptr->chassis_power_buffer;
        referee->shooter_id1_17mm_cooling_heat = struct_ptr->shooter_id1_17mm_cooling_heat;
        referee->shooter_id2_17mm_cooling_heat = struct_ptr->shooter_id2_17mm_cooling_heat;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_game_robot_pos(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
//    ext_game_robot_pos_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_buff(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
//    ext_buff_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_aerial_robot_energy(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        // aerial_robot_energy_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_robot_hurt(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_robot_hurt_t *) data_ptr;

        referee->armor_id = struct_ptr->armor_id;
        referee->hurt_type = struct_ptr->hurt_type;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_shoot_data(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_shoot_data_t *) data_ptr;

        referee->bullet_type = struct_ptr->bullet_type;
        referee->bullet_id = struct_ptr->bullet_id;
        referee->bullet_freq = struct_ptr->bullet_freq;
        referee->bullet_speed = struct_ptr->bullet_speed;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_bullet_remaining(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_bullet_remaining_t *) data_ptr;

        referee->bullet_remaining_num_17mm = struct_ptr->bullet_remaining_num_17mm;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_rfid_status(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        //ext_rfid_status_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_dart_cmd(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
//	  ext_dart_client_cmd_t *struct_ptr = data_ptr;

        return PARSE_SUCCEEDED;
    }

    uint8_t P_ext_robot_cmd(Referee_RefereeDataTypeDef *referee, void *data_ptr) {
        auto *struct_ptr = (ext_robot_cmd_t *) data_ptr;

        referee->command_keyboard = struct_ptr->command_keyboard;
        referee->destination_x = struct_ptr->target_position_x;
        referee->destination_z = struct_ptr->target_position_z;
        return PARSE_SUCCEEDED;
    }

/********** END OF REFEREE CMD PARSER FUNCTION **********/


    const uint16_t Const_Referee_FRAME_HEADER_SOF = 0xA5;     // 裁判系统指令帧头长度
    const Referee_RobotAndClientIDTypeDef   // 机器人ID及对应客户端ID，0表示无对应客户端
    HERO_RED = {1, 0x0101},    // 英雄(红)
    ENGINEER_RED = {2, 0x0102},    // 工程(红)
    INFANTRY3_RED = {3, 0x0103},    // 步兵3(红)
    INFANTRY4_RED = {4, 0x0104},    // 步兵4(红)
    INFANTRY5_RED = {5, 0x0105},    // 步兵5(红)
    AERIAL_RED = {6, 0x0106},    // 空中(红)
    SENTRY_RED = {7, 0},         // 哨兵(红)
    HERO_BLUE = {101, 0x0165},    // 英雄(蓝)
    ENGINEER_BLUE = {102, 0x0166},    // 工程(蓝)
    INFANTRY3_BLUE = {103, 0x0167},    // 步兵3(蓝)
    INFANTRY4_BLUE = {104, 0x0168},    // 步兵4(蓝)
    INFANTRY5_BLUE = {105, 0x0169},    // 步兵5(蓝)
    AERIAL_BLUE = {106, 0x016A},    // 空中(蓝)
    SENTRY_BLUE = {107, 0};         // 哨兵(蓝)

#define Const_Referee_CMD_NUM                 20       // 裁判系统指令个数（不含交互指令）
    const Referee_RefereeCmdTypeDef Const_Referee_CMD_LIST[Const_Referee_CMD_NUM] = {           // 裁判系统消息命令ID列表
        {0x0001, 11, &P_ext_game_status},                // 比赛状态数据，1Hz 周期发送
        {0x0002, 1,  &P_ext_game_result},                // 比赛结果数据，比赛结束后发送
        {0x0003, 28, &P_ext_game_robot_HP},              // 比赛机器人血量数据，1Hz 周期发送
        {0x0005, 11, nullptr},                              // （未使用）人工智能挑战赛加成与惩罚区状态，1Hz周期发送
        {0x0101, 4,  &P_ext_event_data},                 // 场地事件数据，事件改变后发送
        {0x0102, 4,  &P_ext_supply_projectile_action},   // 场地补给站动作标识数据，动作改变后发送
        {0x0103, 2,  nullptr},                              // （已废弃）请求补给站补弹数据，由参赛队发送，上限 10Hz。（RM 对抗赛尚未开放）
        {0x0104, 2,  &P_ext_referee_warning},            // 裁判警告数据，警告发生后发送
        {0x0105, 1,  &P_ext_dart_remaining_time},        // 飞镖发射口倒计时，1Hz周期发送
        {0x0201, 15, &P_ext_game_robot_status},          // 机器人状态数据，10Hz 周期发送
        {0x0202, 14, &P_ext_power_heat_data},            // 实时功率热量数据，50Hz 周期发送
        {0x0203, 16, &P_ext_game_robot_pos},             // 机器人位置数据，10Hz 发送
        {0x0204, 1,  &P_ext_buff},                       // 机器人增益数据，1Hz 周期发送
        {0x0205, 3,  &P_aerial_robot_energy},            // 空中机器人能量状态数据，10Hz 周期发送，只有空中机器人主控发送
        {0x0206, 1,  &P_ext_robot_hurt},                 // 伤害状态数据，伤害发生后发送
        {0x0207, 7,  &P_ext_shoot_data},                 // 实时射击数据，子弹发射后发送
        {0x0208, 2,  &P_ext_bullet_remaining},           // 弹丸剩余发射数，仅空中机器人，哨兵机器人以及ICRA机器人发送，1Hz周期发送
        {0x0209, 4,  &P_ext_rfid_status},                // 机器人RFID状态，1Hz周期发送
        {0x020A, 12, &P_ext_dart_cmd},                   // 飞镖机器人客户端指令书，10Hz周期发送
        {0x0303, 15, &P_ext_robot_cmd},                  // 云台手小地图信息（V1.4新增）
    };

    const Referee_RefereeCmdTypeDef Const_Referee_CMD_INTERACTIVE = {0x0301, 6, nullptr};    // 机器人间交互数据，发送方触发发送
// 注：这里的6是交互数据帧头的长度，因为交互数据帧是不定长的
//const uint16_t Const_Referee_DATA_CMD_ID_CLIENT_CUSTOM_DATA       = 0xD180;               // （已废弃）客户端自定义数据内容ID
    const uint16_t Const_Referee_DATA_CMD_ID_INTERACTIVE_DATA_LBOUND = 0x0200;               // 机器人间交互数据内容ID下界
    const uint16_t Const_Referee_DATA_CMD_ID_INTERACTIVE_DATA_UBOUND = 0x02FF;               // 机器人间交互数据内容ID上界
    const uint16_t Const_Referee_DATA_INTERACTIVE_DATA_MAX_LENGTH = 113 - 1;              // 机器人间交互数据内容最大长度
    const uint16_t Const_Referee_GRAPHIC_BUFFER_MAX_LENGTH = 21;                   // 图形缓冲区最大长度
    const Referee_RefereeCmdTypeDef Const_Referee_DATA_CMD_ID_LIST[6] = {                     // 裁判系统交互数据内容ID
        {0x0100, 2,   nullptr},              // 客户端删除图形
        {0x0101, 15,  nullptr},              // 客户端绘制一个图形
        {0x0102, 30,  nullptr},              // 客户端绘制二个图形
        {0x0103, 75,  nullptr},              // 客户端绘制五个图形
        {0x0104, 105, nullptr},              // 客户端绘制七个图形
        {0x0105, 45,  nullptr}               // 客户端绘制字符图形
    };


/**
  * @brief      获取裁判系统数据对象的指针
  * @param      无
  * @retval     指针指向裁判系统数据对象
  */
    Referee_RefereeDataTypeDef *Referee_GetRefereeDataPtr( ) {
        return &Referee_RefereeData;
    }


/**
  * @brief      重置裁判系统数据对象
  * @param      无
  * @retval     无
  */
    void Referee_ResetRefereeData( ) {
        Referee_RefereeDataTypeDef *referee = Referee_GetRefereeDataPtr();
        Referee_RefereeStateEnum state = referee->state;        // backup state
        uint32_t last_update_time = referee->last_update_time;  // backup time
        memset(referee, 0, sizeof(Referee_RefereeDataTypeDef));
        referee->state = state;
        referee->last_update_time = last_update_time;
    }

    HAL_StatusTypeDef Uart_ReceiveDMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        /* Check that a Rx process is not already ongoing */
        if (huart->RxState == HAL_UART_STATE_READY) {
            if ((pData == nullptr) || (Size == 0U)) {
                return HAL_ERROR;
            }

            __HAL_LOCK(huart);

            /* Set Reception type to Standard reception */
            huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

            if (!(IS_LPUART_INSTANCE(huart->Instance))) {
                /* Check that USART RTOEN bit is set */
                if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U) {
                    /* Enable the UART Receiver Timeout Interrupt */
                    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
                }
            }

            return (UART_Start_Receive_DMA(huart, pData, Size));
        } else {
            return HAL_BUSY;
        }
    }

/**
  * @brief      初始化裁判系统
  * @param      无
  * @retval     无
  */
    void init(UART_HandleTypeDef *huart) {
        Const_Referee_UART_HANDLER = huart;
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        Referee_ResetRefereeData();
        referee->last_update_time = HAL_GetTick();
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
        Uart_ReceiveDMA(Const_Referee_UART_HANDLER, Referee_RxData, Const_Referee_RX_BUFF_LEN);
    }


/**
  * @brief      通过机器人ID获取对应客户端ID
  * @param      robot_id: 机器人ID
  * @retval     客户端ID
  */
    uint16_t Referee_GetClientIDByRobotID(uint8_t robot_id) {
        if (robot_id == 7 || robot_id == 107) { return 0; }
        if ((robot_id >= 1 && robot_id <= 6) || (robot_id >= 101 && robot_id <= 106)) {
            return robot_id + 0x100;
        }
        return 0;
    }

    void Uart_ErrorHandler(uint32_t ret) {
        while (true) {

        }
    }

    void Uart_SendMessage(UART_HandleTypeDef *huart, uint8_t txdata[], uint16_t size, uint32_t timeout) {
        /* Start the Transmission process */
        uint32_t ret = HAL_UART_Transmit(huart, txdata, size, timeout);
        if (ret != HAL_OK) {
            /* Transmission request Error */
            Uart_ErrorHandler(ret);
        }
    }

/**
  * @brief      裁判系统交互数据发送函数（阻塞）
  * @param      data_cmd_id: 数据内容ID
  * @param      receiver_ID: 接受者ID
  * @param      interactive_data: 交互数据帧
  * @param      interactive_data_length: 交互数据帧长度
  * @retval     无
  */
    void Referee_SendInteractiveData(
        uint16_t data_cmd_id, uint16_t receiver_ID, uint8_t *interactive_data, uint16_t interactive_data_length
    ) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        static uint8_t bag_count = 0;
        uint8_t *buf = Referee_TxData;
        buf[0] = Const_Referee_FRAME_HEADER_SOF;//0xA5

        uint16_t *data_length_ptr = static_cast<uint16_t *>((void *) (buf + 1));//初始化指针指向的位置
        *data_length_ptr = interactive_data_length;//赋值

        uint8_t *seq_ptr = static_cast<uint8_t *>((void *) (buf + 3));
        *seq_ptr = bag_count;   // not obvious in doc

        if (bag_count != 255) {
            bag_count++;
        } else {
            bag_count = 0;
        }

        uint8_t *crc8_ptr = static_cast<uint8_t *>((void *) (buf + 4));
        *crc8_ptr = CRC_GetCRC8CheckSum(buf, 4, CRC8_INIT);

        buf[5] = 0x01;
        buf[6] = 0x03;//前七字节为校验码

        ext_student_interactive_header_data_t *header = static_cast<ext_student_interactive_header_data_t *>((void *) (
            buf +
            7));
        header->data_cmd_id = data_cmd_id;
        header->sender_ID = (uint16_t) referee->robot_id;
        header->receiver_ID = receiver_ID;//包头6字节

        memcpy(buf + 5 + 2 + Const_Referee_CMD_INTERACTIVE.data_length, interactive_data, interactive_data_length);

        uint16_t *crc16_ptr = static_cast<uint16_t *>((void *) (buf + 5 + 2 + *data_length_ptr));
        *crc16_ptr = CRC_GetCRC16CheckSum(buf, 5 + 2 + *data_length_ptr, CRC16_INIT);//结尾加上16位校验码

        uint16_t tx_size = 5 + 2 + 8 + 2;//*data_length_ptr
        Uart_SendMessage(Const_Referee_UART_HANDLER, buf, tx_size, 15);
    }

/**
  * @brief      机器人间交互数据发送函数
  * @param      data_cmd_id: 数据内容ID
  * @param      receiver_ID: 接受者ID
  * @param      data: 数据帧
  * @param      data_length: 数据帧长度
  * @retval     无
  */
    void Referee_SendRobotCustomData(uint16_t data_cmd_id, uint16_t receiver_ID, uint8_t *data, uint16_t data_length) {
        if (data_cmd_id < Const_Referee_DATA_CMD_ID_INTERACTIVE_DATA_LBOUND ||
            data_cmd_id > Const_Referee_DATA_CMD_ID_INTERACTIVE_DATA_UBOUND) {
            return;
        }                   // wrong data cmd id
        if (receiver_ID == 0 || (receiver_ID > 10 && receiver_ID < 100) ||
            receiver_ID > 107) { return; }   // wrong receiver id
        if (data_length > Const_Referee_DATA_INTERACTIVE_DATA_MAX_LENGTH) {
            return;
        }                       // interactive data too long
        Referee_SendInteractiveData(data_cmd_id, receiver_ID, data, data_length);
    }

/**
  * @brief      发送客户端自定义图形命令组
  * @param      graph: 数组包括指定数量个图形命令
  * @param      mode: 发送模式，1、2、3、4对应1、2、5、7个一组
  * @retval     无
  */
    void Referee_SendDrawingCmd(graphic_data_struct_t graph[], uint8_t mode) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        if (mode == 0 || mode >= 5) { return; }

        uint8_t buf[120], cellsize = sizeof(graphic_data_struct_t);
        if (mode >= 1) {
            memcpy(buf, graph, cellsize);
        }
        if (mode >= 2) {
            memcpy(buf + cellsize, graph + 1, cellsize);
        }
        if (mode >= 3) {
            memcpy(buf + cellsize * 2, graph + 2, cellsize);
            memcpy(buf + cellsize * 3, graph + 3, cellsize);
            memcpy(buf + cellsize * 4, graph + 4, cellsize);
        }
        if (mode >= 4) {
            memcpy(buf + cellsize * 5, graph + 5, cellsize);
            memcpy(buf + cellsize * 6, graph + 6, cellsize);
        }

        Referee_SendInteractiveData(
            Const_Referee_DATA_CMD_ID_LIST[mode].cmd_id, referee->client_id,
            buf, Const_Referee_DATA_CMD_ID_LIST[mode].data_length
        );
    }


/**
  * @brief      发送客户端自定义图形显示字符串命令
  * @param      pgraph: 指针指向显示字符串图形命令
  * @param      str: 字符串（定长为30）
  * @retval     无
  */
    void Referee_SendDrawingStringCmd(graphic_data_struct_t *pgraph, uint8_t str[]) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;

        ext_client_custom_character_t struct_data;
        memcpy(&struct_data.grapic_data_struct, pgraph, sizeof(graphic_data_struct_t));
        memcpy(&struct_data.data, str, Const_Referee_DATA_CMD_ID_LIST[5].data_length - sizeof(graphic_data_struct_t));

        Referee_SendInteractiveData(
            Const_Referee_DATA_CMD_ID_LIST[5].cmd_id, referee->client_id,
            (uint8_t *) ((void *) &struct_data), Const_Referee_DATA_CMD_ID_LIST[5].data_length
        );
    }


/**
  * @brief      客户端自定义图形缓冲区是否为空
  * @param      无
  * @retval     1为空，0为非空
  */
    uint8_t Referee_IsDrawingBufferEmpty( ) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        return referee->graphic_buf_len == 0;
    }


/**
  * @brief      客户端自定义图形缓冲区刷写函数
  * @param      无
  * @retval     无
  */
    void Referee_DrawingBufferFlush( ) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        if (Referee_IsDrawingBufferEmpty()) { return; }
        uint8_t cur = 0;
        while (cur + 7 <= referee->graphic_buf_len) {
            Referee_SendDrawingCmd(referee->graphic_buf + cur, 4);
            cur += 7;
        }
        while (cur + 5 <= referee->graphic_buf_len) {
            Referee_SendDrawingCmd(referee->graphic_buf + cur, 3);
            cur += 5;
        }
        while (cur + 2 <= referee->graphic_buf_len) {
            Referee_SendDrawingCmd(referee->graphic_buf + cur, 2);
            cur += 2;
        }
        while (cur + 1 <= referee->graphic_buf_len) {
            Referee_SendDrawingCmd(referee->graphic_buf + cur, 1);
            cur += 1;
        }
        referee->graphic_buf_len = 0;
    }


/**
  * @brief      将图形命令加入客户端自定义图形缓冲区
  * @param      pgraph: 指针指向图形命令
  * @retval     无
  */
    void Referee_DrawingBufferPush(graphic_data_struct_t *pgraph) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        memcpy(referee->graphic_buf + referee->graphic_buf_len, pgraph, sizeof(graphic_data_struct_t));
        ++referee->graphic_buf_len;
        if (referee->graphic_buf_len >= 7) {
            Referee_DrawingBufferFlush();
        }
    }


/**
  * @brief      客户端自定义图形时基函数
  * @param      无
  * @retval     无
  */
    void Referee_DrawingTimeBaseCallback( ) {
        static uint8_t tick = 0;
        ++tick;
        if (tick == 5) {
            tick = 0;
            Referee_DrawingBufferFlush();
        }
    }


/**
  * @brief      打包图形命令
  * @param      详见协议及头文件定义
  * @retval     是否合法（1为是，0为否）
  */
    uint32_t Referee_PackGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, Draw_GraphicType graphic_type, uint8_t layer,
        Draw_Color color, uint16_t start_angle, uint16_t end_angle,
        uint8_t width, uint16_t start_x, uint16_t start_y,
        uint16_t radius, uint16_t end_x, uint16_t end_y
    ) {
        if (graph_id > 0xffffff) { return PARSE_FAILED; }
        pgraph->graphic_name[0] = graph_id & 0xff;
        pgraph->graphic_name[1] = (graph_id >> 8) & 0xff;
        pgraph->graphic_name[2] = (graph_id >> 16) & 0xff;

        pgraph->operate_type = (uint8_t) operate_type;
        pgraph->graphic_type = (uint8_t) graphic_type;

        if (layer > 9) { return PARSE_FAILED; }
        pgraph->layer = layer;

        pgraph->color = (uint8_t) color;

        if (start_angle > 360 || end_angle > 360) { return PARSE_FAILED; }
        pgraph->start_angle = start_angle;
        pgraph->end_angle = end_angle;

        pgraph->width = width;

        if (start_x > 0x7ff || start_x > 0x7ff || radius > 0x3ff || end_x > 0x7ff ||
            end_y > 0x7ff) { return PARSE_FAILED; }
        pgraph->start_x = start_x;
        pgraph->start_y = start_y;
        pgraph->radius = radius;
        pgraph->end_x = end_x;
        pgraph->end_y = end_y;

        return PARSE_SUCCEEDED;
    }


/**
  * @brief      打包显示浮点数图形命令
  * @param      详见协议及头文件定义
  * @retval     是否合法（1为是，0为否）
  */
    uint32_t Referee_PackFloatGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, uint8_t layer,
        Draw_Color color, uint16_t font_size, uint16_t decimal_digit,
        uint8_t width, uint16_t start_x, uint16_t start_y, float value
    ) {
        Referee_GraphicDataConverterUnion conv;
        conv.int_data = (int32_t) (value * 1000.0f);
        uint16_t radius = (conv.ui32_data) & 0x3ff;
        uint16_t end_x = (conv.ui32_data >> 10) & 0x7ff;
        uint16_t end_y = (conv.ui32_data >> 21) & 0x7ff;
        return Referee_PackGraphicData(
            pgraph, graph_id, operate_type, Draw_TYPE_FLOAT, layer, color, font_size,
            decimal_digit, width, start_x, start_y, radius, end_x, end_y
        );
    }


/**
  * @brief      打包显示整数图形命令
  * @param      详见协议及头文件定义
  * @retval     是否合法（1为是，0为否）
  */
    uint32_t Referee_PackIntGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, uint8_t layer,
        Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, int value
    ) {
        Referee_GraphicDataConverterUnion conv;
        conv.int_data = value;
        return Referee_PackGraphicData(
            pgraph, graph_id, operate_type, Draw_TYPE_INT, layer, color, font_size,
            0, width, start_x, start_y, conv.graphic_data.radius,
            conv.graphic_data.end_x, conv.graphic_data.end_y
        );
    }


/**
  * @brief      打包显示字符串图形命令
  * @param      详见协议及头文件定义
  * @retval     是否合法（1为是，0为否）
  */
    uint32_t Referee_PackStringGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, uint8_t layer,
        Draw_Color color, uint16_t font_size, uint8_t length,
        uint8_t width, uint16_t start_x, uint16_t start_y
    ) {
        if (length >
            Const_Referee_DATA_CMD_ID_LIST[5].data_length - sizeof(graphic_data_struct_t)) { return PARSE_FAILED; }
        return Referee_PackGraphicData(
            pgraph, graph_id, operate_type, Draw_TYPE_STRING, layer, color, font_size,
            length, width, start_x, start_y, 0, 0, 0
        );
    }


/********** REFEREE CUSTOM GRAPHIC DRAWING FUNCTION **********/


/**
  * @brief      绘图函数，清空指定图层
  * @param      layer: 图层号（0~9）
  * @retval     无
  */
    void Draw_ClearLayer(uint8_t layer) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        Referee_DrawingBufferFlush();
        uint8_t buf[2];
        buf[0] = 1;
        buf[1] = layer;
        Referee_SendInteractiveData(
            Const_Referee_DATA_CMD_ID_LIST[0].cmd_id, referee->client_id,
            buf, Const_Referee_DATA_CMD_ID_LIST[0].data_length
        );
    }


/**
  * @brief      绘图函数，清空全部
  * @param      无
  * @retval     无
  */
    void Draw_ClearAll( ) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        Referee_DrawingBufferFlush();
        uint8_t buf[2];
        buf[0] = 2;
        buf[1] = 0;
        Referee_SendInteractiveData(
            Const_Referee_DATA_CMD_ID_LIST[0].cmd_id, referee->client_id,
            buf, Const_Referee_DATA_CMD_ID_LIST[0].data_length
        );
    }


/**
  * @brief      绘图函数，清空指定图形
  * @param      graph_id: 图形ID
  * @retval     无
  */
    void Draw_Delete(uint32_t graph_id) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_DELETE, (Draw_GraphicType) 0, 0,
            (Draw_Color) 0, 0, 0, 0, 0, 0, 0, 0, 0
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画直线（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddLine(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, Draw_TYPE_LINE, layer, color,
            0, 0, width, start_x, start_y, 0, end_x, end_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画直线（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyLine(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, Draw_TYPE_LINE, layer, color,
            0, 0, width, start_x, start_y, 0, end_x, end_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画矩形（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddRectangle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, Draw_TYPE_RECTANGLE, layer, color,
            0, 0, width, start_x, start_y, 0, end_x, end_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画矩形（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyRectangle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, Draw_TYPE_RECTANGLE, layer, color,
            0, 0, width, start_x, start_y, 0, end_x, end_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画圆（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddCircle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, Draw_TYPE_CIRCLE, layer, color,
            0, 0, width, center_x, center_y, radius, 0, 0
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画圆（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyCircle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, Draw_TYPE_CIRCLE, layer, color,
            0, 0, width, center_x, center_y, radius, 0, 0
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画椭圆（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddEllipse(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, Draw_TYPE_ELLIPSE, layer, color,
            0, 0, width, center_x, center_y, 0, radius_x, radius_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画椭圆（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyEllipse(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, Draw_TYPE_ELLIPSE, layer, color,
            0, 0, width, center_x, center_y, 0, radius_x, radius_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画圆弧（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddArc(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t start_angle, uint16_t end_angle,
        uint8_t width, uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, Draw_TYPE_ARC, layer, color,
            start_angle, end_angle, width, center_x, center_y, 0, radius_x, radius_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，画圆弧（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyArc(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t start_angle, uint16_t end_angle,
        uint8_t width, uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, Draw_TYPE_ARC, layer, color,
            start_angle, end_angle, width, center_x, center_y, 0, radius_x, radius_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，显示浮点数（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddFloat(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size, uint16_t decimal_digit,
        uint8_t width, uint16_t start_x, uint16_t start_y, float value
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackFloatGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, layer, color,
            font_size, decimal_digit, width, start_x, start_y, value
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，显示浮点数（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyFloat(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size, uint16_t decimal_digit,
        uint8_t width, uint16_t start_x, uint16_t start_y, float value
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackFloatGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, layer, color,
            font_size, decimal_digit, width, start_x, start_y, value
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，显示整数（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddInt(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, int value
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackIntGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, layer, color,
            font_size, width, start_x, start_y, value
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，显示整数（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyInt(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, int value
    ) {
        graphic_data_struct_t graph;
        if (Referee_PackIntGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, layer, color,
            font_size, width, start_x, start_y, value
        ) != PARSE_SUCCEEDED) {
            return;
        }
        Referee_DrawingBufferPush(&graph);
    }


/**
  * @brief      绘图函数，显示字符串（新增）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_AddString(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, const char str[]
    ) {
        graphic_data_struct_t graph;
        Referee_DrawingBufferFlush();
        uint8_t len = strlen(str);
        if (Referee_PackStringGraphicData(
            &graph, graph_id, Draw_OPERATE_ADD, layer, color,
            font_size, len, width, start_x, start_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        uint8_t buf[35];
        memcpy(buf, str, len);
        Referee_SendDrawingStringCmd(&graph, buf);
    }

/**
  * @brief      绘图函数，显示字符串（修改）
  * @param      详见协议及头文件定义
  * @retval     无
  */
    void Draw_ModifyString(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, const char str[]
    ) {
        graphic_data_struct_t graph;
        Referee_DrawingBufferFlush();
        uint8_t len = strlen(str);
        if (Referee_PackStringGraphicData(
            &graph, graph_id, Draw_OPERATE_MODIFY, layer, color,
            font_size, len, width, start_x, start_y
        ) != PARSE_SUCCEEDED) {
            return;
        }
        uint8_t buf[35];
        memcpy(buf, str, len);
        Referee_SendDrawingStringCmd(&graph, buf);
    }
/********** END OF REFEREE CUSTOM GRAPHIC DRAWING FUNCTION **********/


/**
  * @brief      判断裁判系统是否离线
  * @param      无
  * @retval     是否离线（1为是，0为否）
  */
    uint8_t Referee_IsRefereeOffline( ) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        uint32_t now = HAL_GetTick();
        if ((now - referee->last_update_time) > Const_Referee_REMOTE_OFFLINE_TIME) {
            referee->state = Referee_STATE_LOST;
        }
        return referee->state == Referee_STATE_LOST;
    }

/**
  * @brief      机器人间交互数据解析函数
  * @param      data: 数据帧
  * @param      data_length: 数据帧长度
  * @retval     解析结果（0为失败，1为成功）
  */
    uint8_t Referee_ParseRobotCustomData(uint8_t *data, uint16_t data_length) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;
        ext_student_interactive_header_data_t *header_struct_ptr = static_cast<ext_student_interactive_header_data_t *>((void *) data);

        if (header_struct_ptr->data_cmd_id < Const_Referee_DATA_CMD_ID_INTERACTIVE_DATA_LBOUND ||
            header_struct_ptr->data_cmd_id > Const_Referee_DATA_CMD_ID_INTERACTIVE_DATA_UBOUND) {
            return PARSE_FAILED;
        }    // wrong data cmd id
        if (header_struct_ptr->receiver_ID != referee->robot_id) { return PARSE_FAILED; }           // wrong receiver id

        // Interactive Data Recieve Callback
//	if(header_struct_ptr->data_cmd_id == 0x210){   //操作手指令cmd_id = 0x210
//		uint8_t * interactive_data_ptr = data + sizeof(ext_student_interactive_header_data_t);  //数据起始地址（去掉数据头）
//		//if (data_length != Const_Referee_CMD_INTERACTIVE.data_length) return PARSE_FAILED;      // wrong data length
//		if(self_color == 1 && header_struct_ptr->sender_ID == 6 && header_struct_ptr->receiver_ID == 7){
//			Summoner_Data.Summoner_Mode = interactive_data_ptr[0];
//			Summoner_Data.is_Summoner_Shoot = interactive_data_ptr[1];
//		}
//		else if(self_color == 2 && header_struct_ptr->sender_ID == 106 && header_struct_ptr->receiver_ID == 107){
//			Summoner_Data.Summoner_Mode = interactive_data_ptr[0];
//			Summoner_Data.is_Summoner_Shoot = interactive_data_ptr[1];
//		}
//	}
//	else{
//		return PARSE_FAILED;   // wrong data cmd id
//	}

        return PARSE_SUCCEEDED;
    }


/**
  * @brief      裁判系统数据解析函数
  * @param      cmd_id: 命令ID
  * @param      data: 数据帧
  * @param      data_length: 数据帧长度
  * @retval     解析结果（0为失败，1为成功）
  */
    uint8_t Referee_ParseRefereeCmd(uint16_t cmd_id, uint8_t *data, uint16_t data_length) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;

        if (cmd_id == Const_Referee_CMD_INTERACTIVE.cmd_id) { return Referee_ParseRobotCustomData(data, data_length); }

        for (int i = 0; i < Const_Referee_CMD_NUM; ++i) {
            if (cmd_id == Const_Referee_CMD_LIST[i].cmd_id) {
                //if (data_length != Const_Referee_CMD_LIST[i].data_length) return PARSE_FAILED;  // wrong data length
                if (Const_Referee_CMD_LIST[i].parse_func == NULL) { return PARSE_FAILED; }          // unsupported cmd
                return (*(Const_Referee_CMD_LIST[i].parse_func))(referee, data);                // parse cmd
            }
        }
        return PARSE_FAILED;    // unknown cmd
    }


/**
  * @brief      裁判系统串口数据解码函数
  * @param      buff: 数据缓冲区
  * @param      rxdatalen: 数据长度
  * @retval     无
  */
    void Referee_DecodeRefereeData(uint8_t *buff, uint16_t rxdatalen) {
        Referee_RefereeDataTypeDef *referee = &Referee_RefereeData;

        referee->state = Referee_STATE_PENDING;    // 加锁防止互斥读写
        referee->last_update_time = HAL_GetTick();            // 时间戳

        if (buff[0] != Const_Referee_FRAME_HEADER_SOF) {
            referee->state = Referee_STATE_ERROR;
            return;
        }

        if (!CRC_VerifyCRC8CheckSum(buff, 5)) {
            referee->state = Referee_STATE_ERROR;
            return;
        }

        uint16_t data_length = (uint16_t) buff[2] << 8 | buff[1];
        uint8_t seq = buff[3];
        if (seq == 0) {
            referee->state = Referee_STATE_ERROR;
            //return;
        }
        if (!CRC_VerifyCRC16CheckSum(buff, data_length + 9)) {
            referee->state = Referee_STATE_ERROR;
            return;
        }

        uint16_t cmd_id = (uint16_t) buff[6] << 8 | buff[5];
        if (!Referee_ParseRefereeCmd(cmd_id, buff + 7, data_length)) {
            referee->state = Referee_STATE_ERROR;
            return;
        }

        referee->state = Referee_STATE_CONNECTED;  // 解锁
    }

/**
  * @brief      裁判系统串口回调函数
  * @param      huart: 指针指向串口句柄
  * @retval     无
  */
#define __HAL_DMA_GET_COUNTER(__HANDLE__) ((__HANDLE__)->Instance->CNDTR)
#define __HAL_DMA_SET_COUNTER(__HANDLE__, __COUNTER__) ((__HANDLE__)->Instance->CNDTR = (uint16_t)(__COUNTER__))

    void Referee_RXCallback(UART_HandleTypeDef *huart) {
        /* clear DMA transfer complete flag */
        __HAL_DMA_DISABLE(huart->hdmarx);

        /* handle dbus data dbus_buf from DMA */
        uint16_t rxdatalen = Const_Referee_RX_BUFF_LEN - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        for (uint16_t i = 0; i < rxdatalen; i++) {
            if (Referee_RxData[i] == Const_Referee_FRAME_HEADER_SOF) {
                Referee_DecodeRefereeData(Referee_RxData + i, rxdatalen);
            }
        }
        /* restart dma transmission */
        __HAL_DMA_SET_COUNTER(huart->hdmarx, Const_Referee_RX_BUFF_LEN);
        //HAL_DMA_Start(huart->hdmarx,(uint32_t)&huart->Instance->DR,(uint32_t)Referee_RxData,Const_Referee_RX_BUFF_LEN);
        __HAL_DMA_ENABLE(huart->hdmarx);
    }
}
/************************ (C) COPYRIGHT BIT DreamChaser *****END OF FILE****/

