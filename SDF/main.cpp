/*
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 * https://gist.github.com/niw/5963798
 *
 * Modified by Nikolai Salin to generate signed distance field.
 * Using “The eight-points signed sequential Euclidean distance 
 * transform algorithm,” or 8SSEDT for short.
 *
 * To build, use the next instruction on OS X.
 * $ brew install libpng
 * $ clang -lz -lpng15 libpng_test.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "PNGImage.h"
#include "Grid.h"

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("Usage: SDF <image_in> <image_out> <spread>\n");
        return 0;
    }
    
    int spread = atoi(argv[3]);
    
    if (spread < 1)
    {
        printf("Spread must be > 1\n");
        return 0;
    }
    
    PNGImage srcImg;
    // loading input image
    printf("Loading source image\n");
    srcImg.load(argv[1]);
    
    PNGImage destImg(srcImg.getWidth() + spread*2,
                     srcImg.getHeight()+ spread*2);
    
    destImg.paste(srcImg, spread, spread);
    
    // Create grid
    Grid grid1(destImg.getWidth(), destImg.getHeight());
    Grid grid2(destImg.getWidth(), destImg.getHeight());
    
    // init grid
    printf("Initialize grids\n");
    for (int y=0; y<destImg.getHeight(); y++)
    {
        for (int x=0; x<destImg.getWidth(); x++)
        {
            if (destImg.getColor(x, y).a < 128)
            {
                grid1.set(x, y, inside);
                grid2.set(x, y, outside);
            }
            else
            {
                grid1.set(x, y, outside);
                grid2.set(x, y, inside);
            }
        }
    }
    
    // generate SDF
    printf("Generating SDF\n");
    grid1.generateSDF();
    grid2.generateSDF();
    
    printf("Calculating final pixels\n");
    const double minDist = -spread;
    const double maxDist =  spread;
    for (int y=0; y<destImg.getHeight(); y++)
    {
        for (int x=0; x<destImg.getWidth(); x++)
        {
            double dist1 = sqrt((double)grid1.get(x, y).getDistSq());
            double dist2 = sqrt((double)grid2.get(x, y).getDistSq());
            double distRes = dist1-dist2;
            
            // normalize
            if (distRes < 0)
                distRes = -128 * (distRes - minDist) / minDist;
            else
                distRes = 128 + 128 * distRes / maxDist;
            
            // clamp
            if (distRes < 0)
                distRes = 0;
            else if (distRes > 255)
                distRes = 255;
            
            Color col = {(png_byte)distRes,(png_byte)distRes,(png_byte)distRes,(png_byte)distRes};
            destImg.setColor(x, y, col);
        }
    }
    
    // saving result
    printf("Saving result\n");
    destImg.save(argv[2]);
    
    printf("Done!\n");
    
    return 0;
}