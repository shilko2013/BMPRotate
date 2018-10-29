#include <malloc.h>
#include <string.h>
#include "bmp.h"

static enum read_status read_bmp_header(FILE *file, struct bmp_header *header) {

    if (!file) return READ_INVALID_FILE;

    if (fread(&header->file, sizeof(header->file), 1, file) != 1) return READ_INVALID_FILE;

    if (header->file.bfType != 0x4D42) return READ_INVALID_BITMAP_FILE_HEADER;

    if (header->file.bfReserved) return READ_INVALID_BITMAP_FILE_HEADER;

    if (fread(&header->info, sizeof(header->info), 1, file) != 1) return READ_INVALID_FILE;

    if (header->info.biSize != 0x28) return READ_FILE_UNSUPPORTED_VERSION;

    if (header->info.biWidth < 1 || header->info.biHeight < 1) return READ_INVALID_BITMAP_INFO_HEADER;

    if (header->info.biPlanes != 1) return READ_INVALID_BITMAP_INFO_HEADER;

    if (header->info.biBitCount != 0x18) return READ_FILE_UNSUPPORTED_VERSION;

    if (header->info.biCompression) return READ_FILE_UNSUPPORTED_VERSION;

    return READ_OK;
}

struct pixel *image_get(
        struct image *img,
        uint64_t x,
        uint64_t y
) {
    return img->data + y * img->width + x;
}

struct image image_create(
        uint64_t width,
        uint64_t height
) {
    return (struct image) {
            .width = width,
            .height = height,
            .data = malloc(sizeof(struct pixel) * width * height)
    };
}


void image_destroy(struct image *img) {
    img->width = 0;
    img->height = 0;
    free(img->data);
}


static uint64_t bmp_padding(uint64_t width) { return width % 4; }

enum read_status bmp_from_file(FILE *file, struct image *const img) {

    if (!file)
        return READ_NULL_PTR_FILE;

    struct bmp_header header;

    const enum read_status read_header_stat = read_bmp_header(file, &header);
    if (read_header_stat != READ_OK) return read_header_stat;

    fseek(file, header.file.bfOffBits, SEEK_SET);

    *img = image_create(header.info.biWidth, header.info.biHeight);

    const uint64_t padding = bmp_padding(img->width);

    for (uint64_t i = 0; i < img->height; i++)
        if (fread(image_get(img, 0, i),
                  img->width * sizeof(struct pixel),
                  1,
                  file
        )) {
            fseek(file, padding, SEEK_CUR);
        } else {
            image_destroy(img);
            return READ_INVALID_FILE;
        }
// fopen( "w" ); -> "wb"
    return READ_OK;
}

static struct bmp_header bmp_header_generate(
        const struct image *img
) {
    struct bmp_header header;
    header.file = (struct bmp_file_header) {
            .bfType=0x4D42,
            .bfReserved = 0,
            .bfOffBits = sizeof(struct bmp_header),
            .bfSize = (uint32_t) (sizeof(struct bmp_header) + (bmp_padding(img->width) + img->width * 3) * img->height)
    };
    header.info = (struct bmp_info_header) {0};
    header.info.biSizeImage = (uint32_t) ((bmp_padding(img->width) + img->width * 3) * img->height);
    header.info.biSize = 0x28;
    header.info.biWidth = (uint32_t) (img->width);
    header.info.biHeight = (uint32_t) (img->height);
    header.info.biPlanes = 1;
    header.info.biBitCount = 0x18;
    return header;
}

enum write_status bmp_to_file(FILE *file, struct image *const img) {
    if (!file) return WRITE_INVALID_FILE;
    if (!img) return WRITE_NULL_PTR_IMAGE;


    struct bmp_header header = bmp_header_generate(img);
    fwrite(&header, sizeof(header), 1, file);
    const uint64_t padding = bmp_padding(img->width);

    uint64_t stub = 0;

    for (uint64_t i = 0; i < img->height; i++)
        if (!fwrite(image_get(img, 0, i),
                    img->width * sizeof(struct pixel),
                    1,
                    file
        )
            ||
            (padding && !fwrite(&stub, padding, 1, file)))
            return WRITE_INVALID_FILE;

    return WRITE_OK;
}

static void swap_pixel(struct pixel *left, struct pixel *right) {
    struct pixel *temp = malloc(sizeof(struct pixel));
    memcpy(temp, left, sizeof(struct pixel));
    memcpy(left, right, sizeof(struct pixel));
    memcpy(right, temp, sizeof(struct pixel));
    free(temp);
}

int rotate180(struct image *img) {
    if (!img)
        return 0;
    for (uint64_t i = 0; i < img->height / 2; ++i)
        for (uint64_t j = 0; j < img->width; ++j)
            swap_pixel(image_get(img, i, j), image_get(img, img->height - i - 1, img->width - j - 1));
    return 1;
}

int rotate90(struct image *img) {
    if (!img)
        return 0;
    struct pixel *temp = malloc(sizeof(struct pixel));
    for (uint64_t i = 0; i < img->height / 2; ++i)
        for (uint64_t j = 0; j < img->width / 2; ++j) {
            memcpy(temp, image_get(img, i, j), sizeof(struct pixel));
            memcpy(image_get(img, i, j), image_get(img, j, img->height - i - 1), sizeof(struct pixel));
            memcpy(image_get(img, j, img->height - i - 1), image_get(img, img->height - i - 1, img->width - j - 1),
                   sizeof(struct pixel));
            memcpy(image_get(img, img->height - i - 1, img->width - j - 1), image_get(img, img->width - j - 1, i),
                   sizeof(struct pixel));
            memcpy(image_get(img, img->width - j - 1, i), temp, sizeof(struct pixel));
        }
    free(temp);
    return 1;
}