/*******************************************************************************
FILE : renderalias.h

LAST MODIFIED : 19 February 1998

DESCRIPTION :
==============================================================================*/
#if !defined (RENDER_ALIAS_H)
#define RENDER_ALIAS_H

/*
Global functions
----------------
*/
#if defined (__cplusplus)
extern "C"  //  Makes this function available to C by not mangling function name
#endif /* defined (__cplusplus) */
int export_to_alias(struct Scene *scene,char *filename);
/******************************************************************************
LAST MODIFIED :  7 January 1998

DESCRIPTION :
Saves the visible objects in an Alias wire file. (Currently only volumes and
surfaces.)
==============================================================================*/

#if defined (__cplusplus)
extern "C"  //  Makes this function available to C by not mangling function name
#endif /* defined (__cplusplus) */
int export_to_alias_frames(struct Scene *scene,char *filename,float frame_in,
	float frame_out, int save_now,int destroy_when_saved);
/******************************************************************************
LAST MODIFIED : 7 January 1998

DESCRIPTION :
Saves the visible objects in an Alias wire file. (Currently only volumes and
surfaces.)
If <frame_in> or <frame_out> are non zero then the group node is set
so that the given object is visible only between the frame in and out values.
<save_now> is a flag that indicates that the current Alias world should be
written to the file indicated by filename.  Otherwise the objects are created
but not saved.
<destroy_when_saved> requests that the entire alias universe is destroyed
after it has been saved.
==============================================================================*/

#if defined (__cplusplus)
extern "C"  //  Makes this function available to C by not mangling function name
#endif /* defined (__cplusplus) */
int export_to_alias_sdl( struct Scene *scene, char *filename,
	char *retrieve_filename, float view_frame	);
/******************************************************************************
LAST MODIFIED : 2 February 1998

DESCRIPTION :
Saves all the visible objects in an Alias SDL file ready to be rendered.
All visible objects in the scene are created in the Alias universe.
The camera position and so on can be set by setting a animation
curves using Alias, saving a wire file and supplying that file for the
<retrieve_filename> which is read in, the <view_frame> command is issued to
get to the particular place in the animation curves and then the SDL (without
animation information) is written for that one frame.  The Alias universe is
cleared when the SDL has been written.
==============================================================================*/
#endif /* !defined (RENDER_ALIAS_H) */
