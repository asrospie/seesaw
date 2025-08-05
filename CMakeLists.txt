#Include required subdirectories
add_subdirectory(drivers)

# add subdirectories to the comiler's include search path (.h files)
zephyr_include_directories(drivers)