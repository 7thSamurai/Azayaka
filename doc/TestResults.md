# Test Results

## Blargg's Tests

**7/8 passed**

| Test | Result |
| --- | --- |
| cgb_sound.gb | :+1: |
| cpu_instrs.gb | :+1: |
| dmg_sound.gb | :+1: |
| halt_bug.gb | :+1: |
| instr_timing.gb | :+1: |
| interrupt_time.gb | :+1: |
| mem_timing.gb | :+1: |
| oam_bug.gb | :x: |

## Mooneye GB Tests

**85/112 passed**

| Test | Result |
| --- | --- |
| acceptance/add_sp_e_timing.gb | :+1: |
| acceptance/bits/mem_oam.gb | :+1: |
| acceptance/bits/reg_f.gb | :+1: |
| acceptance/bits/unused_hwio-GS.gb | :x: |
| acceptance/boot_div-S.gb | :x: |
| acceptance/boot_div-dmg0.gb | :x: |
| acceptance/boot_div-dmgABCmgb.gb | :x: |
| acceptance/boot_div2-S.gb | :x: |
| acceptance/boot_hwio-S.gb | :x: |
| acceptance/boot_hwio-dmg0.gb | :x: |
| acceptance/boot_hwio-dmgABCmgb.gb | :x: |
| acceptance/boot_regs-dmg0.gb | :x: |
| acceptance/boot_regs-dmgABC.gb | :+1: |
| acceptance/boot_regs-mgb.gb | :x: |
| acceptance/boot_regs-sgb.gb | :x: |
| acceptance/boot_regs-sgb2.gb | :x: |
| acceptance/call_cc_timing.gb | :+1: |
| acceptance/call_cc_timing2.gb | :+1: |
| acceptance/call_timing.gb | :+1: |
| acceptance/call_timing2.gb | :+1: |
| acceptance/di_timing-GS.gb | :+1: |
| acceptance/div_timing.gb | :+1: |
| acceptance/ei_sequence.gb | :+1: |
| acceptance/ei_timing.gb | :+1: |
| acceptance/halt_ime0_ei.gb | :+1: |
| acceptance/halt_ime0_nointr_timing.gb | :+1: |
| acceptance/halt_ime1_timing.gb | :+1: |
| acceptance/halt_ime1_timing2-GS.gb | :+1: |
| acceptance/if_ie_registers.gb | :+1: |
| acceptance/instr/daa.gb | :+1: |
| acceptance/interrupts/ie_push.gb | :+1: |
| acceptance/intr_timing.gb | :+1: |
| acceptance/jp_cc_timing.gb | :+1: |
| acceptance/jp_timing.gb | :+1: |
| acceptance/ld_hl_sp_e_timing.gb | :+1: |
| acceptance/oam_dma/basic.gb | :+1: |
| acceptance/oam_dma/reg_read.gb | :+1: |
| acceptance/oam_dma/sources-GS.gb | :+1: |
| acceptance/oam_dma_restart.gb | :+1: |
| acceptance/oam_dma_start.gb | :+1: |
| acceptance/oam_dma_timing.gb | :+1: |
| acceptance/pop_timing.gb | :+1: |
| acceptance/ppu/hblank_ly_scx_timing-GS.gb | :+1: |
| acceptance/ppu/intr_1_2_timing-GS.gb | :+1: |
| acceptance/ppu/intr_2_0_timing.gb | :+1: |
| acceptance/ppu/intr_2_mode0_timing.gb | :x: |
| acceptance/ppu/intr_2_mode0_timing_sprites.gb | :x: |
| acceptance/ppu/intr_2_mode3_timing.gb | :x: |
| acceptance/ppu/intr_2_oam_ok_timing.gb | :x: |
| acceptance/ppu/lcdon_timing-GS.gb | :+1: |
| acceptance/ppu/lcdon_write_timing-GS.gb | :x: |
| acceptance/ppu/stat_irq_blocking.gb | :+1: |
| acceptance/ppu/stat_lyc_onoff.gb | :+1: |
| acceptance/ppu/vblank_stat_intr-GS.gb | :+1: |
| acceptance/push_timing.gb | :+1: |
| acceptance/rapid_di_ei.gb | :+1: |
| acceptance/ret_cc_timing.gb | :+1: |
| acceptance/ret_timing.gb | :+1: |
| acceptance/reti_intr_timing.gb | :+1: |
| acceptance/reti_timing.gb | :+1: |
| acceptance/rst_timing.gb | :+1: |
| acceptance/serial/boot_sclk_align-dmgABCmgb.gb | :x: |
| acceptance/timer/div_write.gb | :+1: |
| acceptance/timer/rapid_toggle.gb | :+1: |
| acceptance/timer/tim00.gb | :+1: |
| acceptance/timer/tim00_div_trigger.gb | :+1: |
| acceptance/timer/tim01.gb | :+1: |
| acceptance/timer/tim01_div_trigger.gb | :+1: |
| acceptance/timer/tim10.gb | :+1: |
| acceptance/timer/tim10_div_trigger.gb | :+1: |
| acceptance/timer/tim11.gb | :+1: |
| acceptance/timer/tim11_div_trigger.gb | :+1: |
| acceptance/timer/tima_reload.gb | :+1: |
| acceptance/timer/tima_write_reloading.gb | :x: |
| acceptance/timer/tma_write_reloading.gb | :x: |
| emulator-only/mbc1/bits_bank1.gb | :+1: |
| emulator-only/mbc1/bits_bank2.gb | :+1: |
| emulator-only/mbc1/bits_mode.gb | :+1: |
| emulator-only/mbc1/bits_ramg.gb | :+1: |
| emulator-only/mbc1/multicart_rom_8Mb.gb | :+1: |
| emulator-only/mbc1/ram_256kb.gb | :+1: |
| emulator-only/mbc1/ram_64kb.gb | :+1: |
| emulator-only/mbc1/rom_16Mb.gb | :+1: |
| emulator-only/mbc1/rom_1Mb.gb | :+1: |
| emulator-only/mbc1/rom_2Mb.gb | :+1: |
| emulator-only/mbc1/rom_4Mb.gb | :+1: |
| emulator-only/mbc1/rom_512kb.gb | :+1: |
| emulator-only/mbc1/rom_8Mb.gb | :+1: |
| emulator-only/mbc2/bits_ramg.gb | :+1: |
| emulator-only/mbc2/bits_romb.gb | :+1: |
| emulator-only/mbc2/bits_unused.gb | :+1: |
| emulator-only/mbc2/ram.gb | :+1: |
| emulator-only/mbc2/rom_1Mb.gb | :+1: |
| emulator-only/mbc2/rom_2Mb.gb | :+1: |
| emulator-only/mbc2/rom_512kb.gb | :+1: |
| emulator-only/mbc5/rom_16Mb.gb | :+1: |
| emulator-only/mbc5/rom_1Mb.gb | :+1: |
| emulator-only/mbc5/rom_2Mb.gb | :+1: |
| emulator-only/mbc5/rom_32Mb.gb | :+1: |
| emulator-only/mbc5/rom_4Mb.gb | :+1: |
| emulator-only/mbc5/rom_512kb.gb | :+1: |
| emulator-only/mbc5/rom_64Mb.gb | :+1: |
| emulator-only/mbc5/rom_8Mb.gb | :+1: |
| manual-only/sprite_priority.gb | :+1: |
| misc/bits/unused_hwio-C.gb | :+1: |
| misc/boot_div-A.gb | :x: |
| misc/boot_div-cgb0.gb | :x: |
| misc/boot_div-cgbABCDE.gb | :x: |
| misc/boot_hwio-C.gb | :x: |
| misc/boot_regs-A.gb | :x: |
| misc/boot_regs-cgb.gb | :x: |
| misc/ppu/vblank_stat_intr-C.gb | :x: |