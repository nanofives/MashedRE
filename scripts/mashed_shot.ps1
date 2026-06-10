# mashed_shot.ps1 — launch original MASHED.exe, foreground it (Alt-unlock trick),
# and screenshot at a few timepoints into verify/. For diagnosing the boot state
# (loading screen vs menu vs game) without terminal occlusion.
param([int]$MaxSeconds = 20)
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class M {
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a,int x,int y,int cx,int cy,uint f);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk,byte s,uint f,IntPtr e);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left,Top,Right,Bottom; }
}
"@
Add-Type -AssemblyName System.Drawing
$root="C:\Users\maria\Desktop\Proyectos\Mashed"
$exe=Join-Path $root "original\MASHED.exe"; $wd=Join-Path $root "original"
$p=Start-Process -FilePath $exe -WorkingDirectory $wd -PassThru
Write-Host "launched pid=$($p.Id)"
function Foreground($h){ for($i=0;$i -lt 6;$i++){ [M]::keybd_event(0x12,0,0,[IntPtr]::Zero);[M]::keybd_event(0x12,0,2,[IntPtr]::Zero);[void][M]::SetForegroundWindow($h);Start-Sleep -Milliseconds 120; if([M]::GetForegroundWindow() -eq $h){return $true} } return $false }
function Shot($h,$name){
  $r=New-Object M+RECT; if(-not [M]::GetWindowRect($h,[ref]$r)){return "norect"}
  $w=$r.Right-$r.Left;$ht=$r.Bottom-$r.Top; if($w-le 0){return "0x0"}
  $b=New-Object System.Drawing.Bitmap $w,$ht;$g=[System.Drawing.Graphics]::FromImage($b)
  $g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size $w,$ht))
  $b.Save((Join-Path $root "verify\$name"),[System.Drawing.Imaging.ImageFormat]::Png);$g.Dispose();$b.Dispose()
  return "saved $name (${w}x${ht})"
}
foreach($t in 8,14,20){
  while(((Get-Date)-(Get-Date $p.StartTime)).TotalSeconds -lt $t){ Start-Sleep -Milliseconds 200; try{$p.Refresh()}catch{}; if($p.HasExited){break} }
  try{$p.Refresh()}catch{}
  if($p.HasExited){ Write-Host "EXITED at ~${t}s code=$($p.ExitCode) (0x$('{0:X8}' -f $p.ExitCode))"; break }
  $h=$p.MainWindowHandle
  [void][M]::SetWindowPos($h,[IntPtr](-1),0,0,0,0,(0x1 -bor 0x40))
  $fg=Foreground $h
  Start-Sleep -Milliseconds 400
  Write-Host ("t=$t fg=$fg " + (Shot $h ("mashed_t{0:D2}.png" -f $t)))
}
try{$p.Refresh(); if(-not $p.HasExited){ Write-Host "still alive; killing"; Stop-Process -Id $p.Id -Force } else { Write-Host "exit code=$($p.ExitCode)" } }catch{}
