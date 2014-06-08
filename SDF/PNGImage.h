//
//  PNGImage.h
//  SDF
//
//  Created by Nikolai Salin on 08.06.14.
//  Copyright (c) 2014 Nikolai Salin. All rights reserved.
//

#ifndef __SDF__PNGImage__
#define __SDF__PNGImage__

#include <png.h>

struct Color
{
    png_byte r,g,b,a;
};

class PNGImage
{
public:
    PNGImage();
    PNGImage(int width, int height);
    ~PNGImage();
    
    void load(const char * filename);
    void save(const char * filename);
    void paste(const PNGImage & img, int offsetX, int offsetY);
    
    Color getColor(int x, int y) const;
    void  setColor(int x, int y, const Color & c);
    
    int   getWidth() const;
    int   getHeight() const;
    
private:
    void  clean();
    
    int _width;
    int _height;
    png_bytep * _rowPointers;
};

#endif /* defined(__SDF__PNGImage__) */
