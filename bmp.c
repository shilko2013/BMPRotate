#include <stdint.h>
#include <stdio.h>
#include <malloc.h>

#pragma pack(push, 2)
struct _BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint32_t bfReserved;
    uint32_t bfOffBits;
};
struct _BITMAPINFOHEADER {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
struct _pixel {
    uint8_t r, g, b;
};
#pragma pack(pop)

typedef struct _pixel pixel;

typedef struct {
    uint64_t width, height;
    pixel **data;
} image;

typedef struct _BITMAPFILEHEADER BITMAPFILEHEADER;
typedef struct _BITMAPINFOHEADER BITMAPINFOHEADER;

typedef enum {
    READ_OK = 0,
    READ_INVALID_BITMAP_FILE_HEADER,
    READ_INVALID_BITMAP_INFO_HEADER,
    READ_INVALID_FILE,
    READ_ALLOCATE_MEMORY_ERROR,
    READ_FILE_UNSUPPORTED_VERSION
} read_status;

typedef enum {
    WRITE_OK = 0,
    WRITE_INVALID_FILE,
    WRITE_NULL_PTR_IMAGE
} write_status;

read_status bmp_from_file(FILE *file, image *const img) {
    if (!file)
        return READ_INVALID_FILE;
    BITMAPFILEHEADER *bitmapfileheader = malloc(sizeof(BITMAPFILEHEADER));
    if (!bitmapfileheader)
        return READ_ALLOCATE_MEMORY_ERROR;
    if (fread(bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, file) != 1)
        return READ_INVALID_FILE;
    if (bitmapfileheader->bfType != 0x4D42)
        return READ_INVALID_BITMAP_FILE_HEADER;
    if (bitmapfileheader->bfReserved)
        return READ_INVALID_BITMAP_FILE_HEADER;
    BITMAPINFOHEADER *bitmapinfoheader = malloc(sizeof(BITMAPINFOHEADER));
    if (!bitmapinfoheader)
        return READ_ALLOCATE_MEMORY_ERROR;
    if (fread(bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, file) != 1)
        return READ_INVALID_FILE;
    if (bitmapinfoheader->biSize != 0x28)
        return READ_FILE_UNSUPPORTED_VERSION;
    if (bitmapinfoheader->biWidth < 1 || bitmapinfoheader->biHeight < 1)
        return READ_INVALID_BITMAP_INFO_HEADER;
    img->width = bitmapinfoheader->biWidth;
    img->height = bitmapinfoheader->biHeight;
    if (bitmapinfoheader->biPlanes != 1)
        return READ_INVALID_BITMAP_INFO_HEADER;
    if (bitmapinfoheader->biBitCount != 8)
        return READ_FILE_UNSUPPORTED_VERSION;
    if (bitmapinfoheader->biCompression)
        return READ_FILE_UNSUPPORTED_VERSION;
    fseek(file, bitmapfileheader->bfOffBits, SEEK_SET);
    uint64_t sizeBits = 3 * img->width * img->height + img->width % 4 * img->width;
    uint8_t *buffer_pixel = malloc(sizeBits);
    /*free(bitmapfileheader);
    free(bitmapinfoheader);*/
    if (!buffer_pixel)
        return READ_ALLOCATE_MEMORY_ERROR;
    if (fread(buffer_pixel, sizeof(buffer_pixel), 1, file) != 1)
        return READ_INVALID_FILE;
    pixel **pixels = malloc(sizeof(pixel *) * img->width);
    if (!pixels)
        return READ_ALLOCATE_MEMORY_ERROR;
    for (uint64_t i = 0; i < img->width; ++i) {
        pixels[i] = malloc(sizeof(pixel) * img->height);
        if (!pixels[i])
            return READ_ALLOCATE_MEMORY_ERROR;
    }
    uint64_t offset = img->width % 4;
    uint64_t pointer = 0;
    for (uint64_t i = img->width - 1; i > 0; --i) {
        for (uint64_t j = 0; j < img->height; ++j) {
            pixels[i][j].r = buffer_pixel[pointer++];
            pixels[i][j].g = buffer_pixel[pointer++];
            pixels[i][j].b = buffer_pixel[pointer++];
        }
        pointer += offset;
    }
    free(buffer_pixel);
    img->data = pixels;

    file = fopen("VTA.bmp", "w");
    bitmapfileheader->bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    fwrite(bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, file);
    fwrite(buffer_pixel, sizeBits, 1, file);
    fclose(file);

    return READ_OK;
}

write_status bmp_to_file(FILE *file, image *const img) {
    if (!file)
        return WRITE_INVALID_FILE;
    if (!img)
        return WRITE_NULL_PTR_IMAGE;
    BITMAPFILEHEADER* bitmapfileheader = malloc(sizeof(BITMAPFILEHEADER));
    bitmapfileheader->bfType = 0x4D42;
    bitmapfileheader->bfSize = 3 * img->width * img->height + img->width % 4 * img->width; //+ ; TODO
}