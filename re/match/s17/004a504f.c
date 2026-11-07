// FLAGS: /O1 /GS
/* MATCH-S17 0x004a504f: CRT _output (VC7 crt/src/output.c, ANSI, non-POSITIONAL), hand-written from the CRT source
   under /O1 /GS. Holds the file-static helpers write_char 0x004a4fc1, write_multi_char 0x004a4ff4 and write_string
   0x004a5018 (VC7 static-function register conventions). Score: 100% (regs 100, ops 100) 608/608 ins, whole symbol 1941 B BYTE-IDENTICAL incl. the state jump table; helpers 100% each. The va_arg double is read as the fltintrn.h struct DOUBLE (a plain double gives fld/fstp and a smaller frame). relocheck OK. */
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
typedef struct { char *_ptr; int _cnt; char *_base; int _flag; int _file; int _charbuf; int _bufsiz; char *_tmpfname; } CRT_FILE;
typedef char *crt_va_list;
#define CRT_INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define crt_va_arg(ap, t) (*(t *)((ap += CRT_INTSIZEOF(t)) - CRT_INTSIZEOF(t)))
typedef void (__cdecl *PFV)(void);
typedef struct { double x; } DOUBLE;
typedef void (__cdecl *CFLTCVT)(DOUBLE *, char *, int, int, int);
typedef void (__cdecl *CFLTSTR)(char *);

#define BUFFERSIZE 512
#define MAXPRECISION BUFFERSIZE
#define CVTBUFSIZE (309 + 40)

#define FL_SIGN       0x00001
#define FL_SIGNSP     0x00002
#define FL_LEFT       0x00004
#define FL_LEADZERO   0x00008
#define FL_LONG       0x00010
#define FL_SHORT      0x00020
#define FL_SIGNED     0x00040
#define FL_ALTERNATE  0x00080
#define FL_NEGATIVE   0x00100
#define FL_FORCEOCTAL 0x00200
#define FL_LONGDOUBLE 0x00400
#define FL_WIDECHAR   0x00800
#define FL_I64        0x08000

enum STATE { ST_NORMAL, ST_PERCENT, ST_FLAG, ST_WIDTH, ST_DOT, ST_PRECIS, ST_SIZE, ST_TYPE };
#define NUMSTATES (ST_TYPE + 1)
enum CHARTYPE { CH_OTHER, CH_PERCENT, CH_DOT, CH_STAR, CH_ZERO, CH_DIGIT, CH_FLAG, CH_SIZE, CH_TYPE };

MGLOBALA(const char, DAT_005d12a0, 0x60, 0x005d12a0);
MGLOBAL(unsigned short *, DAT_006169cc, 0x006169cc);
MGLOBAL(char *, DAT_006160d0, 0x006160d0);
MGLOBAL(unsigned short *, DAT_006160d4, 0x006160d4);
MGLOBALA(PFV, DAT_006160d8, 6, 0x006160d8);

#define find_char_class(c) ((c) < ' ' || (c) > 'x' ? CH_OTHER : DAT_005d12a0[(c) - ' '] & 0xF)
#define find_next_state(class, state) (DAT_005d12a0[(class) * NUMSTATES + (state)] >> 4)

int FUN_004a4ea8(int, CRT_FILE *);
int FUN_004ac435(char *, unsigned short);
void *FUN_004a45fb(unsigned int);
void FUN_004a460d(void *);
unsigned int FUN_004a9410(const char *);

static __inline int get_int_arg(crt_va_list *pargptr) { return crt_va_arg(*pargptr, int); }
static __inline void *get_ptr_arg(crt_va_list *pargptr) { return crt_va_arg(*pargptr, void *); }
static __inline short get_short_arg(crt_va_list *pargptr) { return crt_va_arg(*pargptr, short); }
static __inline __int64 get_int64_arg(crt_va_list *pargptr) { return crt_va_arg(*pargptr, __int64); }

// MATCH: 0x004a4fc1 FUN_004a4fc1
static void FUN_004a4fc1(char ch, CRT_FILE *f, int *pnumwritten)
{
  if ((f->_flag & 0x40) && f->_base == 0) {
    ++(*pnumwritten);
    return;
  }
  if ((--f->_cnt >= 0 ? 0xff & (*f->_ptr++ = (char)ch) : FUN_004a4ea8(ch, f)) == -1)
    *pnumwritten = -1;
  else
    ++(*pnumwritten);
}

// MATCH: 0x004a4ff4 FUN_004a4ff4
static void FUN_004a4ff4(char ch, int num, CRT_FILE *f, int *pnumwritten)
{
  while (num-- > 0) {
    FUN_004a4fc1(ch, f, pnumwritten);
    if (*pnumwritten == -1)
      break;
  }
}

// MATCH: 0x004a5018 FUN_004a5018
static void FUN_004a5018(char *string, int len, CRT_FILE *f, int *pnumwritten)
{
  if ((f->_flag & 0x40) && f->_base == 0) {
    (*pnumwritten) += len;
    return;
  }
  while (len-- > 0) {
    FUN_004a4fc1(*string++, f, pnumwritten);
    if (*pnumwritten == -1)
      break;
  }
}

// MATCH: 0x004a504f FUN_004a504f
int FUN_004a504f(CRT_FILE *stream, const char *format, crt_va_list argptr)
{
  int hexadd;
  char ch;
  int flags;
  enum STATE state;
  enum CHARTYPE chclass;
  int radix;
  int charsout;
  int fldwidth;
  int precision;
  char prefix[2];
  int prefixlen;
  int capexp;
  int no_output;
  union { char *sz; unsigned short *wz; } text;
  int textlen;
  union { char sz[BUFFERSIZE]; } buffer;
  unsigned short wchar;
  int bufferiswide;
  char *heapbuf;

  textlen = 0;
  charsout = 0;
  heapbuf = 0;
  state = ST_NORMAL;

  while ((ch = *format++) != '\0' && charsout >= 0) {
    chclass = (enum CHARTYPE)find_char_class(ch);
    state = (enum STATE)find_next_state(chclass, state);

    switch (state) {

    case ST_NORMAL:
    NORMAL_STATE:
      bufferiswide = 0;
      if (DAT_006169cc[(unsigned char)ch] & 0x8000) {
        FUN_004a4fc1(ch, stream, &charsout);
        ch = *format++;
      }
      FUN_004a4fc1(ch, stream, &charsout);
      break;

    case ST_PERCENT:
      prefixlen = fldwidth = no_output = capexp = 0;
      flags = 0;
      precision = -1;
      bufferiswide = 0;
      break;

    case ST_FLAG:
      switch (ch) {
      case '-': flags |= FL_LEFT; break;
      case '+': flags |= FL_SIGN; break;
      case ' ': flags |= FL_SIGNSP; break;
      case '#': flags |= FL_ALTERNATE; break;
      case '0': flags |= FL_LEADZERO; break;
      }
      break;

    case ST_WIDTH:
      if (ch == '*') {
        fldwidth = get_int_arg(&argptr);
        if (fldwidth < 0) {
          flags |= FL_LEFT;
          fldwidth = -fldwidth;
        }
      }
      else {
        fldwidth = fldwidth * 10 + (ch - '0');
      }
      break;

    case ST_DOT:
      precision = 0;
      break;

    case ST_PRECIS:
      if (ch == '*') {
        precision = get_int_arg(&argptr);
        if (precision < 0)
          precision = -1;
      }
      else {
        precision = precision * 10 + (ch - '0');
      }
      break;

    case ST_SIZE:
      switch (ch) {
      case 'l':
        flags |= FL_LONG;
        break;
      case 'I':
        if ((*format == '6') && (*(format + 1) == '4')) {
          format += 2;
          flags |= FL_I64;
        }
        else if ((*format == '3') && (*(format + 1) == '2')) {
          format += 2;
          flags &= ~FL_I64;
        }
        else if ((*format == 'd') || (*format == 'i') || (*format == 'o') ||
                 (*format == 'u') || (*format == 'x') || (*format == 'X')) {
        }
        else {
          state = ST_NORMAL;
          goto NORMAL_STATE;
        }
        break;
      case 'h':
        flags |= FL_SHORT;
        break;
      case 'w':
        flags |= FL_WIDECHAR;
        break;
      }
      break;

    case ST_TYPE:
      switch (ch) {

      case 'C':
        if (!(flags & (FL_SHORT | FL_LONG | FL_WIDECHAR)))
          flags |= FL_WIDECHAR;

      case 'c': {
        if (flags & (FL_LONG | FL_WIDECHAR)) {
          wchar = (unsigned short)get_short_arg(&argptr);
          textlen = FUN_004ac435(buffer.sz, wchar);
          if (textlen < 0)
            no_output = 1;
        } else {
          unsigned short temp;
          temp = (unsigned short)get_int_arg(&argptr);
          {
            buffer.sz[0] = (char)temp;
            textlen = 1;
          }
        }
        text.sz = buffer.sz;
      }
      break;

      case 'Z': {
        struct _count_string {
          short Length;
          short MaximumLength;
          char *Buffer;
        } *pstr;

        pstr = (struct _count_string *)get_ptr_arg(&argptr);
        if (pstr == 0 || pstr->Buffer == 0) {
          text.sz = DAT_006160d0;
          textlen = (int)FUN_004a9410(text.sz);
        } else {
          if (flags & FL_WIDECHAR) {
            text.wz = (unsigned short *)pstr->Buffer;
            textlen = pstr->Length / (int)sizeof(unsigned short);
            bufferiswide = 1;
          } else {
            bufferiswide = 0;
            text.sz = pstr->Buffer;
            textlen = pstr->Length;
          }
        }
      }
      break;

      case 'S':
        if (!(flags & (FL_SHORT | FL_LONG | FL_WIDECHAR)))
          flags |= FL_WIDECHAR;

      case 's': {
        int i;
        char *p;
        unsigned short *pwch;

        i = (precision == -1) ? 0x7fffffff : precision;
        text.sz = (char *)get_ptr_arg(&argptr);

        if (flags & (FL_LONG | FL_WIDECHAR)) {
          if (text.wz == 0)
            text.wz = DAT_006160d4;
          bufferiswide = 1;
          pwch = text.wz;
          while (i-- && *pwch)
            ++pwch;
          textlen = (int)(pwch - text.wz);
        } else {
          if (text.sz == 0)
            text.sz = DAT_006160d0;
          p = text.sz;
          while (i-- && *p)
            ++p;
          textlen = (int)(p - text.sz);
        }
      }
      break;

      case 'n': {
        void *p;
        p = get_ptr_arg(&argptr);
        if (flags & FL_SHORT)
          *(short *)p = (short)charsout;
        else
          *(int *)p = charsout;
        no_output = 1;
      }
      break;

      case 'E':
      case 'G':
        capexp = 1;
        ch += 'a' - 'A';
      case 'e':
      case 'f':
      case 'g': {
        flags |= FL_SIGNED;
        text.sz = buffer.sz;

        if (precision < 0)
          precision = 6;
        else if (precision == 0 && ch == 'g')
          precision = 1;
        else if (precision > MAXPRECISION)
          precision = MAXPRECISION;

        if (precision > BUFFERSIZE - CVTBUFSIZE) {
          heapbuf = (char *)FUN_004a45fb(CVTBUFSIZE + precision);
          if (heapbuf != 0)
            text.sz = heapbuf;
          else
            precision = BUFFERSIZE - CVTBUFSIZE;
        }

        {
          DOUBLE tmp;
          tmp = crt_va_arg(argptr, DOUBLE);
          (*(CFLTCVT)DAT_006160d8[0])(&tmp, text.sz, (char)ch, precision, capexp);
        }

        if ((flags & FL_ALTERNATE) && precision == 0)
          (*(CFLTSTR)DAT_006160d8[3])(text.sz);

        if (ch == 'g' && !(flags & FL_ALTERNATE))
          (*(CFLTSTR)DAT_006160d8[1])(text.sz);

        if (*text.sz == '-') {
          flags |= FL_NEGATIVE;
          ++text.sz;
        }

        textlen = (int)FUN_004a9410(text.sz);
      }
      break;

      case 'd':
      case 'i':
        flags |= FL_SIGNED;
        radix = 10;
        goto COMMON_INT;

      case 'u':
        radix = 10;
        goto COMMON_INT;

      case 'p':
        precision = 2 * sizeof(void *);

      case 'X':
        hexadd = 'A' - '9' - 1;
        goto COMMON_HEX;

      case 'x':
        hexadd = 'a' - '9' - 1;

      COMMON_HEX:
        radix = 16;
        if (flags & FL_ALTERNATE) {
          prefix[0] = '0';
          prefix[1] = (char)('x' - 'a' + '9' + 1 + hexadd);
          prefixlen = 2;
        }
        goto COMMON_INT;

      case 'o':
        radix = 8;
        if (flags & FL_ALTERNATE) {
          flags |= FL_FORCEOCTAL;
        }

      COMMON_INT: {
        unsigned __int64 number;
        int digit;
        __int64 l;

        if (flags & FL_I64)
          l = get_int64_arg(&argptr);
        else
        if (flags & FL_SHORT) {
          if (flags & FL_SIGNED)
            l = (short)get_int_arg(&argptr);
          else
            l = (unsigned short)get_int_arg(&argptr);
        } else
        {
          if (flags & FL_SIGNED)
            l = get_int_arg(&argptr);
          else
            l = (unsigned int)get_int_arg(&argptr);
        }

        if ((flags & FL_SIGNED) && l < 0) {
          number = -l;
          flags |= FL_NEGATIVE;
        } else {
          number = l;
        }

        if ((flags & FL_I64) == 0) {
          number &= 0xffffffff;
        }

        if (precision < 0)
          precision = 1;
        else {
          flags &= ~FL_LEADZERO;
          if (precision > MAXPRECISION)
            precision = MAXPRECISION;
        }

        if (number == 0)
          prefixlen = 0;

        text.sz = &buffer.sz[BUFFERSIZE - 1];

        while (precision-- > 0 || number != 0) {
          digit = (int)(number % radix) + '0';
          number /= radix;
          if (digit > '9') {
            digit += hexadd;
          }
          *text.sz-- = (char)digit;
        }

        textlen = (int)((char *)&buffer.sz[BUFFERSIZE - 1] - text.sz);
        ++text.sz;

        if (flags & FL_FORCEOCTAL) {
          if (*text.sz != '0' || textlen == 0) {
            *--text.sz = '0';
            ++textlen;
          }
        }
      }
      break;
      }

      if (!no_output) {
        int padding;

        if (flags & FL_SIGNED) {
          if (flags & FL_NEGATIVE) {
            prefix[0] = '-';
            prefixlen = 1;
          }
          else if (flags & FL_SIGN) {
            prefix[0] = '+';
            prefixlen = 1;
          }
          else if (flags & FL_SIGNSP) {
            prefix[0] = ' ';
            prefixlen = 1;
          }
        }

        padding = fldwidth - textlen - prefixlen;

        if (!(flags & (FL_LEFT | FL_LEADZERO))) {
          FUN_004a4ff4(' ', padding, stream, &charsout);
        }

        FUN_004a5018(prefix, prefixlen, stream, &charsout);

        if ((flags & FL_LEADZERO) && !(flags & FL_LEFT)) {
          FUN_004a4ff4('0', padding, stream, &charsout);
        }

        if (bufferiswide && (textlen > 0)) {
          unsigned short *p;
          int retval, count;
          char mbuf[5 + 1];

          p = text.wz;
          count = textlen;
          while (count--) {
            retval = FUN_004ac435(mbuf, *p++);
            if (retval <= 0)
              break;
            FUN_004a5018(mbuf, retval, stream, &charsout);
          }
        } else {
          FUN_004a5018(text.sz, textlen, stream, &charsout);
        }

        if (flags & FL_LEFT) {
          FUN_004a4ff4(' ', padding, stream, &charsout);
        }
      }
      if (heapbuf) {
        FUN_004a460d(heapbuf);
        heapbuf = 0;
      }
      break;
    }
  }

  return charsout;
}
