#include <Arduino.h>
#include "convertUnaccent.h"


// Function to convert a single Vietnamese Unicode character to an unaccented character
char remove_diacritic(unsigned int codepoint) {
    switch (codepoint) {
        // Lowercase letters
        case 0x00E0: case 0x00E1: case 0x1EA1: case 0x1EB1: case 0x1EA3: case 0x00E2: case 0x1EB3:
        case 0x00E3: case 0x1EA5:case 0x1EA7: case 0x1EA9: case 0x1EAD: case 0x1EAF: case 0x1EB5: case 0x1EB7: case 0x00E4: case 0x00E5:case 0x1EAB: case 0x0103:
            return 'a';
        case 0x00E8: case 0x00E9: case 0x1EB9: case 0x1EBF: case 0x1EC1: case 0x1EBD: case 0x1EC3: case 0x00EA: case 0x1EBB:
        case 0x1EC5: case 0x1EC7: case 0x00EB:
            return 'e';
        case 0x00EC: case 0x00ED: case 0x1EC9: case 0x1ECB: case 0x0129: case 0x00EE: case 0x00EF:
            return 'i';
        case 0x00F2: case 0x00F3: case 0x1ECD: case 0x1ECF: case 0x1ED1: case 0x00F4: case 0x1ED7: case 0x1ED3: case 0x1ED5:
        case 0x00F5: case 0x1EDD: case 0x1EDF: case 0x1EE1: case 0x1EE3: case 0x00F6: case 0x01A1: case 0x1ED9: case 0x1EDB:
            return 'o';
        case 0x00F9: case 0x00FA: case 0x1EE7: case 0x1EE9: case 0x0169: case 0x01B0: case 0x1EEB: case 0x1EED:
        case 0x00FB: case 0x1EE5: case 0x1EEF: case 0x00FC: case 0x1EF1:
            return 'u';
        case 0x00FD: case 0x1EF3: case 0x1EF7: case 0x1EF9: case 0x1EF5:
            return 'y';
        case 0x0111:
            return 'd';

        // Uppercase letters
        case 0x00C0: case 0x00C1: case 0x1EA0: case 0x1EB0: case 0x1EA2: case 0x00C2: case 0x1EAC: case 0x1EB2:
        case 0x00C3: case 0x1EA6: case 0x1EA8: case 0x1EAA: case 0x1EAE: case 0x1EB4: case 0x1EB6: case 0x00C4: case 0x00C5: case 0x0102:
            return 'A';
        case 0x00C8: case 0x00C9: case 0x1EB8: case 0x1EBE: case 0x1EC0: case 0x1EBC: case 0x1EC2: case 0x00CA: case 0x1EBA:
        case 0x1EC4: case 0x1EC6: case 0x00CB: case 0x1EA4:
            return 'E';
        case 0x00CC: case 0x00CD: case 0x1EC8: case 0x1ECA: case 0x0128: case 0x00CE: case 0x00CF:
            return 'I';
        case 0x00D2: case 0x00D3: case 0x1ECC: case 0x1ED0: case 0x00D4: case 0x1ED6: case 0x1ED2: case 0x1ED4:
        case 0x00D5: case 0x1EE0: case 0x1EE2: case 0x00D6: case 0x01A0: case 0x1EDC: case 0x1EDE: case 0x1EDA: case 0x1ED8:
            return 'O';
        case 0x00D9: case 0x00DA: case 0x1EE4: case 0x1EE6: case 0x1EE8: case 0x0168: case 0x01AF: case 0x1EEA: case 0x1EEC:
        case 0x00DB: case 0x1EEE: case 0x00DC: case 0x1EF0: case 0x1ECE:
            return 'U';
        case 0x00DD: case 0x1EF2: case 0x1EF6: case 0x1EF8: case 0x1EF4:
            return 'Y';
        case 0x0110:
            return 'D';

        default:
            return '?'; // Unknown character
    }
}

// Function to convert the input UTF-8 encoded Vietnamese string to an unaccented string
void convert_to_unaccented(char *input, char *output) {
    unsigned int codepoint;
    while (*input) {
        //uint16_t codepoint;
        if ((*input & 0x80) == 0) {
            // 1-byte character (ASCII)
            codepoint = *input++;
        } else if ((*input & 0xE0) == 0xC0) {
            // 2-byte character
            codepoint = ((*input & 0x1F) << 6) | (*(input + 1) & 0x3F);
            input += 2;
        } else if ((*input & 0xF0) == 0xE0) {
            // 3-byte character
            codepoint = ((*input & 0x0F) << 12) | ((*(input + 1) & 0x3F) << 6) | (*(input + 2) & 0x3F);
            input += 3;
        } else {
            // Invalid or unsupported character
            codepoint = '?';
            input++;
        }

        //Serial.print("codepoint: ");
        //Serial.println(codepoint);

        char unaccented_char = remove_diacritic(codepoint);

        if(unaccented_char == '?')
        {
          unaccented_char = codepoint;
        }


        //Serial.print("unaccented_char: ");
        //Serial.println(unaccented_char);

        *output++ = unaccented_char;
        codepoint = 0;
    }
    *output = '\0'; // Null-terminate the output string
}
