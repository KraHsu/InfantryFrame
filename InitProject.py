import sys
import xml.etree.ElementTree as ET
import re
import os


def modifyOption(project_name):
    uvprojx = f"../{project_name}/MDK-ARM/{project_name}.uvprojx"
    with open(uvprojx, "r") as file:
        data = file.read()
    data = data.replace("<TargetOption>", "<uAC6>1</uAC6>\n<TargetOption>")
    data = data.replace("<Optim>4</Optim>", "<Optim>6</Optim>")
    data = data.replace("<wLevel>2</wLevel>", "<wLevel>3</wLevel>")
    data = data.replace("<ClangAsOpt>1</ClangAsOpt>", "<uClangAs>0</uClangAs>")
    with open(uvprojx, "w") as file:
        file.write(data)


def modifyIncludePath(project_name):
    uvprojx = f"../{project_name}/MDK-ARM/{project_name}.uvprojx"
    tree = ET.parse(uvprojx)
    root = tree.getroot()

    # 寻找Cads>VariousControls>IncludePath
    for include_path in root.findall(".//Cads/VariousControls/IncludePath"):
        current_paths = include_path.text
        new_path = rf"..\..\InfantryFrame\Infantry\algorithm\Inc;..\..\InfantryFrame\Infantry\device\Inc;..\..\InfantryFrame\Infantry\frame\Inc;..\..\InfantryFrame\Infantry\plugins\graph_task\Inc;..\..\InfantryFrame\Infantry\plugins\logger\Inc;..\..\InfantryFrame\Infantry\library\Inc;..\..\InfantryFrame\Infantry\periph\Inc;..\..\InfantryFrame\Infantry\example\Inc;..\..\InfantryFrame\Infantry;..\..\InfantryFrame\RT-Thread\RealThread_RTOS\include;..\..\InfantryFrame\RT-Thread\RealThread_RTOS\finsh;..\..\InfantryFrame\RT-Thread\RealThread_RTOS\components\finsh;..\..\InfantryFrame\RT-Thread;..\..\InfantryFrame\DSP\Inc;..\App\Inc"
        include_path.text = (
            current_paths + ";" + new_path if current_paths else new_path
        )

    tree.write(uvprojx, encoding="UTF-8", xml_declaration=True)


def modifySourceGroup(project_name, group_list):
    uvprojx = f"../{project_name}/MDK-ARM/{project_name}.uvprojx"
    tree = ET.parse(uvprojx)
    root = tree.getroot()

    for group_info in group_list:
        new_group = ET.Element("Group")

        group_name = ET.SubElement(new_group, "GroupName")
        group_name.text = group_info["GroupName"]

        files = ET.SubElement(new_group, "Files")
        for file_info in group_info["Files"]:
            file = ET.SubElement(files, "File")

            file_name = ET.SubElement(file, "FileName")
            file_name.text = file_info["FileName"]

            file_type = ET.SubElement(file, "FileType")
            file_type.text = file_info["FileType"]

            file_path = ET.SubElement(file, "FilePath")
            file_path.text = file_info["FilePath"]

        groups = root.find(".//Groups")
        if groups is not None:
            groups.append(new_group)
        else:
            print("No <Groups> element found in the XML.")

    tree.write(uvprojx, encoding="UTF-8", xml_declaration=True)

    with open(uvprojx, "r") as file:
        data = file.read()
    data = data.replace(
        """<FileName>SEGGER_RTT_ASM_ARMv7M.S</FileName><FileType>2</FileType><FilePath>..\..\InfantryFrame\Infantry\plugins\logger\Src\SEGGER_RTT_ASM_ARMv7M.S</FilePath>""",
        """<FileName>SEGGER_RTT_ASM_ARMv7M.S</FileName>
              <FileType>2</FileType>
              <FilePath>..\..\InfantryFrame\Infantry\plugins\logger\Src\SEGGER_RTT_ASM_ARMv7M.S</FilePath>
              <FileOption>
                <CommonProperty>
                  <UseCPPCompiler>2</UseCPPCompiler>
                  <RVCTCodeConst>0</RVCTCodeConst>
                  <RVCTZI>0</RVCTZI>
                  <RVCTOtherData>0</RVCTOtherData>
                  <ModuleSelection>0</ModuleSelection>
                  <IncludeInBuild>2</IncludeInBuild>
                  <AlwaysBuild>2</AlwaysBuild>
                  <GenerateAssemblyFile>2</GenerateAssemblyFile>
                  <AssembleAssemblyFile>2</AssembleAssemblyFile>
                  <PublicsOnly>2</PublicsOnly>
                  <StopOnExitCode>11</StopOnExitCode>
                  <CustomArgument></CustomArgument>
                  <IncludeLibraryModules></IncludeLibraryModules>
                  <ComprImg>1</ComprImg>
                </CommonProperty>
                <FileArmAds>
                  <Aads>
                    <interw>2</interw>
                    <Ropi>2</Ropi>
                    <Rwpi>2</Rwpi>
                    <thumb>2</thumb>
                    <SplitLS>2</SplitLS>
                    <SwStkChk>2</SwStkChk>
                    <NoWarn>2</NoWarn>
                    <uSurpInc>2</uSurpInc>
                    <useXO>2</useXO>
                    <uClangAs>1</uClangAs>
                    <VariousControls>
                      <MiscControls></MiscControls>
                      <Define></Define>
                      <Undefine></Undefine>
                      <IncludePath></IncludePath>
                    </VariousControls>
                  </Aads>
                </FileArmAds>
              </FileOption>""",
    )
    with open(uvprojx, "w") as file:
        file.write(data)


def createCMakeList(project_name):
    with open(f"../{project_name}/CMakeLists.txt", "w") as file:
        file.write(
            r"""#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.25)

#*************************************************************
# 1. 设定项目名称
# 需要给定 PROJECT_NAME 的值，必须与项目名称一致，否则 MDK 生成 *.sct 时路径不对
set(PROJECT_NAME """
            + project_name
            + """)
# 2. 设定芯片类型
# 设定芯片类型，必须正确以 -mcpu 的参数规则命名，后边的编译参数会调用这个值
set(CMAKE_SYSTEM_PROCESSOR Cortex-M4)
# 3. 设定优化等级
# 优化选项 -O 有 1~3   -Os 是平衡  -Oz 是最小体积
set(OPTIMIZATION_LEVEL "-Os")


############################################################
# 1. 指定编译器和链接器，避免使用默认的 gcc
############################################################
# 设置编译器 C 的编译器
set(CMAKE_C_COMPILER armclang.exe)
set(CMAKE_C_COMPILER_WORKS TRUE)

# 即便用不到C++ 的编译器，还是要显示说明的,否则报错
set(CMAKE_CXX_COMPILER armclang.exe)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

#设置 ASM 的编译器（不设置配合 -masm=auto 使用）
set(CMAKE_ASM_COMPILER armclang.exe)
#set(CMAKE_ASM_COMPILER armasm.exe)     # 指明 ASM 编译器，配合 第二种 CMAKE_ASM_FLAGS_INIT 方式使用
set(CMAKE_ASM_COMPILER_WORKS TRUE)

#设置链接器
set(CMAKE_C_LINK_EXECUTABLE armlink.exe)
set(CMAKE_ASM_LINK_EXECUTABLE armlink.exe)
set(CMAKE_CXX_LINK_EXECUTABLE armlink.exe)


#*******************************************************************************************#
# 2. 获取芯片地址描述信息
# 获取当前 MCU 的 section 描述，及存储空间和起始地址的描述（注意：需要先用 keil 打开编译一下当前项目才能生成）
# 设置Sections脚本路径,CubeMX生成 MDK 后先编译一下，就会在MDK-ARM 目录下生成工程名命名的文件夹
# 在那个目录里会生成以项目名命名的sct文件。 由于在本地调用，不需要加 global 全局变量标记，更简洁
set(SECTIONS_SCRIPT_PATH ${CMAKE_HOME_DIRECTORY}/MDK-ARM/${PROJECT_NAME}/${PROJECT_NAME}.sct)


#********************************************************************************************#
# 3. 设置与芯片对应的 --target 编译选项
# 设置编译时默认参数片段，这些参数参见 MDK 中的配置，注意：Target 分页中要选 6.12 的 ARM 编译器,并抄各分页的 --target 配置
set(C_TARGET_FLAG --target=arm-arm-none-eabi)               # MDK 的 link 分页中的配置
set(ASM_TARGET_FLAG --target=arm-arm-none-eabi)             # MDK 的 link 分页中的配置(不支持显示指定 ASM 编译器的方式)
set(LINKER_TARGET_FLAG --cpu=${CMAKE_SYSTEM_PROCESSOR})     # MDK 的 link 分页中的配置


#********************************************************************************************#
# 4. 设置编译规则，这些规则变量要被外部的 CMakelistes.txt 调用，所以需要用 CACHE STRING "global" 声明为全局变量
# 编译参数，这些编译参数是从 MDK 中来的，具体的含义看 《 armclang Reference Guide 》 第一章节有详细介绍
# 需要注意的是不是 MDK 中的每个参数都要哪进来的，有些参数已经在 include 和 source 中被配置了
set(COMPILE_RULE_FLAG "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")

# 设置 C 编译器选项(这里就把MDK中的 C/C++ 分页里最下边一栏的属性贴进来)
# 参数 -w 表示忽略所有警告，不然要配具体忽略哪些警告，尽管贴过来也行，但是太乱
set(CMAKE_C_FLAGS_INIT "${C_TARGET_FLAG} ${COMPILE_RULE_FLAG} ${OPTIMIZATION_LEVEL} -xc -std=c11 -fno-rtti -funsigned-char -fshort-enums -fshort-wchar -gdwarf-3 -ffunction-sections -w")

# 设置 C++ 编译器选项
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -xc++ -std=c++11")

# 设置ASM编译器选项
# 注意： -masm=auto 选项是 MDK 的 link 分页里没有的参数，需要加上
# -g 是debug 用的加不加都行
set(CMAKE_ASM_FLAGS_INIT "${ASM_TARGET_FLAG} ${COMPILE_RULE_FLAG} -masm=auto -c -gdwarf-3 ")
# 第二种方式 配套显示执行 armasm.exe 为 ASM 编译器的方法，看上起更清晰一些
#set(CMAKE_ASM_FLAGS_INIT "${ASM_TARGET_FLAG} --cpu=Cortex-M4")


# 设置链接器选项
# 这些参数再 ARMCC 文档里么有，但 MDK 的 link 分页有，
# 使用忽略所有警告的配置时没有 --map 及其之后的内容， 这里根据需要保留了一些信息，在 demo.map 中可以看到
set(CMAKE_EXE_LINKER_FLAGS_INIT " ${LINKER_TARGET_FLAG} --strict --scatter ${SECTIONS_SCRIPT_PATH} --info sizes --info totals --info unused --info veneers --summary_stderr --info summarysizes")

# project settings
project("""
            + project_name
            + """ C CXX ASM)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

#Uncomment for hardware floating point
# -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
#add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

include_directories(
        ./Core/Inc
        ./USB_Device/App
        ./USB_Device/Target
        ./Drivers/STM32G4xx_HAL_Driver/Inc
        ./Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
        ./Middlewares/ST/STM32_USB_Device_Library/Core/Inc
        ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
        ./Drivers/CMSIS/Device/ST/STM32G4xx/Include
        ./Drivers/CMSIS/Include
        # 步兵框架
        ../InfantryFrame/RT-Thread
        ../InfantryFrame/RT-Thread/RealThread_RTOS/include
        ../InfantryFrame/RT-Thread/RealThread_RTOS/finsh
        ../InfantryFrame/RT-Thread/RealThread_RTOS/components/finsh
        ../InfantryFrame/DSP/Inc
        ../InfantryFrame/Infantry
        ../InfantryFrame/Infantry/frame/Inc
        ../InfantryFrame/Infantry/device/Inc
        ../InfantryFrame/Infantry/algorithm/Inc
        ../InfantryFrame/Infantry/library/Inc
        ../InfantryFrame/Infantry/periph/Inc
        ../InfantryFrame/Infantry/example/Inc
        ../InfantryFrame/Infantry/plugins/logger/Inc
        ../InfantryFrame/Infantry/plugins/graph_task/Inc
        # 核心代码
        ./App/Inc
        ./Public/Inc
)

add_definitions(-D__UVISION_VERSION="527" -D_RTE_ -DSTM32G473xx -DUSE_HAL_DRIVER -DSTM32G473xx -DARM_MATH_CM4 -D__TARGET_FPU_VFP -D__FPU_USED -D__FPU_PRESENT -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING)

file(GLOB_RECURSE SOURCES
        "./Middlewares/*.*" "./Drivers/*.*" "./Core/*.*" "./USB_Device/*.*"
        # 步兵框架
        "../InfantryFrame/DSP/Src/*.*"
        "../InfantryFrame/RT-Thread/*.*"
        "../InfantryFrame/Infantry/algorithm/Src/*.*"./
        "../InfantryFrame/Infantry/device/Src/*.*"
        "../InfantryFrame/Infantry/frame/Src/*.*"
        "../InfantryFrame/Infantry/library/Src/*.*"
        "../InfantryFrame/Infantry/periph/Src/*.*"
        "../InfantryFrame/Infantry/example/Src/*.*"
        "../InfantryFrame/Infantry/plugins/logger/Src/*.*"
        "../InfantryFrame/Infantry/plugins/graph_task/Src/*.*"
        "./Public/*.*"./
        "./App/Src/*.*"
        "./Public/Src/*.*"
        # "Example/Src/*.* "
        )
# 对于混合兼容的环境，需要屏蔽各种编译环境引起的文件“干扰”,通过 list(REMOVE_ITEM) 命令移除不同编译环境下的干扰文件
# 在原来 CubeMX 自动生成的 gcc 编译环境目录上,附加 ARMCC 编译需要的文件
file(GLOB_RECURSE SOURCES ${SOURCES} "${CMAKE_HOME_DIRECTORY}/MDK-ARM/startup_stm32g473xx.s")
# 将由 CubeMX 生成的 GCC 编译环境中的会干扰ARMCC环境的文件，放在 EXCLUDE_SRCS 自定义列表中
file(GLOB_RECURSE EXCLUDE_SRCS
        #                "./Middlewares/Third_Party/RealThread_RTOS/libcpu/arm/cortex-m4/context_gcc.S"
        "./Core/Startup/*.*"
        "./Core/Src/syscalls.c"
        "./Core/Src/sysmem.c"
        "./STM32G473CBTX_FLASH.ld "
        )
# 从源文件列表(SOURCES)中剔除干扰文件(EXCLUDE_SRCS)
list(REMOVE_ITEM SOURCES ${EXCLUDE_SRCS})


add_executable(${PROJECT_NAME} ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(ELF_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf)

# 使用 armclang 自带的 fromelf 工具，实现 elf 转 hex
set(ARMCC_fromelf fromelf.exe)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # 相当于fromelf.exe" --i32combined --output="xxx/demo.hex" "xxx/demo.elf"
        COMMAND ${ARMCC_fromelf} --i32combined --output="${HEX_FILE}" "${ELF_FILE}"
        COMMENT "Building ${HEX_FILE}"
        )

## 使用了 gcc 的 arm-none-eabi-objcopy 工具实现 elf 转 hex 和 bin， 跟 armclang 不是一脉
#set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
#        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
#        COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")

"""
        )


def findFunction(input_string, func):
    # 使用正则表达式匹配void SystemClock_Config(void)的定义
    pattern = re.compile(func)
    match = pattern.search(input_string)

    if match:
        # 获取匹配到的位置
        start_index = match.start()

        while input_string[start_index] != "{":
            start_index += 1

        # 初始化计数器
        brace_count = 1

        # 从起始位置开始迭代，找到函数体的结束位置
        for i in range(start_index + 1, len(input_string)):
            if input_string[i] == "{":
                brace_count += 1
            elif input_string[i] == "}":
                brace_count -= 1

            # 当计数器减为零时，找到函数体的结束位置
            if brace_count == 0:
                end_index = i
                break
        else:
            # 如果没有找到匹配的结束位置，返回None
            return None

        # 返回函数体的内容
        return input_string[start_index : end_index + 1]
    else:
        # 如果没有找到匹配的函数定义，返回None
        return None


def remove_empty_lines(input_string):
    lines = input_string.splitlines()
    non_empty_lines = [line for line in lines if line.strip()]
    result_string = "\n".join(non_empty_lines)
    return result_string


def createMain(project_name):
    with open(f"../{project_name}/Core/Src/main.c", "r") as file:
        cMainData = file.read()
    includePattern = r"/\* Includes ------------------------------------------------------------------\*/\n((.|\n)*?)/\* Private includes"
    includeMatch = re.search(includePattern, cMainData).group(1).replace(" ", "")

    initializePattern = r"/\* Initialize all configured peripherals \*/\n((.|\n)*?)/\* USER CODE BEGIN 2 \*/"
    initializeMatch = re.search(initializePattern, cMainData).group(1).replace(" ", "")

    clockMatch = "void SystemClock_Config()" + findFunction(
        cMainData, r"\bvoid\s+SystemClock_Config\s*\(void\s*\)\s*{"
    )

    timMatch = (
        (
            "void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)"
            + findFunction(
                cMainData,
                r"\bvoid\s+HAL_TIM_PeriodElapsedCallback\s*\(TIM_HandleTypeDef\s*\*\s*htim\s*\)\s*{",
            )
        )
        .replace("/* USER CODE BEGIN Callback 0 */", "")
        .replace("/* USER CODE END Callback 0 */", "")
        .replace("/* USER CODE BEGIN Callback 1 */", "")
        .replace("/* USER CODE END Callback 1 */", "")
    )
    timMatch = remove_empty_lines(timMatch)

    cppMain = f"""
#define LOG_TAG "main"

{includeMatch}
#include "infantry_frame.h"

extern "C" {'{'}
void SystemClock_Config(void);
void SysTick_Handler();
{'}'}

EXPORT_BOARD_SETUP(init_border) {'{'}
    HAL_Init();
    SystemClock_Config();
    SystemCoreClockUpdate();
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / RT_TICK_PER_SECOND);

    {initializeMatch}
{'}'}

void SysTick_Handler() {'{'}
    rt_interrupt_enter();

    rt_tick_increase();

    rt_interrupt_leave();
{'}'}

{clockMatch}

{timMatch}
"""

    os.makedirs(os.path.dirname(f"../{project_name}/App/Src/main.cpp"), exist_ok=True)
    os.makedirs(os.path.dirname(f"../{project_name}/App/Inc/main.h"), exist_ok=True)
    os.remove(f"../{project_name}/Core/Src/main.c")

    with open(f"../{project_name}/App/Src/main.cpp", "w") as file:
        file.write(cppMain)

    with open(f"../{project_name}/Core/Inc/main.h", "r") as file:
        mainH = file.read()
    with open(f"../{project_name}/App/Inc/main.h", "w") as file:
        file.write(mainH)
    os.remove(f"../{project_name}/Core/Inc/main.h")

    uvprojx = f"../{project_name}/MDK-ARM/{project_name}.uvprojx"
    with open(uvprojx, "r") as file:
        data = file.read()
    data = data.replace(
        """<File>
              <FileName>main.c</FileName>
              <FileType>1</FileType>
              <FilePath>../Core/Src/main.c</FilePath>
            </File>""",
        "",
    )
    with open(uvprojx, "w") as file:
        file.write(data)


group = group_list = [
    {
        "GroupName": "Application",
        "Files": [
            {
                "FileName": "main.cpp",
                "FileType": "8",
                "FilePath": "..\\App\\Src\\main.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/algorithm",
        "Files": [
            {
                "FileName": "infantry_pid_controller.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\algorithm\\Src\\infantry_pid_controller.cpp",
            },
            {
                "FileName": "infantry_quaternion_extended_kalman_filter.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\algorithm\\Src\\infantry_quaternion_extended_kalman_filter.cpp",
            },
            {
                "FileName": "infantry_kalman.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\algorithm\\Src\\infantry_kalman.cpp",
            },
            {
                "FileName": "infantry_math.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\algorithm\\Src\\infantry_math.cpp",
            },
            {
                "FileName": "infantry_matrix.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\algorithm\\Src\\infantry_matrix.cpp",
            },
            {
                "FileName": "infantry_low_pass_filter.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\algorithm\\Src\\infantry_low_pass_filter.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/device",
        "Files": [
            {
                "FileName": "infantry_device_list.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\device\\Src\\infantry_device_list.cpp",
            },
            {
                "FileName": "infantry_fdcan_device.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\device\\Src\\infantry_fdcan_device.cpp",
            },
            {
                "FileName": "infantry_pwm_device.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\device\\Src\\infantry_pwm_device.cpp",
            },
            {
                "FileName": "infantry_usart_device.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\device\\Src\\infantry_usart_device.cpp",
            },
            {
                "FileName": "infantry_gpio_device.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\device\\Src\\infantry_gpio_device.cpp",
            },
            {
                "FileName": "infantry_spi_device.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\device\\Src\\infantry_spi_device.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/frame",
        "Files": [
            {
                "FileName": "infantry_board.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_board.cpp",
            },
            {
                "FileName": "infantry_cplus.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_cplus.cpp",
            },
            {
                "FileName": "infantry_event.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_event.cpp",
            },
            {
                "FileName": "infantry_fsm.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_fsm.cpp",
            },
            {
                "FileName": "infantry_mailbox.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_mailbox.cpp",
            },
            {
                "FileName": "infantry_messagequeue.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_messagequeue.cpp",
            },
            {
                "FileName": "infantry_mutex.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_mutex.cpp",
            },
            {
                "FileName": "infantry_sem.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_sem.cpp",
            },
            {
                "FileName": "infantry_thread.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\frame\\Src\\infantry_thread.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/logger",
        "Files": [
            {
                "FileName": "elog.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\elog.c",
            },
            {
                "FileName": "elog_async.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\elog_async.c",
            },
            {
                "FileName": "elog_buf.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\elog_buf.c",
            },
            {
                "FileName": "elog_port.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\elog_port.c",
            },
            {
                "FileName": "elog_utils.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\elog_utils.c",
            },
            {
                "FileName": "infantry_log.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\infantry_log.cpp",
            },
            {
                "FileName": "SEGGER_RTT.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\SEGGER_RTT.c",
            },
            {
                "FileName": "SEGGER_RTT_ASM_ARMv7M.S",
                "FileType": "2",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\SEGGER_RTT_ASM_ARMv7M.S",
            },
            {
                "FileName": "SEGGER_RTT_printf.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\SEGGER_RTT_printf.c",
            },
            {
                "FileName": "SEGGER_RTT_Syscalls_KEIL.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\logger\\Src\\SEGGER_RTT_Syscalls_KEIL.c",
            },
        ],
    },
    {
        "GroupName": "Infantry/grapher",
        "Files": [
            {
                "FileName": "infantry_graph.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\graph_task\\Src\\infantry_graph.cpp",
            },
            {
                "FileName": "infantry_task.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\plugins\\graph_task\\Src\\infantry_task.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/lib",
        "Files": [
            {
                "FileName": "infantry_dwt_lib.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\library\\Src\\infantry_dwt_lib.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/example",
        "Files": [
            {
                "FileName": "bmi088.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\example\\Src\\bmi088.cpp",
            },
            {
                "FileName": "motor.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\example\\Src\\motor.cpp",
            },
        ],
    },
    {
        "GroupName": "Infantry/periph",
        "Files": [
            {
                "FileName": "infantry_ins.cpp",
                "FileType": "8",
                "FilePath": "..\\..\\InfantryFrame\\Infantry\\periph\\Src\\infantry_ins.cpp",
            },
        ],
    },
    {
        "GroupName": "RT-Thread/kernel",
        "Files": [
            {
                "FileName": "context_rvds.S",
                "FileType": "2",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\libcpu\\arm\\cortex-m4\\context_rvds.S",
            },
            {
                "FileName": "cpuport.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\libcpu\\arm\\cortex-m4\\cpuport.c",
            },
            {
                "FileName": "clock.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\clock.c",
            },
            {
                "FileName": "components.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\components.c",
            },
            {
                "FileName": "cpu.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\cpu.c",
            },
            {
                "FileName": "idle.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\idle.c",
            },
            {
                "FileName": "ipc.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\ipc.c",
            },
            {
                "FileName": "irq.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\irq.c",
            },
            {
                "FileName": "kservice.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\kservice.c",
            },
            {
                "FileName": "mem.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\mem.c",
            },
            {
                "FileName": "memheap.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\memheap.c",
            },
            {
                "FileName": "mempool.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\mempool.c",
            },
            {
                "FileName": "object.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\object.c",
            },
            {
                "FileName": "scheduler.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\scheduler.c",
            },
            {
                "FileName": "slab.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\slab.c",
            },
            {
                "FileName": "thread.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\thread.c",
            },
            {
                "FileName": "timer.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\src\\timer.c",
            },
        ],
    },
    {
        "GroupName": "RT-Thread/finsh",
        "Files": [
            {
                "FileName": "cmd.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\components\\finsh\\cmd.c",
            },
            {
                "FileName": "finsh_port.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\components\\finsh\\finsh_port.c",
            },
            {
                "FileName": "msh.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\components\\finsh\\msh.c",
            },
            {
                "FileName": "shell.c",
                "FileType": "1",
                "FilePath": "..\\..\\InfantryFrame\\RT-Thread\\RealThread_RTOS\\components\\finsh\\shell.c",
            },
        ],
    },
    {
        "GroupName": "DSP",
        "Files": [
            {
                "FileName": "arm_cortexM4lf_math.lib",
                "FileType": "4",
                "FilePath": "..\\..\\InfantryFrame\\DSP\\ARM\\arm_cortexM4lf_math.lib",
            },
        ],
    },
]

if __name__ == "__main__":
    if len(sys.argv) == 3 and sys.argv[1] == "init":
        modifyOption(sys.argv[2])
        modifyIncludePath(sys.argv[2])
        modifySourceGroup(sys.argv[2], group)
        createCMakeList(sys.argv[2])
        createMain(sys.argv[2])
    else:
        print("Usage: ./InitProject.exe init <projectName>")
