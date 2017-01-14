
#include <cc65.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "include/cc65-libgeometry.h"
#include "include/cc65-libmemory.h"
#include "include/cc65-libmouse.h"
#include "include/cc65-librender.h"
#include "include/cc65-libstl.h"

#define MAXMESH 8


/*****************************************************************************/
/*                                 Main Code                                 */
/*****************************************************************************/

int main (void) 
{
	unsigned int i;
	bool doRender = true;
	clock_t time;
	
	fix8 *dim[MAXMESH], *pos[MAXMESH];
	int nVerts[MAXMESH], nTris[MAXMESH];
	fix8 *verts[MAXMESH], *norms[MAXMESH];
	int *tris[MAXMESH], *pxls[MAXMESH];
	
	// Reset Screen
	clrscr (); gotoxy (0, 0);
	printf ("C64 Studio is Initializing...\n");
	
	// Init Memory
	InitMemory();
	InitMouse();
	InitTGI();
	
	// Initialize position/dimension
	for (i = 0; i < MAXMESH; ++i) {
		dim[i] = (fix8*) malloc (3*sizeof(fix8));
		pos[i] = (fix8*) malloc (3*sizeof(fix8));
	}
	
	// Create Box
	//dim[0][0] = Int2Fix8(10); dim[0][1] = Int2Fix8(10); dim[0][2] = Int2Fix8(10);
	//pos[0][0] = Int2Fix8(-20); pos[0][1] = Int2Fix8(0); pos[0][2] = Int2Fix8(0);	
	//CreateBox(dim[0], pos[0], &nVerts[0], &nTris[0], &verts[0], &norms[0], &tris[0], &pxls[0]);
	
	// Read STL Mesh
	ReadSTL("logo.stl", &nVerts[1], &nTris[1], &verts[1], &norms[1], &tris[1], &pxls[1]);
/*	gotoxy (0, 5); cprintf ("Triangle:%d,%d,%d", (*tris)[1][0], (*tris)[1][1], (*tris)[1][2]);
	gotoxy (0, 9); cprintf ("Normal:%ld,%ld,%ld", (*norms)[1][0], (*norms)[1][1], (*norms)[1][2]);
*/	
	// Initialize Screen
	StartTGI();
	DrawPanel();

	// Show Mouse
	mouse_show ();
	mouse_info(&info);	
	mouseX = info.pos.x;
	mouseY = info.pos.y;	
		
    // Enable Mouse Interface
	while (!kbhit()) {			
		//Get mouse Information
        mouse_info (&info);	
		if (info.buttons & MOUSE_BTN_LEFT) {	
			// Rotating the axes
			if (abs(mouseX - info.pos.x) > 0 || abs(mouseY - info.pos.y) > 0) {
				// Compute delta
				xCam = (xCam + (mouseY - info.pos.y))%360;
				zCam = (zCam + (mouseX - info.pos.x))%360;
				mouseX = info.pos.x;
				mouseY = info.pos.y;

				// Refresh axes
				UpdateCamera();
				RenderAxes();	
				doRender = true;
			}			
		} else {
			// Redraw is the axes were rotated
			if (doRender) {
				// Reset state
				doRender = false;
				UpdateCamera();
				ResetCanvas();

				// Render scene
				time = clock();
				//Rasterize(nVerts[0], verts[0], &pxls[0]);
				//RenderMesh(nTris[0], tris[0], norms[0], pxls[0]);
				Rasterize(nVerts[1], &verts[1], &pxls[1]);
				RenderMesh(nTris[1], &tris[1], &norms[1], &pxls[1]);
				RenderAxes();							
				time = clock() - time;			
			}
		}
	}
	cgetc();
	StopTGI();
	
	// Show stats
	ShowStats(time, 1);
	
    // Done
    return EXIT_SUCCESS;
}