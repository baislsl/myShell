# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /opt/clion-2017.1.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /opt/clion-2017.1.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/baislsl/c_c++/myShell

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/baislsl/c_c++/myShell/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/myShell.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/myShell.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myShell.dir/flags.make

CMakeFiles/myShell.dir/myshell.c.o: CMakeFiles/myShell.dir/flags.make
CMakeFiles/myShell.dir/myshell.c.o: ../myshell.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/myShell.dir/myshell.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myShell.dir/myshell.c.o   -c /home/baislsl/c_c++/myShell/myshell.c

CMakeFiles/myShell.dir/myshell.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myShell.dir/myshell.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/baislsl/c_c++/myShell/myshell.c > CMakeFiles/myShell.dir/myshell.c.i

CMakeFiles/myShell.dir/myshell.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myShell.dir/myshell.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/baislsl/c_c++/myShell/myshell.c -o CMakeFiles/myShell.dir/myshell.c.s

CMakeFiles/myShell.dir/myshell.c.o.requires:

.PHONY : CMakeFiles/myShell.dir/myshell.c.o.requires

CMakeFiles/myShell.dir/myshell.c.o.provides: CMakeFiles/myShell.dir/myshell.c.o.requires
	$(MAKE) -f CMakeFiles/myShell.dir/build.make CMakeFiles/myShell.dir/myshell.c.o.provides.build
.PHONY : CMakeFiles/myShell.dir/myshell.c.o.provides

CMakeFiles/myShell.dir/myshell.c.o.provides.build: CMakeFiles/myShell.dir/myshell.c.o


CMakeFiles/myShell.dir/utility.c.o: CMakeFiles/myShell.dir/flags.make
CMakeFiles/myShell.dir/utility.c.o: ../utility.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/myShell.dir/utility.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myShell.dir/utility.c.o   -c /home/baislsl/c_c++/myShell/utility.c

CMakeFiles/myShell.dir/utility.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myShell.dir/utility.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/baislsl/c_c++/myShell/utility.c > CMakeFiles/myShell.dir/utility.c.i

CMakeFiles/myShell.dir/utility.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myShell.dir/utility.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/baislsl/c_c++/myShell/utility.c -o CMakeFiles/myShell.dir/utility.c.s

CMakeFiles/myShell.dir/utility.c.o.requires:

.PHONY : CMakeFiles/myShell.dir/utility.c.o.requires

CMakeFiles/myShell.dir/utility.c.o.provides: CMakeFiles/myShell.dir/utility.c.o.requires
	$(MAKE) -f CMakeFiles/myShell.dir/build.make CMakeFiles/myShell.dir/utility.c.o.provides.build
.PHONY : CMakeFiles/myShell.dir/utility.c.o.provides

CMakeFiles/myShell.dir/utility.c.o.provides.build: CMakeFiles/myShell.dir/utility.c.o


CMakeFiles/myShell.dir/command.c.o: CMakeFiles/myShell.dir/flags.make
CMakeFiles/myShell.dir/command.c.o: ../command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/myShell.dir/command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myShell.dir/command.c.o   -c /home/baislsl/c_c++/myShell/command.c

CMakeFiles/myShell.dir/command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myShell.dir/command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/baislsl/c_c++/myShell/command.c > CMakeFiles/myShell.dir/command.c.i

CMakeFiles/myShell.dir/command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myShell.dir/command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/baislsl/c_c++/myShell/command.c -o CMakeFiles/myShell.dir/command.c.s

CMakeFiles/myShell.dir/command.c.o.requires:

.PHONY : CMakeFiles/myShell.dir/command.c.o.requires

CMakeFiles/myShell.dir/command.c.o.provides: CMakeFiles/myShell.dir/command.c.o.requires
	$(MAKE) -f CMakeFiles/myShell.dir/build.make CMakeFiles/myShell.dir/command.c.o.provides.build
.PHONY : CMakeFiles/myShell.dir/command.c.o.provides

CMakeFiles/myShell.dir/command.c.o.provides.build: CMakeFiles/myShell.dir/command.c.o


CMakeFiles/myShell.dir/internal.c.o: CMakeFiles/myShell.dir/flags.make
CMakeFiles/myShell.dir/internal.c.o: ../internal.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/myShell.dir/internal.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myShell.dir/internal.c.o   -c /home/baislsl/c_c++/myShell/internal.c

CMakeFiles/myShell.dir/internal.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myShell.dir/internal.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/baislsl/c_c++/myShell/internal.c > CMakeFiles/myShell.dir/internal.c.i

CMakeFiles/myShell.dir/internal.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myShell.dir/internal.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/baislsl/c_c++/myShell/internal.c -o CMakeFiles/myShell.dir/internal.c.s

CMakeFiles/myShell.dir/internal.c.o.requires:

.PHONY : CMakeFiles/myShell.dir/internal.c.o.requires

CMakeFiles/myShell.dir/internal.c.o.provides: CMakeFiles/myShell.dir/internal.c.o.requires
	$(MAKE) -f CMakeFiles/myShell.dir/build.make CMakeFiles/myShell.dir/internal.c.o.provides.build
.PHONY : CMakeFiles/myShell.dir/internal.c.o.provides

CMakeFiles/myShell.dir/internal.c.o.provides.build: CMakeFiles/myShell.dir/internal.c.o


CMakeFiles/myShell.dir/parser.c.o: CMakeFiles/myShell.dir/flags.make
CMakeFiles/myShell.dir/parser.c.o: ../parser.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/myShell.dir/parser.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myShell.dir/parser.c.o   -c /home/baislsl/c_c++/myShell/parser.c

CMakeFiles/myShell.dir/parser.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myShell.dir/parser.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/baislsl/c_c++/myShell/parser.c > CMakeFiles/myShell.dir/parser.c.i

CMakeFiles/myShell.dir/parser.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myShell.dir/parser.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/baislsl/c_c++/myShell/parser.c -o CMakeFiles/myShell.dir/parser.c.s

CMakeFiles/myShell.dir/parser.c.o.requires:

.PHONY : CMakeFiles/myShell.dir/parser.c.o.requires

CMakeFiles/myShell.dir/parser.c.o.provides: CMakeFiles/myShell.dir/parser.c.o.requires
	$(MAKE) -f CMakeFiles/myShell.dir/build.make CMakeFiles/myShell.dir/parser.c.o.provides.build
.PHONY : CMakeFiles/myShell.dir/parser.c.o.provides

CMakeFiles/myShell.dir/parser.c.o.provides.build: CMakeFiles/myShell.dir/parser.c.o


# Object files for target myShell
myShell_OBJECTS = \
"CMakeFiles/myShell.dir/myshell.c.o" \
"CMakeFiles/myShell.dir/utility.c.o" \
"CMakeFiles/myShell.dir/command.c.o" \
"CMakeFiles/myShell.dir/internal.c.o" \
"CMakeFiles/myShell.dir/parser.c.o"

# External object files for target myShell
myShell_EXTERNAL_OBJECTS =

myShell: CMakeFiles/myShell.dir/myshell.c.o
myShell: CMakeFiles/myShell.dir/utility.c.o
myShell: CMakeFiles/myShell.dir/command.c.o
myShell: CMakeFiles/myShell.dir/internal.c.o
myShell: CMakeFiles/myShell.dir/parser.c.o
myShell: CMakeFiles/myShell.dir/build.make
myShell: CMakeFiles/myShell.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C executable myShell"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myShell.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myShell.dir/build: myShell

.PHONY : CMakeFiles/myShell.dir/build

CMakeFiles/myShell.dir/requires: CMakeFiles/myShell.dir/myshell.c.o.requires
CMakeFiles/myShell.dir/requires: CMakeFiles/myShell.dir/utility.c.o.requires
CMakeFiles/myShell.dir/requires: CMakeFiles/myShell.dir/command.c.o.requires
CMakeFiles/myShell.dir/requires: CMakeFiles/myShell.dir/internal.c.o.requires
CMakeFiles/myShell.dir/requires: CMakeFiles/myShell.dir/parser.c.o.requires

.PHONY : CMakeFiles/myShell.dir/requires

CMakeFiles/myShell.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myShell.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myShell.dir/clean

CMakeFiles/myShell.dir/depend:
	cd /home/baislsl/c_c++/myShell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/baislsl/c_c++/myShell /home/baislsl/c_c++/myShell /home/baislsl/c_c++/myShell/cmake-build-debug /home/baislsl/c_c++/myShell/cmake-build-debug /home/baislsl/c_c++/myShell/cmake-build-debug/CMakeFiles/myShell.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myShell.dir/depend

