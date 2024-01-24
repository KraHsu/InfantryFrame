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

#pragma once

#include "main.h"
#include "infantry_crc.h"
#include "infantry_usart_device.h"
#include <cstring>

namespace Referee {
    class referee_interface : public infantry::UsartRxDataType {
    public:
        using infantry::UsartRxDataType::UsartRxDataType;

        UsartDataType * rxCallback() override;
    };

    void registerRxCallBack( );
}

/********** START OF REFEREE CMD STRUCT DEFINITION **********/
namespace Referee {
    typedef enum {
        WATCH_NOTHING = 0,
        WATCH_HIGHWAY = 1,
        WATCH_OUTPOST = 2,
        WATCH_HIGHLAND = 3,
    } Summoner_ModeType_e;

    typedef struct {
        Summoner_ModeType_e Summoner_Mode;
        uint8_t is_Summoner_Shoot;
    } Summoner_Data_t;

    typedef struct __attribute__((packed)) {
        uint8_t game_type: 4;
        uint8_t game_progress: 4;
        uint16_t stage_remain_time;
        uint64_t SyncTimeStamp;
    } ext_game_status_t;

    typedef struct __attribute__((packed)) {
        uint8_t winner;
    } ext_game_result_t;

    typedef struct __attribute__((packed)) {
        uint16_t red_1_robot_HP;
        uint16_t red_2_robot_HP;
        uint16_t red_3_robot_HP;
        uint16_t red_4_robot_HP;
        uint16_t red_5_robot_HP;
        uint16_t red_7_robot_HP;
        uint16_t red_outpost_HP;
        uint16_t red_base_HP;
        uint16_t blue_1_robot_HP;
        uint16_t blue_2_robot_HP;
        uint16_t blue_3_robot_HP;
        uint16_t blue_4_robot_HP;
        uint16_t blue_5_robot_HP;
        uint16_t blue_7_robot_HP;
        uint16_t blue_outpost_HP;
        uint16_t blue_base_HP;
    } ext_game_robot_HP_t;

    typedef struct __attribute__((packed)) {
        uint8_t F1_zone_status: 1;
        uint8_t F1_zone_buff_debuff_status: 3;
        uint8_t F2_zone_status: 1;
        uint8_t F2_zone_buff_debuff_status: 3;
        uint8_t F3_zone_status: 1;
        uint8_t F3_zone_buff_debuff_status: 3;
        uint8_t F4_zone_status: 1;
        uint8_t F4_zone_buff_debuff_status: 3;
        uint8_t F5_zone_status: 1;
        uint8_t F5_zone_buff_debuff_status: 3;
        uint8_t F6_zone_status: 1;
        uint8_t F6_zone_buff_debuff_status: 3;
    } ext_ICRA_buff_debuff_zone_status_t;

    typedef struct __attribute__((packed)) {
        uint32_t event_type;
    } ext_event_data_t;

    typedef struct __attribute__((packed)) {
        uint8_t supply_projectile_id;
        uint8_t supply_robot_id;
        uint8_t supply_projectile_step;
        uint8_t supply_projectile_num;
    } ext_supply_projectile_action_t;

    typedef struct __attribute__((packed)) {
        uint16_t dart_remaining_time;
    } ext_dart_remaining_time_t;

    typedef struct __attribute__((packed)) {
        uint8_t level;
        uint8_t foul_robot_id;
    } ext_referee_warning_t;

    typedef struct __attribute__((packed)) {
        uint8_t robot_id;
        uint8_t robot_level;
        uint16_t remain_HP;
        uint16_t max_HP;
        uint16_t shooter_id1_17mm_cooling_rate;
        uint16_t shooter_id1_17mm_cooling_limit;
        uint16_t shooter_id1_17mm_speed_limit;
        uint16_t shooter_id2_17mm_cooling_rate;
        uint16_t shooter_id2_17mm_cooling_limit;
        uint16_t shooter_id2_17mm_speed_limit;
        uint16_t shooter_id1_42mm_cooling_rate;
        uint16_t shooter_id1_42mm_cooling_limit;
        uint16_t shooter_id1_42mm_speed_limit;
        uint16_t chassis_power_limit;
        uint8_t mains_power_gimbal_output: 1;
        uint8_t mains_power_chassis_output: 1;
        uint8_t mains_power_shooter_output: 1;
    } ext_game_robot_status_t;

    typedef struct __attribute__((packed)) {
        uint16_t chassis_volt;
        uint16_t chassis_current;
        float chassis_power;
        uint16_t chassis_power_buffer;
        uint16_t shooter_id1_17mm_cooling_heat;
        uint16_t shooter_id2_17mm_cooling_heat;
        uint16_t shooter_id1_42mm_cooling_heat;
    } ext_power_heat_data_t;

    typedef struct __attribute__((packed)) {
        float x;
        float y;
        float z;
        float yaw;
    } ext_game_robot_pos_t;

    typedef struct __attribute__((packed)) {
        uint8_t power_rune_buff;
    } ext_buff_t;

    typedef struct __attribute__((packed)) {
        uint8_t attack_time;
    } aerial_robot_energy_t;

    typedef struct __attribute__((packed)) {
        uint8_t armor_id: 4;
        uint8_t hurt_type: 4;
    } ext_robot_hurt_t;

    typedef struct __attribute__((packed)) {
        uint8_t bullet_type;
        uint8_t bullet_id;
        uint8_t bullet_freq;
        float bullet_speed;
    } ext_shoot_data_t;

    typedef struct __attribute__((packed)) {
        uint16_t bullet_remaining_num_17mm;
        uint16_t bullet_remaining_num_42mm;
        uint16_t coin_remaining_num;
    } ext_bullet_remaining_t;

    typedef struct __attribute__((packed)) {
        uint32_t rfid_status;
    } ext_rfid_status_t;

    typedef struct __attribute__((packed)) {
        uint8_t dart_launch_openning_status;
        uint8_t dart_attack_target;
        uint8_t target_change_time;
        uint16_t operate_launch_cmd_time;
    } ext_dart_client_cmd_t;

    typedef struct __attribute__((packed)) {
        float target_position_x;
        float target_position_y;
        float target_position_z;
        uint8_t command_keyboard;
        uint16_t target_robot_ID;
    } ext_robot_cmd_t;

// ------------------------------

    typedef struct __attribute__((packed)) {
        uint16_t data_cmd_id;
        uint16_t sender_ID;
        uint16_t receiver_ID;
    } ext_student_interactive_header_data_t;

/* （obsolete）
typedef struct __attribute__((packed)) struct {
    float data1;
    float data2;
    float data3;
    uint8_t masks;
} client_custom_data_t;
*/

//typedef struct __attribute__((packed)) struct {
//    uint8_t data[];
//} robot_interactive_data_t;

    typedef struct __attribute__((packed)) {
        uint8_t operate_type;
        uint8_t layer;
    } ext_client_custom_graphic_delete_t;

/* （obsolete）
typedef struct __attribute__((packed)) struct {
    uint8_t operate_type;
    uint8_t graphic_type;
    uint8_t graphic_name[5];
    uint8_t layer;
    uint8_t color;
    uint8_t width;
    uint16_t start_x;
    uint16_t start_y;
    uint16_t radius;
    uint16_t end_x;
    uint16_t end_y;
    int16_t start_angle;
    int16_t end_angle;
    uint8_t text_lenght;
    uint8_t text[30];
} ext_client_graphic_draw_t;
*/

    typedef struct __attribute__((packed)) {
        uint8_t graphic_name[3];
        uint32_t operate_type: 3;
        uint32_t graphic_type: 3;
        uint32_t layer: 4;
        uint32_t color: 4;
        uint32_t start_angle: 9;
        uint32_t end_angle: 9;
        uint32_t width: 10;
        uint32_t start_x: 11;
        uint32_t start_y: 11;
        uint32_t radius: 10;
        uint32_t end_x: 11;
        uint32_t end_y: 11;
    } graphic_data_struct_t;

    typedef struct __attribute__((packed)) {
        graphic_data_struct_t grapic_data_struct;
    } ext_client_custom_graphic_single_t;

    typedef struct __attribute__((packed)) {
        graphic_data_struct_t grapic_data_struct[2];
    } ext_client_custom_graphic_double_t;

    typedef struct __attribute__((packed)) {
        graphic_data_struct_t grapic_data_struct[5];
    } ext_client_custom_graphic_five_t;

    typedef struct __attribute__((packed)) {
        graphic_data_struct_t grapic_data_struct[7];
    } ext_client_custom_graphic_seven_t;

    typedef struct __attribute__((packed)) {
        graphic_data_struct_t grapic_data_struct;
        uint8_t data[30];
    } ext_client_custom_character_t;

/********** END OF REFEREE CMD STRUCT DEFINITION **********/


    typedef enum {
        Referee_STATE_NULL = 0,
        Referee_STATE_CONNECTED = 1,
        Referee_STATE_LOST = 2,
        Referee_STATE_ERROR = 3,
        Referee_STATE_PENDING = 4
    } Referee_RefereeStateEnum;

    typedef struct {
        Referee_RefereeStateEnum state;             // 裁判系统当前状态
        uint32_t last_update_time;                  // 裁判系统上次更新时间


        uint16_t client_id;                         // 客户端ID
//  client_custom_data_t custom_data;           // （已废弃）客户端自定义数据
//  ext_client_graphic_draw_t graphic_draw;     // （已废弃）客户端自定义绘图
        graphic_data_struct_t graphic_buf[30];      // 客户端自定义绘图缓冲区
        uint8_t graphic_buf_len;                    // 客户端自定义绘图缓冲区已占用长度
//  uint32_t graphic_current_id;                // 客户端自定义绘图当前序号


        uint8_t game_type;                        //  游戏类型,    1:RoboMaster 机甲大师赛；
        //              2:RoboMaster 机甲大师单项赛；
        //      	    3：ICRA RoboMaster 人工智能挑战赛
        //              4：RoboMaster 联盟赛3V3
        //              5：RoboMaster 联盟赛1V1
        uint8_t game_progress;                      //  当前比赛阶段,0：未开始比赛；
        //              1：准备阶段；
        //              2：自检阶段；
        //              3：5s倒计时；
        //              4：对战中；
        //              5：比赛结算中
        uint16_t stage_remain_time;                 //  当前阶段剩余时间，单位s
        uint64_t SyncTimeStamp;            //同步时间戳


        uint16_t red_1_robot_HP;
        uint16_t red_2_robot_HP;
        uint16_t red_3_robot_HP;
        uint16_t red_4_robot_HP;
        uint16_t red_5_robot_HP;
        uint16_t red_7_robot_HP;
        uint16_t red_outpost_HP;
        uint16_t red_base_HP;
        uint16_t blue_1_robot_HP;
        uint16_t blue_2_robot_HP;
        uint16_t blue_3_robot_HP;
        uint16_t blue_4_robot_HP;
        uint16_t blue_5_robot_HP;
        uint16_t blue_7_robot_HP;
        uint16_t blue_outpost_HP;
        uint16_t blue_base_HP;


        uint32_t event_type;
        uint8_t SelfCharge_1_OccupyStatus;  // 己方补给站1号补血点占领状态
        uint8_t SelfCharge_2_OccupyStatus;  // 己方补给站2号补血点占领状态
        uint8_t SelfCharge_3_OccupyStatus;  // 己方补给站3号补血点占领状态
        uint8_t SelfEnergyBuff_ShootingPoint_OccupyStatus;  //己方能量机关打击点占领状态
        uint8_t SelfSmallEnergyBuff_ActivateStatus;  //己方小能量机关激活状态
        uint8_t SelfBigEnergyBuff_ActivateStatus;  //己方大能量机关激活状态
        uint8_t SelfR2_B2_CircularHighland_OccupypStatus;   //己方R2/B2环形高地占领状态
        uint8_t SelfR3_B3_Trapezoidal_OccupypStatus;   //己方R3/B3梯形高地占领状态
        uint8_t SelfR4_B4_Trapezoidal_OccupypStatus;   //己方R4/B4梯形高地占领状态
        uint8_t SelfBase_Shield_Status;   //己方基地护盾状态
        uint8_t SelfOutpost_Status;       //己方前哨站状态


        uint8_t robot_id;
        uint8_t robot_level;
        uint16_t remain_HP;
        uint16_t max_HP;
        uint16_t shooter_id1_17mm_cooling_rate;
        uint16_t shooter_id1_17mm_cooling_limit;
        uint16_t shooter_id1_17mm_speed_limit;
        uint16_t shooter_id2_17mm_cooling_rate;
        uint16_t shooter_id2_17mm_cooling_limit;
        uint16_t shooter_id2_17mm_speed_limit;
        uint16_t chassis_power_limit;
        uint8_t mains_power_gimbal_output;
        uint8_t mains_power_chassis_output;
        uint8_t mains_power_shooter_output;


        float chassis_power;
        uint16_t chassis_power_buffer;
        uint16_t shooter_id1_17mm_cooling_heat;
        uint16_t shooter_id2_17mm_cooling_heat;


        uint8_t armor_id;
        uint8_t hurt_type;


        uint8_t bullet_type;
        uint8_t bullet_id;
        uint8_t bullet_freq;
        float bullet_speed;

        uint16_t bullet_remaining_num_17mm;

        uint8_t command_keyboard;
        float destination_x;
        float destination_z;

    } Referee_RefereeDataTypeDef;

    typedef uint8_t (*Referee_RefereeCmdParseFuncDef)(Referee_RefereeDataTypeDef *referee, void *data_ptr);

    typedef struct {
        uint16_t cmd_id;                            // 指令ID
        uint8_t data_length;                        // 数据帧长度
        Referee_RefereeCmdParseFuncDef parse_func;  // 解析函数指针
    } Referee_RefereeCmdTypeDef;

    typedef struct {
        uint16_t robot_id;
        uint16_t client_id;
    } Referee_RobotAndClientIDTypeDef;


    typedef union {
        struct {
            uint32_t radius: 10;
            uint32_t end_x: 11;
            uint32_t end_y: 11;
        } graphic_data;
        uint32_t ui32_data;
        float float_data;
        int32_t int_data;
    } Referee_GraphicDataConverterUnion;

    typedef enum {
        Draw_OPERATE_NULL = 0,    // 空操作
        Draw_OPERATE_ADD = 1,    // 增加
        Draw_OPERATE_MODIFY = 2,    // 修改
        Draw_OPERATE_DELETE = 3     // 删除
    } Draw_OperateType;             // 图形操作

    typedef enum {
        Draw_TYPE_LINE = 0,    // 直线
        Draw_TYPE_RECTANGLE = 1,    // 矩形
        Draw_TYPE_CIRCLE = 2,    // 整圆
        Draw_TYPE_ELLIPSE = 3,    // 椭圆
        Draw_TYPE_ARC = 4,    // 圆弧
        Draw_TYPE_FLOAT = 6,    // 浮点数
        Draw_TYPE_INT = 5,    // 整型数
        Draw_TYPE_STRING = 7     // 字符
    } Draw_GraphicType;             // 图形类型

    typedef enum {
        Draw_COLOR_RED_BLUE = 0,   // 红蓝主色
        Draw_COLOR_YELLOW = 1,   // 黄色
        Draw_COLOR_GREEN = 2,   // 绿色
        Draw_COLOR_ORANGE = 3,   // 橙色
        Draw_COLOR_VIOLET = 4,   // 紫红色
        Draw_COLOR_PINK = 5,   // 粉色
        Draw_COLOR_CYAN = 6,   // 青色
        Draw_COLOR_BLACK = 7,   // 黑色
        Draw_COLOR_WHITE = 8    // 白色
    } Draw_Color;                   // 颜色



    extern const uint16_t Const_Referee_RX_BUFF_LEN;
    extern const uint16_t Const_Referee_REMOTE_OFFLINE_TIME;

    extern UART_HandleTypeDef *Const_Referee_UART_HANDLER;

    extern Summoner_Data_t Summoner_Data;

    Referee_RefereeDataTypeDef *Referee_GetRefereeDataPtr( );

    void Referee_ResetRefereeData( );

    void init(UART_HandleTypeDef *huart);

    uint16_t Referee_GetClientIDByRobotID(uint8_t robot_id);

    void Referee_SendInteractiveData(
        uint16_t data_cmd_id, uint16_t receiver_ID, uint8_t *interactive_data, uint16_t interactive_data_length
    );

//void Referee_SetClientCustomDataLED(uint8_t led_no, uint8_t led_state);
//void Referee_SendClientCustomData(void);
    void Referee_SendRobotCustomData(uint16_t data_cmd_id, uint16_t receiver_ID, uint8_t *data, uint16_t data_length);

//void Referee_SendClientGraphicsDraw(void);
    void Referee_SendDrawingCmd(graphic_data_struct_t graph[], uint8_t mode);

    void Referee_SendDrawingStringCmd(graphic_data_struct_t *pgraph, uint8_t str[]);

    uint8_t Referee_IsDrawingBufferEmpty( );

    void Referee_DrawingBufferFlush( );

    void Referee_DrawingBufferPush(graphic_data_struct_t *graph);

    void Referee_DrawingTimeBaseCallback( );

    uint32_t Referee_PackGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, Draw_GraphicType graphic_type, uint8_t layer,
        Draw_Color color, uint16_t start_angle, uint16_t end_angle,
        uint8_t width, uint16_t start_x, uint16_t start_y,
        uint16_t radius, uint16_t end_x, uint16_t end_y
    );

    uint32_t Referee_PackFloatGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, uint8_t layer,
        Draw_Color color, uint16_t font_size, uint16_t decimal_digit,
        uint8_t width, uint16_t start_x, uint16_t start_y, float value
    );

    uint32_t Referee_PackIntGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, uint8_t layer,
        Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, int value
    );

    uint32_t Referee_PackStringGraphicData(
        graphic_data_struct_t *pgraph, uint32_t graph_id,
        Draw_OperateType operate_type, uint8_t layer,
        Draw_Color color, uint16_t font_size, uint8_t length,
        uint8_t width, uint16_t start_x, uint16_t start_y
    );

    void Draw_ClearLayer(uint8_t layer);

    void Draw_ClearAll( );

    void Draw_Delete(uint32_t graph_id);

    void Draw_AddLine(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    );

    void Draw_ModifyLine(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    );

    void Draw_AddRectangle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    );

    void Draw_ModifyRectangle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y
    );

    void Draw_AddCircle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius
    );

    void Draw_ModifyCircle(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius
    );

    void Draw_AddEllipse(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    );

    void Draw_ModifyEllipse(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint8_t width,
        uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    );

    void Draw_AddArc(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t start_angle, uint16_t end_angle,
        uint8_t width, uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    );

    void Draw_ModifyArc(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t start_angle, uint16_t end_angle,
        uint8_t width, uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y
    );

    void Draw_AddFloat(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size, uint16_t decimal_digit,
        uint8_t width, uint16_t start_x, uint16_t start_y, float value
    );

    void Draw_ModifyFloat(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size, uint16_t decimal_digit,
        uint8_t width, uint16_t start_x, uint16_t start_y, float value
    );

    void Draw_AddInt(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, int value
    );

    void Draw_ModifyInt(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, int value
    );

    void Draw_AddString(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, const char str[]
    );

    void Draw_ModifyString(
        uint32_t graph_id, uint8_t layer, Draw_Color color, uint16_t font_size,
        uint8_t width, uint16_t start_x, uint16_t start_y, const char str[]
    );

    uint8_t Referee_IsRefereeOffline( );

    uint8_t Referee_CheckDataLengthByCmdID(uint16_t cmd_id, uint16_t data_length);

    uint8_t Referee_ParseRobotCustomData(uint8_t *data, uint16_t data_length);

    uint8_t Referee_ParseRefereeCmd(uint16_t cmd_id, uint8_t *data, uint16_t data_length);

    void Referee_DecodeRefereeData(uint8_t *buff, uint16_t rxdatalen);

    void Referee_RXCallback(UART_HandleTypeDef *huart);
}

extern Referee::Referee_RefereeDataTypeDef Referee_RefereeData;
