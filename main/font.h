#ifndef __FONT_H__
#define __FONT_H__

#define ASIZE(a) (sizeof(a)/sizeof(a[0]))

enum{
    FontSize_6x8,
    FontSize_8x16,
    FontSize_12x24,
    FontSize_16x32,
};

typedef struct {
    char *en;
    int *cn;
    int len;
} language_t;

extern unsigned char ascii_6x8[][6];
extern unsigned char ascii_8x16[][16];
extern unsigned char ascii_12x24[][36];
extern unsigned char ascii_16x32[][64];
extern unsigned char extend_6x8[][6];
extern unsigned char extend_8x16[][16];
extern unsigned char CN[][32];
extern language_t language[];

#endif

