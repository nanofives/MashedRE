// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00521520: zlib 1.1.3 deflate.c deflate, with the static putShortMSB and flush_pending
   (defined later in deflate.c) inlined, written as static __forceinline copies above the function. The zlib header
   expression is written pre-folded ((w_bits<<12) - 0x7800, same value): the official form gave
   sub 8 / shl 12 / add 0x800 where the original has shl 12 / sub 0x7800.
   84.5% (regs 84.5, ops 94.3). Residual: block layout only. The original keeps three small return blocks
   (ERR_RETURN Z_BUF_ERROR after the avail_out test, the two 'return Z_OK' after need_more/finish_started) inline
   at the fall-through with their own epilogue copies; ours moves them to the end and shares one epilogue. Same
   pattern as 0x00522b50 / 0x00521110. Tried: permuter 197 variants. Class (b). */
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
MGLOBALA(char *, DAT_00622840, 10, 0x00622840); /* z_errmsg */
void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memset)
#pragma intrinsic(memcpy)
void FUN_00528ef0(deflate_state *s); /* _tr_align */
void FUN_00529050(deflate_state *s, charf *buf, ulg stored_len, int eof); /* _tr_stored_block */
#define ERR_RETURN(strm,err) return (strm->msg = DAT_00622840[2-(err)], (err))
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}

static __forceinline void putShortMSB (deflate_state *s, uInt b)
{
    put_byte(s, (Byte)(b >> 8));
    put_byte(s, (Byte)(b & 0xff));
}

static __forceinline void flush_pending(z_streamp strm)
{
    unsigned len = strm->state->pending;

    if (len > strm->avail_out) len = strm->avail_out;
    if (len == 0) return;

    memcpy(strm->next_out, strm->state->pending_out, len);
    strm->next_out  += len;
    strm->state->pending_out  += len;
    strm->total_out += len;
    strm->avail_out  -= len;
    strm->state->pending -= len;
    if (strm->state->pending == 0) {
        strm->state->pending_out = strm->state->pending_buf;
    }
}

// MATCH: 0x00521520 FUN_00521520
int FUN_00521520(z_streamp strm, int flush)
{
    int old_flush; 
    deflate_state *s;

    if (strm == 0 || strm->state == 0 ||
	flush > 4 || flush < 0) {
        return -2;
    }
    s = strm->state;

    if (strm->next_out == 0 ||
        (strm->next_in == 0 && strm->avail_in != 0) ||
	(s->status == 666 && flush != 4)) {
        ERR_RETURN(strm, -2);
    }
    if (strm->avail_out == 0) ERR_RETURN(strm, -5);

    s->strm = strm; 
    old_flush = s->last_flush;
    s->last_flush = flush;

    
    if (s->status == 42) {

        uInt header = (s->w_bits<<12) - 0x7800; /* (Z_DEFLATED + ((w_bits-8)<<4)) << 8, folded */
        uInt level_flags = (s->level-1) >> 1;

        if (level_flags > 3) level_flags = 3;
        header |= (level_flags << 6);
	if (s->strstart != 0) header |= 0x20;
        header += 31 - (header % 31);

        s->status = 113;
        putShortMSB(s, header);

	
	if (s->strstart != 0) {
	    putShortMSB(s, (uInt)(strm->adler >> 16));
	    putShortMSB(s, (uInt)(strm->adler & 0xffff));
	}
	strm->adler = 1L;
    }

    
    if (s->pending != 0) {
        flush_pending(strm);
        if (strm->avail_out == 0) {
	    
	    s->last_flush = -1;
	    return 0;
	}

    
    } else if (strm->avail_in == 0 && flush <= old_flush &&
	       flush != 4) {
        ERR_RETURN(strm, -5);
    }

    
    if (s->status == 666 && strm->avail_in != 0) {
        ERR_RETURN(strm, -5);
    }

    
    if (strm->avail_in != 0 || s->lookahead != 0 ||
        (flush != 0 && s->status != 666)) {
        block_state bstate;

	bstate = (*(DAT_005e33e8[s->level].func))(s, flush);

        if (bstate == 2 || bstate == 3) {
            s->status = 666;
        }
        if (bstate == 0 || bstate == 2) {
	    if (strm->avail_out == 0) {
	        s->last_flush = -1; 
	    }
	    return 0;
	    
	}
        if (bstate == 1) {
            if (flush == 1) {
                FUN_00528ef0(s);
            } else { 
                FUN_00529050(s, (char*)0, 0L, 0);
                
                if (flush == 3) {
                    s->head[s->hash_size-1] = 0; memset((Bytef *)s->head, 0, (unsigned)(s->hash_size-1)*sizeof(*s->head));             
                }
            }
            flush_pending(strm);
	    if (strm->avail_out == 0) {
	      s->last_flush = -1; 
	      return 0;
	    }
        }
    }

    if (flush != 4) return 0;
    if (s->noheader) return 1;

    
    putShortMSB(s, (uInt)(strm->adler >> 16));
    putShortMSB(s, (uInt)(strm->adler & 0xffff));
    flush_pending(strm);
    
    s->noheader = -1; 
    return s->pending != 0 ? 0 : 1;
}
