#include "bmp.c"

read_status bmp_from_file(FILE *file, image *const read);

void rotate(image *const img);

write_status bmp_to_file(FILE *file, image *const img);