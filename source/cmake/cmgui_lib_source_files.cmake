
# Defines CMGUI_LIB_SRCS
SET( CMGUI_LIB_BASE_SRCS ${API_SRCS} ${COMFILE_SRCS} ${COMMAND_SRCS}
	${COMPUTED_FIELD_SRCS} ${CONTEXT_SRCS} ${CURVE_SRCS} ${ELEMENT_SRCS} ${EMOTER_SRCS}
	${FIELD_IO_SRCS} ${FINITE_ELEMENT_GRAPHICS_SRCS} ${FINITE_ELEMENT_ADDITIONAL_SRCS} ${GENERAL_SRCS}
	${GRAPHICS_SRCS} ${HELP_SRCS} ${IMAGE_PROCESSING_SRCS} ${INTERACTION_SRCS}
	${IO_DEVICES_SRCS} ${NODE_SRCS} ${SELECTION_SRCS} ${INTERPRETER_SRCS} 
	${THREE_D_DRAWING_SRCS} ${TIME_SRCS} ${USER_INTERFACE_SRCS} ${OPENCASCADE_SRCS} )
SET( CMGUI_LIB_BASE_HDRS ${API_HDRS} ${COMFILE_HDRS} ${COMMAND_HDRS}
	${COMPUTED_FIELD_HDRS} ${CONTEXT_HDRS} ${CURVE_HDRS} ${ELEMENT_HDRS} ${EMOTER_HDRS}
	${FIELD_IO_HDRS} ${FINITE_ELEMENT_GRAPHICS_HDRS} ${FINITE_ELEMENT_ADDITIONAL_HDRS} ${GENERAL_HDRS}
	${GRAPHICS_HDRS} ${HELP_HDRS} ${IMAGE_PROCESSING_HDRS} ${INTERACTION_HDRS}
	${IO_DEVICES_HDRS} ${NODE_HDRS} ${SELECTION_HDRS} ${INTERPRETER_HDRS} 
	${THREE_D_DRAWING_HDRS} ${TIME_HDRS} ${USER_INTERFACE_HDRS} ${OPENCASCADE_HDRS} )

IF( WX_USER_INTERFACE )
	SET( CMGUI_LIB_INTERFACE_SRCS ${API_INTERFACE_SRCS} ${COLOUR_INTERFACE_SRCS}
		${COMFILE_INTERFACE_SRCS}
		${COMMAND_INTERFACE_SRCS} ${COMPUTED_FIELD_INTERFACE_SRCS}
		${GRAPHICS_INTERFACE_SRCS}
		${MATERIAL_INTERFACE_SRCS} ${NODE_INTERFACE_SRCS}
		${REGION_INTERFACE_SRCS}
		${TRANSFORMATION_INTERFACE_SRCS} )
	SET( CMGUI_LIB_INTERFACE_HDRS ${API_INTERFACE_HDRS} ${COLOUR_INTERFACE_HDRS}
		${COMFILE_INTERFACE_HDRS}
		${COMMAND_INTERFACE_HDRS} ${COMPUTED_FIELD_INTERFACE_HDRS}
		${GRAPHICS_INTERFACE_HDRS}
		${MATERIAL_INTERFACE_HDRS} ${NODE_INTERFACE_HDRS}
		${REGION_INTERFACE_HDRS}
		${TRANSFORMATION_INTERFACE_HDRS} )
ENDIF( WX_USER_INTERFACE )

SET( CMGUI_LIB_SRCS
	${CMGUI_LIB_BASE_SRCS}
	${CMGUI_LIB_INTERFACE_SRCS} )

SET( CMGUI_LIB_HDRS
	${CMGUI_LIB_BASE_HDRS}
	${CMGUI_LIB_INTERFACE_HDRS}
	${wxWidgets_GENERATED_HDRS} )
#	source/api/cmiss_command_data.c
#	source/api/cmiss_region.c )

#IF( ${GRAPHICS_API} MATCHES OPENGL_GRAPHICS )
#	SET( CMGUI_LIB_SRCS ${CMGUI_LIB_SRCS}
#		source/api/cmiss_scene_viewer.cpp
#		source/api/cmiss_texture.c )
#ENDIF( ${GRAPHICS_API} MATCHES OPENGL_GRAPHICS )
 
