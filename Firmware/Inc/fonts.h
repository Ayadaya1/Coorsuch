#ifndef __FONTS_H
#define __FONTS_H

// font parameters main structure
typedef struct {
  const unsigned char FontWidth;
  const unsigned char FontHeight;
  const unsigned char *fontEn;
  const unsigned char *fontRu;
} FontDef;

// some available fonts
extern FontDef Font_7x9;

#endif

