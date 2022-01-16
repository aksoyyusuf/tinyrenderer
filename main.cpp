#include "lesson1.h"
#include "canvas.h"

void testCheckerImage(int canvasWidth, int canvasHeight, Canvas::Canvas& canvas)
{
    int i, j, c;
    int constant = 0;

    for (int i = 0; i < canvasWidth; i++)
    {
        for (int j = 0; j < canvasHeight; j++)
        {
            c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;            

            canvas.SetPixel(i, j, (GLubyte)c, (GLubyte)c, (GLubyte)c);
        }
    }
}

int main()
{
    int canvasWidth = 128;
    int canvasHeight = 128;

	Canvas::Canvas canvas(canvasWidth, canvasHeight);

    do
    {    
        testCheckerImage(canvasHeight, canvasWidth, canvas);

    } while (canvas.RenderLoop());
}
