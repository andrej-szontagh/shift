
#ifndef _RENDERER_GUI_H
#define _RENDERER_GUI_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
//	PARAMS :
//
//	x		- X potition in screen coordinates (- 1.0 .. 1.0)
//	y		- Y potition in screen coordinates (- 1.0 .. 1.0)
//	sizex	- horizontal size of source character
//	sizey	- vertical size of source character
//	tex		- id of loaded POT texture that has exactly 16*16 characters (256 - ASCII)
//	blend	- use blending ?
//	text	- string to be rendered.
//
//	DESCRIPTION :
//
//		Draws bitmap text. Each character is same size in source and destination too
//		
///////////////////////////////////////////////////////////
VOID		dr_DrawText (

				FLOAT_32	x,
				FLOAT_32	y,
				FLOAT_32	sizex,
				FLOAT_32	sizey,
				UINT_32		tex,
				BOOL		blend,
				CHARP		text
	);

#endif