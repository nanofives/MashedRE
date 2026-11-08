// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00521110: zlib 1.1.3 deflate.c deflateInit2_, with deflateReset (and its lm_init) inlined,
   written as a static __forceinline copy above the function; deflateEnd is called.
   83.9% (regs 84.7, ops 94.1). Residual: block layout. The original places the return epilogue of
   'if (strm == 0) return -2' and 'if (s == 0) return -4' inline right after the test (the parameter-range
   checks jump back to that -2 block), ours sends them to shared epilogues at the end. Same pattern as
   inflateInit2_ 0x00522b50. Plus one register name in the inlined CLEAR_HASH (edx vs ecx). Class (b). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef unsigned char Byte;
typedef unsigned int uInt;
typedef unsigned long uLong;
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void *voidpf;
typedef void *voidp;
typedef voidpf (*alloc_func) (voidpf opaque, uInt items, uInt size);
typedef void (*free_func) (voidpf opaque, voidpf address);
typedef struct z_stream_s { Bytef *next_in; uInt avail_in; uLong total_in; Bytef *next_out; uInt avail_out; uLong total_out; char *msg; struct internal_state *state; alloc_func zalloc; free_func zfree; voidpf opaque; int data_type; uLong adler; uLong reserved; } z_stream;
typedef z_stream *z_streamp;
typedef unsigned char uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long ulg;
typedef uLong ( *check_func) (uLong check, const Bytef *buf, uInt len);
typedef struct ct_data_s { union { ush freq; ush code; } fc; union { ush dad; ush len; } dl; } ct_data;
typedef struct static_tree_desc_s static_tree_desc;
typedef struct tree_desc_s { ct_data *dyn_tree; int max_code; static_tree_desc *stat_desc; } tree_desc;
typedef ush Pos;
typedef Pos Posf;
typedef unsigned IPos;
typedef struct internal_state { z_streamp strm; int status; Bytef *pending_buf; ulg pending_buf_size; Bytef *pending_out; int pending; int noheader; Byte data_type; Byte method; int last_flush; uInt w_size; uInt w_bits; uInt w_mask; Bytef *window; ulg window_size; Posf *prev; Posf *head; uInt ins_h; uInt hash_size; uInt hash_bits; uInt hash_mask; uInt hash_shift; long block_start; uInt match_length; IPos prev_match; int match_available; uInt strstart; uInt match_start; uInt lookahead; uInt prev_length; uInt max_chain_length; uInt max_lazy_match; int level; int strategy; uInt good_match; int nice_match; struct ct_data_s dyn_ltree[(2*(256+1+29)+1)]; struct ct_data_s dyn_dtree[2*30+1]; struct ct_data_s bl_tree[2*19+1]; struct tree_desc_s l_desc; struct tree_desc_s d_desc; struct tree_desc_s bl_desc; ush bl_count[15+1]; int heap[2*(256+1+29)+1]; int heap_len; int heap_max; uch depth[2*(256+1+29)+1]; uchf *l_buf; uInt lit_bufsize; uInt last_lit; ushf *d_buf; ulg opt_len; ulg static_len; uInt matches; int last_eob_len; ush bi_buf; int bi_valid; } deflate_state;
typedef int block_state;
typedef block_state (*compress_func)(struct internal_state *s, int flush);
typedef struct config_s { ush good_length; ush max_lazy; ush nice_length; ush max_chain; compress_func func; } config;
MGLOBALA(config, DAT_005e33e8, 10, 0x005e33e8); /* configuration_table */
MGLOBAL(const char *, DAT_00621c54, 0x00621c54); /* my_version */
MGLOBALA(char *, DAT_00622840, 10, 0x00622840); /* z_errmsg */
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
voidpf FUN_00528e00(voidpf opaque, unsigned items, unsigned size); /* zcalloc */
void FUN_00528e20(voidpf opaque, voidpf ptr); /* zcfree */
void FUN_00528e30(deflate_state *s); /* _tr_init */
int FUN_00521990(z_streamp strm); /* deflateEnd */

static __forceinline int deflateReset(z_streamp strm)
{
    deflate_state *s;

    if (strm == 0 || strm->state == 0 ||
        strm->zalloc == 0 || strm->zfree == 0) return -2;

    strm->total_in = strm->total_out = 0;
    strm->msg = 0;
    strm->data_type = 2;

    s = (deflate_state *)strm->state;
    s->pending = 0;
    s->pending_out = s->pending_buf;

    if (s->noheader < 0) {
        s->noheader = 0;
    }
    s->status = s->noheader ? 113 : 42;
    strm->adler = 1;
    s->last_flush = 0;

    FUN_00528e30(s);

    s->window_size = (ulg)2L*s->w_size;

    s->head[s->hash_size-1] = 0;
    memset((Bytef *)s->head, 0, (unsigned)(s->hash_size-1)*sizeof(*s->head));

    s->max_lazy_match   = DAT_005e33e8[s->level].max_lazy;
    s->good_match       = DAT_005e33e8[s->level].good_length;
    s->nice_match       = DAT_005e33e8[s->level].nice_length;
    s->max_chain_length = DAT_005e33e8[s->level].max_chain;

    s->strstart = 0;
    s->block_start = 0L;
    s->lookahead = 0;
    s->match_length = s->prev_length = 3-1;
    s->match_available = 0;
    s->ins_h = 0;

    return 0;
}

// MATCH: 0x00521110 FUN_00521110
int FUN_00521110(z_streamp strm, int level, int method, int windowBits, int memLevel, int strategy,
    const char *version, int stream_size)
{
    deflate_state *s;
    int noheader = 0;

    ushf *overlay;
    

    if (version == 0 || version[0] != DAT_00621c54[0] ||
        stream_size != sizeof(z_stream)) {
	return -6;
    }
    if (strm == 0) return -2;

    strm->msg = 0;
    if (strm->zalloc == 0) {
	strm->zalloc = FUN_00528e00;
	strm->opaque = (voidpf)0;
    }
    if (strm->zfree == 0) strm->zfree = FUN_00528e20;

    if (level == -1) level = 6;

    if (windowBits < 0) { 
        noheader = 1;
        windowBits = -windowBits;
    }
    if (memLevel < 1 || memLevel > 9 || method != 8 ||
        windowBits < 8 || windowBits > 15 || level < 0 || level > 9 ||
	strategy < 0 || strategy > 2) {
        return -2;
    }
    s = (deflate_state *) (*((strm)->zalloc))((strm)->opaque,(1),(sizeof(deflate_state)));
    if (s == 0) return -4;
    strm->state = (struct internal_state *)s;
    s->strm = strm;

    s->noheader = noheader;
    s->w_bits = windowBits;
    s->w_size = 1 << s->w_bits;
    s->w_mask = s->w_size - 1;

    s->hash_bits = memLevel + 7;
    s->hash_size = 1 << s->hash_bits;
    s->hash_mask = s->hash_size - 1;
    s->hash_shift =  ((s->hash_bits+3-1)/3);

    s->window = (Bytef *) (*((strm)->zalloc))((strm)->opaque,(s->w_size),(2*sizeof(Byte)));
    s->prev   = (Posf *)  (*((strm)->zalloc))((strm)->opaque,(s->w_size),(sizeof(Pos)));
    s->head   = (Posf *)  (*((strm)->zalloc))((strm)->opaque,(s->hash_size),(sizeof(Pos)));

    s->lit_bufsize = 1 << (memLevel + 6); 

    overlay = (ushf *) (*((strm)->zalloc))((strm)->opaque,(s->lit_bufsize),(sizeof(ush)+2));
    s->pending_buf = (uchf *) overlay;
    s->pending_buf_size = (ulg)s->lit_bufsize * (sizeof(ush)+2L);

    if (s->window == 0 || s->prev == 0 || s->head == 0 ||
        s->pending_buf == 0) {
        strm->msg = DAT_00622840[2-(-4)];
        FUN_00521990(strm);
        return -4;
    }
    s->d_buf = overlay + s->lit_bufsize/sizeof(ush);
    s->l_buf = s->pending_buf + (1+sizeof(ush))*s->lit_bufsize;

    s->level = level;
    s->strategy = strategy;
    s->method = (Byte)method;

    return deflateReset(strm);
}
