import re

# ======== Configuration ========
# Target dimensions for the fixed grid (e.g. 8x8)
TARGET_WIDTH = 8
TARGET_HEIGHT = 8
# Assume the baseline is the bottom row (index TARGET_HEIGHT - 1)
BASELINE = TARGET_HEIGHT - 1

# ======== TomThumb Data ========
# Sample TomThumbBitmaps data.
# In your real conversion, replace this list with the full bitmap array.
tomthumb_bitmaps = [
    0,
    32,
    232,
    33,
    180,
    34,
    190,
    250,
    35,
    121,
    228,
    36,
    133,
    66,
    37,
    219,
    214,
    38,
    192,
    39,
    106,
    64,
    40,
    149,
    128,
    41,
    170,
    128,
    42,
    93,
    0,
    43,
    96,
    44,
    224,
    45,
    128,
    46,
    37,
    72,
    47,
    118,
    220,
    48,
    117,
    64,
    49,
    197,
    78,
    50,
    197,
    28,
    51,
    183,
    146,
    52,
    243,
    28,
    53,
    115,
    222,
    54,
    229,
    72,
    55,
    247,
    222,
    56,
    247,
    156,
    57,
    160,
    58,
    70,
    59,
    42,
    34,
    60,
    227,
    128,
    61,
    136,
    168,
    62,
    229,
    4,
    63,
    87,
    198,
    64,
    87,
    218,
    65,
    215,
    92,
    66,
    114,
    70,
    67,
    214,
    220,
    68,
    243,
    206,
    69,
    243,
    200,
    70,
    115,
    214,
    71,
    183,
    218,
    72,
    233,
    46,
    73,
    36,
    212,
    74,
    183,
    90,
    75,
    146,
    78,
    76,
    191,
    218,
    77,
    191,
    250,
    78,
    86,
    212,
    79,
    215,
    72,
    80,
    86,
    246,
    81,
    215,
    234,
    82,
    113,
    28,
    83,
    233,
    36,
    84,
    182,
    214,
    85,
    182,
    164,
    86,
    183,
    250,
    87,
    181,
    90,
    88,
    181,
    36,
    89,
    229,
    78,
    90,
    242,
    78,
    91,
    136,
    128,
    92,
    228,
    158,
    93,
    84,
    94,
    224,
    95,
    144,
    96
]


# Sample TomThumbGlyphs data as a multiline string.
# Format for each glyph: {offset, width, height, advance, x offset, y offset}
# Comments indicate the ASCII code.
tomthumb_glyphs_str = """
{0, 1, 1, 2, 0, -5},   /* 0x20 space */
{1, 1, 5, 2, 0, -5},   /* 0x21 exclam */
{2, 3, 2, 4, 0, -5},   /* 0x22 quotedbl */
{3, 3, 5, 4, 0, -5},   /* 0x23 numbersign */
{5, 3, 5, 4, 0, -5},   /* 0x24 dollar */
{7, 3, 5, 4, 0, -5},   /* 0x25 percent */
{9, 3, 5, 4, 0, -5},   /* 0x26 ampersand */
{11, 1, 2, 2, 0, -5},  /* 0x27 quotesingle */
{12, 2, 5, 3, 0, -5},  /* 0x28 parenleft */
{14, 2, 5, 3, 0, -5},  /* 0x29 parenright */
{16, 3, 3, 4, 0, -5},  /* 0x2A asterisk */
{18, 3, 3, 4, 0, -4},  /* 0x2B plus */
{20, 2, 2, 3, 0, -2},  /* 0x2C comma */
{21, 3, 1, 4, 0, -3},  /* 0x2D hyphen */
{22, 1, 1, 2, 0, -1},  /* 0x2E period */
{23, 3, 5, 4, 0, -5},  /* 0x2F slash */
{25, 3, 5, 4, 0, -5},  /* 0x30 zero */
{27, 2, 5, 3, 0, -5},  /* 0x31 one */
{29, 3, 5, 4, 0, -5},  /* 0x32 two */
{31, 3, 5, 4, 0, -5},  /* 0x33 three */
{33, 3, 5, 4, 0, -5},  /* 0x34 four */
{35, 3, 5, 4, 0, -5},  /* 0x35 five */
{37, 3, 5, 4, 0, -5},  /* 0x36 six */
{39, 3, 5, 4, 0, -5},  /* 0x37 seven */
{41, 3, 5, 4, 0, -5},  /* 0x38 eight */
{43, 3, 5, 4, 0, -5},  /* 0x39 nine */
{45, 1, 3, 2, 0, -4},  /* 0x3A colon */
{46, 2, 4, 3, 0, -4},  /* 0x3B semicolon */
{47, 3, 5, 4, 0, -5},  /* 0x3C less */
{49, 3, 3, 4, 0, -4},  /* 0x3D equal */
{51, 3, 5, 4, 0, -5},  /* 0x3E greater */
{53, 3, 5, 4, 0, -5},  /* 0x3F question */
{55, 3, 5, 4, 0, -5},  /* 0x40 at */
{57, 3, 5, 4, 0, -5},  /* 0x41 A */
{59, 3, 5, 4, 0, -5},  /* 0x42 B */
{61, 3, 5, 4, 0, -5},  /* 0x43 C */
{63, 3, 5, 4, 0, -5},  /* 0x44 D */
{65, 3, 5, 4, 0, -5},  /* 0x45 E */
{67, 3, 5, 4, 0, -5},  /* 0x46 F */
{69, 3, 5, 4, 0, -5},  /* 0x47 G */
{71, 3, 5, 4, 0, -5},  /* 0x48 H */
{73, 3, 5, 4, 0, -5},  /* 0x49 I */
{75, 3, 5, 4, 0, -5},  /* 0x4A J */
{77, 3, 5, 4, 0, -5},  /* 0x4B K */
{79, 3, 5, 4, 0, -5},  /* 0x4C L */
{81, 3, 5, 4, 0, -5},  /* 0x4D M */
{83, 3, 5, 4, 0, -5},  /* 0x4E N */
{85, 3, 5, 4, 0, -5},  /* 0x4F O */
{87, 3, 5, 4, 0, -5},  /* 0x50 P */
{89, 3, 5, 4, 0, -5},  /* 0x51 Q */
{91, 3, 5, 4, 0, -5},  /* 0x52 R */
{93, 3, 5, 4, 0, -5},  /* 0x53 S */
{95, 3, 5, 4, 0, -5},  /* 0x54 T */
{97, 3, 5, 4, 0, -5},  /* 0x55 U */
{99, 3, 5, 4, 0, -5},  /* 0x56 V */
{101, 3, 5, 4, 0, -5}, /* 0x57 W */
{103, 3, 5, 4, 0, -5}, /* 0x58 X */
{105, 3, 5, 4, 0, -5}, /* 0x59 Y */
{107, 3, 5, 4, 0, -5}, /* 0x5A Z */
{109, 3, 5, 4, 0, -5}, /* 0x5B bracketleft */
{111, 3, 3, 4, 0, -4}, /* 0x5C backslash */
{113, 3, 5, 4, 0, -5}, /* 0x5D bracketright */
{115, 3, 2, 4, 0, -5}, /* 0x5E asciicircum */
{116, 3, 1, 4, 0, -1}, /* 0x5F underscore */
{117, 2, 2, 3, 0, -5}, /* 0x60 grave */
{118, 3, 4, 4, 0, -4}, /* 0x61 a */
{120, 3, 5, 4, 0, -5}, /* 0x62 b */
{122, 3, 4, 4, 0, -4}, /* 0x63 c */
{124, 3, 5, 4, 0, -5}, /* 0x64 d */
{126, 3, 4, 4, 0, -4}, /* 0x65 e */
{128, 3, 5, 4, 0, -5}, /* 0x66 f */
{130, 3, 5, 4, 0, -4}, /* 0x67 g */
{132, 3, 5, 4, 0, -5}, /* 0x68 h */
{134, 1, 5, 2, 0, -5}, /* 0x69 i */
{135, 3, 6, 4, 0, -5}, /* 0x6A j */
{138, 3, 5, 4, 0, -5}, /* 0x6B k */
{140, 3, 5, 4, 0, -5}, /* 0x6C l */
{142, 3, 4, 4, 0, -4}, /* 0x6D m */
{144, 3, 4, 4, 0, -4}, /* 0x6E n */
{146, 3, 4, 4, 0, -4}, /* 0x6F o */
{148, 3, 5, 4, 0, -4}, /* 0x70 p */
{150, 3, 5, 4, 0, -4}, /* 0x71 q */
{152, 3, 4, 4, 0, -4}, /* 0x72 r */
{154, 3, 4, 4, 0, -4}, /* 0x73 s */
{156, 3, 5, 4, 0, -5}, /* 0x74 t */
{158, 3, 4, 4, 0, -4}, /* 0x75 u */
{160, 3, 4, 4, 0, -4}, /* 0x76 v */
{162, 3, 4, 4, 0, -4}, /* 0x77 w */
{164, 3, 4, 4, 0, -4}, /* 0x78 x */
{166, 3, 5, 4, 0, -4}, /* 0x79 y */
{168, 3, 4, 4, 0, -4}, /* 0x7A z */
{170, 3, 5, 4, 0, -5}, /* 0x7B braceleft */
{172, 1, 5, 2, 0, -5}, /* 0x7C bar */
{173, 3, 5, 4, 0, -5}, /* 0x7D braceright */
{175, 3, 2, 4, 0, -5}, /* 0x7E asciitilde */
{176, 1, 5, 2, 0, -5}, /* 0xA1 exclamdown */
{177, 3, 5, 4, 0, -5}, /* 0xA2 cent */
{179, 3, 5, 4, 0, -5}, /* 0xA3 sterling */
{181, 3, 5, 4, 0, -5}, /* 0xA4 currency */
{183, 3, 5, 4, 0, -5}, /* 0xA5 yen */
{185, 1, 5, 2, 0, -5}, /* 0xA6 brokenbar */
{186, 3, 5, 4, 0, -5}, /* 0xA7 section */
{188, 3, 1, 4, 0, -5}, /* 0xA8 dieresis */
{189, 3, 3, 4, 0, -5}, /* 0xA9 copyright */
{191, 3, 5, 4, 0, -5}, /* 0xAA ordfeminine */
{193, 2, 3, 3, 0, -5}, /* 0xAB guillemotleft */
{194, 3, 2, 4, 0, -4}, /* 0xAC logicalnot */
{195, 2, 1, 3, 0, -3}, /* 0xAD softhyphen */
{196, 3, 3, 4, 0, -5}, /* 0xAE registered */
{198, 3, 1, 4, 0, -5}, /* 0xAF macron */
{199, 3, 3, 4, 0, -5}, /* 0xB0 degree */
{201, 3, 5, 4, 0, -5}, /* 0xB1 plusminus */
{203, 3, 3, 4, 0, -5}, /* 0xB2 twosuperior */
{205, 3, 3, 4, 0, -5}, /* 0xB3 threesuperior */
{207, 2, 2, 3, 0, -5}, /* 0xB4 acute */
{208, 3, 5, 4, 0, -5}, /* 0xB5 mu */
{210, 3, 5, 4, 0, -5}, /* 0xB6 paragraph */
{212, 3, 3, 4, 0, -4}, /* 0xB7 periodcentered */
{214, 3, 3, 4, 0, -3}, /* 0xB8 cedilla */
{216, 1, 3, 2, 0, -5}, /* 0xB9 onesuperior */
{217, 3, 5, 4, 0, -5}, /* 0xBA ordmasculine */
{219, 2, 3, 3, 0, -5}, /* 0xBB guillemotright */
{220, 3, 5, 4, 0, -5}, /* 0xBC onequarter */
{222, 3, 5, 4, 0, -5}, /* 0xBD onehalf */
{224, 3, 5, 4, 0, -5}, /* 0xBE threequarters */
{226, 3, 5, 4, 0, -5}, /* 0xBF questiondown */
{228, 3, 5, 4, 0, -5}, /* 0xC0 Agrave */
{230, 3, 5, 4, 0, -5}, /* 0xC1 Aacute */
{232, 3, 5, 4, 0, -5}, /* 0xC2 Acircumflex */
{234, 3, 5, 4, 0, -5}, /* 0xC3 Atilde */
{236, 3, 5, 4, 0, -5}, /* 0xC4 Adieresis */
{238, 3, 5, 4, 0, -5}, /* 0xC5 Aring */
{240, 3, 5, 4, 0, -5}, /* 0xC6 AE */
{242, 3, 6, 4, 0, -5}, /* 0xC7 Ccedilla */
{245, 3, 5, 4, 0, -5}, /* 0xC8 Egrave */
{247, 3, 5, 4, 0, -5}, /* 0xC9 Eacute */
{249, 3, 5, 4, 0, -5}, /* 0xCA Ecircumflex */
{251, 3, 5, 4, 0, -5}, /* 0xCB Edieresis */
{253, 3, 5, 4, 0, -5}, /* 0xCC Igrave */
{255, 3, 5, 4, 0, -5}, /* 0xCD Iacute */
{257, 3, 5, 4, 0, -5}, /* 0xCE Icircumflex */
{259, 3, 5, 4, 0, -5}, /* 0xCF Idieresis */
{261, 3, 5, 4, 0, -5}, /* 0xD0 Eth */
{263, 3, 5, 4, 0, -5}, /* 0xD1 Ntilde */
{265, 3, 5, 4, 0, -5}, /* 0xD2 Ograve */
{267, 3, 5, 4, 0, -5}, /* 0xD3 Oacute */
{269, 3, 5, 4, 0, -5}, /* 0xD4 Ocircumflex */
{271, 3, 5, 4, 0, -5}, /* 0xD5 Otilde */
{273, 3, 5, 4, 0, -5}, /* 0xD6 Odieresis */
{275, 3, 3, 4, 0, -4}, /* 0xD7 multiply */
{277, 3, 5, 4, 0, -5}, /* 0xD8 Oslash */
{279, 3, 5, 4, 0, -5}, /* 0xD9 Ugrave */
{281, 3, 5, 4, 0, -5}, /* 0xDA Uacute */
{283, 3, 5, 4, 0, -5}, /* 0xDB Ucircumflex */
{285, 3, 5, 4, 0, -5}, /* 0xDC Udieresis */
{287, 3, 5, 4, 0, -5}, /* 0xDD Yacute */
{289, 3, 5, 4, 0, -5}, /* 0xDE Thorn */
{291, 3, 6, 4, 0, -5}, /* 0xDF germandbls */
{294, 3, 5, 4, 0, -5}, /* 0xE0 agrave */
{296, 3, 5, 4, 0, -5}, /* 0xE1 aacute */
{298, 3, 5, 4, 0, -5}, /* 0xE2 acircumflex */
{300, 3, 5, 4, 0, -5}, /* 0xE3 atilde */
{302, 3, 5, 4, 0, -5}, /* 0xE4 adieresis */
{304, 3, 5, 4, 0, -5}, /* 0xE5 aring */
{306, 3, 4, 4, 0, -4}, /* 0xE6 ae */
{308, 3, 5, 4, 0, -4}, /* 0xE7 ccedilla */
{310, 3, 5, 4, 0, -5}, /* 0xE8 egrave */
{312, 3, 5, 4, 0, -5}, /* 0xE9 eacute */
{314, 3, 5, 4, 0, -5}, /* 0xEA ecircumflex */
{316, 3, 5, 4, 0, -5}, /* 0xEB edieresis */
{318, 2, 5, 3, 0, -5}, /* 0xEC igrave */
{320, 2, 5, 3, 0, -5}, /* 0xED iacute */
{322, 3, 5, 4, 0, -5}, /* 0xEE icircumflex */
{324, 3, 5, 4, 0, -5}, /* 0xEF idieresis */
{326, 3, 5, 4, 0, -5}, /* 0xF0 eth */
{328, 3, 5, 4, 0, -5}, /* 0xF1 ntilde */
{330, 3, 5, 4, 0, -5}, /* 0xF2 ograve */
{332, 3, 5, 4, 0, -5}, /* 0xF3 oacute */
{334, 3, 5, 4, 0, -5}, /* 0xF4 ocircumflex */
{336, 3, 5, 4, 0, -5}, /* 0xF5 otilde */
{338, 3, 5, 4, 0, -5}, /* 0xF6 odieresis */
{340, 3, 5, 4, 0, -5}, /* 0xF7 divide */
{342, 3, 4, 4, 0, -4}, /* 0xF8 oslash */
{344, 3, 5, 4, 0, -5}, /* 0xF9 ugrave */
{346, 3, 5, 4, 0, -5}, /* 0xFA uacute */
{348, 3, 5, 4, 0, -5}, /* 0xFB ucircumflex */
{350, 3, 5, 4, 0, -5}, /* 0xFC udieresis */
{352, 3, 6, 4, 0, -5}, /* 0xFD yacute */
{355, 3, 5, 4, 0, -4}, /* 0xFE thorn */
{357, 3, 6, 4, 0, -5}, /* 0xFF ydieresis */
{360, 1, 1, 2, 0, -1}, /* 0x11D gcircumflex */
{361, 3, 5, 4, 0, -5}, /* 0x152 OE */
{363, 3, 4, 4, 0, -4}, /* 0x153 oe */
{365, 3, 5, 4, 0, -5}, /* 0x160 Scaron */
{367, 3, 5, 4, 0, -5}, /* 0x161 scaron */
{369, 3, 5, 4, 0, -5}, /* 0x178 Ydieresis */
{371, 3, 5, 4, 0, -5}, /* 0x17D Zcaron */
{373, 3, 5, 4, 0, -5}, /* 0x17E zcaron */
{375, 1, 1, 2, 0, -1}, /* 0xEA4 uni0EA4 */
{376, 1, 1, 2, 0, -1}, /* 0x13A0 uni13A0 */
{377, 1, 1, 2, 0, -3}, /* 0x2022 bullet */
{378, 3, 1, 4, 0, -1}, /* 0x2026 ellipsis */
{379, 3, 5, 4, 0, -5}, /* 0x20AC Euro */
{381, 4, 5, 5, 0, -5}, /* 0xFFFD uniFFFD */
"""

# Parse the glyph string using a regex.
glyph_pattern = re.compile(r"\{([^}]+)\}")
tomthumb_glyphs = []
for match in glyph_pattern.finditer(tomthumb_glyphs_str):
    # Extract the comma-separated values and convert to integers.
    numbers = [int(x.strip()) for x in match.group(1).split(',')]
    if len(numbers) >= 6:
        tomthumb_glyphs.append(tuple(numbers[:6]))

# For this conversion, assume that the glyphs cover ASCII codes starting at 0x20.
ascii_start = 32
num_glyphs = len(tomthumb_glyphs)
ascii_end = ascii_start + num_glyphs - 1

# ======== Conversion Functions ========
def render_glyph(glyph):
    """
    Render a single glyph (from TomThumb data) into a fixed TARGET_WIDTH x TARGET_HEIGHT grid.
    The glyph is drawn at a position determined by its x and y offsets relative to a baseline.
    """
    offset, width, height, advance, x_offset, y_offset = glyph

    # Create an empty grid: rows are indexed 0..TARGET_HEIGHT-1.
    grid = [[0 for _ in range(TARGET_WIDTH)] for _ in range(TARGET_HEIGHT)]

    # Determine starting drawing coordinates:
    #   x position: shift right by -x_offset
    #   y position: baseline + y_offset (since y_offset is negative if the glyph is above the baseline)
    draw_x = -x_offset
    draw_y = BASELINE + y_offset

    for row in range(height):
        # Get the corresponding row's data from tomthumb_bitmaps.
        # (Make sure we don't exceed the bitmap array length.)
        if offset + row < len(tomthumb_bitmaps):
            row_data = tomthumb_bitmaps[offset + row]
        else:
            row_data = 0
        for col in range(width):
            # Check the bit in row_data (assuming MSB is the leftmost pixel).
            if row_data & (0x80 >> col):
                x_pos = draw_x + col
                y_pos = draw_y + row
                if 0 <= x_pos < TARGET_WIDTH and 0 <= y_pos < TARGET_HEIGHT:
                    grid[y_pos][x_pos] = 1
    return grid

def grid_to_byte_array(grid):
    """
    Convert an 8-bit row grid to a list of bytes. Each row of the grid becomes one byte.
    """
    byte_array = []
    for row in grid:
        byte = 0
        for bit in row:
            byte = (byte << 1) | bit
        byte_array.append(byte)
    return byte_array

# ======== Convert and Output ========
# Create an output array for all glyphs (for the defined range).
output_array = []  # Each element will be a list of TARGET_HEIGHT bytes.
for i in range(num_glyphs):
    grid = render_glyph(tomthumb_glyphs[i])
    byte_array = grid_to_byte_array(grid)
    output_array.append(byte_array)

# Print the output as a C array.
print("const uint8_t font8x8_tomthumb[{}][{}] = {{".format(num_glyphs, TARGET_HEIGHT))
for glyph_bytes in output_array:
    # Format each row as a hexadecimal value.
    row_str = ", ".join("0x{:02X}".format(b) for b in glyph_bytes)
    print("    {" + row_str + "},")
print("};")
