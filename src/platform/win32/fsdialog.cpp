

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <direct.h>

#include <time.h>

#include <ysclass.h>
#include <ysport.h>

#include <fs.h>
#include <fsfilename.h>
#include <fsopengl.h>
#include <fswindow.h>

#include <windows.h>
#include <float.h>

#include <ysbitmap.h>

#include <fsgui.h>
// #include "../fsguiselectiondialogbase.h"
// #include "../fschoose.h"


extern void FsClearEventQueue(void);
extern int FsCheckKeyHeldDown(void);


static FsWorld *currentWorld=NULL;

extern HWND FsWin32GetMainWindowHandle(void);



