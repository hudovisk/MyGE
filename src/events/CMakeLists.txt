set(events_source_files EventManager.cpp InputContext.cpp )
set(events_header_files EventManager.h Events.h InputContext.h InputTypes.h )

add_library(events ${events_source_files} ${events_header_files})

target_link_libraries(events my_debug ${EXTRA_LIBS_PATH}/SDL2.lib)

