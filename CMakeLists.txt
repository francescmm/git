file(GLOB SRC_FILES
  *.cpp
  *.h
  *.ui
)


add_library(git STATIC
    ${SRC_FILES}
)

target_link_libraries(git
    PUBLIC
    Qt::Core
    QLogger
)
target_include_directories(git PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
