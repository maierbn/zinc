/***************************************************************************//**
 * scene.hpp
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#if !defined (CMZN_SCENE_HPP)
#define CMZN_SCENE_HPP

#include "general/callback_class.hpp"

struct cmzn_scene;
class Render_graphics_compile_members;
class Render_graphics_opengl;

int cmzn_scene_compile_scene(cmzn_scene *scene,
	Render_graphics_compile_members *renderer);

int cmzn_scene_compile_graphics(cmzn_scene *scene,
	Render_graphics_compile_members *renderer, int force_rebuild);

int execute_scene_exporter_output(struct cmzn_scene *scene,
	Render_graphics_opengl *renderer);

int execute_scene_threejs_output(struct cmzn_scene *scene,
	Render_graphics_opengl *renderer);

int execute_cmzn_scene(cmzn_scene *scene,
	Render_graphics_opengl *renderer);

int cmzn_scene_graphics_render_opengl(struct cmzn_scene *scene,
	Render_graphics_opengl *renderer);

int cmzn_scene_render_child_scene(struct cmzn_scene *scene,
	Render_graphics_opengl *renderer);

int Scene_render_opengl(cmzn_scene *scene, Render_graphics_opengl *renderer);

int cmzn_scene_compile_tree(cmzn_scene *scene,
	Render_graphics_compile_members *renderer);

int build_Scene(cmzn_scene *scene, cmzn_scenefilter *filter);

#endif /* !defined (CMZN_SCENE_HPP) */
