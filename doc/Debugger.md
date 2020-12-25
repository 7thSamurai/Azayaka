# Debugger

Azayaka features a powerful console based debugger.

To use it, you must start Azayaka with **--debug** to have debug-mode enabled.

To enter debug-mode and enter a command you can press <kbd>Ctrl+C</kbd> to interrupt the emulator.

## Parameters

### Constants

- A decimal number is written as is with no special prefix.
- A hexadecimal number is prefixed with **$** or **0x**.
- A binary number is prefixed with **0b**.

### Registers

- 8-bit registers: **a**, **f**, **b**, **c**, **d**, **e**, **h** and **l**.
- 16-bit registers: **af**, **bc**, **de**, **hl**, **pc** and **sp**.

### Memory Address

A memory address can either be a constant value, or it can be retrieved from a register.

To actually get a value from the memory, the address must be enclosed in brackets("**[]**") or parenthesis("**()**").

## Terminology

### Breakpoint

A *breakpoint* is an address of a location in the program, that when reached, will alert the user and temporally halt execution of the program.

### Watchpoint

A *watchpoint* is an address of a location in the memory, that when changed, will alert the user and temporally halt execution of the program.

## Commands

Each command has a long form, and an optional alias.

### Summary

| Command | Alias | Description |
| --- | --- | --- |
| break | b | Add a breakpoint |
| cartridge | cart | Prints information about the cartridge |
| continue | c | Run until a breakpoint or watchpoint is triggered |
| delete | d | Delete a breakpoint |
| disassemble | dis | Disassemble an instruction |
| help | h | Show a help message |
| lcd | | Prints information about the LCD |
| list | l | List breakpoints and watchpoints |
| print | p | Print a value |
| quit | q | Quit the debugger |
| registers | reg | Print the values of the registers |
| step | s | Run the next instruction |
| unwatch | u | Remove a watchpoint |
| watch | w | Adds a watchpoint |

### break (b)

Adds a breakpoint by it's address.

```
Usage: break [Address]
```

### cartridge (cart)

Prints information about the cartridge.

```
Usage: cartridge
```

### continue (c)

Run until a breakpoint or watchpoint is triggered.

```
Usage: continue
```

### delete (d)

Removes a breakpoint by it's address, or removes all of the breakpoints.

```
Usage: delete [Address]
```

### disassemble (dis)

Disassembles an instruction by it's address.

```
Usage: disassemble [Address]
```

### help (h)

Shows a help message for a specific command or every command.

```
Usage: help [Command]
```

### lcd

Prints information about the LCD.

```
Usage: lcd
```

### list (l)

Lists all of the breakpoints and watchpoints.

```
Usage: list
```

### print (p)

Prints the value of a register or a value in the memory.

```
Usage: print [Register name or Address]
```

### quit (q)

Quits the debugger.

```
Usage: quit
```

### registers (reg)

Prints the values of the registers.

```
Usage: registers
```

### step (s)

Runs the next instruction.

```
Usage: step
```

### unwatch (u)

Removes a watchpoint by it's address, or removes all of the watchpoints.

```
Usage: unwatch [Address]
```

### watch (w)

Adds a watchpoint by it's address.

```
Usage: watch [Address]
```