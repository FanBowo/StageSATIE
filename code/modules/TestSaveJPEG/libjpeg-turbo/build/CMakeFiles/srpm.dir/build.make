# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build

# Utility rule file for srpm.

# Include the progress variables for this target.
include CMakeFiles/srpm.dir/progress.make

CMakeFiles/srpm:
	sh pkgscripts/makesrpm

srpm: CMakeFiles/srpm
srpm: CMakeFiles/srpm.dir/build.make

.PHONY : srpm

# Rule to build all files generated by this target.
CMakeFiles/srpm.dir/build: srpm

.PHONY : CMakeFiles/srpm.dir/build

CMakeFiles/srpm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/srpm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/srpm.dir/clean

CMakeFiles/srpm.dir/depend:
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/CMakeFiles/srpm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/srpm.dir/depend

