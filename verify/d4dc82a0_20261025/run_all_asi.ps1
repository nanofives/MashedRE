# One lock hold for the .asi-side runs: the in-process A/B probe (original 0x004282a0 vs the shared body's
# .asi export) and run_diff's registered path1 vectors for menu_menus_ba, both against the private root.
$W = 'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-DC-82a0'
$V = "$W\verify\d4dc82a0_20261025"
Set-Location $W
"== asi_measure_probe"
& py.exe -3.12 "$V\asi_measure_probe.py" "$V\asi_measure_probe.json" 'D:\MashedWT\D4-DC-82a0_data\after\mashed_re_dev.asi'
"== run_diff menu_menus_ba"
& py.exe -3.12 "$V\run_diff_private.py" menu_menus_ba
Copy-Item "$W\log\diff_menu_menus_ba.csv" "$V\diff_menu_menus_ba.csv"
"== run_diff menu_menus_ba --self-baseline"
& py.exe -3.12 "$V\run_diff_private.py" menu_menus_ba --self-baseline
Copy-Item "$W\log\diff_menu_menus_ba_selfbaseline.csv" "$V\diff_menu_menus_ba_selfbaseline.csv"
