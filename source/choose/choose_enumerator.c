/*******************************************************************************
FILE : choose_enumerator.c

LAST MODIFIED : 21 January 2000

DESCRIPTION :
Widgets for editing a FE_field_scalar object = scalar function of a field.
==============================================================================*/
#include <stdio.h>
#if defined (MOTIF)
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#endif /* defined (MOTIF) */
#include "general/debug.h"
#include "choose/choose_enumerator.h"
#include "choose/chooser.h"
#include "user_interface/message.h"
#include "user_interface/user_interface.h"

/*
Module variables
----------------
*/
struct Choose_enumerator
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Contains information required by the choose_enumerator_widget.
==============================================================================*/
{
	struct Callback_data update_callback;
	struct Chooser *chooser;
	Widget widget,parent;
}; /* struct Choose_enumerator */

/*
Module functions
----------------
*/

static int choose_enumerator_update(struct Choose_enumerator *choose_enumerator)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Tells client that the component has changed.
==============================================================================*/
{
	int return_code;

	ENTER(choose_enumerator_update);
	if (choose_enumerator)
	{
		if (choose_enumerator->update_callback.procedure)
		{
			(choose_enumerator->update_callback.procedure)(
				(Widget)NULL,choose_enumerator->update_callback.data,
				Chooser_get_item(choose_enumerator->chooser));
		}
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_update.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* choose_enumerator_update */

static void choose_enumerator_destroy_CB(Widget widget,
	XtPointer client_data,XtPointer reason)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Callback when chooser destroyed - so also destroy choose_enumerator.
==============================================================================*/
{
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_destroy_CB);
	USE_PARAMETER(widget);
	USE_PARAMETER(reason);
	if (choose_enumerator=(struct Choose_enumerator *)client_data)
	{
		DESTROY(Chooser)(&(choose_enumerator->chooser)); \
		DEALLOCATE(choose_enumerator);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_destroy_CB.  Invalid argument(s)");
	}
	LEAVE;
} /* choose_enumerator_destroy_CB */

static void choose_enumerator_update_callback(Widget widget,
	void *choose_enumerator_void,void *current_item_void)
/*******************************************************************************
LAST MODIFIED : 20 January 2000

DESCRIPTION :
Callback for change of coordinate field.
==============================================================================*/
{
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_update_callback);
	USE_PARAMETER(widget);
	USE_PARAMETER(current_item_void);
	if (choose_enumerator=(struct Choose_enumerator *)choose_enumerator_void)
	{
		choose_enumerator_update(choose_enumerator);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_update_callback.  Invalid argument(s)");
	}
	LEAVE;
} /* choose_enumerator_update_callback */

/*
Global functions
----------------
*/
Widget create_choose_enumerator_widget(Widget parent,
	int number_of_valid_strings,char **valid_strings,char *enumerator_string)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Creates an editor for specifying a string out of the <valid_strings>, with the
<enumerator_string> chosen as the current_value. The string should be converted
to its appropriate type by a function like:
enum Enumerated_type Enumerated_type_from_string(char *string);
Note: Choose_enumerator will be automatically DESTROYed with its widgets.
==============================================================================*/
{
	struct Callback_data callback;
	struct Choose_enumerator *choose_enumerator;
	Widget return_widget;

	ENTER(create_choose_enumerator_widget);
	return_widget=(Widget)NULL;
	if (parent&&valid_strings&&(0<number_of_valid_strings)&&enumerator_string)
	{
		if (ALLOCATE(choose_enumerator,struct Choose_enumerator,1))
		{
			choose_enumerator->chooser=(struct Chooser *)NULL;
			choose_enumerator->widget=(Widget)NULL;
			choose_enumerator->parent=parent;
			choose_enumerator->update_callback.procedure=(Callback_procedure *)NULL;
			choose_enumerator->update_callback.data=(void *)NULL;
			if (choose_enumerator->chooser=
				CREATE(Chooser)(parent,number_of_valid_strings,(void **)valid_strings,
					valid_strings,(void *)enumerator_string,&(choose_enumerator->widget)))
			{
				/* add choose_enumerator as user data to chooser widget */
				XtVaSetValues(choose_enumerator->widget,XmNuserData,choose_enumerator,
					NULL);
				/* add destroy callback for chooser widget */
				XtAddCallback(choose_enumerator->widget,XmNdestroyCallback,
					choose_enumerator_destroy_CB,(XtPointer)choose_enumerator);
				/* get updates when chooser changes */ \
				callback.data=(void *)choose_enumerator;
				callback.procedure=choose_enumerator_update_callback;
				Chooser_set_update_callback(choose_enumerator->chooser,&callback);
				return_widget=choose_enumerator->widget;
			}
			else
			{
				display_message(ERROR_MESSAGE,
					"create_choose_enumerator_widget.  Could not create chooser");
				DEALLOCATE(choose_enumerator);
			}
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"create_choose_enumerator_widget.  Not enough memory");
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"create_choose_enumerator_widget.  Invalid argument(s)");
	}
	LEAVE;

	return (return_widget);
} /* create_choose_enumerator_widget */

struct Callback_data *choose_enumerator_get_callback(
	Widget choose_enumerator_widget)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Returns a pointer to the callback item of the choose_enumerator_widget.
==============================================================================*/
{
	struct Callback_data *callback;
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_get_callback);
	if (choose_enumerator_widget)
	{
		/* Get the pointer to the data for the choose_enumerator_widget */
		XtVaGetValues(choose_enumerator_widget,XmNuserData,&choose_enumerator,NULL);
		if (choose_enumerator)
		{
			callback = &(choose_enumerator->update_callback);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"choose_enumerator_get_callback.  Missing chooser");
			callback=(struct Callback_data *)NULL;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_get_callback.  Invalid argument");
		callback=(struct Callback_data *)NULL;
	}
	LEAVE;

	return (callback);
} /* choose_enumerator_get_callback */

int choose_enumerator_set_callback(Widget choose_enumerator_widget,
	struct Callback_data *new_callback)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Changes the callback item of the choose_enumerator_widget.
==============================================================================*/
{
	int return_code;
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_set_callback);
	if (choose_enumerator_widget&&new_callback)
	{
		/* Get the pointer to the data for the choose_enumerator_widget */
		XtVaGetValues(choose_enumerator_widget,XmNuserData,&choose_enumerator,NULL);
		if (choose_enumerator)
		{
			choose_enumerator->update_callback.procedure=new_callback->procedure;
			choose_enumerator->update_callback.data=new_callback->data;
			return_code=1;
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"choose_enumerator_set_callback.  Missing chooser");
			return_code=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_set_callback.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* choose_enumerator_set_callback */

char *choose_enumerator_get_string(Widget choose_enumerator_widget)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Returns the current enumerator_string in use by the editor. Calling function
must not destroy or modify the returned static string.
==============================================================================*/
{
	char *enumerator_string;
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_get_string);
	if (choose_enumerator_widget)
	{
		/* Get the pointer to the data for the choose_enumerator_widget */
		XtVaGetValues(choose_enumerator_widget,XmNuserData,&choose_enumerator,NULL);
		if (choose_enumerator)
		{
			enumerator_string=(char *)Chooser_get_item(choose_enumerator->chooser);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"choose_enumerator_get_string.  Missing chooser");
			enumerator_string=(char *)NULL;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_get_string.  Invalid argument");
		enumerator_string=(char *)NULL;
	}
	LEAVE;

	return (enumerator_string);
} /* choose_enumerator_get_string */

int choose_enumerator_set_string(Widget choose_enumerator_widget,
	char *enumerator_string)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Changes the enumerator_string in the choose_enumerator_widget.
==============================================================================*/
{
	int return_code;
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_set_string);
	if (choose_enumerator_widget&&enumerator_string)
	{
		/* Get the pointer to the data for the choose_enumerator_widget */
		XtVaGetValues(choose_enumerator_widget,XmNuserData,&choose_enumerator,NULL);
		if (choose_enumerator)
		{
			return_code=
				Chooser_set_item(choose_enumerator->chooser,(void *)enumerator_string);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"choose_enumerator_set_string.  Missing chooser");
			return_code=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_set_string.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* choose_enumerator_set_string */

int choose_enumerator_set_valid_strings(Widget choose_enumerator_widget,
	int number_of_valid_strings,char **valid_strings,char *enumerator_string)
/*******************************************************************************
LAST MODIFIED : 21 January 2000

DESCRIPTION :
Changes the list of <valid_strings> in the choose_enumerator_widget.
==============================================================================*/
{
	int return_code;
	struct Choose_enumerator *choose_enumerator;

	ENTER(choose_enumerator_set_valid_strings);
	if (choose_enumerator_widget&&(0<number_of_valid_strings)&&valid_strings&&
		enumerator_string)
	{
		/* Get the pointer to the data for the choose_enumerator_widget */
		XtVaGetValues(choose_enumerator_widget,XmNuserData,&choose_enumerator,NULL);
		if (choose_enumerator)
		{
			if (!(return_code=Chooser_build_main_menu(
				choose_enumerator->chooser,number_of_valid_strings,
				(void **)valid_strings,valid_strings,(void *)enumerator_string)))
			{
				display_message(ERROR_MESSAGE,
					"choose_enumerator_set_valid_strings.  Could not build menu");
				return_code=0;
			}
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"choose_enumerator_set_valid_strings.  Missing chooser");
			return_code=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"choose_enumerator_set_valid_strings.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* choose_enumerator_set_valid_strings */
