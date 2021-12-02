#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2021 Antmicro
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

LX_DEPENDENCIES = ["riscv", "icestorm", "yosys", "nextpnr-ice40"]

# Import lxbuildenv to integrate the deps/ directory
import lxbuildenv

import os, os.path, sys
import argparse

from migen import *
from migen.genlib.resetsync import AsyncResetSynchronizer

from litex.soc.integration.builder import *
from litex.soc.integration.soc import SoCRegion
from litex_boards.targets.kosagi_fomu import BaseSoC

from valentyusb.usbcore import io as usbio
from valentyusb.usbcore.cpu import eptri

from rtl.fomurgb import FomuRGB
from rtl.fomutouch import FomuTouch

from dfu import *
from autoboot import *

kB = 1024


def main():
    ## Read and parse arguments
    parser = argparse.ArgumentParser(
        description="Fomu Keystroke Injector - gateware/BIOS builder"
    )
    parser.add_argument(
        "--seed", default=1, help="Seed to use in nextpnr"
    )
    parser.add_argument(
        "--timeout", default=1, help="Timeout until Foboot switches to injector gateware"
    )
    builder_args(parser)
    args = parser.parse_args()

    ## Image layout
    binaries = {
        # name,       path,                                         size
        "bitstream": ["{build_dir}/gateware/{build_name}.bin",      0x20000], # 0x0
        "bios":      ["{build_dir}/software/bios/bios.bin",         0x08000], # 0x20000
        "firmware":  ["{build_dir}/software/firmware/firmware.fbi", 0x10000], # 0x28000
    }

    ## Flash layout:
    flash_offset_bitstream = 0x40000
    flash_offset_bios = flash_offset_bitstream + binaries["bitstream"][1]
    flash_offset_firmware = flash_offset_bios + binaries["bios"][1]
    flash_offset_script = flash_offset_firmware + binaries["firmware"][1]

    ## Create SoC
    soc = BaseSoC(bios_flash_offset=flash_offset_bios,
                  cpu_type="vexriscv", cpu_variant="minimal",
                  with_uart=False, with_led_chaser=False, with_spi_master=True
                  )

    # Add LED driver block
    rgb_pins = soc.platform.request("rgb_led")
    soc.submodules.rgb = FomuRGB(rgb_pins)
    soc.add_csr("rgb")

    # Add touch buttons
    touch_pins = [soc.platform.request("user_touch_n", i) for i in range (0, 4)]
    soc.submodules.touch = FomuTouch(touch_pins)
    soc.add_csr("touch")

    # Add USB device controller
    usb_pads = soc.platform.request('usb')
    usb_iobuf = usbio.IoBuf(usb_pads.d_p, usb_pads.d_n, usb_pads.pullup)
    soc.submodules.usb = eptri.TriEndpointInterface(usb_iobuf)
    soc.add_interrupt("usb")

    # BIOS/software constants
    soc.add_constant("CONFIG_SIM_DISABLE_BIOS_PROMPT")
    soc.add_constant("SPIFLASH_SKIP_FREQ_INIT")
    soc.add_constant("TERM_MINI")
    soc.add_constant("TERM_NO_HIST")
    # Application must be running from RAM to be able to write data to SPI flash
    soc.add_constant("MAIN_RAM_ADDRESS", soc.mem_map["main_ram"])
    soc.add_constant("PART_GFS", flash_offset_bitstream)
    soc.add_constant("FLASH_BOOT_ADDRESS", soc.mem_map["spiflash"] + flash_offset_firmware)
    soc.add_constant("PART_SCRIPT", flash_offset_script)

    # Build final SoC
    builder = Builder(soc, **builder_argdict(args))
    if not args.no_compile_software:
        builder.add_software_package("firmware", os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "sw")))
    builder.build(seed=args.seed)

    if not args.no_compile_gateware:
        # Set autoboot parameter
        if autoboot_timeout(os.path.join(builder.output_dir, "gateware", f"{soc.platform.name}.bin"), args.timeout):
            print("Autoboot timeout set to {} sec".format(args.timeout))
        else:
            print("Couldn't set autoboot timeout")

    # Merge gateware and firmware into single binary
    merge_image(binaries, builder.output_dir, soc.platform.name)

    # Add DFU suffix to the final binary
    vid = '5bf0'
    pid = '1209'
    add_dfu_suffix(os.path.join(builder.output_dir, f"{soc.platform.name}.bin"), vid, pid)

if __name__ == "__main__":
    main()
