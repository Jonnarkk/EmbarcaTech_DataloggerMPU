# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)


project(Usar_SSD C CXX ASM)
pico_sdk_init()

set(FREERTOS_KERNEL_PATH "C:/Users/Miller/Desktop/Univasf/Semestre III/Embarca/FreeRTOS-Kernel")
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_subdirectory(lib/FatFs_SPI)    

add_executable(${PROJECT_NAME}  
        Cartao_FatFS_SPI.c
        hw_config.c
        ${CMAKE_CURRENT_LIST_DIR}/libs/led_matriz.c
        ${CMAKE_CURRENT_LIST_DIR}/libs/ssd1306.c
        ${CMAKE_CURRENT_LIST_DIR}/libs/buzzer.c
        )

    

target_link_libraries(${PROJECT_NAME} 
        FreeRTOS-Kernel         # Kernel do FreeRTOS
        FreeRTOS-Kernel-Heap4   # Gerenciador de memoria
        pico_stdlib 
        FatFs_SPI
        hardware_clocks
        hardware_adc
        hardware_i2c
        hardware_pio
        )

# Adiciona os diretórios de inclusão para o compilador encontrar os .h
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}           # Diretório raiz do projeto (onde está o FreeRTOSConfig.h)
    ${CMAKE_CURRENT_LIST_DIR}/libs      # Diretório das suas bibliotecas (ssd1306.h, etc.)
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio_matriz.pio)

