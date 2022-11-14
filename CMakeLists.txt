cmake_minimum_required(VERSION 3.2.0)
project(zp C CXX ASM)

set(ELF_NAME ${PROJECT_NAME}.elf)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(ATTITUDE_MANAGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager")
set(LAMINAR_OS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/LaminarOS")

include_directories(
    ${ATTITUDE_MANAGER_DIR}/Inc
    ${ATTITUDE_MANAGER_DIR}/ControlAlgorithms/Inc
    ${LAMINAR_OS_DIR}/Interface/Inc
    ${LAMINAR_OS_DIR}/Drivers/Inc
    ${LAMINAR_OS_DIR}/Core/Inc
    ${LAMINAR_OS_DIR}/boardfiles/${FOLDER_NAME}/Inc
    ${LAMINAR_OS_DIR}/boardfiles/${FOLDER_NAME}/Drivers/${FAMILY_NAME}_HAL_Driver/Inc
    ${LAMINAR_OS_DIR}/boardfiles/${FOLDER_NAME}/Core/Inc
    ${LAMINAR_OS_DIR}/boardfiles/${FOLDER_NAME}/Drivers/CMSIS/Device/ST/${FAMILY_NAME}/Include
    ${LAMINAR_OS_DIR}/boardfiles/${FOLDER_NAME}/Drivers/CMSIS/Include
)

# Libraries
add_subdirectory(LaminarOS)

set(STARTUP_ASM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/LaminarOS/${STARTUP_ASM})
set(LINKER_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/LaminarOS/${LINKER_SCRIPT})

set(ATTITUDE_MANAGER_CXX_SOURCES
  ${ATTITUDE_MANAGER_DIR}/Src/AM.cpp
  ${ATTITUDE_MANAGER_DIR}/Src/PID.cpp
  ${ATTITUDE_MANAGER_DIR}/ControlAlgorithms/Src/AM_FixedControl.cpp
  ${ATTITUDE_MANAGER_DIR}/ControlAlgorithms/Src/AM_LevelQuadControl.cpp
)

file(GLOB_RECURSE CXX_SOURCES ${ATTITUDE_MANAGER_CXX_SOURCES})
#message("C++ Sources: ${CXX_SOURCES}")

add_executable(${ELF_NAME} main.cpp ${CXX_SOURCES} ${STARTUP_ASM_FILE})

target_link_libraries(${ELF_NAME} LaminarOS)

# Add project-specific linker flags (.ld script, .map file)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT_FILE} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref")

set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

# objcopy the .elf file to .bin file for programming
add_custom_target("${PROJECT_NAME}.bin" ALL
  COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${ELF_NAME}> ${BIN_FILE}
  DEPENDS ${ELF_NAME}
)

# Print size information after compiling
add_custom_command(TARGET ${ELF_NAME}
  POST_BUILD
  COMMENT "Binary size information:"
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${ELF_NAME}>
)

# the following is windows only
set(INSTALL_CMD "ST-LINK_CLI")
set(INSTALL_OPTS "-c SWD -P \"${BIN_FILE}\" 0x08000000 -NoPrompt -Rst -Run")

install(CODE
  "execute_process(
    COMMAND ${INSTALL_CMD} ${INSTALL_OPTS}
  )"
)

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message("Release build.")
ELSE()
    message("Some other build type.")
ENDIF()

add_custom_target(UPLOAD
        arm-none-eabi-gdb -iex "target remote tcp:127.0.0.1:3333"
        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
        -iex "monitor reset init"
        -iex "disconnect" -iex "quit")