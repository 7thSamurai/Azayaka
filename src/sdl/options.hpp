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

#ifndef _OPTIONS_HPP_
#define _OPTIONS_HPP_

#include "common/parser.hpp"

class DebugOption : public Option
{
public:
    DebugOption();

    bool get_debug() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool debug;
};

class DumpUsageOption : public Option
{
public:
    DumpUsageOption();

    bool get_dump_usage() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool dump_usage;
};

class ScaleOption : public Option
{
public:
    ScaleOption();

    float get_scale() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    float scale;
};

class RateOption : public Option
{
public:
    RateOption();

    int get_rate() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    int rate;
};

class ForceGbOption : public Option
{
public:
    ForceGbOption();

    bool get_force_gb() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool force_gb;
};

class ForceGbcOption : public Option
{
public:
    ForceGbcOption();

    bool get_force_gbc() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool force_gbc;
};

// Force SDL as the Video-Driver
class ForceSDLOption : public Option
{
public:
    ForceSDLOption();

    bool get_force_sdl() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool force_sdl;
};

// Enable verbose debugging information
class VerboseOption : public Option
{
public:
    VerboseOption();

    bool get_verbose() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool verbose;
};

// Enable GameBoy Printer
class PrinterOption : public Option
{
public:
    PrinterOption();

    bool get_printer() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool printer;
};

// Enable GameBoy Link-Cable
class LinkOption : public Option
{
public:
    LinkOption();

    bool get_link() const;

    std::string get_description() const;
    void set(char **argv, int index);

private:
    bool link;
};

#endif // _OPTIONS_HPP_
