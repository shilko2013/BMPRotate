#include <stdio.h>
#include "bmp.h"

int main(void) {

    FILE *f = fopen("VT.bmp", "rb");
    image *img = malloc(sizeof(image));
    printf("%d", bmp_from_file(f, img));

    return 0;
}