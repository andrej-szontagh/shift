
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define M_DR_SET_SHADER_CULLON(shader) {    \
                                            \
    if (shader != dr_state_shader) {        \
        glUseProgram (dr_state_shader = shader);    \
                                                    \
        M_STATE_CULLFACE_SET;   \
    }   \
}

#define M_DR_SET_SHADER_CULLOFF(shader) {   \
                                            \
    if (shader != dr_state_shader) {        \
        glUseProgram (dr_state_shader = shader);    \
                                                    \
        M_STATE_CULLFACE_CLEAR;   \
    }   \
}

#define M_DR_DRAW_LIST_INSTANCED(   func, ID, display_list)             {  func (ID);  dr_DrawListInstanced (ID, display_list);    }
#define M_DR_DRAW_LIST_SINGLE(      func, ID, display_list)             {  func (ID);  dr_DrawListSingle    (ID, display_list);    }
#define M_DR_DRAW_VBO_INSTANCED(    func, ID, model_mode, model_count)  {  func (ID);  dr_DrawVBOInstanced  (ID, model_mode, model_count);    }
#define M_DR_DRAW_VBO_SINGLE(       func, ID, model_mode, model_count)  {  func (ID);  dr_DrawVBOSingle     (ID, model_mode, model_count);    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INLINE DRAW ROUTINES
////////////////////////////////////////////////////////////////////////////////////////////////////////////

INLINE VOID dr_DrawListInstanced (

            UINT_32 ID,
            UINT_32 list
    )
{
    register FLOAT_32P transform = dr_object_instances_transforms [ID];

    for (INT_32 i = dr_object_instances [ID] - 1; i >= 0; i --) {

        // transform ..
        glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
        glMultiTexCoord4fv (GL_TEXTURE5, &(transform [ 3]));   transform += 7;

		// draw ..
		glCallList (list);
    }
}

INLINE VOID dr_DrawListSingle (

            UINT_32 ID,
            UINT_32 list
    )
{
    register FLOAT_32P transform = &dr_object_transforms [ID << 4];

    // matrix (3x3 + translation)
    glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
    glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
    glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
    glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

	// draw ..
	glCallList (list);
}

INLINE VOID dr_DrawVBOInstanced (

            UINT_32 ID,
            UINT_32 model_mode,
            UINT_32 model_count
    )
{
    register FLOAT_32P transform = dr_object_instances_transforms [ID];

    for (INT_32 i = dr_object_instances [ID] - 1; i >= 0; i --) {

        // transform ..
        glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
        glMultiTexCoord4fv (GL_TEXTURE5, &(transform [ 3]));   transform += 7;

        // draw ..
        glDrawElements (model_mode, model_count, GL_UNSIGNED_SHORT, NULL);            
    }
}

INLINE VOID dr_DrawVBOSingle (

            UINT_32 ID,
            UINT_32 model_mode,
            UINT_32 model_count
    )
{
    register FLOAT_32P transform = &dr_object_transforms [ID << 4];

    // matrix (3x3 + translation)
    glMultiTexCoord3fv (GL_TEXTURE4, &(transform [ 0]));
    glMultiTexCoord3fv (GL_TEXTURE5, &(transform [ 4]));
    glMultiTexCoord3fv (GL_TEXTURE6, &(transform [ 8]));
    glMultiTexCoord3fv (GL_TEXTURE7, &(transform [12]));

    // draw ..
    glDrawElements (model_mode, model_count, GL_UNSIGNED_SHORT, NULL);
}
