# Declare current directory as a Zephyr library (defaults to directory name)
zephyr_library()

# Source code files for the library
zephyr_library_sources(seesaw.c)

# Add header files to CMake search directories
zephyr_include_directories(.)