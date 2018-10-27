#include <stdio.h>
#include <malloc.h>
#include "bmp.h"

int main(void) {

    FILE *f = fopen("VT.bmp", "rb");
    struct image *img = malloc(sizeof(struct image));
    printf("%d", bmp_from_file(f, img));
    fclose(f);
    rotate180(img);
    f = fopen("VT180.bmp","wb");
    printf("%d", bmp_to_file(f, img));
    rotate90(img);
    f = fopen("VT270.bmp","wb");
    printf("%d", bmp_to_file(f, img));
    fclose(f);
    return 0;
}