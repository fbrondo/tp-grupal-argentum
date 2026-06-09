target_sources(taller_launcher
    PRIVATE
        main.cpp
        login_window.cpp
        character_window.cpp
        signup_window.cpp
        resources.qrc
    PUBLIC
        login_window.h
        character_window.h
        signup_window.h
)

if(NOT DEFINED ARGENTUM_SHARE_PATH)
    set(ARGENTUM_SHARE_PATH "${CMAKE_SOURCE_DIR}")
endif()
target_compile_definitions(taller_launcher PRIVATE
    "ARGENTUM_SHARE_PATH=\"${ARGENTUM_SHARE_PATH}\"")
