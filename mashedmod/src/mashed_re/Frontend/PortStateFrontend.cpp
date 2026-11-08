// Mashed RE — frontend port-state (PORT-STATE-01, 2026-11-08). Exe only. See PortStateFrontend.h.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
#include "PortStateFrontend.h"

#include <cstdio>
#include <cstdlib>

namespace mashed_re {
namespace Frontend {
namespace {

std::int32_t& Word() { return *reinterpret_cast<std::int32_t*>(kGameModeWord); }

struct SiteStat {
    long         calls;
    long         bridged;      // calls where the bridge had to change the word
    std::int32_t last_value;
    std::int32_t last_ret;
    bool         any;
};
SiteStat s_stat[kSiteCount] = {};
const char* const kSiteName[kSiteCount] = { "00430830.rowavail", "00430830.arrows", "004309b0.gate" };

} // namespace

bool PortStateFrontend_On() {
    static const bool s_on = (std::getenv("MASHED_PORTSTATE_FRONTEND") != nullptr);
    return s_on;
}

ModeWordScope::ModeWordScope(ModeWordSite site, std::int32_t value)
    : m_site(site), m_saved(Word()), m_changed(Word() != value) {
    if (m_changed) Word() = value;
}

ModeWordScope::~ModeWordScope() {
    if (m_changed) Word() = m_saved;
}

void ModeWordScope::Result(std::int32_t ret) const {
    if (!PortStateFrontend_On()) return;
    SiteStat& s = s_stat[m_site];
    ++s.calls;
    if (m_changed) ++s.bridged;
    const std::int32_t value = Word();   // still inside the scope: the value the matched body read
    // One line per change of (value, ret) at a site, plus every 600th call, so a menu walk stays readable.
    if (!s.any || value != s.last_value || ret != s.last_ret || (s.calls % 600) == 0) {
        if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
            std::fprintf(fp, "[portstate-frontend] site=%s word@0x%08x=%d ret=%d calls=%ld bridged=%ld\n",
                         kSiteName[m_site], static_cast<unsigned>(kGameModeWord), value, ret, s.calls, s.bridged);
            std::fclose(fp);
        }
        s.any = true;
        s.last_value = value;
        s.last_ret = ret;
    }
}

} // namespace Frontend
} // namespace mashed_re
