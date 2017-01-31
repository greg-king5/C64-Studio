/*****************************************************************************/
/*                               Drawing Stuff                               */
/*****************************************************************************/

#include <stdbool.h>
#include <tgi.h>

#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_BORDER    TGI_COLOR_GRAY2
#define COLOR_FORE      TGI_COLOR_WHITE

static unsigned MaxX;
static unsigned MaxY;
static unsigned AspectRatio;

static const unsigned char Palette[2] = { COLOR_FORE, COLOR_BACK };
const tgi_vectorfont* font = 0;

static int selMesh = 0;
const int listX = 230;
const int listY = 18;
const int listW = 80;
const int listH = 48;

const int propsX = 250;
const int propsY = 68;
const int propsW = 70;
const int propsH = 36;

static int selTab = 0;
const int tabsX = 220;
const int tabsY = 115;
const int tabsW = 100;
const int tabsH = 12;

static void CheckTGIError (const char* S) 
{
    unsigned char Error = tgi_geterror ();
    if (Error != TGI_ERR_OK) {
        cprintf ("%s: %d\n", S, Error);
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    }
}

static const tgi_vectorfont* LoadFont (const char* Name)
/* Load a font, do error checking */
{   
    const tgi_vectorfont* F;

    F = tgi_load_vectorfont (Name);
    CheckTGIError ("tgi_load_vectorfont");
    return F;
}

static void InitTGI () 
{	
	printf ("Loading Video Driver...\n");	
#if DYN_DRV
    /* Load the driver */
    tgi_load_driver (tgi_stddrv);
    CheckTGIError ("tgi_load_driver");
#else
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    CheckTGIError ("tgi_install");
#endif

	/* Load the vector fonts */
	printf ("Loading Font (litt.tch)...\n");
    font = LoadFont ("litt.tch");
    tgi_install_vectorfont (font);
}

static void StartTGI ()
{
    // Initialize Video Mode
    tgi_init ();
    CheckTGIError ("tgi_init");

	// Set colors
    bordercolor (COLOR_BORDER);
    tgi_setpalette (Palette);	
	tgi_setcolor (COLOR_BACK);
	tgi_clear ();	

    // Get stuff from the driver
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();
	
	// Set Text Style
	tgi_settextstyle (0x100, 0x100, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
}

static void StopTGI () 
{
#if DYN_DRV
    /* Unload the driver */
    tgi_unload ();
#else
    /* Uninstall the driver */
    tgi_uninstall ();
#endif
}

static void DrawButton (int x, int y, int w, int h, char *text, bool highlight) 
{
	tgi_setcolor (COLOR_FORE);
	if (highlight) {
		tgi_bar(x+1, y+1, x+w-1, y+h-1);
		tgi_setcolor (COLOR_BACK);		
	}
	tgi_outtextxy(x+5, y+h-3, text);
	tgi_setcolor (COLOR_FORE);
	tgi_line(x, y, x+w, y);	
	tgi_line(x, y+h, x+w, y+h);	
	tgi_line(x, y, x, y+h);	
	tgi_line(x+w, y, x+w, y+h);
}

static void DrawList (int x, int y, int w, int h, const char *names[], int sel) 
{
	int i = 0;
	
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(x, y, x+w, y+h);
	tgi_setcolor (COLOR_FORE);
	
	// Draw list
	tgi_line(x, y, x+w, y);	
	tgi_line(x, y+h, x+w, y+h);	
	tgi_line(x, y, x, y+h);	
	tgi_line(x+w, y, x+w, y+h);	
	tgi_line(x+w-10, y, x+w-10, y+h);	
	while (names[i] != NULL && i<9) {
		if (i==sel) {
			tgi_setcolor (COLOR_FORE);
			tgi_bar(x+8, y+(i+1)*12-10, x+w-18, y+(i+1)*12-2);
			tgi_setcolor (COLOR_BACK);
			tgi_outtextxy(x+10, y+(i+1)*12-3, names[i]);
			tgi_setcolor (COLOR_FORE);
		} else {
			tgi_outtextxy(x+10, y+(i+1)*12-3, names[i]);
		}
		i++;
	}
}

static void DrawProps (fix8 pos[3], fix8 rot[3], fix8 dim[3])
{
	char dump[8];
		
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(225, 70, 319, 110);
	tgi_setcolor (COLOR_FORE);

	// Draw props
	tgi_outtextxy(225, 80, "Pos:");
	snprintf(dump, 8, "%ld", pos[0]/256);
	tgi_outtextxy(250, 80, dump);
	snprintf(dump, 8, "%ld", pos[1]/256);
	tgi_outtextxy(275, 80, dump);
	snprintf(dump, 8, "%ld", pos[2]/256);
	tgi_outtextxy(300, 80, dump);

	tgi_outtextxy(225, 92, "Rot:");
	snprintf(dump, 8, "%ld", rot[0]/256);
	tgi_outtextxy(250, 92, dump);
	snprintf(dump, 8, "%ld", rot[1]/256);
	tgi_outtextxy(275, 92, dump);
	snprintf(dump, 8, "%ld", rot[2]/256);
	tgi_outtextxy(300, 92, dump);
	
	tgi_outtextxy(225, 104, "Dim:");
	snprintf(dump, 8, "%ld", dim[0]/256);
	tgi_outtextxy(250, 104, dump);
	snprintf(dump, 8, "%ld", dim[1]/256);
	tgi_outtextxy(275, 104, dump);
	snprintf(dump, 8, "%ld", dim[2]/256);
	tgi_outtextxy(300, 104, dump);
}

static void DrawControls()
{
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(221, 116, 319, 189);
	tgi_setcolor (COLOR_FORE);
	
	// Draw tabs
	DrawButton(220, 115, 33, 12, "Prim", selTab == 0);
	DrawButton(253, 115, 33, 12, "Bool", selTab == 1);
	DrawButton(286, 115, 33, 12, "Scene", selTab == 2);
		
	// Draw buttons
	if (selTab == 0) {
		DrawButton(230, 130, 80, 12, "Add Box", false);
		DrawButton(230, 145, 80, 12, "Add Cone", false);
		DrawButton(230, 160, 80, 12, "Add Cylinder", false);
		DrawButton(230, 175, 80, 12, "Add Sphere", false);
	}
	
	if (selTab == 1) {
		DrawButton(230, 130, 80, 12, "Difference", false);
		DrawButton(230, 145, 80, 12, "Intersect", false);
		DrawButton(230, 160, 80, 12, "Union", false);
	}	
	
	if (selTab == 2) {
		DrawButton(230, 130, 80, 12, "Load Disk", false);
		DrawButton(230, 145, 80, 12, "Save Disk", false);
		DrawButton(230, 160, 80, 12, "Download", false);
		DrawButton(230, 175, 80, 12, "Upload", false);
	}		
}

static void DrawStatus (const char *message)
{
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 191, 319, 199);
	
	// Output message
	tgi_setcolor (COLOR_FORE);
	tgi_outtextxy(10, 198, message);
}

static void DrawPerf (int num, clock_t time)
{
	char dump[30];
	unsigned long sec;
    unsigned sec10;

    // Calculate stats
    sec = (time * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;
	
    // Output stats
	snprintf(dump, 30, "Rendered %d meshes in %lu.%us", num, sec, sec10);
	DrawStatus(&dump[0]);
}

static void DrawGUI (const char *names[], fix8 pos[3], fix8 rot[3], fix8 dim[3]) 
{	
	// Separators
	tgi_setcolor (COLOR_FORE);
	tgi_line(220, 0, 220, 190);
	tgi_line(220, 115, 319, 115);
	tgi_line(0, 190, 319, 190);
	
	// Top Section
	tgi_outtextxy(240, 10, "C64 Studio");	
	DrawList(listX, listY, listW, listH, names, selMesh);
	DrawProps(pos, rot, dim);
	
	// Bottom Section
	DrawControls();
}
