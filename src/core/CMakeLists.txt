add_library(core STATIC
	component.cpp
	gameboy.cpp
	globals.cpp
	rewinder.cpp
	rom_list.cpp
	settings.cpp
	state.cpp
	audio/apu.cpp
	audio/audio_driver.cpp
	audio/channel1.cpp
	audio/channel2.cpp
	audio/channel3.cpp
	audio/channel4.cpp
	audio/channel.cpp
	audio/frequency_sweep.cpp
	audio/length_counter.cpp
	audio/volume_envelope.cpp
	common/binary_file.cpp
	common/bmp.cpp
	common/file_utils.cpp
	common/hash.cpp
	common/image.cpp
	common/ini_file.cpp
	common/logger.cpp
	common/parser.cpp
	common/png.cpp
	common/utils.cpp
	common/wave_file.cpp
	cpu/cpu.cpp
	cpu/timer.cpp
	debug/cpu_debugger.cpp
	debug/debugger.cpp
	debug/memory_debugger.cpp
	display/display.cpp
	display/font.cpp
	gpu/color_palette.cpp
	gpu/dmg_palette.cpp
	gpu/gpu.cpp
	gpu/lcdc.cpp
	gpu/sprite.cpp
	gpu/tile_attribute.cpp
	input/input.cpp
	input/joypad.cpp
	memory/boot_rom.cpp
	memory/dma.cpp
	memory/gbc_reg.cpp
	memory/hdma.cpp
	memory/mmu.cpp
	rom/cart.cpp
	rom/mbc.cpp
	rom/mbc1.cpp
	rom/mbc2.cpp
	rom/mbc3.cpp
	rom/mbc5.cpp
	rom/rom.cpp
	rom/rtc.cpp
	serial/serial.cpp
	tools/disassembler.cpp)

target_include_directories(core PUBLIC ../core)
target_link_libraries(core PUBLIC stdc++fs)

if (PNG_FOUND)
	target_link_libraries(core PUBLIC png)
endif()
