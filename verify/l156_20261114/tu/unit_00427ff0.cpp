// TOOLCHAIN vc19
// LANG c++
// FLAGS: /O2 /EHa /DMASHED_STANDALONE
// MATCH: 0x00427ff0 _FUN_00427ff0
#define MATCH_PORT 1
#define FUN_00427680 Port_00427680
#define FUN_004277a0 Port_004277a0
namespace U {
extern "C" {
#include "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/re/match/s06/00427ff0.c"
}
}
namespace H0 {
using namespace U;
extern "C" {
#include "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/re/match/s06/00427680.c"
}
}
namespace H1 {
using namespace U;
extern "C" {
#include "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/re/match/s06/004277a0.c"
}
}
