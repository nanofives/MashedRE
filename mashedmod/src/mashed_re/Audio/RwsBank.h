// RwsBank — loader for Mashed's RenderWare RWS audio banks (2026-06-15).
//
// Format cracked + verified on toastaudio/pc/audio/pcdics/arctic.rws (see
// re/tools/rws_extract_wav.py + re/analysis/audio_rws_loader/):
//   0x809 wrapper
//     0x80A bank header (trailing ASCII "<bank>\0<name>.WAV")
//     0x80C data: [u32 waveCount] then waveCount x:
//       0x802 wave wrapper
//         0x803 format (176B): +4 u32 sampleRate, +12 u32 pcmBytes,
//                              +112 ASCIIZ wave name
//         0x804 PCM sample data (16-bit MONO; "stereo.WAV" is the bank's
//               source name, not the per-wave channel count — confirmed by
//               consecutive-sample roughness analysis).
#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace mashed_re {
namespace Audio {

struct RwsWave {
    std::string                name;
    std::uint32_t              rate     = 22050;
    int                        channels = 1;     // 16-bit mono
    int                        bits     = 16;
    std::vector<std::uint8_t>  pcm;
    bool valid() const { return !pcm.empty(); }
};

// Parse every wave in the RWS bank at `path`. Returns false on read/format
// error (out is then empty). Appends a one-line report to `log_path` if given.
bool RwsBankLoad(const char* path, std::vector<RwsWave>& out,
                 const char* log_path = nullptr);

// Find a wave by case-insensitive substring of its name (e.g. "windy").
// Returns nullptr if none matches.
const RwsWave* RwsBankFind(const std::vector<RwsWave>& waves, const char* nameSub);

// Decode a 0x80d STREAMED RWS bank (cdaudio.rws / per-car english/<car>.rws):
// the audio payload is continuous IMA ADPCM (cracked 2026-06-16 — autocorr 0.96
// on cdaudio; rate 44100 from the 0x80e header). Finds the largest leaf chunk
// (the ADPCM stream), decodes to 16-bit mono PCM. `maxSamples` caps the decode
// (0 = all). Returns false on read/parse error.
bool RwsStreamDecode(const char* path, std::vector<std::int16_t>& pcmOut,
                     std::uint32_t& rateOut, std::size_t maxSamples = 0,
                     const char* log_path = nullptr);

}  // namespace Audio
}  // namespace mashed_re
