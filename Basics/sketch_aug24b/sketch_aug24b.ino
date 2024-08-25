#include <Arduino.h>
#include "convertUnaccent.h"

// Assuming remove_diacritic and convert_to_unaccented functions are defined above

void test_unaccented_conversion(char *unicode_str) {
    char unaccented_str[100];
    convert_to_unaccented(unicode_str, unaccented_str);
    Serial.println(unaccented_str);
}

void setup() {
    Serial.begin(115200);

    Serial.println("\nConvert Vietnamese unicode strings to unaccented strings");
    Serial.println("Author: daonamthai@gmail.com");

    // Lowercase vowels
    char lowercase_a[] = "àáạảãâầấậẩẫăằắặẳẵ";
    char lowercase_e[] = "èéẹẻẽêềếệểễ";
    char lowercase_i[] = "ìíịỉĩ";
    char lowercase_o[] = "òóọỏõôồốộổỗơờớợởỡ";
    char lowercase_u[] = "ùúụủũưừứựửữ";
    char lowercase_y[] = "ỳýỵỷỹ";

    // Lowercase consonant
    char lowercase_d[] = "đ";

    // Uppercase vowels
    char uppercase_A[] = "ÀÁẠẢÃÂẦẤẬẨẪĂẰẮẶẲẴ";
    char uppercase_E[] = "ÈÉẸẺẼÊỀẾỆỂỄ";
    char uppercase_I[] = "ÌÍỊỈĨ";
    char uppercase_O[] = "ÒÓỌỎÕÔỒỐỘỔỖƠỜỚỢỞỠ";
    char uppercase_U[] = "ÙÚỤỦŨƯỪỨỰỬỮ";
    char uppercase_Y[] = "ỲÝỴỶỸ";

    // Uppercase consonant
    char uppercase_D[] = "Đ";

    // Random quote
    char uppercase_random[] = "Không có gì quý hơn độc lập tự do";

    // Testing lowercase vowels
    Serial.print("Original lowercase   a: ");
    Serial.println(lowercase_a);
    Serial.print("Unaccented lowercase a: ");
    test_unaccented_conversion(lowercase_a);

    Serial.print("Original lowercase   e: ");
    Serial.println(lowercase_e);
    Serial.print("Unaccented lowercase e: ");
    test_unaccented_conversion(lowercase_e);

    Serial.print("Original lowercase   i: ");
    Serial.println(lowercase_i);
    Serial.print("Unaccented lowercase i: ");
    test_unaccented_conversion(lowercase_i);
    
    Serial.print("Original lowercase   o: ");
    Serial.println(lowercase_o);
    Serial.print("Unaccented lowercase o: ");
    test_unaccented_conversion(lowercase_o);
    
    Serial.print("Original lowercase   u: ");
    Serial.println(lowercase_u);
    Serial.print("Unaccented lowercase u: ");
    test_unaccented_conversion(lowercase_u);
    
    Serial.print("Original lowercase   y: ");
    Serial.println(lowercase_y);
    Serial.print("Unaccented lowercase y: ");
    test_unaccented_conversion(lowercase_y);

    // Testing lowercase consonant
    Serial.print("Original lowercase   d: ");
    Serial.println(lowercase_d);
    Serial.print("Unaccented lowercase d: ");
    test_unaccented_conversion(lowercase_d);

    // Testing uppercase vowels
    Serial.print("Original uppercase   A: ");
    Serial.println(uppercase_A);
    Serial.print("Unaccented uppercase A: ");
    test_unaccented_conversion(uppercase_A);

    Serial.print("Original uppercase   E: ");
    Serial.println(uppercase_E);  
    Serial.print("Unaccented uppercase E: ");
    test_unaccented_conversion(uppercase_E);

    Serial.print("Original uppercase   I: ");
    Serial.println(uppercase_I);
    Serial.print("Unaccented uppercase I: ");
    test_unaccented_conversion(uppercase_I);

    Serial.print("Original uppercase   O: ");
    Serial.println(uppercase_O);
    Serial.print("Unaccented uppercase O: ");
    test_unaccented_conversion(uppercase_O);
    
    Serial.print("Original uppercase   U: ");
    Serial.println(uppercase_U);
    Serial.print("Unaccented uppercase U: ");
    test_unaccented_conversion(uppercase_U);
    
    Serial.print("Original uppercase   Y: ");
    Serial.println(uppercase_Y);
    Serial.print("Unaccented uppercase Y: ");
    test_unaccented_conversion(uppercase_Y);

    // Testing uppercase consonant
    Serial.print("Original uppercase   D: ");
    Serial.println(uppercase_D);
    Serial.print("Unaccented uppercase D: ");
    test_unaccented_conversion(uppercase_D);

    // Testing random quote
    Serial.print("Random quote          : ");
    Serial.println(uppercase_random);
    Serial.print("Unaccented  rand quote: ");
    test_unaccented_conversion(uppercase_random);
}

void loop() {
    // Nothing to do here
}
