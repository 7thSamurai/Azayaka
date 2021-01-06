// Copyright (C) 2020-2021 Zach Collins <the_7thSamurai@protonmail.com>
//
// Azayaka is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Azayaka is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Azayaka. If not, see <https://www.gnu.org/licenses/>.

#include "options.hpp"

DebugOption::DebugOption() : Option("debug", 'd', 0) {
    debug = false;
}

bool DebugOption::get_debug() const {
    return debug;
}

std::string DebugOption::get_description() const {
    return "\tRun the emulator in debug mode";
}

void DebugOption::set(char **argv, int index) {
    debug = true;
}


DumpUsageOption::DumpUsageOption() : Option("dump-usage", 'D', 0) {
    dump_usage = false;
}

bool DumpUsageOption::get_dump_usage() const {
    return dump_usage;
}

std::string DumpUsageOption::get_description() const {
    return "Saves the ROM usage(CDL) to a file";
}

void DumpUsageOption::set(char **argv, int index) {
    dump_usage = true;
}


ScaleOption::ScaleOption() : Option("scale", 's', 1) {
    scale = 4;
}

float ScaleOption::get_scale() const {
    return scale;
}

std::string ScaleOption::get_description() const {
    return "\tScales the screen by the factor given";
}

void ScaleOption::set(char **argv, int index) {
    scale = std::atof(argv[index+1]);
}


RateOption::RateOption() : Option("sample-rate", 'r', 1) {
    rate = 44100;
}

int RateOption::get_rate() const {
    return rate;
}

std::string RateOption::get_description() const {
    return "Sets the sample-rate of the audio";
}

void RateOption::set(char **argv, int index) {
    rate = std::atoi(argv[index+1]);
}


ForceGbOption::ForceGbOption() : Option("force-gb", 'g', 0){
    force_gb = false;
}

bool ForceGbOption::get_force_gb() const {
    return force_gb;
}

std::string ForceGbOption::get_description() const {
    return "\tForce the emulator into GameBoy mode";
}

void ForceGbOption::set(char **argv, int index) {
    force_gb = true;
}


ForceGbcOption::ForceGbcOption() : Option("force-gbc", 'c', 0){
    force_gbc = false;
}

bool ForceGbcOption::get_force_gbc() const {
    return force_gbc;
}

std::string ForceGbcOption::get_description() const {
    return "\tForce the emulator into GameBoy-Color mode";
}

void ForceGbcOption::set(char **argv, int index) {
    force_gbc = true;
}


ForceSDLOption::ForceSDLOption() : Option("force-sdl", 0, 0) {
    force_sdl = false;
}

bool ForceSDLOption::get_force_sdl() const {
    return force_sdl;
}

std::string ForceSDLOption::get_description() const {
    return "\tForce the emulator to use SDL as the Video-Driver";
}

void ForceSDLOption::set(char **argv, int index) {
    force_sdl = true;
}


VerboseOption::VerboseOption() : Option("verbose", 'v', 0) {
    verbose = false;
}

bool VerboseOption::get_verbose() const {
    return verbose;
}

std::string VerboseOption::get_description() const {
    return "\tEnable verbose debugging information";
}

void VerboseOption::set(char **argv, int index) {
    verbose = true;
}


PrinterOption::PrinterOption() : Option("printer", 'p', 0) {
    printer = false;
}

bool PrinterOption::get_printer() const {
    return printer;
}

std::string PrinterOption::get_description() const {
    return "\tEnable GameBoy printer";
}

void PrinterOption::set(char **argv, int index) {
    printer = true;
}
