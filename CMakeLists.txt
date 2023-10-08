cmake_minimum_required(VERSION 3.0)

project(amdgpu-fan-control)

add_executable(
	amdgpu-fanctrl
	src/logger2.cpp
	src/main.cpp
	src/pwm_actuator.cpp
	src/pwm_actuator_factory.cpp
	src/pwm_controller.cpp
	src/pwm_controllers.cpp
	src/runtime_config.cpp
	src/temp_sensor.cpp
	src/temp_sensor_factory.cpp
)

add_executable(amdgpu-write-test prototypes/write-test.cpp)

add_executable(amdgpu-read-test prototypes/read-test.cpp)

target_compile_options(amdgpu-fanctrl PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(amdgpu-fanctrl PRIVATE cxx_std_17)

target_compile_options(amdgpu-write-test PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(amdgpu-write-test PRIVATE cxx_std_17)

target_compile_options(amdgpu-read-test PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(amdgpu-read-test PRIVATE cxx_std_17)

install(TARGETS amdgpu-fanctrl RUNTIME DESTINATION bin)
