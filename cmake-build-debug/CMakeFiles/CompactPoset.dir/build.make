# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/kliment/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6911.21/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/kliment/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6911.21/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kliment/CLionProjects/CompactPoset

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kliment/CLionProjects/CompactPoset/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/CompactPoset.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CompactPoset.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CompactPoset.dir/flags.make

CMakeFiles/CompactPoset.dir/main.cpp.o: CMakeFiles/CompactPoset.dir/flags.make
CMakeFiles/CompactPoset.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kliment/CLionProjects/CompactPoset/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CompactPoset.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CompactPoset.dir/main.cpp.o -c /home/kliment/CLionProjects/CompactPoset/main.cpp

CMakeFiles/CompactPoset.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CompactPoset.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kliment/CLionProjects/CompactPoset/main.cpp > CMakeFiles/CompactPoset.dir/main.cpp.i

CMakeFiles/CompactPoset.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CompactPoset.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kliment/CLionProjects/CompactPoset/main.cpp -o CMakeFiles/CompactPoset.dir/main.cpp.s

# Object files for target CompactPoset
CompactPoset_OBJECTS = \
"CMakeFiles/CompactPoset.dir/main.cpp.o"

# External object files for target CompactPoset
CompactPoset_EXTERNAL_OBJECTS =

CompactPoset: CMakeFiles/CompactPoset.dir/main.cpp.o
CompactPoset: CMakeFiles/CompactPoset.dir/build.make
CompactPoset: CMakeFiles/CompactPoset.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kliment/CLionProjects/CompactPoset/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable CompactPoset"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CompactPoset.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CompactPoset.dir/build: CompactPoset

.PHONY : CMakeFiles/CompactPoset.dir/build

CMakeFiles/CompactPoset.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CompactPoset.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CompactPoset.dir/clean

CMakeFiles/CompactPoset.dir/depend:
	cd /home/kliment/CLionProjects/CompactPoset/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kliment/CLionProjects/CompactPoset /home/kliment/CLionProjects/CompactPoset /home/kliment/CLionProjects/CompactPoset/cmake-build-debug /home/kliment/CLionProjects/CompactPoset/cmake-build-debug /home/kliment/CLionProjects/CompactPoset/cmake-build-debug/CMakeFiles/CompactPoset.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CompactPoset.dir/depend

