#include <string.h>

#define BYTECONV_WIDTH 4
// Táblázat oszlopai.
// ascii kód, unicode1, unicode2, terminátor
// Ha több oszlopra van szükség, akkor csak növelni kell a
// BYTECONV_WIDTH makró értékét és ki kell egészíteni a táblázatot
static unsigned char byteconv_table[] = {
    128, 0xc2, 0x80, 0,
    129, 0xc2, 0x81, 0,
    130, 0xc2, 0x82, 0,
    131, 0xc2, 0x83, 0,
    132, 0xc2, 0x84, 0,
    133, 0xc2, 0x85, 0,
    134, 0xc2, 0x86, 0,
    135, 0xc2, 0x87, 0,
    136, 0xc2, 0x88, 0,
    137, 0xc2, 0x89, 0,
    138, 0xc2, 0x8a, 0,
    139, 0xc2, 0x8b, 0,
    140, 0xc2, 0x8c, 0,
    141, 0xc2, 0x8d, 0,
    142, 0xc2, 0x8e, 0,
    143, 0xc2, 0x8f, 0,
    144, 0xc2, 0x90, 0,
    145, 0xc2, 0x91, 0,
    146, 0xc2, 0x92, 0,
    147, 0xc2, 0x93, 0,
    148, 0xc2, 0x94, 0,
    149, 0xc2, 0x95, 0,
    150, 0xc2, 0x96, 0,
    151, 0xc2, 0x97, 0,
    152, 0xc2, 0x98, 0,
    153, 0xc2, 0x99, 0,
    154, 0xc2, 0x9a, 0,
    155, 0xc2, 0x9b, 0,
    156, 0xc2, 0x9c, 0,
    157, 0xc2, 0x9d, 0,
    158, 0xc2, 0x9e, 0,
    159, 0xc2, 0x9f, 0,
    160, 0xc2, 0xa0, 0,
    161, 0xc2, 0xa1, 0,
    162, 0xc2, 0xa2, 0,
    163, 0xc2, 0xa3, 0,
    164, 0xc2, 0xa4, 0,
    165, 0xc2, 0xa5, 0,
    166, 0xc2, 0xa6, 0,
    167, 0xc2, 0xa7, 0,
    168, 0xc2, 0xa8, 0,
    169, 0xc2, 0xa9, 0,
    170, 0xc2, 0xaa, 0,
    171, 0xc2, 0xab, 0,
    172, 0xc2, 0xac, 0,
    173, 0xc2, 0xad, 0,
    174, 0xc2, 0xae, 0,
    175, 0xc2, 0xaf, 0,
    176, 0xc2, 0xb0, 0,
    177, 0xc2, 0xb1, 0,
    178, 0xc2, 0xb2, 0,
    179, 0xc2, 0xb3, 0,
    180, 0xc2, 0xb4, 0,
    181, 0xc2, 0xb5, 0,
    182, 0xc2, 0xb6, 0,
    183, 0xc2, 0xb7, 0,
    184, 0xc2, 0xb8, 0,
    185, 0xc2, 0xb9, 0,
    186, 0xc2, 0xba, 0,
    187, 0xc2, 0xbb, 0,
    188, 0xc2, 0xbc, 0,
    189, 0xc2, 0xbd, 0,
    190, 0xc2, 0xbe, 0,
    191, 0xc2, 0xbf, 0,
    192, 0xc3, 0x80, 0,
    193, 0xc3, 0x81, 0,
    194, 0xc3, 0x82, 0,
    195, 0xc3, 0x83, 0,
    196, 0xc3, 0x84, 0,
    197, 0xc3, 0x85, 0,
    198, 0xc3, 0x86, 0,
    199, 0xc3, 0x87, 0,
    200, 0xc3, 0x88, 0,
    201, 0xc3, 0x89, 0,
    202, 0xc3, 0x8a, 0,
    203, 0xc3, 0x8b, 0,
    204, 0xc3, 0x8c, 0,
    205, 0xc3, 0x8d, 0,
    206, 0xc3, 0x8e, 0,
    207, 0xc3, 0x8f, 0,
    208, 0xc3, 0x90, 0,
    209, 0xc3, 0x91, 0,
    210, 0xc3, 0x92, 0,
    211, 0xc3, 0x93, 0,
    212, 0xc3, 0x94, 0,
    213, 0xc3, 0x95, 0,
    214, 0xc3, 0x96, 0,
    215, 0xc3, 0x97, 0,
    216, 0xc3, 0x98, 0,
    217, 0xc3, 0x99, 0,
    218, 0xc3, 0x9a, 0,
    219, 0xc3, 0x9b, 0,
    220, 0xc3, 0x9c, 0,
    221, 0xc3, 0x9d, 0,
    222, 0xc3, 0x9e, 0,
    223, 0xc3, 0x9f, 0,
    224, 0xc3, 0xa0, 0,
    225, 0xc3, 0xa1, 0,
    226, 0xc3, 0xa2, 0,
    227, 0xc3, 0xa3, 0,
    228, 0xc3, 0xa4, 0,
    229, 0xc3, 0xa5, 0,
    230, 0xc3, 0xa6, 0,
    231, 0xc3, 0xa7, 0,
    232, 0xc3, 0xa8, 0,
    233, 0xc3, 0xa9, 0,
    234, 0xc3, 0xaa, 0,
    235, 0xc3, 0xab, 0,
    236, 0xc3, 0xac, 0,
    237, 0xc3, 0xad, 0,
    238, 0xc3, 0xae, 0,
    239, 0xc3, 0xaf, 0,
    240, 0xc3, 0xb0, 0,
    241, 0xc3, 0xb1, 0,
    242, 0xc3, 0xb2, 0,
    243, 0xc3, 0xb3, 0,
    244, 0xc3, 0xb4, 0,
    245, 0xc3, 0xb5, 0,
    246, 0xc3, 0xb6, 0,
    247, 0xc3, 0xb7, 0,
    248, 0xc3, 0xb8, 0,
    249, 0xc3, 0xb9, 0,
    250, 0xc3, 0xba, 0,
    251, 0xc3, 0xbb, 0,
    252, 0xc3, 0xbc, 0,
    253, 0xc3, 0xbd, 0,
    254, 0xc3, 0xbe, 0,
    255, 0xc3, 0xbf, 0,
    251, 197, 177, 0, // ű
    245, 197, 145, 0, // ő
    219, 197, 176, 0, // Ű
    213, 197, 144, 0, // Ő
    //~ 160, 0xc2, 0xa0, 0, 0,
    //~ 161, 0xc2, 0xa1, 0, 0,
    //~ 162, 0xc2, 0xa2, 0, 0,
    //~ 163, 0xc2, 0xa9, 0, 0,
    //~ 192, 0xc3, 0x80, 0x7f, 0,
    //~ 193, 0xc3, 0x81, 0, 0,
    //~ 65, 96, 98, 99, 0,
    //~ 66, 97, 99, 99, 0,
    //~ 67, 97, 98, 99, 0,
    //~ 68, 97, 98, 100, 0,
    0,
};

static unsigned char byteconv_temp[BYTECONV_WIDTH];
static int byteconv_n = 0;

static int byteconv (char from, char *to) {
    int i, a;
//~ printf("from = %d\n", (unsigned char)from);
    for (i = 0; byteconv_table[i]; i = i + BYTECONV_WIDTH) {
        unsigned char item = byteconv_table[i + byteconv_n + 1];
//~ printf("item = %d\n", item);
        if (item == (unsigned char)from) {
            int not = 0;
            for (a = 0; a < byteconv_n; a++) {
                unsigned char ca = byteconv_table[i + a + 1];
                unsigned char cb = byteconv_temp[a];
                if (ca != cb)
                    not = 1;
            }
//~ printf("not = %d\n", not);
            if (!not) {
                if (byteconv_table[i + byteconv_n + 2] == 0) {
                    to[0] = byteconv_table[i];
                    to[1] = 0;
                    goto end;
                } else {
                    byteconv_temp[byteconv_n] = item;
                    byteconv_n++;
                    to[0] = 0;
                    return 0;
                }
            }
        }
        
    }
    if (!byteconv_n) {
        to[0] = from;
        to[1] = 0;
    } else {
        to = "?";
    }


end:
    for (i = 0; i < BYTECONV_WIDTH; i++) {
        byteconv_temp[i] = 0;
        byteconv_n = 0;
    }
    return 1;
}

// UTF8-ból ISO-8859-2-be konvertál. Az eredményt a to paraméter által
// jelölt memóriaterületre menti. 
int stringconv (char *from, int from_len, char *to, int to_len) {
    int i;
    int l;
    int len = 0;
    char temp[8];
    
    for (i = 0; i < to_len; i++)
        to[i] = 0;

    for (i = 0; i < from_len; i++) {
        l = byteconv(from[i], temp);
//~ printf("%c %d %s %d\n", from[i], (unsigned char)from[i], temp, l);
        len += l;
        if (len > to_len)
            return 0;
        if (l)
            strncat(to, temp, 1);
    }
    return 0;
}

//~ int main (int argc, char **argv) {
    //~ char str[80];
    //~ char *src = "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP";
    //~ 
    //~ stringconv(src, strlen(src), str, 80);
    //~ 
    //~ printf("result: %s\n", str);
//~ 
    //~ return 0;
//~ }

