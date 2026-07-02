<#
sysaudio.ps1 — control the Windows DEFAULT playback device mute state via Core Audio.

Why this exists: the ORIGINAL MASHED.exe has no mute env (MASHED_MUTE only gates the
standalone port). When capturing the original we must silence Windows itself. The
volume-mute *toggle* key is stateful/unreliable, so this drives the Core Audio
IAudioEndpointVolume::SetMute / GetMute directly (deterministic set, not toggle).

Usage:
  pwsh scripts/sysaudio.ps1 status     # prints "muted" or "unmuted" (exit 0); machine-readable
  pwsh scripts/sysaudio.ps1 mute       # force mute   (idempotent)
  pwsh scripts/sysaudio.ps1 unmute     # force unmute (idempotent)

Output: the LAST line printed is exactly "muted" or "unmuted" (the resulting state),
so callers can parse it. Other diagnostic text goes before it.
Exit code: 0 on success, 1 on failure.
#>
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('status', 'mute', 'unmute')]
    [string]$Action
)

$ErrorActionPreference = 'Stop'

# --- Core Audio COM interop (IMMDeviceEnumerator -> default endpoint -> IAudioEndpointVolume) ---
$source = @'
using System;
using System.Runtime.InteropServices;

namespace MashedSysAudio {

    [Guid("A95664D2-9614-4F35-A746-DE8DB63617E6"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IMMDeviceEnumerator {
        int NotImpl1();
        int GetDefaultAudioEndpoint(int dataFlow, int role, out IMMDevice ppDevice);
        // remaining methods unused
    }

    [Guid("D666063F-1587-4E43-81F1-B948E807363F"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IMMDevice {
        int Activate(ref Guid iid, int dwClsCtx, IntPtr pActivationParams,
                     [MarshalAs(UnmanagedType.IUnknown)] out object ppInterface);
        // remaining methods unused
    }

    [Guid("5CDF2C82-841E-4546-9722-0CF74078229A"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IAudioEndpointVolume {
        int RegisterControlChangeNotify(IntPtr pNotify);
        int UnregisterControlChangeNotify(IntPtr pNotify);
        int GetChannelCount(out uint pnChannelCount);
        int SetMasterVolumeLevel(float fLevelDB, ref Guid pguidEventContext);
        int SetMasterVolumeLevelScalar(float fLevel, ref Guid pguidEventContext);
        int GetMasterVolumeLevel(out float pfLevelDB);
        int GetMasterVolumeLevelScalar(out float pfLevel);
        int SetChannelVolumeLevel(uint nChannel, float fLevelDB, ref Guid pguidEventContext);
        int SetChannelVolumeLevelScalar(uint nChannel, float fLevel, ref Guid pguidEventContext);
        int GetChannelVolumeLevel(uint nChannel, out float pfLevelDB);
        int GetChannelVolumeLevelScalar(uint nChannel, out float pfLevel);
        int SetMute([MarshalAs(UnmanagedType.Bool)] bool bMute, ref Guid pguidEventContext);
        int GetMute([MarshalAs(UnmanagedType.Bool)] out bool pbMute);
        // remaining methods unused
    }

    [ComImport, Guid("BCDE0395-E52F-467C-8E3D-C4579291692E")]
    class MMDeviceEnumeratorComObject { }

    public static class Volume {
        // eRender = 0 (playback), eConsole = 0
        const int eRender = 0;
        const int eConsole = 0;

        static IAudioEndpointVolume GetEndpointVolume() {
            var enumerator = (IMMDeviceEnumerator)(new MMDeviceEnumeratorComObject());
            IMMDevice device;
            int hr = enumerator.GetDefaultAudioEndpoint(eRender, eConsole, out device);
            if (hr != 0 || device == null)
                throw new Exception("GetDefaultAudioEndpoint failed hr=0x" + hr.ToString("X8") + " (no default playback device?)");
            Guid iid = typeof(IAudioEndpointVolume).GUID;
            object o;
            hr = device.Activate(ref iid, 1 /*CLSCTX_INPROC_SERVER*/, IntPtr.Zero, out o);
            if (hr != 0 || o == null)
                throw new Exception("Activate(IAudioEndpointVolume) failed hr=0x" + hr.ToString("X8"));
            return (IAudioEndpointVolume)o;
        }

        public static bool GetMute() {
            var v = GetEndpointVolume();
            bool m;
            int hr = v.GetMute(out m);
            if (hr != 0) throw new Exception("GetMute failed hr=0x" + hr.ToString("X8"));
            return m;
        }

        public static void SetMute(bool mute) {
            var v = GetEndpointVolume();
            Guid ctx = Guid.Empty;
            int hr = v.SetMute(mute, ref ctx);
            if (hr != 0) throw new Exception("SetMute failed hr=0x" + hr.ToString("X8"));
        }
    }
}
'@

try {
    Add-Type -TypeDefinition $source -ErrorAction Stop
} catch {
    # Add-Type can throw if the type is already loaded in this session; ignore that case.
    if ($_.Exception.Message -notmatch 'already') {
        Write-Error "Failed to compile Core Audio interop: $($_.Exception.Message)"
        exit 1
    }
}

function Print-State([bool]$m) {
    if ($m) { 'muted' } else { 'unmuted' }
}

try {
    switch ($Action) {
        'status' {
            $m = [MashedSysAudio.Volume]::GetMute()
            Print-State $m
            exit 0
        }
        'mute' {
            [MashedSysAudio.Volume]::SetMute($true)
            $m = [MashedSysAudio.Volume]::GetMute()
            if (-not $m) { Write-Error "SetMute(true) did not take effect"; exit 1 }
            Print-State $m
            exit 0
        }
        'unmute' {
            [MashedSysAudio.Volume]::SetMute($false)
            $m = [MashedSysAudio.Volume]::GetMute()
            if ($m) { Write-Error "SetMute(false) did not take effect"; exit 1 }
            Print-State $m
            exit 0
        }
    }
} catch {
    Write-Error "sysaudio $Action failed: $($_.Exception.Message)"
    exit 1
}
