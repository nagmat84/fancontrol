cmake_minimum_required(VERSION 3.0)

project(fancontrol)

add_executable(
	fancontrol
	src/config/config_option.cpp
	src/config/config.cpp
	src/config/controller_config.cpp
	src/control/pwm_controller.cpp
	src/control/pwm_controllers.cpp
	src/hal/pwm_actuator.cpp
	src/hal/pwm_actuator_factory.cpp
	src/hal/temp_sensor.cpp
	src/hal/temp_sensor_factory.cpp
	src/logging/logger2.cpp
	src/main.cpp
)

target_compile_options(fancontrol PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(fancontrol PRIVATE cxx_std_17)

install(TARGETS fancontrol RUNTIME DESTINATION bin)
