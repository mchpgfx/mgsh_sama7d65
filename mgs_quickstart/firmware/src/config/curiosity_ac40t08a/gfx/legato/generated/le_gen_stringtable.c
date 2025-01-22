#include "gfx/legato/generated/le_gen_assets.h"

/*****************************************************************************
 * Legato String Table
 * Encoding        ASCII
 * Language Count: 1
 * String Count:   6
 *****************************************************************************/

/*****************************************************************************
 * string table data
 * 
 * this table contains the raw character data for each string
 * 
 * unsigned short - number of indices in the table
 * unsigned short - number of languages in the table
 * 
 * index array (size = number of indices * number of languages
 * 
 * for each index in the array:
 *   unsigned byte - the font ID for the index
 *   unsigned byte[3] - the offset of the string codepoint data in
 *                      the table
 * 
 * string data is found by jumping to the index offset from the start
 * of the table
 * 
 * string data entry:
 *     unsigned short - length of the string in bytes (encoding dependent)
 *     codepoint data - the string data
 ****************************************************************************/

const uint8_t stringTable_data[94] =
{
    0x06,0x00,0x01,0x00,0x02,0x1C,0x00,0x00,0x01,0x2A,0x00,0x00,0x00,0x40,0x00,0x00,
    0x02,0x48,0x00,0x00,0x00,0x4E,0x00,0x00,0x00,0x56,0x00,0x00,0x0B,0x00,0x51,0x75,
    0x69,0x63,0x6B,0x20,0x53,0x74,0x61,0x72,0x74,0x00,0x14,0x00,0x53,0x63,0x61,0x6E,
    0x20,0x74,0x6F,0x20,0x67,0x65,0x74,0x20,0x73,0x74,0x61,0x72,0x74,0x65,0x64,0x2E,
    0x06,0x00,0x53,0x6D,0x61,0x72,0x74,0x2E,0x04,0x00,0x42,0x61,0x63,0x6B,0x05,0x00,
    0x46,0x61,0x73,0x74,0x2E,0x00,0x05,0x00,0x45,0x61,0x73,0x79,0x2E,0x00,
};

/* font asset pointer list */
leFont* fontList[3] =
{
    (leFont*)&bigFont,
    (leFont*)&smallFont,
    (leFont*)&extraSmallFont,
};

const leStringTable stringTable =
{
    {
        LE_STREAM_LOCATION_ID_INTERNAL, // data location id
        (void*)stringTable_data, // data address pointer
        94, // data size
    },
    (void*)stringTable_data, // string table data
    fontList, // font lookup table
    LE_STRING_ENCODING_ASCII // encoding standard
};


// string list
leTableString string_strQuickstart;
leTableString string_srtHelp;
leTableString string_strSmart;
leTableString string_strBack;
leTableString string_strFast;
leTableString string_strEasy;

void initializeStrings(void)
{
    leTableString_Constructor(&string_strQuickstart, stringID_strQuickstart);
    leTableString_Constructor(&string_srtHelp, stringID_srtHelp);
    leTableString_Constructor(&string_strSmart, stringID_strSmart);
    leTableString_Constructor(&string_strBack, stringID_strBack);
    leTableString_Constructor(&string_strFast, stringID_strFast);
    leTableString_Constructor(&string_strEasy, stringID_strEasy);
}
