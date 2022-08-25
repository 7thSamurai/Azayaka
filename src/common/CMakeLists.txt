add_library(common STATIC
	binary_file.cpp
	bmp.cpp
	color.cpp
	file_utils.cpp
	hash.cpp
	image.cpp
	ini_file.cpp
	logger.cpp
	parser.cpp
	png.cpp
	string_utils.cpp
	wave_file.cpp
)

target_link_libraries(common PUBLIC stdc++fs)

if (PNG_FOUND)
	target_link_libraries(common PUBLIC png)
endif()

if (OpenGL_FOUND)
	target_sources(common PRIVATE
		opengl/shader.cpp
		opengl/texture.cpp
	)

	target_link_libraries(common PRIVATE OpenGL::GL)
endif()
