/*******************************************************************************
FILE : auxiliary_graphics_types.c

LAST MODIFIED : 3 May 2001

DESCRIPTION :
Structures and enumerated types needed to produce graphics primitives but not
specific to any of them. Examples are:
- struct Element_discretization: stores the number of segments used to
represent curvesin three xi-directions;
- Triple;
==============================================================================*/
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is cmgui.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "command/parser.h"
#include "general/debug.h"
#include "general/enumerator_private.h"
#include "general/random.h"
#include "graphics/auxiliary_graphics_types.h"
#include "user_interface/user_interface.h"
#include "user_interface/message.h"

/*
Global functions
----------------
*/

PROTOTYPE_ENUMERATOR_STRING_FUNCTION(Graphics_select_mode)
{
	const char *enumerator_string;

	ENTER(ENUMERATOR_STRING(Graphics_select_mode));
	switch (enumerator_value)
	{
		case GRAPHICS_SELECT_ON:
		{
			enumerator_string = "select_on";
		} break;
		case GRAPHICS_NO_SELECT:
		{
			enumerator_string = "no_select";
		} break;
		case GRAPHICS_DRAW_SELECTED:
		{
			enumerator_string = "draw_selected";
		} break;
		case GRAPHICS_DRAW_UNSELECTED:
		{
			enumerator_string = "draw_unselected";
		} break;
		default:
		{
			enumerator_string = (const char *)NULL;
		} break;
	}
	LEAVE;

	return (enumerator_string);
} /* ENUMERATOR_STRING(Graphics_select_mode) */

DEFINE_DEFAULT_ENUMERATOR_FUNCTIONS(Graphics_select_mode)

int set_exterior(struct Parse_state *state,void *value_address_void,
	void *dummy_user_data)
/*******************************************************************************
LAST MODIFIED : 8 June 2004

DESCRIPTION :
A modifier function for setting exterior flag and face number.
==============================================================================*/
{
	const char *current_token;
	int return_code,*value_address;

	ENTER(set_exterior);
	USE_PARAMETER(dummy_user_data);
	if (state)
	{
		current_token=state->current_token;
		if (current_token)
		{
			if (strcmp(PARSER_HELP_STRING,current_token)&&
				strcmp(PARSER_RECURSIVE_HELP_STRING,current_token))
			{
				value_address=(int *)value_address_void;
				if (value_address)
				{
					return_code=1;
					/*???DB.  Only valid for cubes (not polygons) */
					if (fuzzy_string_compare(current_token,"XI1_0"))
					{
						*value_address=0;
						shift_Parse_state(state,1);
					}
					else
					{
						if (fuzzy_string_compare(current_token,"XI1_1"))
						{
							*value_address=1;
							shift_Parse_state(state,1);
						}
						else
						{
							if (fuzzy_string_compare(current_token,"XI2_0"))
							{
								*value_address=2;
								shift_Parse_state(state,1);
							}
							else
							{
								if (fuzzy_string_compare(current_token,"XI2_1"))
								{
									*value_address=3;
									shift_Parse_state(state,1);
								}
								else
								{
									if (fuzzy_string_compare(current_token,"XI3_0"))
									{
										*value_address=4;
										shift_Parse_state(state,1);
									}
									else
									{
										if (fuzzy_string_compare(current_token,"XI3_1"))
										{
											*value_address=5;
											shift_Parse_state(state,1);
										}
										else
										{
											*value_address=-1;
										}
									}
								}
							}
						}
					}
				}
				else
				{
					display_message(ERROR_MESSAGE,"set_exterior.  Missing value_address");
					return_code=0;
				}
			}
			else
			{
				display_message(INFORMATION_MESSAGE,
					" <xi1_0|xi1_1|xi2_0|xi2_1|xi3_0|xi3_1>");
				return_code=1;
			}
		}
		else
		{
			value_address=(int *)value_address_void;
			if (value_address)
			{
				*value_address=1;
				return_code=1;
			}
			else
			{
				display_message(ERROR_MESSAGE,"set_exterior.  Missing value_address");
				return_code=0;
			}
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,"set_exterior.  Missing state");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* set_exterior */

int read_circle_discretization_defaults(int *default_value,
	struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 11 December 1998

DESCRIPTION :
Reads that maximum and default number of line segments used to approximate
a circle. Minimum is always 2, but this does not look much like a circle!
???RC Changed so that does not report error if no user_interface - just returns
the defaults of 6 and 64. This is bad - should be able to read defaults without
any x application - incl. for compatibility with win32.
==============================================================================*/
{
	int return_code;
#if defined (MOTIF_USER_INTERFACE)
#define XmNdefaultCircleDiscretization "defaultCircleDiscretization"
#define XmCDefaultCircleDiscretization "DefaultCircleDiscretization"
#define XmNmaximumCircleDiscretization "maximumCircleDiscretization"
#define XmCMaximumCircleDiscretization "MaximumCircleDiscretization"
	static int resources_read=0; /* flag so resources only read once */
	static struct Discretization
	{
		int default_value;
	} discretization;
	static XtResource resources[]=
	{
		{
			XmNdefaultCircleDiscretization,
			XmCDefaultCircleDiscretization,
			XmRInt,
			sizeof(int),
			XtOffsetOf(struct Discretization,default_value),
			XmRString,
			"6"
		}
	};
#endif /* defined (MOTIF_USER_INTERFACE) */

	ENTER(read_circle_discretization_defaults);
#if !defined (MOTIF_USER_INTERFACE)
	USE_PARAMETER(user_interface);
#endif /* !defined (MOTIF_USER_INTERFACE) */
	if (default_value)
	{
#if defined (MOTIF_USER_INTERFACE)
		if (!resources_read)
		{
			if (user_interface)
			{
				/* retrieve the settings */
				XtVaGetApplicationResources(User_interface_get_application_shell(user_interface),
					&discretization,resources,XtNumber(resources),NULL);
				resources_read=1;
			}
			else
			{
				discretization.default_value=6;
			}
		}
		*default_value=discretization.default_value;
#else /* defined (MOTIF_USER_INTERFACE) */
		*default_value=6;
#endif /* defined (MOTIF_USER_INTERFACE) */
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"read_circle_discretization_defaults.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* read_circle_discretization_defaults */

int check_Circle_discretization(int *circle_discretization,
	struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 11 December 1998

DESCRIPTION :
Ensures the <circle_discretization> is within the bounds of the minimum of 2
and the maximum read in from the defaults.
???RC user_interface argument not checked as may not be needed in
read_Circle_discretization_defaults().
==============================================================================*/
{
	int return_code;
	int default_value,initial_value;

	ENTER(check_Circle_discretization);
	if (circle_discretization)
	{
		if (0 != (return_code=read_circle_discretization_defaults(&default_value,
			user_interface)))
		{
			initial_value = *circle_discretization;
			if (2 > *circle_discretization)
			{
				*circle_discretization=2;
			}
			if (*circle_discretization != initial_value)
			{
				display_message(WARNING_MESSAGE,
					"Circle discretization values must be at least 2\n"
					"%d changed to %d",initial_value,
					*circle_discretization);
			}
			return_code=1;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"check_Circle_discretization.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return return_code;
} /* check_Circle_discretization */

int set_Circle_discretization(struct Parse_state *state,
	void *circle_discretization_void,void *user_interface_void)
/*******************************************************************************
LAST MODIFIED : 27 April 1999

DESCRIPTION :
A modifier function for setting number of segments used to draw circles.
==============================================================================*/
{
	const char *current_token;
	int return_code;
	int *circle_discretization;
	struct User_interface *user_interface;

	ENTER(set_Circle_discretization);
	if (state)
	{
		current_token=state->current_token;
		if (current_token)
		{
			if (strcmp(PARSER_HELP_STRING,current_token)&&
				strcmp(PARSER_RECURSIVE_HELP_STRING,current_token))
			{
				user_interface=(struct User_interface *)user_interface_void;
				circle_discretization = (int *)circle_discretization_void;
				if (circle_discretization)
				{
					if (1==sscanf(current_token,"%d",circle_discretization))
					{
						shift_Parse_state(state,1);
						return_code=check_Circle_discretization(circle_discretization,
							user_interface);
					}
					else
					{
						display_message(ERROR_MESSAGE,"Invalid discretization: %s",
							current_token);
						display_parse_state_location(state);
						return_code=0;
					}
				}
				else
				{
					display_message(ERROR_MESSAGE,
						"set_Circle_discretization.  Missing circle_discretization");
					return_code=0;
				}
			}
			else
			{
				circle_discretization = (int *)circle_discretization_void;
				if (circle_discretization)
				{
					/* use negative value to signal that it is not determined which
						 current value is the default */
					if (0 > *circle_discretization)
					{
						display_message(INFORMATION_MESSAGE," #[CURRENT]{integer}");
					}
					else
					{
						display_message(INFORMATION_MESSAGE,
							" #[%d]{integer}",*circle_discretization);
					}
				}
				else
				{
					display_message(INFORMATION_MESSAGE," #{integer}");
				}
				return_code=1;
			}
		}
		else
		{
			display_message(ERROR_MESSAGE,"Missing circle discretization");
			display_parse_state_location(state);
			return_code=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,"set_Circle_discretization.  Missing state");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* set_Circle_discretization */

int read_element_discretization_defaults(int *default_value,
	struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 11 December 1998

DESCRIPTION :
Reads that maximum and default number of line segments used to approximate
element curves. Minimum is always 1.
???RC Changed so that does not report error if no user_interface - just returns
the defaults of 4 and 50. This is bad - should be able to read defaults without
any x application - incl. for compatibility with win32.
==============================================================================*/
{
	int return_code;
#if defined (MOTIF_USER_INTERFACE)
#define XmNdefaultElementDiscretization "defaultElementDiscretization"
#define XmCDefaultElementDiscretization "DefaultElementDiscretization"
#define XmNmaximumElementDiscretization "maximumElementDiscretization"
#define XmCMaximumElementDiscretization "MaximumElementDiscretization"
	static int resources_read=0; /* flag so resources only read once */
	static struct Discretization
	{
		int default_value;
	} discretization;
	static XtResource resources[]=
	{
		{
			XmNdefaultElementDiscretization,
			XmCDefaultElementDiscretization,
			XmRInt,
			sizeof(int),
			XtOffsetOf(struct Discretization,default_value),
			XmRString,
			"4"
		}
	};
#endif /* defined (MOTIF_USER_INTERFACE) */

	ENTER(read_element_discretization_defaults);
#if !defined (MOTIF_USER_INTERFACE)
	USE_PARAMETER(user_interface);
#endif /* !defined (MOTIF_USER_INTERFACE) */
	if (default_value)
	{
#if defined (MOTIF_USER_INTERFACE)
		if (!resources_read)
		{
			/* Some functions call this without the user_interface, 
			   we want those to use the values that may have already been read. */
			if (user_interface)
			{
				/* retrieve the settings */
				XtVaGetApplicationResources(User_interface_get_application_shell(user_interface),
					&discretization,resources,XtNumber(resources),NULL);
				resources_read=1;
			}
			else
			{
				discretization.default_value=4;
			}
		}
		*default_value=discretization.default_value;
#else /* defined (MOTIF_USER_INTERFACE) */
		*default_value=4;
#endif /* defined (MOTIF_USER_INTERFACE) */
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"read_element_discretization_defaults.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* read_element_discretization_defaults */

int check_Element_discretization(struct Element_discretization
	*element_discretization,struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 11 December 1998

DESCRIPTION :
Ensures the <element_discretization> is within the bounds of the minimum of 1
and the maximum read in from the defaults.
???DB.  Changed the lower bound to 1 because also used for elements.
???DB.  Need to make consistent.
???RC user_interface argument not checked as may not be needed in
read_Element_discretization_defaults().
==============================================================================*/
{
	int discretization_change,return_code;
	struct Element_discretization initial;
	int default_value;

	ENTER(check_Element_discretization);
	if (element_discretization)
	{
		if (0 != (return_code=read_element_discretization_defaults(&default_value,
			user_interface)))
		{
			discretization_change=0;
			initial.number_in_xi1=element_discretization->number_in_xi1;
			initial.number_in_xi2=element_discretization->number_in_xi2;
			initial.number_in_xi3=element_discretization->number_in_xi3;
			if (1 > element_discretization->number_in_xi1)
			{
				element_discretization->number_in_xi1=1;
				discretization_change=1;
			}
			if (1 > element_discretization->number_in_xi2)
			{
				element_discretization->number_in_xi2=1;
				discretization_change=1;
			}
			if (1 > element_discretization->number_in_xi3)
			{
				element_discretization->number_in_xi3=1;
				discretization_change=1;
			}
			if (discretization_change)
			{
				display_message(WARNING_MESSAGE,
					"Element discretization values must be at least 1\n"
					"%d*%d*%d changed to %d*%d*%d",
					initial.number_in_xi1,initial.number_in_xi2,initial.number_in_xi3,
					element_discretization->number_in_xi1,
					element_discretization->number_in_xi2,
					element_discretization->number_in_xi3);
			}
			return_code=1;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"check_Element_discretization.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return return_code;
} /* check_Element_discretization */

int set_Element_discretization(struct Parse_state *state,
	void *element_discretization_void,void *user_interface_void)
/*******************************************************************************
LAST MODIFIED : 27 April 1999

DESCRIPTION :
A modifier function for setting discretization in each element direction.
==============================================================================*/
{
	const char *current_token;
	int return_code,multiple_default;
	struct Element_discretization *element_discretization;
	struct User_interface *user_interface;

	ENTER(set_Element_discretization);
	if (state)
	{
		current_token=state->current_token;
		if (current_token)
		{
			if (strcmp(PARSER_HELP_STRING,current_token)&&
				strcmp(PARSER_RECURSIVE_HELP_STRING,current_token))
			{
				user_interface=(struct User_interface *)user_interface_void;
				element_discretization=
					(struct Element_discretization *)element_discretization_void;
				if (element_discretization)
				{
					if (1==sscanf(current_token,"%d",
						&(element_discretization->number_in_xi1)))
					{
						if (2==sscanf(current_token,"%d*%d",
							&(element_discretization->number_in_xi1),
							&(element_discretization->number_in_xi2)))
						{
							if (3!=sscanf(current_token,"%d*%d*%d",
								&(element_discretization->number_in_xi1),
								&(element_discretization->number_in_xi2),
								&(element_discretization->number_in_xi3)))
							{
								element_discretization->number_in_xi3=
									element_discretization->number_in_xi2;
							}
						}
						else
						{
							element_discretization->number_in_xi2=
								element_discretization->number_in_xi1;
							element_discretization->number_in_xi3=
								element_discretization->number_in_xi2;
						}
						shift_Parse_state(state,1);
						return_code=check_Element_discretization(element_discretization,
							user_interface);
					}
					else
					{
						display_message(ERROR_MESSAGE,"Invalid discretization: %s",
							current_token);
						display_parse_state_location(state);
						return_code=0;
					}
				}
				else
				{
					display_message(ERROR_MESSAGE,
						"set_Element_discretization.  Missing element_discretization");
					return_code=0;
				}
			}
			else
			{
				/* write help */
				element_discretization=
					(struct Element_discretization *)element_discretization_void;
				if (element_discretization)
				{
					multiple_default=
						(0>element_discretization->number_in_xi1)
						||(0>element_discretization->number_in_xi2)
						||(0>element_discretization->number_in_xi3);
					if (element_discretization->number_in_xi3)
					{
						if (multiple_default)
						{
							display_message(INFORMATION_MESSAGE,
								" #*#*#[CURRENT]{integer*integer*integer}");
						}
						else
						{
							display_message(INFORMATION_MESSAGE,
								" #*#*#[%d*%d*%d]{integer*integer*integer}",
								element_discretization->number_in_xi1,
								element_discretization->number_in_xi2,
								element_discretization->number_in_xi3);
						}
					}
					else
					{
						if (element_discretization->number_in_xi2)
						{
							if (multiple_default)
							{
								display_message(INFORMATION_MESSAGE,
									" #*#[CURRENT]{integer*integer}");
							}
							else
							{
								display_message(INFORMATION_MESSAGE,
									" #*#[%d*%d]{integer*integer}",
									element_discretization->number_in_xi1,
									element_discretization->number_in_xi2);
							}
						}
						else
						{
							if (multiple_default)
							{
								display_message(INFORMATION_MESSAGE," #[CURRENT]{integer}");
							}
							else
							{
								display_message(INFORMATION_MESSAGE," #[%d]{integer}",
									element_discretization->number_in_xi1);
							}
						}
					}
				}
				else
				{
					display_message(INFORMATION_MESSAGE,
						" #*#*#{integer*integer*integer}");
				}
				return_code=1;
			}
		}
		else
		{
			display_message(ERROR_MESSAGE,"Missing element discretization");
			display_parse_state_location(state);
			return_code=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,"set_Element_discretization.  Missing state");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* set_Element_discretization */

PROTOTYPE_ENUMERATOR_STRING_FUNCTION(Streamline_type)
{
	const char *enumerator_string;

	ENTER(ENUMERATOR_STRING(Streamline_type));
	switch (enumerator_value)
	{
		case STREAM_EXTRUDED_ELLIPSE:
		{
			enumerator_string = "ellipse";
		} break;
		case STREAM_LINE:
		{
			enumerator_string = "line";
		} break;
		case STREAM_EXTRUDED_RECTANGLE:
		{
			enumerator_string = "rectangle";
		} break;
		case STREAM_RIBBON:
		{
			enumerator_string = "ribbon";
		} break;
		case STREAM_EXTRUDED_CIRCLE:
		{
			enumerator_string = "cylinder";
		} break;
		default:
		{
			enumerator_string = (const char *)NULL;
		} break;
	}
	LEAVE;

	return (enumerator_string);
} /* ENUMERATOR_STRING(Streamline_type) */

DEFINE_DEFAULT_ENUMERATOR_FUNCTIONS(Streamline_type)

PROTOTYPE_ENUMERATOR_STRING_FUNCTION(Streamline_data_type)
{
	const char *enumerator_string;

	ENTER(ENUMERATOR_STRING(Streamline_data_type));
	switch (enumerator_value)
	{
		case STREAM_NO_DATA:
		{
			enumerator_string = "no_data";
		} break;
		case STREAM_FIELD_SCALAR:
		{
			enumerator_string = "field_scalar";
		} break;
		case STREAM_MAGNITUDE_SCALAR:
		{
			enumerator_string = "magnitude_scalar";
		} break;
		case STREAM_TRAVEL_SCALAR:
		{
			enumerator_string = "travel_scalar";
		} break;
		default:
		{
			enumerator_string = (const char *)NULL;
		} break;
	}
	LEAVE;

	return (enumerator_string);
} /* ENUMERATOR_STRING(Streamline_data_type) */

DEFINE_DEFAULT_ENUMERATOR_FUNCTIONS(Streamline_data_type)

PROTOTYPE_ENUMERATOR_STRING_FUNCTION(Cmiss_graphics_render_type)
{
	const char *enumerator_string;

	ENTER(ENUMERATOR_STRING(Cmiss_graphics_render_type));
	switch (enumerator_value)
	{
		case CMISS_GRAPHICS_RENDER_TYPE_SHADED:
		{
			enumerator_string = "render_shaded";
		} break;
		case CMISS_GRAPHICS_RENDER_TYPE_WIREFRAME:
		{
			enumerator_string = "render_wireframe";
		} break;
		default:
		{
			enumerator_string = (const char *)NULL;
		} break;
	}
	LEAVE;

	return (enumerator_string);
} /* ENUMERATOR_STRING(Cmiss_graphic_render_type) */

DEFINE_DEFAULT_ENUMERATOR_FUNCTIONS(Cmiss_graphics_render_type)

PROTOTYPE_ENUMERATOR_STRING_FUNCTION(Xi_discretization_mode)
{
	const char *enumerator_string;

	ENTER(ENUMERATOR_STRING(Xi_discretization_mode));
	switch (enumerator_value)
	{
		case XI_DISCRETIZATION_CELL_CENTRES:
		{
			enumerator_string = "cell_centres";
		} break;
		case XI_DISCRETIZATION_CELL_CORNERS:
		{
			enumerator_string = "cell_corners";
		} break;
		case XI_DISCRETIZATION_CELL_DENSITY:
		{
			enumerator_string = "cell_density";
		} break;
		case XI_DISCRETIZATION_CELL_POISSON:
		{
			enumerator_string = "cell_poisson";
		} break;
		case XI_DISCRETIZATION_CELL_RANDOM:
		{
			enumerator_string = "cell_random";
		} break;
		case XI_DISCRETIZATION_EXACT_XI:
		{
			enumerator_string = "exact_xi";
		} break;
		default:
		{
			enumerator_string = (const char *)NULL;
		} break;
	}
	LEAVE;

	return (enumerator_string);
} /* ENUMERATOR_STRING(Xi_discretization_mode) */

DEFINE_DEFAULT_ENUMERATOR_FUNCTIONS(Xi_discretization_mode)

