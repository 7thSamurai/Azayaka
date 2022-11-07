project(Azayaka-tester)

add_executable(
    Azayaka-tester
	csv_file.cpp
	main.cpp
	suites/blargg.cpp
	suites/mooneye.cpp
	suites/suite.cpp
    tester.cpp
)

target_link_libraries(Azayaka-tester PRIVATE core)
