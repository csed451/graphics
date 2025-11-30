#include "core/render/texture.h"

#include <cstdio>
#include <vector>
#include <iostream>

#include <png.h>

namespace {
struct PngDeleter {
    png_structp pngPtr = nullptr;
    png_infop infoPtr = nullptr;
    PngDeleter(png_structp p, png_infop i) : pngPtr(p), infoPtr(i) {}
    ~PngDeleter() {
        if (pngPtr || infoPtr)
            png_destroy_read_struct(pngPtr ? &pngPtr : nullptr,
                                    infoPtr ? &infoPtr : nullptr,
                                    nullptr);
    }
};
}

Texture2D load_texture_png(const std::string& path, bool generateMips, bool flipY) {
    FILE* fp = std::fopen(path.c_str(), "rb");
    if (!fp) {
        std::cerr << "[Texture] Failed to open " << path << '\n';
        return {};
    }

    png_byte header[8];
    if (std::fread(header, 1, 8, fp) != 8 || png_sig_cmp(header, 0, 8)) {
        std::cerr << "[Texture] " << path << " is not a PNG\n";
        std::fclose(fp);
        return {};
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngPtr) {
        std::fclose(fp);
        return {};
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr) {
        png_destroy_read_struct(&pngPtr, nullptr, nullptr);
        std::fclose(fp);
        return {};
    }

    if (setjmp(png_jmpbuf(pngPtr))) {
        std::cerr << "[Texture] libpng error while reading " << path << '\n';
        std::fclose(fp);
        png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
        return {};
    }

    PngDeleter guard(pngPtr, infoPtr);
    png_init_io(pngPtr, fp);
    png_set_sig_bytes(pngPtr, 8);
    png_read_info(pngPtr, infoPtr);

    png_uint_32 width, height;
    int bitDepth, colorType;
    png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr);

    // Expand palette/gray to RGB, ensure 8-bit channels, add alpha if missing.
    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pngPtr);
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(pngPtr);
    if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(pngPtr);
    if (bitDepth == 16)
        png_set_strip_16(pngPtr);
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pngPtr);
    if (colorType == PNG_COLOR_TYPE_RGB)
        png_set_filler(pngPtr, 0xFF, PNG_FILLER_AFTER);

    png_read_update_info(pngPtr, infoPtr);
    png_size_t rowBytes = png_get_rowbytes(pngPtr, infoPtr);

    std::vector<png_byte> data(rowBytes * height);
    std::vector<png_bytep> rows(height);
    for (size_t y = 0; y < height; ++y) {
        size_t rowIndex = flipY ? (height - 1 - y) : y;
        rows[y] = data.data() + rowIndex * rowBytes;
    }

    png_read_image(pngPtr, rows.data());
    std::fclose(fp);

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (generateMips)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    Texture2D out;
    out.id = tex;
    out.width = static_cast<int>(width);
    out.height = static_cast<int>(height);
    return out;
}
