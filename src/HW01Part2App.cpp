/**
 * @file HW01Part2App.cpp
 * CSE 274 - Fall 2012
 *
 * @author Jordan Komnick
 * @date 2012-09-05
 *
 * @note This file is (c) 2012. It is licensed under the 
 * CC BY 3.0 license (http://creativecommons.org/licenses/by/3.0/),
 * which means you are free to use, share, and remix it as long as you
 * give attribution. Commercial uses are allowed.
 *
 * @note Some code and theories come from https://github.com/brinkmwj/HW01
 *
 * @note This project satisfies goals A.1 (rectangle), A.2 (circle), A.3 (line), A.6 (tint), B.1 (blur), E.5 (animation), and E.6 (mouse interaction)
 */

/* EDITED AND REVIEWED BY BRANDON SONODA */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class HW01Part2App : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);
	void drawCircle(uint8_t* pixels, int center_x, int center_y, int radius, Color8u color);
	void drawRect(uint8_t* pixels, int corner_x, int corner_y, int width, int height, Color8u color);
	void gradient(uint8_t* pixels, Color8u color1, Color8u color2);
	void tint(uint8_t* pixels, Color8u color);
	void blur(uint8_t* pixels);
	void drawLine(uint8_t* pixels, int startX, int startY, int endX, int endY, Color8u color);
    void drawPixel(uint8_t* pixels, int posX, int posY, Color8u color);
	
	private:
	Surface* mySurface_;
	uint8_t* dataArray;

	//window width and height and texture size
	static const int kAppWidth = 800;
	static const int kAppHeight = 600;
	static const int kTextureSize = 1024;

	//keeps track of whether or not the image has been saved
	bool hasSaved;

	//location variables for animation
	int aniX;
	int aniY;

	//boolean for adding tint with a mouse click
	bool hasClicked;
};

void HW01Part2App::prepareSettings(Settings* settings){
	(*settings).setWindowSize(kAppWidth,kAppHeight);
	(*settings).setResizable(false);
}

void HW01Part2App::setup()
{
	mySurface_ = new Surface(kTextureSize,kTextureSize,false);
	hasSaved = false;
	hasClicked = false;
	aniX = 0;
	aniY = 0;
}

void HW01Part2App::drawPixel(uint8_t* pixels, int posX, int posY, Color8u color){
    int offset = 3*(posX + posY*kTextureSize);
    pixels[offset] = color.r;
    pixels[offset+1] = color.g;
    pixels[offset+2] = color.b;
    
}


/*
* This is a non-working attempt at goal A.4.  It colors the whole screen color1 rather than creating a gradient.
*/
void HW01Part2App::gradient(uint8_t* pixels, Color8u color1, Color8u color2)
{
	for(int y = 0; y <= kAppHeight; y++)
	{
		int ratio = y/kAppHeight;
		for(int x = 0; x <= kAppWidth; x++)
		{
			int offset = 3*(x + y*kTextureSize);
			// color gradient math adapted from http://www.objectdefinitions.com/odblog/2008/calculating-a-color-gradient/
			pixels[offset] = (int) (color1.r * (1 - ratio) + color2.r * ratio);
			pixels[offset+1] = (int) (color1.g * (1 - ratio) + color2.g * ratio);
			pixels[offset+2] = (int) (color1.b * (1 - ratio) + color2.b * ratio);
		}
	}
}

/*
* Tints the whole screen a certain color.
*
* Satisfies goal A.6.
*/
void HW01Part2App::tint(uint8_t* pixels, Color8u color)
{
	for(int y = 0; y <= kAppHeight; y++)
	{
		int ratio = y/kAppHeight;
		for(int x = 0; x <= kAppWidth; x++)
		{
            /*Brandon Sonoda: I think that the variable "offset" doesn't well represent
            the value.. maybe "index" would be better?
            and since you're dividing integers by two you're actually going to truncate data
            Twice... Two fixes you can do for this is: 
             
             1.less preferred: add them, then divide by two making only one truncation
              = (pixels[index]+color.r)/2;
             2.Converting them into a double, dividing by two, then rounding that number
             = round((double)(pixels[offset]+color.r)/2;
             */
			int offset = 3*(x + y*kTextureSize);
			pixels[offset] = pixels[offset]/2 + color.r/2;
			pixels[offset+1] = pixels[offset+1]/2 + color.g/2;
			pixels[offset+2] = pixels[offset+2]/2 + color.b/2;
             
		}
	}
}

/*
* Draws a circle with the specified center, radius, and color.
*
* Satisfies goal A.2.
*/
void HW01Part2App::drawCircle(uint8_t* pixels, int center_x, int center_y, int radius, Color8u color)
{
    int dist;
	for(int y = center_y - radius; y <= center_y + radius; y++)
	{
		for(int x = center_x - radius; x <= center_x + radius; x++)
		{
			//test for accessing array out of bounds
            /*Brandon Sonoda:
             It might be better to declare variables outside of loops, 
             that way a new one doesnt have to be declared everytime through the loop
             but instead, it is just changed everytime
             */
            
            /* Brandon Sonoda:
             But instead of having this little snippet in all of your draw methods, you could
             take it out, and make this its own method, making the code more readable.
             */
			if(y < 0 || x < 0 || x >= kAppWidth || y >= kAppHeight) continue;
			/*int*/ dist = (int)sqrt((double)((x-center_x)*(x-center_x) + (y-center_y)*(y-center_y)));
			if(dist <= radius)
			{
					/*int offset = 3*(x + y*kTextureSize);
					pixels[offset] = color.r;
					pixels[offset+1] = color.g;
					pixels[offset+2] = color.b;*/
                drawPixel(pixels, x, y, color);
			}
		}
	}
}

/*
* Draws a rectangle with the specified top left corner, width, height, and color.
*
* Satisfies goal A.1.
*/
void HW01Part2App::drawRect(uint8_t* pixels, int corner_x, int corner_y, int width, int height, Color8u color)
{
	for(int y = corner_y; y <= corner_y + height; y++)
	{
		for(int x = corner_x; x <= corner_x + width; x++)
		{
			//test for accessing array out of bounds
			if(y < 0 || x < 0 || x >= kAppWidth || y >= kAppHeight) continue;
					//By blending the colors I get a semi-transparent effect
            /*Brandon Sonoda: Converted into method here:*/
            drawPixel(pixels, x, y, color);

					
		}
	}
}

/*
* Applies a blur to the whole screen using the following kernel:
*
* 1/9 | 1/9 | 1/9
* 1/9 | 1/9 | 1/9
* 1/9 | 1/9 | 1/9
*
* Satisfies goal B.1.
*/
void HW01Part2App::blur(uint8_t* pixels)
{
	for(int y = 1; y <= kAppHeight-1; y++)
	{
		for(int x = 1; x <= kAppWidth-1; x++)
		{
			//int offset = 3*(x + y*kTextureSize);
			uint8_t r_total = 0;
			uint8_t g_total = 0;
			uint8_t b_total = 0;
			for(int y2 = -1; y2 <= 1; y2++)
			{
				for(int x2 = -1; x2 <= 1; x2++)
				{
					int offset2 = 3*(x + x2 + (y + y2)*kTextureSize);
					r_total += pixels[offset2];
					g_total += pixels[offset2 + 1];
					b_total += pixels[offset2 + 2];
				}
			}
            /* Brandon Sonoda: Again, it doesn't really matter with this project, 
             but be mindful of truncation.. I'm passionate about really weird issues */
			/*pixels[offset] = r_total/9;
			pixels[offset + 1] = g_total/9;
			pixels[offset + 2] = b_total/9;*/
            drawPixel(pixels, x, y, Color8u(r_total/9, g_total/9, b_total/9));
			
		}
	}
}

/*
* Draws a line between two points in the specified color.
*
* This method works for horizontal, vertical, and 45 degree lines, but renders as vertical for other angles.
*
* Satisfies (partially) goal A.3.
*/
void HW01Part2App::drawLine(uint8_t* pixels, int startX, int startY, int endX, int endY, Color8u color)
{
	int xDist = abs(startX - endX);
	int yDist = abs(startY - endY);
    
    /*Brandon Sonoda:
     For my draw line method I actually just found the angle of elevation of the line using the tangent function, 
     and used the atan() function to determine the y value for any given x... this cut down on the amount of hard
     coding that you would have to do, and this way you would only have to check if the line were vertical, reducing
     the amount of conditionals (I couldnt find the angle of elevation for a vertical line w/o diving by zero)
     
     ... But the implementation of this line drawer works perfectly finely
     */
    

	int x = startX;
	int y = startY;

	//handle 45 degree lines
	if(xDist == yDist)
	{
		for(y = min(startY, endY); y <= max(startY, endY); y++)
            drawPixel(pixels, x, y, color);
	}
	//bresenham's line algorithm
	else if(xDist > yDist)
	{
		for(x = min(startX, endX); x <= max(startX, endX); x++)
		{
			y = (endY - startY)/(endX - startX)*(x - startX) + startY;
            drawPixel(pixels, x, y, color);

		}
	}
    /*Brandon Sonoda: You dont really need to check if xDist is less than yDist here, because you know it's not equal and you
     know it's not greater than, so it must be less than.
    */
	else /*if(xDist < yDist)*/
	{
		for(y = min(startY, endY); y <= max(startY, endY); y++)
		{
			x = (y - startY)/(endY - startY)*(endX - startX) + startX;
            drawPixel(pixels, x, y, color);

		}
	}
}

void HW01Part2App::mouseDown( MouseEvent event )
{
	hasClicked = true;
}

void HW01Part2App::update()
{
	uint8_t* dataArray = (*mySurface_).getData();
	drawCircle(dataArray, 200, 200, 50, Color8u(0,255,0));
	drawRect(dataArray, 400, 400, 50, 100, Color8u(255,0,0));
	drawCircle(dataArray, 450, 200, 100, Color8u(0,0,255));
	//animation, circle moves across the screen, leaving a trail of color behind it
	//aniX++;
	aniY++;
	//if(aniX >= kAppWidth)
		//aniX = 0;
    
    /*Brandon Sonoda: You don't really need a ternary operator here, but it's a shorter (visually) alternative,
     and if you intended for the circle to disappear off screen before you relocated it, just make sure to account
     for the circles radius*/
    aniX = (aniX>=kAppWidth)? 0:aniX+1;
	if(aniY >= kAppHeight)
		aniY = 0;
	drawCircle(dataArray, aniX, aniY, 100, Color8u(100,255,100));

	drawLine(dataArray, 10, 10, 50, 100, Color8u(255, 255, 255));
    /*Brandon Sonoda:  This seems like a waste, asking the if it's been saved every time, but it seems like 
     this is the best place to put it... unless you were to put it into the mouse down method, but that wouldn't
     insure that the picture is captured.... */
	//write the image to a .png if we haven't already done so
	if(!hasSaved)
	{
		writeImage("komnicjc.png",*mySurface_);
		hasSaved = true;
	}
	/* If the screen has been clicked, tints and blurs the screen.
	* Allows for multiple clicks to "stack" and continually blur the screen.
	*
	* Satisfies goal E.6.
	*/
    
	if(hasClicked)
	{
		tint(dataArray, Color8u(255,255,255));
		blur(dataArray);
		hasClicked = false;
	}
     
}

void HW01Part2App::draw()
{
	gl::draw(*mySurface_); 
}

CINDER_APP_BASIC( HW01Part2App, RendererGl )
