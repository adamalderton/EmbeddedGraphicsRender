#include <stdint.h>

#include "draw_line.h"
#include "draw_triangle.h"

static void swap2DVertices(uint8_t v0[2], uint8_t v1[2])
{
    uint8_t temp[2];

    COPY_2D_VERTEX(temp, v0);
    COPY_2D_VERTEX(v0, v1);
    COPY_2D_VERTEX(v1, temp);
}

/*
    Sorts triangle vertices in descending y.
*/
static void sortTriangle2DVertices(Triangle2D *tri)
{
    if (tri->vs[0][Y] < tri->vs[2][Y]) {
        swap2DVertices(tri->vs[0], tri->vs[2]);
    }

    if (tri->vs[0][Y] < tri->vs[1][Y]) {
        swap2DVertices(tri->vs[0], tri->vs[1]);
    }

    if (tri->vs[1][Y] < tri->vs[2][Y]) {
        swap2DVertices(tri->vs[1], tri->vs[2]);
    }
}

static TriangleSideVarTracker setupLineTracker(uint8_t point_0[2], uint8_t point_1[2])
{
    TriangleSideVarTracker t;

    int16_t dx = point_1[X] - point_0[X];
    int16_t dy = point_1[Y] - point_0[Y];

    if (dx > 0) {
        t.direction = 1;
    } else {
        dx = -dx;
        t.direction = -1;
    }

    if (dx > dy) {
        t.octant = 0;
    } else {
        t.octant = 1;
    }

    if (t.octant == 0) {
        t.p = 2 * dy;
        t.q = t.p - (2 * dx);
        t.err = t.p - dx;
    } else {
        t.p = 2 * dx;
        t.q = t.p - (2 * dy);
        t.err = t.p - dy;
    }

    return t;
}

static void drawFlatTopTriangle(uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS], Triangle2D tri)
{
    /*
        As the vertices have been sorted, the flat line lies between vertices 0 and 1.
        Therefore, we need to draw a line from vertex 2 to vertex 1 (left) and 
        from vertex 2 to vertex 0 (right).

        We'll call the left line A and the right line B.
    */

    TriangleSideVarTracker A;   /* Tracks progression along line A (left). */
    TriangleSideVarTracker B;   /* Tracks progression along line B (right). */
    uint8_t y;                 /* Tracks y coordinate. */
    uint8_t y_end;             /* y coordinate of top line. */
    uint8_t xA;                /* Leftmost x coordinate of left line (A) for a given y.. */
    uint8_t xB;                /* Rightmost y coordinate of right line (B) for a given y. */

    /*
        Ensure triangle follows right hand rule (normal out of the plane).
        This ensures that the 'right' line is on the 'right' and the 'left' line is on the left.
    */
    if (tri.vs[0][X] < tri.vs[1][X]) {
        swap2DVertices(tri.vs[0], tri.vs[1]);
    }

    A = setupLineTracker(tri.vs[2], tri.vs[1]);
    B = setupLineTracker(tri.vs[2], tri.vs[0]);

    /* If a flat top triangle is correctly passed, then tri.vs[0][Y] should equal tri.vs[1][Y]. */
    y = tri.vs[2][Y];
    y_end = tri.vs[1][Y];
    xA = tri.vs[2][X];
    xB = tri.vs[2][X];

    /* Draw first (bottom) point. */
    drawPixel(frame, tri.vs[2][X], y, tri.colour, CLOSEST_DISTANCE);

    /*
        Flat top triangle rasterisation loop.

        Much of the duplicate code here could be put into a separate function however
        this was decided against as to prevent unnecessary numerous pointer dereference.
        A pointer to an octant specific function was also considered but decided against.

        It is a shame that if (A.octant == 0) has be carried out for A and B at each y, but
        in terms of code simplification, it was a price worth paying.
    */
    while (y < y_end) {

        y++;

        /*
            Update A and get xA.
        */
        if (A.octant == 0) {

            while (1) {
                xA += A.direction;

                if (A.err >= 0) {
                    A.err += A.q;
                    break;
                } else {
                    A.err += A.p;
                }
            }

        } else {

            if (A.err >= 0) {
                xA += A.direction;
                A.err += A.q;
            } else {
                A.err += A.p;
            }

        }

        /*
            Update B and get xB.
        */
        if (B.octant == 0) {

            while (1) {
                xB += B.direction;

                if (B.err >= 0) {
                    B.err += B.q;
                    break;
                } else {
                    B.err += B.p;
                }
            }

        } else {

            if (B.err >= 0) {
                xB += B.direction;
                B.err += B.q;
            } else {
                B.err += B.p;
            }

        }

        /* Finally, draw the line at y = y between xA and xB. */
        drawHorizontalLine(frame, y, xA, xB, tri.colour, CLOSEST_DISTANCE);
    }
}

static void drawFlatBottomTriangle(uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS], Triangle2D tri)
{
    /*
        As the vertices have been sorted, the flat line lies between vertices 1 and 2.
        Therefore, we need to draw a line from vertex 1 to vertex 0 (left) and 
        from vertex 2 to vertex 0 (right).

        We'll call the left line A and the right line B.
    */

    TriangleSideVarTracker A;   /* Tracks progression along line A (left). */
    TriangleSideVarTracker B;   /* Tracks progression along line B (right). */
    uint8_t y;                 /* Tracks y coordinate. */
    uint8_t y_end;             /* y coordinate of top line. */
    uint8_t xA;                /* Leftmost x coordinate of left line (A) for a given y.. */
    uint8_t xB;                /* Rightmost y coordinate of right line (B) for a given y. */

    /*
        Ensure triangle follows right hand rule (normal out of the plane).
        This ensures that the 'right' line is on the 'right' and the 'left' line is on the left.
    */
    if (tri.vs[2][X] < tri.vs[1][X]) {
        swap2DVertices(tri.vs[2], tri.vs[1]);
    }

    /* Initialise tracking variables. */
    A = setupLineTracker(tri.vs[1], tri.vs[0]);
    B = setupLineTracker(tri.vs[2], tri.vs[0]);
    y = tri.vs[2][Y];
    y_end = tri.vs[0][Y];
    xA = tri.vs[1][X];
    xB = tri.vs[2][X];

    /* Draw first (bottom) line. */
    drawHorizontalLine(frame, y, xA, xB, tri.colour, CLOSEST_DISTANCE);

    /*
        Flat bottom triangle rasterisation loop.

        Much of the duplicate code here could be put into a separate function however
        this was decided against as to prevent unnecessary numerous pointer dereference.
        A pointer to an octant specific function was also considered but decided against.

        It is a shame that if (A.octant == 0) has be carried out for A and B at each y, but
        in terms of code simplification, it was a price worth paying.
    */
    while (y < y_end) {

        y++;

        /*
            Update A and get xA.
        */
        if (A.octant == 0) {

            while (1) {
                xA += A.direction;

                if (A.err >= 0) {
                    A.err += A.q;
                    break;
                } else {
                    A.err += A.p;
                }
            }

        } else {

            if (A.err >= 0) {
                xA += A.direction;
                A.err += A.q;
            } else {
                A.err += A.p;
            }

        }

        /*
            Update B and get xB.
        */
        if (B.octant == 0) {

            while (1) {
                xB += B.direction;

                if (B.err >= 0) {
                    B.err += B.q;
                    break;
                } else {
                    B.err += B.p;
                }
            }

        } else {

            if (B.err >= 0) {
                xB += B.direction;
                B.err += B.q;
            } else {
                B.err += B.p;
            }

        }

        /* Finally, draw the line at y = y between xA and xB. */
        drawHorizontalLine(frame, y, xA, xB, tri.colour, CLOSEST_DISTANCE);
    }
}

/*
    Draws and fills triangles in one colour.

    First the procedure splits the triangle into a flat top and flat bottom triangle.
    These two-sub triangles are then filled separately.

    The process of filling a flat sub-triangle consists of running Bresenham's algorithm in parallel
    for two lines and drawing a horizontal line between them when the y coordinate is incremented.
    Further documentation on Bresenham's line algorithm and its implementation here is given among
    the drawLine function which was a precursor to the drawTriangle function. The drawLine function was
    adapted from an implementation by Michael Abrash.
*/
void drawTriangle(uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS], Triangle2D tri)
{
    /* Sort triangle vertices in ascending order. Triangle will be drawn from the bottom up. */
    sortTriangle2DVertices(&tri);

    /* Simple case of top flat triangle only. */
    if (tri.vs[0][Y] == tri.vs[1][Y]) {
        drawFlatTopTriangle(frame, tri);
        return;
    }

    /* Simple case of bottom flat triangle only. */
    if (tri.vs[1][Y] == tri.vs[2][Y]) {
        drawFlatBottomTriangle(frame, tri);
        return;
    }

    /*
        General Case. We split the triangle into two - a flat top and a flat bottom. 
        We then draw both.

        To do this, we first find the common fourth point. It lies along the hypotenuse
        and shares an x value with tri.vs[1], if the vertices are sorted correctly as above.
        To find the y coordinate of this point, we apply Thales' theorem to interpolate
        the hypotenuse ar this x value. We will call this point Q.
    */
    uint8_t Q[2];

    /*
        x_Q = (x_2 - x_0)\frac{y_1 - y_0}{y_2 - y_0} + x_0
        The float divide is rounded as opposed to truncated as of course, with an
        extremal point like Q, the importance of rounding up is that much higher.

        Value found is incremented by 0.5 such that the casting process preforms a rounding operation.
    */
    Q[X] = (uint8_t) ( tri.vs[0][X] + (tri.vs[2][X] - tri.vs[0][X]) * ( ( (float) (tri.vs[1][Y] - tri.vs[0][Y]) / (float) (tri.vs[2][Y] - tri.vs[0][Y]) ) ) + 0.5);
    Q[Y] = tri.vs[1][Y];

    /*
        We first draw the bottom half, being the top flat triangle.

        In doing this, we also INCLUDE the horizontal line at Q[Y]. We don't
        include this during the drawing of the top half as it doesn't need to be drawn
        twice.

        Following the sorting of the triangle vertices, we must temporarily
        substitute Q for the top-most vertex. We then draw the resulting top-flat triangle.
    */

    /* Substitute in Q. 'tri.vs[0]' is temporarily stored in 'Q'. */
    swap2DVertices(tri.vs[0], Q);

    drawFlatTopTriangle(frame, tri);

    /* Retrieve Q and reset triangle. temp now stores Q.*/
    swap2DVertices(tri.vs[0], Q);

    /*
        We now draw the top half of the triangle.

        To do this, we substitute the bottom-most vertex and draw the triangle.
        We also increment the y value of the points on the bottom flat side by one
        as to prevent that line from being re-drawn, as previously discussed.
        We can do this by value as the triangle is no longer needed once drawn.
    */
    Q[Y]++;
    tri.vs[1][Y]++;

    swap2DVertices(tri.vs[2], Q);

    drawFlatBottomTriangle(frame, tri);

    /*
        The triangle drawing is finished.

        If needed, Q could be re-swapped out and decremented below
        if further manipulation is needed.
    */
}