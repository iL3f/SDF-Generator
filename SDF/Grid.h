//
//  Grid.h
//  SDF
//
//  Created by Nikolai Salin on 08.06.14.
//  Copyright (c) 2014 Nikolai Salin. All rights reserved.
//

#ifndef __SDF__Grid__
#define __SDF__Grid__

struct Point
{
    int dx, dy;
    
	int getDistSq() const
    {
        return dx*dx + dy*dy;
    }
};

const Point inside  = {0,0};
const Point outside = {9999,9999};

// 
class Grid
{
    Grid(){}
    Grid(const Grid&){}
public:
    Grid(int width, int height);
    ~Grid();
    
    Point get(int x, int y) const;
    void  set(int x, int y, const Point & p);
    void  generateSDF();
private:
    Point compare(const Point & p, int x, int y, int dx, int dy) const;
    
    Point * _grid;
    int _width, _height;
};


#endif /* defined(__SDF__Grid__) */
