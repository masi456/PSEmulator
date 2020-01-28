add_executable (emulator 
    main.cpp
)

target_link_libraries (emulator LINK_PUBLIC libps)
target_link_libraries (emulator LINK_PUBLIC libutils)

target_include_directories (emulator PUBLIC ../)

# target_link_options (emulator PUBLIC --preload-file ../../files)