//
//  PNGImage.cpp
//  SDF
//
//  Created by Nikolai Salin on 08.06.14.
//  Copyright (c) 2014 Nikolai Salin. All rights reserved.
//

#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include "PNGImage.h"

static void assert(bool cond, const char * format, ...)
{
    if (!cond)
    {
        va_list args;
        va_start (args, format);
        printf("Error:");
        vprintf (format, args);
        printf("\n");
        va_end (args);
        exit(1);
    }
}

PNGImage::PNGImage()
{
    _width = 0;
    _height = 0;
    _rowPointers = 0;
}

PNGImage::PNGImage(int width, int height)
{
    assert(width>0 && height>0, "Wrong size for PNGImage (%d,%d)");
    
    _width  = width;
    _height = height;
    
    _rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (int y=0; y<height; y++)
    {
        _rowPointers[y] = (png_byte*)malloc(sizeof(png_byte) * 4 * width);
        memset(_rowPointers[y], 0, width*4);
    }
}

PNGImage::~PNGImage()
{
    clean();
}

void PNGImage::load(const char * filename)
{
    clean();
    
    // Read PNG data
    
    FILE *fp = fopen(filename, "rb");
    assert(fp, "Can't open file \"%s\" for read", filename);
    
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png, "Can't create PNG read struct");
    
    png_infop info = png_create_info_struct(png);
    assert(info, "Can't create PNG info struct");
    
    assert(!setjmp(png_jmpbuf(png)),"Can't set PNG jmp");
    
    png_init_io(png, fp);
    
    png_read_info(png, info);
    
    _width     = png_get_image_width(png, info);
    _height    = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth  = png_get_bit_depth(png, info);
    
    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    
    if(bit_depth == 16)
        png_set_strip_16(png);
    
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    
    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    
    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    
    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    
    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    
    png_read_update_info(png, info);
    
    _rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * _height);
    for(int y = 0; y < _height; y++)
    {
        _rowPointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    
    png_read_image(png, _rowPointers);
    
    fclose(fp);
}

void PNGImage::save(const char * filename)
{
    assert(_rowPointers, "Nothing to write");
    
    FILE *fp = fopen(filename, "wb");
    assert(fp, "Can't open file \"%s\" for write", filename);
    
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png, "Can't create PNG write struct");
    
    png_infop info = png_create_info_struct(png);
    assert(info, "Can't create PNG info struct");
    
    assert(!setjmp(png_jmpbuf(png)),"Can't set PNG jmp");
    
    png_init_io(png, fp);
    
    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
                 png,
                 info,
                 _width, _height,
                 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT
                 );
    png_write_info(png, info);
    
    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);
    
    png_write_image(png, _rowPointers);
    png_write_end(png, NULL);
    
    fclose(fp);
}

void PNGImage::paste(const PNGImage & img, int offsetX, int offsetY)
{
    const int wMax = img.getWidth()+offsetX < _width ? img.getWidth()+offsetX : _width;
    const int hMax = img.getHeight()+offsetY < _height ? img.getHeight()+offsetY : _height;
    for (int y = offsetY; y<hMax; y++)
    {
        for (int x = offsetX; x<wMax; x++)
        {
            Color col = img.getColor(x-offsetX, y-offsetY);
            setColor(x, y, col);
        }
    }
}

Color PNGImage::getColor(int x, int y) const
{
    assert(x>=0 && x<_width && y>=0 && y<_height,
           "Wrong coords (%d,%d) for get color, img size (%d,%d)",_width, _height, x,y);
    
    png_bytep row = _rowPointers[y];
    png_bytep px = &(row[x * 4]);
    
    Color ret = {px[0], px[1], px[2], px[3]};
    return ret;
}

void  PNGImage::setColor(int x, int y, const Color & c)
{
    assert(x>=0 && x<_width && y>=0 && y<_height,
           "Wrong coords (%d,%d) for get color, img size (%d,%d)",_width, _height, x,y);
    
    png_bytep row = _rowPointers[y];
    png_bytep px = &(row[x * 4]);
    
    px[0] = c.r;
    px[1] = c.g;
    px[2] = c.b;
    px[3] = c.a;
}

int   PNGImage::getWidth() const
{
    return _width;
}

int   PNGImage::getHeight() const
{
    return _height;
}

void  PNGImage::clean()
{
    if (_rowPointers)
    {
        for (int y=0; y<_height; y++)
        {
            free(_rowPointers[y]);
        }
        free(_rowPointers);
        _rowPointers = 0;
    }
}