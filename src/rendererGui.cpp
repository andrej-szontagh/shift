
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// dr_DrawText
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

VOID		dr_DrawText (

				FLOAT_32	x,
				FLOAT_32	y,
				FLOAT_32	sizex,
				FLOAT_32	sizey,
				UINT_32		tex,
				BOOL		blend,
				CHARP		text
	) 
{

	FLOAT_32 startx, starty, offx, offy;

	UINT_32 i, l = (UINT_32) strlen (text);

	startx = x;
	starty = y;

	// 16x16 characters = 256 characters of ASCII table

	FLOAT_32 size = 16.0f / 256.0f;

    M_STATE_DEPTHTEST_CLEAR;

    M_STATE_TEX0_SET (tex);

    if (blend)	{

        glEnable (GL_BLEND);    glBlendFunc  (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

	M_STATE_MATRIXMODE_PROJECTION; glPushMatrix (); glLoadIdentity ();
    M_STATE_MATRIXMODE_MODELVIEW;  glPushMatrix (); glLoadIdentity ();

    glUseProgram (0);

	for (i = 0; i < l; i ++) {

		switch (text [i]) {

			case ' '  : x += sizex; 
				continue;

			case '\n' : y -= sizey; x = startx; 
				continue;
		}

		offx =			(text [i] % 16) / 16.0f;
		offy = 1.0f -	(text [i] / 16)	/ 16.0f - size;

        glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
		glBegin (GL_QUADS);
				glTexCoord2f (offx,			offy + size);	glVertex2f (x,			y + sizey);
				glTexCoord2f (offx + size,	offy + size);	glVertex2f (x + sizex,	y + sizey);
				glTexCoord2f (offx + size,	offy);			glVertex2f (x + sizex,	y);
				glTexCoord2f (offx,			offy);			glVertex2f (x,			y);
		glEnd ();

		x += sizex;
	}

	// RESTORE

    if (blend)	glDisable (GL_BLEND);

	M_STATE_MATRIXMODE_MODELVIEW;   glPopMatrix ();
	M_STATE_MATRIXMODE_PROJECTION;  glPopMatrix ();
}

#endif
