//
//  Grid.cpp
//  SDF
//
//  Created by Nikolai Salin on 08.06.14.
//  Copyright (c) 2014 Nikolai Salin. All rights reserved.
//

#include "Grid.h"
#include <stdlib.h>

Grid::Grid(int width, int height)
{
    _width  = width;
    _height = height;
    _grid = (Point*)malloc(sizeof(Point)*width*height);
}

Grid::~Grid()
{
    free(_grid);
}

Point Grid::get(int x, int y) const
{
    return (x<0 || y<0 || x>=_width || y>=_height) ? outside : _grid[y*_width + x];
}

void  Grid::set(int x, int y,const Point & p)
{
    _grid[y*_width + x] = p;
}

void Grid::generateSDF()
{
    for (int y=0; y<_height; y++)
    {
        for (int x=0; x<_width; x++)
        {
            Point p = get(x, y);
            p = compare(p, x, y, -1,  0);
            p = compare(p, x, y,  0, -1);
            p = compare(p, x, y, -1, -1);
            p = compare(p, x, y, +1, -1);
            set(x, y, p);
        }
        
        for (int x=_width-1; x>=0; x--)
        {
            Point p = get(x, y);
            p = compare(p, x, y, +1, 0);
            set(x, y, p);
        }
    }
    
    for (int y=_height-1; y>=0; y--)
    {
        for (int x=_width-1; x>=0; x--)
        {
            Point p = get(x, y);
            p = compare(p, x, y, +1,  0);
            p = compare(p, x, y,  0, +1);
            p = compare(p, x, y, -1, +1);
            p = compare(p, x, y, +1, +1);
            set(x, y, p);
        }
        
        for (int x=0; x<_width; x++)
        {
            Point p = get(x, y);
            p = compare(p, x, y, -1,  0);
            set(x, y, p);
        }
    }
}

Point Grid::compare(const Point & p, int x, int y, int dx, int dy) const
{
    Point other = get(x+dx, y+dy);
    other.dx += dx;
    other.dy += dy;
    
    return (other.getDistSq() < p.getDistSq()) ? other : p;
}
