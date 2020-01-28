enable_testing ()
find_package (GTest CONFIG REQUIRED)
include (GoogleTest)

add_executable (test 
    main.cpp
    test_load_delay_slot.cpp
)
target_link_libraries (test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

gtest_discover_tests (test)

target_link_libraries (test LINK_PUBLIC libps)
target_link_libraries (test LINK_PUBLIC libutils)
