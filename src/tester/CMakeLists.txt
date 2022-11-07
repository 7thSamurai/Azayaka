project(Azayaka-tester)

add_executable(
    Azayaka-tester
	csv_file.cpp
	main.cpp
	suite.cpp
	tester.cpp
	suites/blargg.cpp
	suites/mooneye.cpp
)

target_link_libraries(Azayaka-tester PRIVATE core)
