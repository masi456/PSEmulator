add_library (libutils
    memory_utils.hpp
    memory_utils.cpp
    file.hpp
    file.cpp
    exception.hpp
    platform.hpp
    platform.cpp
    math.hpp
)

target_include_directories (libutils PUBLIC ../)
