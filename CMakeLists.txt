cmake_minimum_required(VERSION 3.12)
project(jumoku VERSION 202306)
add_library(jumoku INTERFACE)
target_compile_features(jumoku INTERFACE cxx_std_20)
target_include_directories(jumoku INTERFACE
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include"
	"$<INSTALL_INTERFACE:include/jumoku>"
	)
enable_testing()
add_subdirectory(test)
install(DIRECTORY include/jumoku DESTINATION include/jumoku)
install(TARGETS jumoku EXPORT jumoku-targets DESTINATION lib)
install(EXPORT jumoku-targets FILE jumoku-targets.cmake DESTINATION lib/cmake/jumoku-${jumoku_VERSION})
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/jumoku-config.cmake"
	INSTALL_DESTINATION "lib/cmake/jumoku-${jumoku_VERSION}"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
	)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/jumoku-config-version.cmake"
	VERSION "${jumoku_VERSION_MAJOR}.${jumoku_VERSION_MINOR}"
	COMPATIBILITY AnyNewerVersion
	)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/jumoku-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/jumoku-config-version.cmake"
	DESTINATION "lib/cmake/jumoku-${jumoku_VERSION}"
	)
