target_sources(taller_editor
    PRIVATE
    # .cpp files
    main.cpp
    mainwindow.cpp
    sprite.cpp
    tile_widget.cpp
    map_scene.cpp
    map_view.cpp
    new_map_dialog.cpp
    PUBLIC
    # .h files
    mainwindow.h
    sprite.h
    tile_widget.h
    map_scene.h
    map_view.h
    new_map_dialog.h
)

if(NOT DEFINED ARGENTUM_SHARE_PATH)
    set(ARGENTUM_SHARE_PATH "${CMAKE_SOURCE_DIR}")
endif()
target_compile_definitions(taller_editor PRIVATE
    "ARGENTUM_SHARE_PATH=\"${ARGENTUM_SHARE_PATH}\"")
