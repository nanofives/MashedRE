"""
Decompile track_collision_geometry session targets from Mashed_pool13.
Targets:
  - FUN_00547bf0 (AABB vs triangle SAT pre-test, S-1964)
  - FUN_00547450 (sphere vs triangle pre-test, S-1965)
  - Lua C BSP setter region 0x0047a1b0..0x0047ab28
"""
import sys
import os

PROJECT_DIR = r"C:\Users\maria\Desktop\Proyectos\Mashed\mashed_pool"
PROJECT_NAME = "Mashed_pool13"
PROGRAM_NAME = "MASHED.exe"
GHIDRA_DIR = r"C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra_12.0.3_PUBLIC"

# Priority RVAs to decompile (VA = image_base 0x00400000 + RVA)
TARGET_ADDRS = [
    0x00547bf0,  # S-1964: AABB vs triangle SAT pre-test
    0x00547450,  # S-1965: sphere vs triangle pre-test
    # Lua C BSP setters — full range scan 0x0047a1b0..0x0047ab28
    # We'll enumerate all functions in this range below
]

BSP_SETTER_RANGE = (0x0047a1b0, 0x0047ab30)

import pyghidra
from pathlib import Path

def main():
    pyghidra.start(install_dir=Path(GHIDRA_DIR))
    with pyghidra.open_program(
        None,
        project_location=PROJECT_DIR,
        project_name=PROJECT_NAME,
        program_name=PROGRAM_NAME,
        analyze=False,
        nested_project_location=False,
    ) as flat_api:
        from ghidra.app.decompiler import DecompInterface
        from ghidra.util.task import ConsoleTaskMonitor

        program = flat_api.getCurrentProgram()
        listing = program.getListing()
        monitor = ConsoleTaskMonitor()

        decomp = DecompInterface()
        decomp.openProgram(program)

        def decompile(addr_int):
            from ghidra.program.model.address import AddressSpace
            addr = program.getAddressFactory().getDefaultAddressSpace().getAddress(addr_int)
            func = listing.getFunctionAt(addr)
            if func is None:
                func = listing.getFunctionContaining(addr)
            if func is None:
                return None, None, "NO_FUNCTION"
            res = decomp.decompileFunction(func, 60, monitor)
            if res is None or not res.decompileCompleted():
                return func.getName(), func.getBody().getNumAddresses(), "DECOMP_FAILED"
            code = res.getDecompiledFunction().getC()
            size = func.getBody().getNumAddresses()
            return func.getName(), size, code

        # Enumerate functions in the BSP setter range
        bsp_addrs = []
        addr_lo = program.getAddressFactory().getDefaultAddressSpace().getAddress(BSP_SETTER_RANGE[0])
        addr_hi = program.getAddressFactory().getDefaultAddressSpace().getAddress(BSP_SETTER_RANGE[1])
        funcs_iter = listing.getFunctions(addr_lo, True)
        while funcs_iter.hasNext():
            f = funcs_iter.next()
            if f.getEntryPoint().getOffset() >= BSP_SETTER_RANGE[1]:
                break
            bsp_addrs.append(f.getEntryPoint().getOffset())

        all_targets = TARGET_ADDRS + bsp_addrs

        for addr in all_targets:
            print(f"\n{'='*72}")
            print(f"0x{addr:08x}")
            print(f"{'='*72}")
            name, size, code = decompile(addr)
            if name:
                print(f"Name: {name}  Size: {size} bytes")
            print(code)

        decomp.dispose()

if __name__ == "__main__":
    main()
