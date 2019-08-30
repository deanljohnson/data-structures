set(SKIP_LIST_SOURCES
    SkipList.h
    SkipList.cpp
)

add_library(skip-list OBJECT ${SKIP_LIST_SOURCES})
target_include_directories(skip-list PRIVATE ${DOC_TEST_INCLUDE_DIR})

add_executable(skip-list-test ${TEST_RUNNER} $<TARGET_OBJECTS:skip-list>)
target_include_directories(skip-list-test PRIVATE ${DOC_TEST_INCLUDE_DIR})
