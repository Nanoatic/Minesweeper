
cmake_minimum_required(VERSION 3.7)
project(Minysweepy)



if (UNIX)

    set(SDL2_LIBRARY_DIR /usr/lib/x86_64-linux-gnu)
    set(SDL2_INCLUDE_DIR /usr/include)

    set(SDL2_LIBRARY libSDL2.so)
    set(SDL2_mixer_LIBRARY libSDL2_mixer.so)
    set(SDL2_ttf_LIBRARY libSDL2_ttf.so)

endif (UNIX)


if (WIN32)

    set(SDL2_LIBRARY_DIR libs/SDL2-2.0.9-Mingw/x86_64-w64-mingw32/lib)
    set(SDL2_DLL_DIR libs/SDL2-2.0.9-Mingw/x86_64-w64-mingw32/lib)

    set(SDL2_INCLUDE_DIR libs/SDL2-2.0.9-Mingw/x86_64-w64-mingw32/include/)

    set(SDL2_LIBRARY libSDL2.dll.a)
    set(SDL2_mixer_LIBRARY libSDL2_mixer.dll.a)
    set(SDL2_ttf_LIBRARY libSDL2_ttf.dll.a)


endif (WIN32)


link_directories(${SDL2_LIBRARY_DIR})
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES src/src.c)

add_executable(Minysweepy  ${SOURCE_FILES} )

if(MINGW)
    target_link_libraries(Minysweepy
            ${SDL2_LIBRARY}
            ${SDL2_ttf_LIBRARY}
            ${SDL2_mixer_LIBRARY} -lmingw32 -lSDL2main -lSDL2 )
ELSE()
    target_link_libraries(Minysweepy
            ${SDL2_LIBRARY}
            ${SDL2_ttf_LIBRARY}
            ${SDL2_mixer_LIBRARY})
ENDIF(MINGW)
set_property(TARGET Minysweepy PROPERTY C_STANDARD 11)