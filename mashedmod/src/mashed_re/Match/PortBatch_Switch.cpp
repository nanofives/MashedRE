// PORT-BATCH-TOOL (D4, 2026-11-09): the runtime switch of the closed-set batch swaps. EXE ONLY.
//
// A batch is one closed set of C4-by-match rows (re/tools/port_closed_sets.py) compiled into mashed_re.exe by the
// Match/PortBatch_<SUB>_<rva>.cpp wrappers that re/tools/port_batch_gen.py writes. Each set is entered at its wiring
// points (the exe forwarders / stand-ins that a reached matched body already calls), and each wiring point asks
// PortBatch_On(<SUB>, <head rva>) whether to run the matched body or keep its old binding.
//
//   MASHED_BATCH_<SUB>=1             switch the whole set on (default OFF: the old bindings run, nothing changes)
//   MASHED_BATCH_<SUB>_ROWS=<list>   with the switch on, only these heads (hex RVAs, comma or space separated).
//                                    The bisect driver (re/tools/port_batch_bisect.py) narrows a moved gate with it.
//
// The first call of each head that runs its matched body writes one line to mashed_re.log, so a gate run shows
// which heads executed. Evidence: verify/portbatch_20261109/RESULT_PORT-BATCH-TOOL.md.
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

struct Sub {
    char name[16];
    int on;                 // MASHED_BATCH_<SUB> set and not "0"
    int all;                // no _ROWS list: every head
    unsigned rows[64];
    int nrows;
    unsigned logged[64];
    int nlogged;
};
Sub s_subs[8];
int s_nsubs;

Sub* SubFor(const char* sub) {
    for (int i = 0; i < s_nsubs; ++i)
        if (std::strcmp(s_subs[i].name, sub) == 0) return &s_subs[i];
    if (s_nsubs == 8) return nullptr;
    Sub* s = &s_subs[s_nsubs++];
    std::memset(s, 0, sizeof(*s));
    std::strncpy(s->name, sub, sizeof(s->name) - 1);
    char key[64];
    std::snprintf(key, sizeof(key), "MASHED_BATCH_%s", sub);
    const char* v = std::getenv(key);
    s->on = (v && v[0] && v[0] != '0');
    std::snprintf(key, sizeof(key), "MASHED_BATCH_%s_ROWS", sub);
    const char* r = std::getenv(key);
    s->all = 1;
    if (r && r[0]) {
        s->all = 0;
        const char* p = r;
        while (*p && s->nrows < 64) {
            while (*p == ',' || *p == ' ' || *p == ';') ++p;
            if (!*p) break;
            char* end = nullptr;
            unsigned long a = std::strtoul(p, &end, 16);
            if (end == p) break;
            s->rows[s->nrows++] = static_cast<unsigned>(a);
            p = end;
        }
    }
    if (s->on) {   // the default (switch off) run writes nothing
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[PORT-BATCH] %s switch ON%s%s\n", sub, s->all ? "" : ", rows ", s->all ? "" : r);
            std::fclose(f);
        }
    }
    return s;
}

}  // namespace

extern "C" int __cdecl PortBatch_On(const char* sub, unsigned rva) {
    Sub* s = SubFor(sub);
    if (!s || !s->on) return 0;
    int hit = s->all;
    for (int i = 0; !hit && i < s->nrows; ++i) hit = (s->rows[i] == rva);
    if (!hit) return 0;
    for (int i = 0; i < s->nlogged; ++i)
        if (s->logged[i] == rva) return 1;
    if (s->nlogged < 64) s->logged[s->nlogged++] = rva;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[PORT-BATCH] %s head 0x%08x: matched body runs (first call)\n", sub, rva);
        std::fclose(f);
    }
    return 1;
}

// The wiring points call this with the matched body's address. Taking the address (a relocated dword in the
// forwarder) is what makes the body an edge of the exe call graph scripts/backfill_port.py walks: a forwarder that
// called the body directly compiled to a conditional-jump tail call (Jcc rel32), which that graph does not follow.
extern "C" void __cdecl PortBatch_Run(const char* sub, unsigned rva, void (__cdecl* fn)(void)) {
    if (PortBatch_On(sub, rva)) fn();
}
#endif  // MASHED_STANDALONE
