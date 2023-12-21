if (WIN32)
    target_sources(${libName} PUBLIC time.h PRIVATE time.cpp)
endif()