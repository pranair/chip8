##
# Chip8
#
# @file
# @version 0.1

CPP := clang++
header_files = Misc.hpp Platform.hpp Chip8VM.hpp
src_files = main.cpp Platform.cpp Misc.cpp Chip8VM.cpp


chip8: $(src_files) $(header_files)
	$(CPP) $(src_files) -std=c++17 -Wall -Wextra -g `sdl2-config --cflags --libs` -ftime-trace -o chip8

# end
