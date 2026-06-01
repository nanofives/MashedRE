# b19_navcheck.ps1 — launch the standalone, foreground it, send N Down-arrow
# presses (so the menu selection moves), then screenshot. Proves B19c menu nav.
param([int]$Downs = 2)

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Nav {
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x,int y,int cx,int cy,uint f);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int n);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, IntPtr extra);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left,Top,Right,Bottom; }
}
"@
Add-Type -AssemblyName System.Drawing
$root="C:\Users\maria\Desktop\Proyectos\Mashed"
$tmp=Join-Path $env:TEMP ("b19nav_"+[guid]::NewGuid().ToString("N").Substring(0,8))
New-Item -ItemType Directory -Force $tmp|Out-Null
Copy-Item (Join-Path $root "mashedmod\build\mashed_re.exe") (Join-Path $tmp "mashed_re.exe") -Force
$p=Start-Process -FilePath (Join-Path $tmp "mashed_re.exe") -WorkingDirectory $root -PassThru
Start-Sleep -Seconds 6
$h=$p.MainWindowHandle
[void][Nav]::SetWindowPos($h,[IntPtr](-1),0,0,0,0,(0x1 -bor 0x40))
[void][Nav]::ShowWindow($h,5) # SW_SHOW
# Foreground-lock unlock trick: tap Alt, then SetForegroundWindow, retry until it sticks.
for ($t=0;$t -lt 6;$t++){
  [Nav]::keybd_event(0x12,0,0,[IntPtr]::Zero); [Nav]::keybd_event(0x12,0,2,[IntPtr]::Zero)
  [void][Nav]::SetForegroundWindow($h)
  Start-Sleep -Milliseconds 150
  if ([Nav]::GetForegroundWindow() -eq $h) { break }
}
Write-Host ("foreground match: {0}" -f ([Nav]::GetForegroundWindow() -eq $h))
Start-Sleep -Milliseconds 400
# VK_DOWN = 0x28; KEYDOWN=0, KEYUP=2
for ($i=0;$i -lt $Downs;$i++){
  [Nav]::keybd_event(0x28,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 120
  [Nav]::keybd_event(0x28,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 250
}
Start-Sleep -Milliseconds 500
$r=New-Object Nav+RECT; [void][Nav]::GetWindowRect($h,[ref]$r)
$w=$r.Right-$r.Left;$hh=$r.Bottom-$r.Top
$bmp=New-Object System.Drawing.Bitmap $w,$hh
$g=[System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size $w,$hh))
$out=Join-Path $root "verify\b19c_nav.png"
$bmp.Save($out,[System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose();$bmp.Dispose()
Write-Host "saved $out (${w}x${hh}); sent $Downs Down presses"
try{Stop-Process -Id $p.Id -Force}catch{}
Remove-Item $tmp -Recurse -Force -ErrorAction SilentlyContinue
