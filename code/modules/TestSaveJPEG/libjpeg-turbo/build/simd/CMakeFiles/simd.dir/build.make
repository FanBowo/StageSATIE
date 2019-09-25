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

# Include any dependencies generated for this target.
include simd/CMakeFiles/simd.dir/depend.make

# Include the progress variables for this target.
include simd/CMakeFiles/simd.dir/progress.make

# Include the compile flags for this target's objects.
include simd/CMakeFiles/simd.dir/flags.make

simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o: simd/CMakeFiles/simd.dir/flags.make
simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o: ../simd/arm64/jsimd_neon.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o"
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd && /usr/bin/cc  $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/simd.dir/arm64/jsimd_neon.S.o -c /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/simd/arm64/jsimd_neon.S

simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.requires:

.PHONY : simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.requires

simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.provides: simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.requires
	$(MAKE) -f simd/CMakeFiles/simd.dir/build.make simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.provides.build
.PHONY : simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.provides

simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.provides.build: simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o


simd/CMakeFiles/simd.dir/arm64/jsimd.c.o: simd/CMakeFiles/simd.dir/flags.make
simd/CMakeFiles/simd.dir/arm64/jsimd.c.o: ../simd/arm64/jsimd.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object simd/CMakeFiles/simd.dir/arm64/jsimd.c.o"
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/simd.dir/arm64/jsimd.c.o   -c /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/simd/arm64/jsimd.c

simd/CMakeFiles/simd.dir/arm64/jsimd.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simd.dir/arm64/jsimd.c.i"
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/simd/arm64/jsimd.c > CMakeFiles/simd.dir/arm64/jsimd.c.i

simd/CMakeFiles/simd.dir/arm64/jsimd.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simd.dir/arm64/jsimd.c.s"
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/simd/arm64/jsimd.c -o CMakeFiles/simd.dir/arm64/jsimd.c.s

simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.requires:

.PHONY : simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.requires

simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.provides: simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.requires
	$(MAKE) -f simd/CMakeFiles/simd.dir/build.make simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.provides.build
.PHONY : simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.provides

simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.provides.build: simd/CMakeFiles/simd.dir/arm64/jsimd.c.o


simd: simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o
simd: simd/CMakeFiles/simd.dir/arm64/jsimd.c.o
simd: simd/CMakeFiles/simd.dir/build.make

.PHONY : simd

# Rule to build all files generated by this target.
simd/CMakeFiles/simd.dir/build: simd

.PHONY : simd/CMakeFiles/simd.dir/build

simd/CMakeFiles/simd.dir/requires: simd/CMakeFiles/simd.dir/arm64/jsimd_neon.S.o.requires
simd/CMakeFiles/simd.dir/requires: simd/CMakeFiles/simd.dir/arm64/jsimd.c.o.requires

.PHONY : simd/CMakeFiles/simd.dir/requires

simd/CMakeFiles/simd.dir/clean:
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd && $(CMAKE_COMMAND) -P CMakeFiles/simd.dir/cmake_clean.cmake
.PHONY : simd/CMakeFiles/simd.dir/clean

simd/CMakeFiles/simd.dir/depend:
	cd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/simd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd /home/firefly/Desktop/StageSATIE/code/modules/TestSaveJPEG/libjpeg-turbo/build/simd/CMakeFiles/simd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : simd/CMakeFiles/simd.dir/depend
