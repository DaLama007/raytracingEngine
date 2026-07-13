## Raytracing Engine by Marwan

![screenShotFinal](https://github.com/DaLama007/raytracingEngine/blob/main/screenShotFinal.png)

This is a 2d raytracing engine. To run run the following executable:
raytracing.exe

This engine handles collisions with Rectangles and Circles. It spawns a source emitting rays defined by: P = O + t*D with O being the origin and D a direction vector.

**Rectangle - Ray collision-handling:**

**Circle - Ray collisio-handling:**

circle is defined by (x-C_x)^2 + (y-C_y)^2 = r^2

 insertion of the formula of a ray: P(x,y) = O(O_x, O_y) + t*D;

 (O + t*D - C)^2 = r^2

For simplification: L = O-C
 => (L + t*D)^2 = r^2
 
L^2 + 2L*t*D + (D)t^2 = r^2
<=> D^2*t^2 + 2LD*t + L^2 - r^2 = 0

We calculate a, b and c summing the x and y
Then calc the discriminant : delta = b^2 - 4ac

And then decide based on result wether there is a collision or not. 

Compiling command:
gcc main.c -o raytracer.exe -I./SDL2/include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2
