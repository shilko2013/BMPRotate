#include <stdio.h>
#include <malloc.h>
#include "bmp.h"

int main(void) {

    FILE *f;
    struct image *img = malloc(sizeof(struct image));
    f = fopen("1.bmp", "rb");
    printf("%d", bmp_from_file(f, img));
    img = rotate_corner(img, 30);
    if (f)
        fclose(f);
    f = fopen("2.bmp", "wb");
    printf("%d", bmp_to_file(f, img));
    if (f)
        fclose(f);
    image_destroy(img);
    free(img);

    return 0;
}