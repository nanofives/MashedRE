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

}  // namespace Audio
}  // namespace mashed_re
