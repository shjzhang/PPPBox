# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kemin/develop/PPPBOX

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kemin/develop/PPPBOX

# Include any dependencies generated for this target.
include apps/Rinextools/CMakeFiles/RinSum.dir/depend.make

# Include the progress variables for this target.
include apps/Rinextools/CMakeFiles/RinSum.dir/progress.make

# Include the compile flags for this target's objects.
include apps/Rinextools/CMakeFiles/RinSum.dir/flags.make

apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o: apps/Rinextools/CMakeFiles/RinSum.dir/flags.make
apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o: apps/Rinextools/RinSum.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kemin/develop/PPPBOX/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o"
	cd /home/kemin/develop/PPPBOX/apps/Rinextools && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/RinSum.dir/RinSum.cpp.o -c /home/kemin/develop/PPPBOX/apps/Rinextools/RinSum.cpp

apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RinSum.dir/RinSum.cpp.i"
	cd /home/kemin/develop/PPPBOX/apps/Rinextools && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kemin/develop/PPPBOX/apps/Rinextools/RinSum.cpp > CMakeFiles/RinSum.dir/RinSum.cpp.i

apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RinSum.dir/RinSum.cpp.s"
	cd /home/kemin/develop/PPPBOX/apps/Rinextools && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kemin/develop/PPPBOX/apps/Rinextools/RinSum.cpp -o CMakeFiles/RinSum.dir/RinSum.cpp.s

apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.requires:
.PHONY : apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.requires

apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.provides: apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.requires
	$(MAKE) -f apps/Rinextools/CMakeFiles/RinSum.dir/build.make apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.provides.build
.PHONY : apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.provides

apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.provides.build: apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o

# Object files for target RinSum
RinSum_OBJECTS = \
"CMakeFiles/RinSum.dir/RinSum.cpp.o"

# External object files for target RinSum
RinSum_EXTERNAL_OBJECTS =

apps/Rinextools/RinSum: apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o
apps/Rinextools/RinSum: apps/Rinextools/CMakeFiles/RinSum.dir/build.make
apps/Rinextools/RinSum: libpppbox.so
apps/Rinextools/RinSum: apps/Rinextools/CMakeFiles/RinSum.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable RinSum"
	cd /home/kemin/develop/PPPBOX/apps/Rinextools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RinSum.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/Rinextools/CMakeFiles/RinSum.dir/build: apps/Rinextools/RinSum
.PHONY : apps/Rinextools/CMakeFiles/RinSum.dir/build

apps/Rinextools/CMakeFiles/RinSum.dir/requires: apps/Rinextools/CMakeFiles/RinSum.dir/RinSum.cpp.o.requires
.PHONY : apps/Rinextools/CMakeFiles/RinSum.dir/requires

apps/Rinextools/CMakeFiles/RinSum.dir/clean:
	cd /home/kemin/develop/PPPBOX/apps/Rinextools && $(CMAKE_COMMAND) -P CMakeFiles/RinSum.dir/cmake_clean.cmake
.PHONY : apps/Rinextools/CMakeFiles/RinSum.dir/clean

apps/Rinextools/CMakeFiles/RinSum.dir/depend:
	cd /home/kemin/develop/PPPBOX && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kemin/develop/PPPBOX /home/kemin/develop/PPPBOX/apps/Rinextools /home/kemin/develop/PPPBOX /home/kemin/develop/PPPBOX/apps/Rinextools /home/kemin/develop/PPPBOX/apps/Rinextools/CMakeFiles/RinSum.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/Rinextools/CMakeFiles/RinSum.dir/depend
