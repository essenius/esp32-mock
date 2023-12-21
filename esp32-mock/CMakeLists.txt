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

foreach (entry ${CMAKE_PREFIX_PATH})
    list(APPEND includeFolders ${entry}/include)
endforeach() 

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