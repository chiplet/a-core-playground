import numpy as np
import argparse
import os
from elftools.elf.elffile import ELFFile

def generate_progmem_init_files(args):
    """
    Extract compiled program data and map it to files that can be used to
    init memory banks using verilog $readmemh directly in the testbench to
    skip programming over JTAG.
    Assuming program memory is divided into four banks such that two LSB
    address bits are used for bank addressing.
    """
    pm_size = 2**args.progmem_depth
    bank_size = int(pm_size / 4)
    banks = [np.zeros(bank_size, dtype=np.uint8) for _ in range(4)]
    # read progmem data to corresponding banks
    with open(args.elffile, 'rb') as fd:
        pm_bytes = b''
        elffile = ELFFile(fd)
        sections = ['.text', '.data', '.sdata', '.rodata']
        for sect_name in sections:
            section = elffile.get_section_by_name(sect_name)
            if section is not None:
                pm_bytes += section.data()
        # split to banks
        for (addr, b) in enumerate(pm_bytes):
            bank_idx = addr & 0b11
            bank_addr = addr >> 2
            banks[bank_idx][bank_addr] = b
    def write_array_as_hex(filepath, arr):
        assert arr.dtype==np.uint8, "arr must have type np.uint8"
        with open(filepath, 'w') as fd:
            for byte in arr:
                fd.write(f"{byte:02x}\n")
    for (i,bank) in enumerate(banks):
        write_array_as_hex(os.path.join(args.output_dir, f"pm_bank{i}.hex"), bank)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_file", required=True, type=str, help="Path to program ELF file.")
    parser.add_argument("--output_dir", required=True, type=str, help="Directory path where output files will be generated.")
    parser.add_argument("progmem_depth", required=True, type=int, help="Program memory size in bytes expressed as 2**progmem_depth.")
    args = parser.parse_args()
    return args


def main():
    args = parse_args()


if __name__ == "__main__":
    main()
