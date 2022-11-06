project(Azayaka-tester)

add_executable(Azayaka-tester
	csv.cpp
	main.cpp
	results.cpp
	suites/suite.cpp
)

target_link_libraries(Azayaka-tester PRIVATE core)
