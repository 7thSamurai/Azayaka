project(Azayaka-sdl)

add_executable(Azayaka-sdl
	audio_sdl.cpp
	debugger_sdl.cpp
	display_sdl.cpp
	input_sdl.cpp
	main.cpp
	options.cpp
	window_sdl.cpp)

if (OpenGL_FOUND)
	target_sources(Azayaka-sdl PRIVATE 
		opengl/display_gl.cpp)
endif()

target_link_libraries(Azayaka-sdl PRIVATE core SDL2::SDL2 SDL2::SDL2main)
