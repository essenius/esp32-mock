include(FindGit)
find_package(Git)
include(FetchContent)

FetchContent_Declare(
  safe-cstring
  GIT_REPOSITORY https://github.com/essenius/safe-cstring
  GIT_TAG        main
)
FetchContent_MakeAvailable(safe-cstring)

add_library(${libName} "")

# If we build under vcpkg, we add the include folder 
if (DEFINED CMAKE_PREFIX_PATH) 
  file(APPEND ${PROJECT_BINARY_DIR}/output.txt "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}\n")
  foreach (entry ${CMAKE_PREFIX_PATH})
      list(APPEND includeFolders ${entry}/include)
  endforeach() 
# Otherwise, we directly add the dependency include folder (it's header only)
else()
  file(APPEND ${PROJECT_BINARY_DIR}/output.txt "CMAKE_PREFIX_PATH not found\n")
  list(APPEND includeFolders ${safe-cstring_SOURCE_DIR}/src)
endif()
file(APPEND ${PROJECT_BINARY_DIR}/output.txt "includeFolders: ${includeFolders}\n")

add_subdirectory(freertos)
add_subdirectory(sys)

set(myHeaders Adafruit_SSD1306.h Client.h ESP.h HTTPClient.h HTTPUpdate.h IPAddress.h Preferences.h PubSubClient.h StringArduino.h WiFi.h WiFiClient.h WiFiClientSecure.h Wire.h)
set(mySources ESP.cpp HTTPClient.cpp HTTPUpdate.cpp IPAddress.cpp Preferences.cpp PubSubClient.cpp WiFi.cpp WiFiClientSecure.cpp Wire.cpp)

target_sources (${libName} PUBLIC ${myHeaders} PRIVATE ${mySources})

# show all variables. 
MESSAGE(STATUS "Variables for esp32-mock:")
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    MESSAGE(STATUS "${_variableName}=${${_variableName}}")
endforeach()

target_include_directories(${libName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${includeFolders} )
install(TARGETS ${libName} DESTINATION lib)
install(FILES ${myHeaders} DESTINATION include) 