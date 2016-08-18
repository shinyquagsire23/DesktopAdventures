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

u8 deskAdvFontBitmaps[];
FONT_INFO deskAdvFontFontInfo;
FONT_CHAR_INFO deskAdvFontDescriptors[];
