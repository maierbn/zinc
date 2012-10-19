/*******************************************************************************
 * material.hpp
 */
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
#if !defined (MATERIAL_HPP)
#define MATERIAL_HPP

#include "general/callback_class.hpp"

class Render_graphics_opengl;
struct Graphical_material;

struct Material_order_independent_transparency
/*******************************************************************************
LAST MODIFIED : 2 May 2005

DESCRIPTION :
Data for compiling materials specially for order independent transparency.
==============================================================================*/
{
	int layer;
	Render_graphics_opengl *renderer;
}; /* struct Material_order_independent_transparency */

int Material_render_opengl(struct Graphical_material *material,
	Render_graphics_opengl *renderer);

int Material_compile_members_opengl(Graphical_material *material,
	Render_graphics_opengl *renderer);

int Material_compile_opengl_display_list(Graphical_material *material,
	Callback_base< Graphical_material* > *execute_function,
	Render_graphics_opengl *renderer);

int Material_execute_opengl_display_list(Graphical_material *material,
	Render_graphics_opengl *renderer);

#endif // MATERIAL_HPP