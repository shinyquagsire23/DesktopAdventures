#ifndef FONT_H
#define FONT_H

// Font data for Coder's Crux 12pt, by NAL

typedef struct
{
    int width; // Character width in bits.
    int offset; // Offset in bytes into font bitmap.
}
FONT_CHAR_INFO;


typedef struct
{
    int height; // Character height in bits.
    char start_char; // Start character.
    char end_char; // End character.
    char space_width; // Space width
    FONT_CHAR_INFO *p_character_descriptor; // Character decriptor array.
    u8 *p_character_bitmaps; // Character bitmap array.
}
FONT_INFO;

#define DESKADV_FONT_FONT_HEIGHT (9)
#define DESKADV_INV_FONT_HEIGHT (10)

const u8 deskAdvFontBitmaps[];
const FONT_INFO deskAdvFontFontInfo;
const FONT_CHAR_INFO deskAdvFontDescriptors[];

const u8 deskAdvInvFontBitmaps[];
const FONT_INFO deskAdvInvFontInfo;
const FONT_CHAR_INFO deskAdvInvFontDescriptors[];

#endif
