/*******************************************************************************
FILE : indexed_list_private.h

LAST MODIFIED : 24 September 1999

DESCRIPTION :
Macros for declaring indexed list types and declaring indexed list functions.
This file contains the details of the internal workings of the list and should
be included in the module that declares the list for a particular object type,
but should not be visible to modules that use lists for a particular object
type.

This list differs from the standard list (see list_private.h) in that it has an
B-tree for faster searching on an index.
==============================================================================*/
#if !defined (INDEXED_LIST_PRIVATE_H)
#define INDEXED_LIST_PRIVATE_H

#include "general/list.h"

/*
Constants
=========
*/
#define B_TREE_ORDER 5

/*
Macros
======
*/

/*
Local types
-----------
*/
#if defined (FULL_NAMES)
#define INDEX_NODE( object_type )  index_node_ ## object_type
#else
#define INDEX_NODE( object_type )  in ## object_type
#endif

#define FULL_DECLARE_INDEXED_LIST_TYPE( object_type ) \
struct INDEX_NODE(object_type) \
/***************************************************************************** \
LAST MODIFIED : 8 April 1996 \
\
DESCRIPTION : \
A node in the index set for the object_type list.  The maximum \
<number_of_indices> is 2*B_TREE_ORDER.  If the node is a leaf node then \
<children> is NULL, otherwise, if there are k <indices> then there are k+1 \
<children>.  Only the leaf nodes ACCESS the objects \
============================================================================*/ \
{ \
	int number_of_indices; \
	/* break point indices (accesed through objects) */ \
	struct object_type **indices; \
	/* the parent node */ \
	struct INDEX_NODE(object_type) *parent; \
	/* the child nodes */ \
	struct INDEX_NODE(object_type) **children; \
}; /* struct INDEX_NODE(object_type) */ \
\
DECLARE_LIST_TYPE(object_type) \
/***************************************************************************** \
LAST MODIFIED : 8 April 1996 \
\
DESCRIPTION : \
The object_type list structure. \
============================================================================*/ \
{ \
	/* the number of objects in the list */ \
	int count; \
	/* the index set for the list */ \
	struct INDEX_NODE(object_type) *index; \
} /* struct LIST(object_type) */

/*
Local functions
---------------
*/
#define CREATE_INDEX_NODE( object_type )  CREATE(INDEX_NODE(object_type))

#define DECLARE_CREATE_INDEX_NODE_FUNCTION( object_type ) \
static struct INDEX_NODE(object_type) *CREATE_INDEX_NODE(object_type)( \
	int leaf) \
/***************************************************************************** \
LAST MODIFIED : 8 April 1996 \
\
DESCRIPTION : \
Creates an object_type index node. \
============================================================================*/ \
{ \
	struct INDEX_NODE(object_type) *node; \
\
	ENTER(CREATE_INDEX_NODE(object_type)); \
	if (ALLOCATE(node,struct INDEX_NODE(object_type),1)&& \
		ALLOCATE(node->indices,struct object_type *,2*B_TREE_ORDER)) \
	{ \
		if (leaf) \
		{ \
			node->children=(struct INDEX_NODE(object_type) **)NULL; \
		} \
		else \
		{ \
			if (!ALLOCATE(node->children,struct INDEX_NODE(object_type) *, \
				2*B_TREE_ORDER+1)) \
			{ \
				DEALLOCATE(node->indices); \
				DEALLOCATE(node); \
				display_message(ERROR_MESSAGE, \
		"CREATE_INDEX_NODE(" #object_type ").  Insufficient memory for children"); \
			} \
		} \
		if (node) \
		{ \
			node->parent=(struct INDEX_NODE(object_type) *)NULL; \
			node->number_of_indices=0; \
		} \
	} \
	else \
	{ \
		if (node) \
		{ \
			DEALLOCATE(node); \
		} \
		display_message(ERROR_MESSAGE, \
			"CREATE_INDEX_NODE(" #object_type ").  Insufficient memory for node"); \
	} \
	LEAVE; \
\
	return (node); \
} /* CREATE_INDEX_NODE(object_type) */

#define DESTROY_INDEX_NODE( object_type )  DESTROY(INDEX_NODE(object_type))

#define DECLARE_DESTROY_INDEX_NODE_FUNCTION( object_type ) \
static int DESTROY_INDEX_NODE(object_type)( \
	struct INDEX_NODE(object_type) **node_address) \
/***************************************************************************** \
LAST MODIFIED : 5 November 1997 \
\
DESCRIPTION : \
Destroys an object_type index node. \
============================================================================*/ \
{ \
	int i,return_code; \
	struct INDEX_NODE(object_type) **child; \
	struct object_type **index; \
\
	ENTER(DESTROY_INDEX_NODE(object_type)); \
	if (node_address) \
	{ \
		if (*node_address) \
		{ \
			if (child=(*node_address)->children) \
			{ \
				/* not a leaf node */ \
				for (i=(*node_address)->number_of_indices;i>=0;i--) \
				{ \
					DESTROY_INDEX_NODE(object_type)(child); \
					child++; \
				} \
				DEALLOCATE((*node_address)->children); \
			} \
			else \
			{ \
				/* leaf node */ \
				index=(*node_address)->indices; \
				for (i=(*node_address)->number_of_indices;i>0;i--) \
				{ \
					DEACCESS(object_type)(index); \
					index++; \
				} \
			} \
			DEALLOCATE((*node_address)->indices); \
			DEALLOCATE(*node_address); \
		} \
		return_code=1; \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"DESTROY_INDEX_NODE(" #object_type ").  Invalid argument"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* DESTROY_INDEX_NODE(object_type) */

#if defined (FULL_NAMES)
#define DUPLICATE_INDEX_NODE( object_type ) \
	duplicate_index_node_ ## object_type
#else
#define DUPLICATE_INDEX_NODE( object_type )  dpin ## object_type
#endif

#define DECLARE_DUPLICATE_INDEX_NODE_FUNCTION( object_type ) \
static struct INDEX_NODE(object_type) *DUPLICATE_INDEX_NODE(object_type)( \
	struct INDEX_NODE(object_type) *node,struct INDEX_NODE(object_type) *parent) \
/***************************************************************************** \
LAST MODIFIED : 24 September 1999 \
\
DESCRIPTION : \
Duplicates an object_type index node, attaching it to the <parent>. \
============================================================================*/ \
{ \
	int i,leaf; \
	struct INDEX_NODE(object_type) **child,**duplicate_child,*duplicate_node; \
	struct object_type **duplicate_index,**index; \
\
	ENTER(DUPLICATE_INDEX_NODE(object_type)); \
	if (node) \
	{ \
		if (node->children) \
		{ \
			leaf=0; \
		} \
		else \
		{ \
			leaf=1; \
		} \
		if (duplicate_node=CREATE_INDEX_NODE(object_type)(leaf)) \
		{ \
			duplicate_node->parent=parent; \
			duplicate_node->number_of_indices=node->number_of_indices; \
			index=node->indices; \
			duplicate_index=duplicate_node->indices; \
			if (leaf) \
			{ \
				/* leaf node */ \
				for (i=node->number_of_indices;i>0;i--) \
				{ \
					*duplicate_index=ACCESS(object_type)(*index); \
					index++; \
					duplicate_index++; \
				} \
			} \
			else \
			{ \
				/* not a leaf node */ \
				child=node->children; \
				duplicate_child=duplicate_node->children; \
				i=node->number_of_indices; \
				while ((i>0)&&(*duplicate_child=DUPLICATE_INDEX_NODE(object_type)( \
					*child,duplicate_node))) \
				{ \
					*duplicate_index= *index; \
					index++; \
					duplicate_index++; \
					child++; \
					duplicate_child++; \
					i--; \
				} \
				if ((i>0)||!(*duplicate_child=DUPLICATE_INDEX_NODE(object_type)( \
					*child,duplicate_node))) \
				{ \
					duplicate_node->number_of_indices=(node->number_of_indices)-i; \
					DESTROY_INDEX_NODE(object_type)(&duplicate_node); \
					display_message(ERROR_MESSAGE, \
					"DUPLICATE_INDEX_NODE(" #object_type ").  Error duplicating node"); \
				} \
			} \
		} \
		else \
		{ \
			display_message(ERROR_MESSAGE, \
	"DUPLICATE_INDEX_NODE(" #object_type ").  Could not create duplicate node"); \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"DUPLICATE_INDEX_NODE(" #object_type ").  Invalid argument"); \
		duplicate_node=(struct INDEX_NODE(object_type) *)NULL; \
	} \
	LEAVE; \
\
	return (duplicate_node); \
} /* DUPLICATE_INDEX_NODE(object_type) */

#if defined (FULL_NAMES)
#define FIND_LEAF_NODE_IN_INDEX( object_type ) \
	find_leaf_node_in_index_ ## object_type
#else
#define FIND_LEAF_NODE_IN_INDEX( object_type )  fli ## object_type
#endif

#define DECLARE_FIND_LEAF_NODE_IN_INDEX_FUNCTION( object_type , \
	identifier, identifier_type , compare_function ) \
static struct INDEX_NODE(object_type) *FIND_LEAF_NODE_IN_INDEX(object_type)( \
	identifier_type identifier,struct INDEX_NODE(object_type) *index) \
/***************************************************************************** \
LAST MODIFIED : 10 April 1996 \
\
DESCRIPTION : \
Finds the leaf node that will contain the object with the specified \
<identifier> if the object is in the list. \
============================================================================*/ \
{ \
	int i; \
	struct object_type **object_index; \
	struct INDEX_NODE(object_type) **child,*node; \
\
	ENTER(FIND_LEAF_NODE_IN_INDEX(object_type)); \
	if (node=index) \
	{ \
		if (child=node->children) \
		{ \
			i=node->number_of_indices; \
			object_index=node->indices; \
			while ((i>0)&&(compare_function(identifier, \
				(*object_index)->identifier)>0)) \
			{ \
				object_index++; \
				child++; \
				i--; \
			} \
			node=FIND_LEAF_NODE_IN_INDEX(object_type)(identifier,*child); \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE,"FIND_LEAF_NODE_IN_INDEX(" #object_type \
			").  Invalid argument"); \
	} \
	LEAVE; \
\
	return (node); \
} /* FIND_LEAF_NODE_IN_INDEX(object_type) */

#if defined (FULL_NAMES)
#define REMOVE_OBJECT_FROM_INDEX( object_type ) \
	index_remove_object_ ## object_type
#else
#define REMOVE_OBJECT_FROM_INDEX( object_type )  rin ## object_type
#endif

#define DECLARE_REMOVE_OBJECT_FROM_INDEX_FUNCTION( object_type , identifier, \
	compare_function ) \
static int REMOVE_OBJECT_FROM_INDEX(object_type)(struct object_type *object, \
	struct INDEX_NODE(object_type) **index_address) \
/***************************************************************************** \
LAST MODIFIED : 5 November 1997 \
\
DESCRIPTION : \
Removes an <object> from the <index>. \
============================================================================*/ \
{ \
	int i,j,return_code; \
	struct INDEX_NODE(object_type) *child,*index; \
\
	ENTER(REMOVE_OBJECT_FROM_INDEX(object_type)); \
	return_code=0; \
	/* check the arguments */ \
	if (index_address&&object) \
	{ \
    if (index= *index_address) \
		{ \
			/* find the object within the node */ \
			i=0; \
			while ((i<index->number_of_indices)&&(0<compare_function( \
				object->identifier,(index->indices)[i]->identifier))) \
			{ \
				i++; \
			} \
  		if (index->children) \
  		{ \
				if (return_code=REMOVE_OBJECT_FROM_INDEX(object_type)(object, \
					&(index->children[i]))) \
        { \
					/* shuffle down any NULL children */ \
					if (!(index->children[i])) \
					{ \
						(index->number_of_indices)--; \
						for (j=i;j<=index->number_of_indices;j++) \
						{ \
							if (j<index->number_of_indices) \
							{ \
								index->indices[j]=index->indices[j+1]; \
							} \
							index->children[j]=index->children[j+1]; \
						} \
					} \
					if (0==index->number_of_indices) \
					{ \
						/* one child - chain from parent */ \
						/* first give it its parents parent */ \
						index->children[0]->parent=index->parent; \
						/* now chain from parent */ \
						*index_address=index->children[0]; \
						/* ensure the destroy does not chain */ \
						index->number_of_indices--; \
						/* this next line is redundant */ \
						index->children[0]=(struct INDEX_NODE(object_type) *)NULL; \
						DESTROY_INDEX_NODE(object_type)(&index); \
					} \
					else \
					{ \
						/* make sure that not using a removed object as an index */ \
						if ((i<index->number_of_indices)&&(object==(index->indices)[i])) \
						{ \
							/* find the last object in the child */ \
							child=(index->children)[i]; \
							while (child->children) \
							{ \
								child=(child->children)[child->number_of_indices]; \
							} \
							index->indices[i]= \
								(child->indices)[(child->number_of_indices)-1]; \
						} \
					} \
				} \
			} \
			else \
			{ \
				if ((i<index->number_of_indices)&&(object==(index->indices)[i])) \
				{ \
					DEACCESS(object_type)(&(index->indices[i])); \
					(index->number_of_indices)--; \
					if (0==index->number_of_indices) \
					{ \
						DESTROY_INDEX_NODE(object_type)(index_address); \
					} \
					else \
					{ \
						for (j=i;j<index->number_of_indices;j++) \
						{ \
							index->indices[j]=index->indices[j+1]; \
						} \
					} \
					return_code=1; \
				} \
			} \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"REMOVE_OBJECT_FROM_INDEX(" #object_type ").  Invalid argument(s)"); \
	} \
	LEAVE; \
\
	return (return_code); \
} /* REMOVE_OBJECT_FROM_INDEX(object_type) */

#if defined (OLD_CODE)
#define DECLARE_REMOVE_OBJECT_FROM_INDEX_FUNCTION( object_type , identifier, \
	compare_function ) \
static int REMOVE_OBJECT_FROM_INDEX(object_type)(struct object_type *object, \
	struct INDEX_NODE(object_type) **index) \
/***************************************************************************** \
LAST MODIFIED : 21 July 1996 \
\
DESCRIPTION : \
Removes an <object> from the <index>. \
============================================================================*/ \
{ \
	int i,return_code; \
	struct INDEX_NODE(object_type) **child,*node; \
	struct object_type **node_index; \
\
	ENTER(REMOVE_OBJECT_FROM_INDEX(object_type)); \
	/* find the leaf node that may contain the object */ \
	node=FIND_LEAF_NODE_IN_INDEX(object_type)(object->identifier,*index); \
	/* find the object within the node */ \
	node_index=node->indices; \
	i=node->number_of_indices; \
	while ((i>0)&&(0<compare_function(object->identifier, \
		(*node_index)->identifier))) \
	{ \
		i--; \
		node_index++; \
	} \
	if ((i>0)&&(object== *node_index)) \
	{ \
		/* remove the object */ \
		if (1==node->number_of_indices) \
		{ \
			if (node->parent) \
			{ \
				node=node->parent; \
				while ((node->parent)&&(0==node->number_of_indices)) \
				{ \
					node=node->parent; \
				} \
				node_index=node->indices; \
				i=node->number_of_indices; \
				while ((i>0)&&(0<compare_function(object->identifier, \
					(*node_index)->identifier))) \
				{ \
					i--; \
					node_index++; \
				} \
			} \
		} \
		/* more than one object in list */ \
		if (child=node->children) \
		{ \
			/* not a leaf node */ \
			if (0==node->number_of_indices) \
			{ \
				/* last object in the list */ \
				DESTROY_INDEX_NODE(object_type)(index); \
			} \
			else \
			{ \
				child += (node->number_of_indices)-i; \
				(node->number_of_indices)--; \
				i--; \
				DESTROY_INDEX_NODE(object_type)(child); \
				while (i>0) \
				{ \
					*node_index= *(node_index+1); \
					node_index++; \
					*child= *(child+1); \
					child++; \
					i--; \
				} \
				*child= *(child+1); \
			} \
		} \
		else \
		{ \
			/* a leaf node */ \
			if (1==node->number_of_indices) \
			{ \
				/* last object in the list */ \
				DESTROY_INDEX_NODE(object_type)(index); \
			} \
			else \
			{ \
				DEACCESS(object_type)(node_index); \
				(node->number_of_indices)--; \
				i--; \
				while (i>0) \
				{ \
					*node_index= *(node_index+1); \
					node_index++; \
					i--; \
				} \
			} \
		} \
		return_code=1; \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"REMOVE_OBJECT_FROM_INDEX(" #object_type ").  Could not find object"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* REMOVE_OBJECT_FROM_INDEX(object_type) */
#endif /* defined (OLD_CODE) */

#if defined (FULL_NAMES)
#define REMOVE_OBJECTS_FROM_INDEX_THAT( object_type ) \
	index_remove_objects_that_ ## object_type
#else
#define REMOVE_OBJECTS_FROM_INDEX_THAT( object_type )  vin ## object_type
#endif

#define DECLARE_REMOVE_OBJECTS_FROM_INDEX_THAT_FUNCTION( object_type ) \
int REMOVE_OBJECTS_FROM_INDEX_THAT(object_type)( \
	LIST_CONDITIONAL_FUNCTION(object_type) *conditional,void *user_data, \
	struct INDEX_NODE(object_type) **index_address) \
/***************************************************************************** \
LAST MODIFIED : 26 November 1996 \
\
DESCRIPTION : \
Removes each object that <iterator> returns true for. \
============================================================================*/ \
{ \
	int i,j,count; \
	struct INDEX_NODE(object_type) *index; \
\
	ENTER(REMOVE_OBJECTS_FROM_INDEX_THAT(object_type)); \
	count=0; \
	/* check the arguments */ \
	if (index_address&&conditional) \
	{ \
		if (index= *index_address) \
		{ \
			if (index->children) \
			{ \
				for (i=0;i<=index->number_of_indices;i++) \
				{ \
					count += REMOVE_OBJECTS_FROM_INDEX_THAT(object_type)( \
						conditional,user_data,&(index->children[i])); \
				} \
				j=0; \
				/* shuffle down any NULL children */ \
				for (i=0;i<=index->number_of_indices;i++) \
				{ \
					if (index->children[i]) \
					{ \
						if (i<index->number_of_indices) \
						{ \
							index->indices[j]=index->indices[i]; \
						} \
						index->children[j]=index->children[i]; \
						j++; \
					} \
				} \
				index->number_of_indices=j-1; \
				if (-1==index->number_of_indices) \
				{ \
					/* no children left */ \
					DESTROY_INDEX_NODE(object_type)(index_address); \
				} \
				else \
				{ \
					if (0==index->number_of_indices) \
					{ \
						/* one child - chain from parent */ \
						*index_address=index->children[0]; \
						/* ensure the destroy does not chain */ \
						index->number_of_indices--; \
						/* this next line is redundant */ \
						index->children[0]=(struct INDEX_NODE(object_type) *)NULL; \
						DESTROY_INDEX_NODE(object_type)(&index); \
					} \
					else \
					{ \
						/* make sure that not using a removed object as an index */ \
						for (i=0;i<index->number_of_indices;i++) \
						{ \
							if ((conditional)(index->indices[i],user_data)) \
							{ \
								index->indices[i]=(((index->children)[i])->indices)[ \
									(((index->children)[i])->number_of_indices)-1]; \
							} \
						} \
					} \
				} \
			} \
			else \
			{ \
				/* loop over all indices */ \
				j=0; \
				for (i=0;i<index->number_of_indices;i++) \
				{ \
					index->indices[j]=index->indices[i]; \
					if ((conditional)(index->indices[j],user_data)) \
					{ \
						/* delete the object */ \
						count++; \
						DEACCESS(object_type)(&(index->indices[j])); \
					} \
					else \
					{ \
						j++; \
					} \
				} \
				index->number_of_indices=j; \
				/* have we killed the node */ \
				if (0==index->number_of_indices) \
				{ \
					DESTROY_INDEX_NODE(object_type)(index_address); \
				} \
			} \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
		"REMOVE_OBJECTS_FROM_INDEX_THAT(" #object_type ").  Invalid argument(s)"); \
	} \
	LEAVE; \
\
	return (count); \
} /* REMOVE_OBJECTS_FROM_INDEX_THAT(object_type) */

#if defined (FULL_NAMES)
#define ADD_INDEX_TO_NODE_PARENT( object_type ) \
	add_index_to_node_parent_ ## object_type
#else
#define ADD_INDEX_TO_NODE_PARENT( object_type )  win ## object_type
#endif

#define DECLARE_ADD_INDEX_TO_NODE_PARENT_FUNCTION( object_type , identifier, \
	compare_function ) \
static int ADD_INDEX_TO_NODE_PARENT(object_type)( \
	struct object_type *add_index,struct INDEX_NODE(object_type) *add_node, \
	struct INDEX_NODE(object_type) *node) \
/***************************************************************************** \
LAST MODIFIED : 12 April 1996 \
\
DESCRIPTION : \
Adds the <add_index> (and the <add_node>) to the parent of the <node>. \
============================================================================*/ \
{ \
	int i,j,return_code; \
	struct INDEX_NODE(object_type) **child,*parent,**new_child,*new_parent; \
	struct object_type **index,**new_index,*split_index; \
\
	ENTER(ADD_INDEX_TO_NODE_PARENT(object_type)); \
	return_code=0; \
	if (parent=node->parent) \
	{ \
		/* find place in index node */ \
		index=parent->indices; \
		i=parent->number_of_indices; \
		while ((i>0)&&(0<compare_function(add_index->identifier, \
			(*index)->identifier))) \
		{ \
			i--; \
			index++; \
		} \
		if (2*B_TREE_ORDER>parent->number_of_indices) \
		{ \
			/* add object to node (node does not need splitting) */ \
			add_node->parent=parent; \
			index=(parent->indices)+(parent->number_of_indices); \
			child=(parent->children)+((parent->number_of_indices)+1); \
			while (i>0) \
			{ \
				index--; \
				child--; \
				*(index+1)= *index; \
				*(child+1)= *child; \
				i--; \
			} \
			*index=add_index; \
			*child=add_node; \
			(parent->number_of_indices)++; \
			return_code=1; \
		} \
		else \
		{ \
			/* node needs splitting */ \
			/* create a new node */ \
			if (new_parent=CREATE_INDEX_NODE(object_type)(0)) \
			{ \
				if (i>B_TREE_ORDER) \
				{ \
					split_index=(parent->indices)[B_TREE_ORDER-1]; \
				} \
				else \
				{ \
					if (i<B_TREE_ORDER) \
					{ \
						split_index=(parent->indices)[B_TREE_ORDER]; \
					} \
					else \
					{ \
						split_index=add_index; \
					} \
				} \
				if (ADD_INDEX_TO_NODE_PARENT(object_type)(split_index, \
					new_parent,parent)) \
				{ \
					if (i<B_TREE_ORDER) \
					{ \
						index=(parent->indices)+(2*B_TREE_ORDER); \
						child=(parent->children)+(2*B_TREE_ORDER); \
						new_index=(new_parent->indices)+B_TREE_ORDER; \
						new_child=(new_parent->children)+B_TREE_ORDER; \
						j=B_TREE_ORDER-i-1; \
						while (i>0) \
						{ \
							index--; \
							new_index--; \
							*new_index= *index; \
							*new_child= *child; \
							(*child)->parent=new_parent; \
							child--; \
							new_child--; \
							i--; \
						} \
						new_index--; \
						*new_index=add_index; \
						*new_child=add_node; \
						add_node->parent=new_parent; \
						while (j>0) \
						{ \
							index--; \
							new_index--; \
							*new_index= *index; \
							new_child--; \
							*new_child= *child; \
							(*child)->parent=new_parent; \
							child--; \
							j--; \
						} \
						new_child--; \
						*new_child= *child; \
						(*child)->parent=new_parent; \
					} \
					else \
					{ \
						index=(parent->indices)+B_TREE_ORDER; \
						child=(parent->children)+B_TREE_ORDER; \
						new_index=new_parent->indices; \
						new_child=new_parent->children; \
						for (j=B_TREE_ORDER;j>0;j--) \
						{ \
							*new_index= *index; \
							index++; \
							new_index++; \
							child++; \
							new_child++; \
							*new_child= *child; \
							(*child)->parent=new_parent; \
						} \
						i -= B_TREE_ORDER; \
						if (0==i) \
						{ \
							(parent->children)[B_TREE_ORDER]=node; \
							(new_parent->children)[0]=add_node; \
							add_node->parent=new_parent; \
						} \
						else \
						{ \
							i--; \
							index=(parent->indices)+(B_TREE_ORDER-1); \
							child=(parent->children)+B_TREE_ORDER; \
							(new_parent->children)[0]= *child; \
							(*child)->parent=new_parent; \
							while (i>0) \
							{ \
								index--; \
								child--; \
								*(index+1)= *index; \
								*(child+1)= *child; \
								i--; \
							} \
							*index=add_index; \
							*child=add_node; \
							add_node->parent=parent; \
						} \
					} \
					new_parent->number_of_indices=B_TREE_ORDER; \
					parent->number_of_indices=B_TREE_ORDER; \
					return_code=1; \
				} \
			} \
			else \
			{ \
				display_message(ERROR_MESSAGE,"ADD_INDEX_TO_NODE_PARENT(" \
					#object_type ").  Could not create new index node"); \
			} \
		} \
	} \
	else \
	{ \
		/* create new root node */ \
		if (parent=CREATE_INDEX_NODE(object_type)(0)) \
		{ \
			parent->number_of_indices=1; \
			(parent->indices)[0]=add_index; \
			(parent->children)[0]=node; \
			node->parent=parent; \
			(parent->children)[1]=add_node; \
			add_node->parent=parent; \
			return_code=1; \
		} \
		else \
		{ \
			display_message(ERROR_MESSAGE,"ADD_INDEX_TO_NODE_PARENT(" \
				#object_type ").  Could not create new root node"); \
		} \
	} \
	LEAVE; \
\
	return (return_code); \
} /* ADD_INDEX_TO_NODE_PARENT(object_type) */

#if defined (FULL_NAMES)
#define ADD_OBJECT_TO_INDEX( object_type ) \
	index_add_object_ ## object_type
#else
#define ADD_OBJECT_TO_INDEX( object_type )  pin ## object_type
#endif

#define DECLARE_ADD_OBJECT_TO_INDEX_FUNCTION( object_type , identifier, \
	compare_function ) \
static int ADD_OBJECT_TO_INDEX(object_type)(struct object_type *object, \
	struct INDEX_NODE(object_type) **index) \
/***************************************************************************** \
LAST MODIFIED : 24 September 1999 \
\
DESCRIPTION : \
Adds an <object> to the <index>. \
============================================================================*/ \
{ \
	int i,j,return_code; \
	struct INDEX_NODE(object_type) *leaf_node,*new_node; \
	struct object_type **leaf_index,**new_index; \
\
	ENTER(ADD_OBJECT_TO_INDEX(object_type)); \
	return_code=0; \
	/* find the leaf node that should contain the object */ \
	leaf_node=FIND_LEAF_NODE_IN_INDEX(object_type)(object->identifier,*index); \
	/* check that the object is not already in the leaf node */ \
	leaf_index=leaf_node->indices; \
	i=leaf_node->number_of_indices; \
	while ((i>0)&&(0<compare_function(object->identifier, \
		(*leaf_index)->identifier))) \
	{ \
		i--; \
		leaf_index++; \
	} \
	if ((i>0)&&!compare_function(object->identifier,(*leaf_index)->identifier)) \
	{ \
		display_message(ERROR_MESSAGE,"ADD_OBJECT_TO_INDEX(" #object_type \
			").  Object already in index"); \
	} \
	else \
	{ \
		if (2*B_TREE_ORDER>leaf_node->number_of_indices) \
		{ \
			/* add object to leaf node (leaf node does not need splitting) */ \
			leaf_index=(leaf_node->indices)+(leaf_node->number_of_indices); \
			while (i>0) \
			{ \
				leaf_index--; \
				*(leaf_index+1)= *leaf_index; \
				i--; \
			} \
			*leaf_index=ACCESS(object_type)(object); \
			(leaf_node->number_of_indices)++; \
			return_code=1; \
		} \
		else \
		{ \
			/* leaf node needs splitting */ \
			/* create a new leaf node */ \
			if (new_node=CREATE_INDEX_NODE(object_type)(1)) \
			{ \
				if (ADD_INDEX_TO_NODE_PARENT(object_type)( \
					(leaf_node->indices)[B_TREE_ORDER-1],new_node,leaf_node)) \
				{ \
					return_code=1; \
					if ((*index)->parent) \
					{ \
						/* update root node */ \
						*index=(*index)->parent; \
					} \
					if (i<=B_TREE_ORDER) \
					{ \
						leaf_index=(leaf_node->indices)+(2*B_TREE_ORDER); \
						new_index=(new_node->indices)+B_TREE_ORDER; \
						j=B_TREE_ORDER-i; \
						while (i>0) \
						{ \
							leaf_index--; \
							*new_index= *leaf_index; \
							new_index--; \
							i--; \
						} \
						*new_index=ACCESS(object_type)(object); \
						while (j>0) \
						{ \
							leaf_index--; \
							new_index--; \
							*new_index= *leaf_index; \
							j--; \
						} \
						new_node->number_of_indices=B_TREE_ORDER+1; \
						leaf_node->number_of_indices=B_TREE_ORDER; \
					} \
					else \
					{ \
						leaf_index=(leaf_node->indices)+B_TREE_ORDER; \
						new_index=new_node->indices; \
						for (j=B_TREE_ORDER;j>0;j--) \
						{ \
							*new_index= *leaf_index; \
							leaf_index++; \
							new_index++; \
						} \
						new_node->number_of_indices=B_TREE_ORDER; \
						i -= B_TREE_ORDER; \
						leaf_index=(leaf_node->indices)+B_TREE_ORDER; \
						while (i>0) \
						{ \
							leaf_index--; \
							*(leaf_index+1)= *leaf_index; \
							i--; \
						} \
						*leaf_index=ACCESS(object_type)(object); \
						leaf_node->number_of_indices=B_TREE_ORDER+1; \
					} \
				} \
			} \
			else \
			{ \
				display_message(ERROR_MESSAGE,"ADD_OBJECT_TO_INDEX(" #object_type \
					").  Could not create new leaf node"); \
			} \
		} \
	} \
	LEAVE; \
\
	return (return_code); \
} /* ADD_OBJECT_TO_INDEX(object_type) */

#if defined (FULL_NAMES)
#define IS_OBJECT_IN_INDEX( object_type ) \
	index_in_index_ ## object_type
#else
#define IS_OBJECT_IN_INDEX( object_type )  iin ## object_type
#endif

#define DECLARE_IS_OBJECT_IN_INDEX_FUNCTION( object_type , identifier, \
	compare_function ) \
static int IS_OBJECT_IN_INDEX(object_type)(struct object_type *object, \
	struct INDEX_NODE(object_type) *index) \
/***************************************************************************** \
LAST MODIFIED : 9 April 1996 \
\
DESCRIPTION : \
Returns true if the <object> is in the <index>. \
============================================================================*/ \
{ \
	int i,return_code; \
	struct INDEX_NODE(object_type) *leaf_node; \
	struct object_type **leaf_index; \
\
	ENTER(IS_OBJECT_IN_INDEX(object_type)); \
	if (index) \
	{ \
		/* search the index for the leaf node for the specified identifier */ \
		if (leaf_node=FIND_LEAF_NODE_IN_INDEX(object_type)(object->identifier, \
			index)) \
		{ \
			leaf_index=leaf_node->indices; \
			i=leaf_node->number_of_indices; \
			while ((i>0)&&(0<compare_function(object->identifier, \
				(*leaf_index)->identifier))) \
			{ \
				i--; \
				leaf_index++; \
			} \
			if ((i>0)&&(object== *leaf_index)) \
			{ \
				return_code=1; \
			} \
			else \
			{ \
				return_code=0; \
			} \
		} \
		else \
		{ \
			return_code=0; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"IS_OBJECT_IN_INDEX(" #object_type ").  Invalid argument"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* IS_OBJECT_IN_INDEX(object_type) */

#if defined (FULL_NAMES)
#define FIRST_OBJECT_IN_INDEX_THAT( object_type ) \
	index_first_that_ ## object_type
#else
#define FIRST_OBJECT_IN_INDEX_THAT( object_type )  tin ## object_type
#endif

#define DECLARE_FIRST_OBJECT_IN_INDEX_THAT_FUNCTION( object_type ) \
static struct object_type *FIRST_OBJECT_IN_INDEX_THAT(object_type)( \
	LIST_CONDITIONAL_FUNCTION(object_type) *conditional,void *user_data, \
	struct INDEX_NODE(object_type) *index) \
/***************************************************************************** \
LAST MODIFIED : 10 April 1996 \
\
DESCRIPTION : \
If <conditional> is not NULL, the "first" object in the <index> that \
<conditional> returns true is returned, otherwise the "first" object in the \
<index> is returned. \
============================================================================*/ \
{ \
	int i; \
	struct INDEX_NODE(object_type) **child; \
	struct object_type *object,**object_index; \
\
	ENTER(FIRST_OBJECT_IN_INDEX_THAT(object_type)); \
	/* check the arguments */ \
	if (index) \
	{ \
		if (child=index->children) \
		{ \
			i=index->number_of_indices; \
			object=(struct object_type *)NULL; \
			while ((i>=0)&&!object) \
			{ \
				object=FIRST_OBJECT_IN_INDEX_THAT(object_type)(conditional,user_data, \
					*child); \
				child++; \
				i--; \
			} \
		} \
		else \
		{ \
			if (conditional) \
			{ \
				i=index->number_of_indices; \
				object_index=index->indices; \
				while ((i>0)&&!((conditional)(*object_index,user_data))) \
				{ \
					i--; \
					object_index++; \
				} \
				if (i>0) \
				{ \
					object= *object_index; \
				} \
				else \
				{ \
					object=(struct object_type *)NULL; \
				} \
			} \
			else \
			{ \
				object=(index->indices)[0]; \
			} \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"FIRST_OBJECT_IN_INDEX_THAT(" #object_type ").  Invalid argument(s)"); \
		object=(struct object_type *)NULL; \
	} \
	LEAVE; \
\
	return (object); \
} /* FIRST_OBJECT_IN_INDEX_THAT(object_type) */

#if defined (FULL_NAMES)
#define FOR_EACH_OBJECT_IN_INDEX( object_type ) \
	index_for_each_ ## object_type
#else
#define FOR_EACH_OBJECT_IN_INDEX( object_type )  ein ## object_type
#endif

#define DECLARE_FOR_EACH_OBJECT_IN_INDEX_FUNCTION( object_type ) \
int FOR_EACH_OBJECT_IN_INDEX(object_type)( \
	LIST_ITERATOR_FUNCTION(object_type) *iterator,void *user_data, \
	struct INDEX_NODE(object_type) *index) \
/***************************************************************************** \
LAST MODIFIED : 10 April 1996 \
\
DESCRIPTION : \
Calls <iterator> for each object in the <index>. \
============================================================================*/ \
{ \
	int i,return_code; \
	struct INDEX_NODE(object_type) **child; \
	struct object_type **object_index; \
\
	ENTER(FOR_EACH_OBJECT_IN_INDEX(object_type)); \
	/* check the arguments */ \
	if (index&&iterator) \
	{ \
		if (child=index->children) \
		{ \
			i=index->number_of_indices; \
			return_code=1; \
			while ((i>=0)&&(return_code=FOR_EACH_OBJECT_IN_INDEX(object_type)( \
				iterator,user_data,*child))) \
			{ \
				child++; \
				i--; \
			} \
		} \
		else \
		{ \
			i=index->number_of_indices; \
			object_index=index->indices; \
			return_code=1; \
			while ((i>0)&&(return_code=(iterator)(*object_index,user_data))) \
			{ \
				i--; \
				object_index++; \
			} \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"FOR_EACH_OBJECT_IN_INDEX(" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* FOR_EACH_OBJECT_IN_INDEX(object_type) */

/*
Global functions
----------------
*/
#define DECLARE_CREATE_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_CREATE_LIST_FUNCTION(object_type) \
{ \
	struct LIST(object_type) *list; \
\
	ENTER(CREATE_LIST(object_type)); \
	if (ALLOCATE(list,struct LIST(object_type),1)) \
	{ \
		list->count=0; \
		list->index=(struct INDEX_NODE(object_type) *)NULL; \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"CREATE_LIST(" #object_type ").  Insufficient memory"); \
	} \
	LEAVE; \
\
	return (list); \
} /* CREATE_LIST(object_type) */

#define DECLARE_DESTROY_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_DESTROY_LIST_FUNCTION(object_type) \
{ \
	int return_code; \
\
	ENTER(DESTROY_LIST(object_type)); \
	if (list_address) \
	{ \
		if (*list_address) \
		{ \
			DESTROY_INDEX_NODE(object_type)(&((*list_address)->index)); \
			DEALLOCATE(*list_address); \
		} \
		return_code=1; \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"DESTROY_LIST(" #object_type ").  Invalid argument"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* DESTROY_LIST(object_type) */

#define DECLARE_COPY_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_COPY_LIST_FUNCTION(object_type) \
{ \
	int return_code; \
	struct INDEX_NODE(object_type) *index; \
\
	ENTER(COPY_LIST(object_type)); \
	if (target_list&&source_list) \
	{ \
		if (source_list->index) \
		{ \
			/* duplicate the index - no parent for top index */ \
			if (index=DUPLICATE_INDEX_NODE(object_type)(source_list->index, \
				(struct INDEX_NODE(object_type) *)NULL)) \
			{ \
				/* remove the current contents of the <target_list> */ \
				DESTROY_INDEX_NODE(object_type)(&(target_list->index)); \
				target_list->index=index; \
				target_list->count=source_list->count; \
				return_code=1; \
			} \
			else \
			{ \
				display_message(ERROR_MESSAGE, \
					"COPY_LIST(" #object_type ").  Could not duplicate index"); \
				return_code=0; \
			} \
		} \
		else \
		{ \
			/* remove the current contents of the <target_list> */ \
			DESTROY_INDEX_NODE(object_type)(&(target_list->index)); \
			target_list->count=0; \
			return_code=1; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"COPY_LIST(" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* COPY_LIST(object_type) */

#define DECLARE_REMOVE_OBJECT_FROM_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_REMOVE_OBJECT_FROM_LIST_FUNCTION(object_type) \
{ \
	int return_code; \
\
	ENTER(REMOVE_OBJECT_FROM_LIST(object_type)); \
	if (object&&list) \
	{ \
		if (list->index) \
		{ \
			/* list is not empty */ \
			if (REMOVE_OBJECT_FROM_INDEX(object_type)(object,&(list->index))) \
			{ \
				(list->count)--; \
				return_code=1; \
			} \
			else \
			{ \
				display_message(ERROR_MESSAGE,"REMOVE_OBJECT_FROM_LIST(" #object_type \
					").  Could not remove from index"); \
				return_code=0; \
			} \
		} \
		else \
		{ \
			display_message(ERROR_MESSAGE, \
				"REMOVE_OBJECT_FROM_LIST(" #object_type ").  Empty list"); \
			return_code=0; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"REMOVE_OBJECT_FROM_LIST(" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* REMOVE_OBJECT_FROM_LIST(object_type) */

#define DECLARE_REMOVE_OBJECTS_FROM_INDEXED_LIST_THAT_FUNCTION( object_type ) \
PROTOTYPE_REMOVE_OBJECTS_FROM_LIST_THAT_FUNCTION(object_type) \
{ \
	int return_code; \
\
	ENTER(REMOVE_OBJECTS_FROM_LIST_THAT(object_type)); \
	if (conditional&&list) \
	{ \
		if (list->index) \
		{ \
			/* list is not empty */ \
			list->count -= REMOVE_OBJECTS_FROM_INDEX_THAT(object_type)(conditional, \
        user_data,&(list->index)); \
      return_code=1; \
		} \
		else \
		{ \
			return_code=1; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"REMOVE_OBJECTS_FROM_LIST_THAT" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* REMOVE_OBJECTS_FROM_LIST_THAT(object_type) */

#define DECLARE_REMOVE_ALL_OBJECTS_FROM_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_REMOVE_ALL_OBJECTS_FROM_LIST_FUNCTION(object_type) \
{ \
	int return_code; \
\
	ENTER(REMOVE_ALL_OBJECTS_FROM_LIST(object_type)); \
	if (list) \
	{ \
		DESTROY_INDEX_NODE(object_type)(&(list->index)); \
		list->count=0; \
		return_code=1; \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"REMOVE_ALL_OBJECTS_FROM_LIST" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* REMOVE_ALL_OBJECTS_FROM_LIST(object_type) */

#define DECLARE_ADD_OBJECT_TO_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_ADD_OBJECT_TO_LIST_FUNCTION( object_type ) \
{ \
	int return_code; \
	struct INDEX_NODE(object_type) *node; \
\
	ENTER(ADD_OBJECT_TO_LIST(object_type)); \
	if (object&&list) \
	{ \
		if (list->index) \
		{ \
			/* list is not empty */ \
			if (ADD_OBJECT_TO_INDEX(object_type)(object,&(list->index))) \
			{ \
				(list->count)++; \
				return_code=1; \
			} \
			else \
			{ \
				display_message(ERROR_MESSAGE,"ADD_OBJECT_TO_LIST(" #object_type \
					").  Could not add to index"); \
				return_code=0; \
			} \
		} \
		else \
		{ \
			/* list is empty */ \
			/* create the root node (a leaf node) */ \
			if (node=CREATE_INDEX_NODE(object_type)(1)) \
			{ \
				list->index=node; \
				list->count=1; \
				node->number_of_indices=1; \
				(node->indices)[0]=ACCESS(object_type)(object); \
				return_code=1; \
			} \
			else \
			{ \
				display_message(ERROR_MESSAGE,"ADD_OBJECT_TO_LIST(" #object_type \
					").  Could not create index"); \
				return_code=0; \
			} \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"ADD_OBJECT_TO_LIST(" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* ADD_OBJECT_TO_LIST(object_type) */

#define DECLARE_NUMBER_IN_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_NUMBER_IN_LIST_FUNCTION(object_type) \
{ \
	int count; \
\
	ENTER(NUMBER_IN_LIST(object_type)); \
	if (list) \
	{ \
		count=list->count; \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"NUMBER_IN_LIST(" #object_type ").  Invalid argument"); \
		count=0; \
	} \
	LEAVE; \
\
	return (count); \
} /* NUMBER_IN_LIST(object_type) */

#define DECLARE_IS_OBJECT_IN_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_IS_OBJECT_IN_LIST_FUNCTION(object_type) \
{ \
	int return_code; \
\
	ENTER(IS_OBJECT_IN_LIST(object_type)); \
	if (list) \
	{ \
		if (list->index) \
		{ \
			return_code=IS_OBJECT_IN_INDEX(object_type)(object,list->index); \
		} \
		else \
		{ \
			return_code=0; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"IS_OBJECT_IN_LIST(" #object_type ").  Invalid argument"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* IS_OBJECT_IN_LIST(object_type) */

#define DECLARE_FIND_BY_IDENTIFIER_IN_INDEXED_LIST_FUNCTION( object_type , \
	identifier , identifier_type , compare_function ) \
PROTOTYPE_FIND_BY_IDENTIFIER_IN_LIST_FUNCTION(object_type,identifier, \
	identifier_type) \
{ \
	int i; \
	struct object_type **index,*object; \
	struct INDEX_NODE(object_type) *leaf_node; \
\
	ENTER(FIND_BY_IDENTIFIER_IN_LIST(object_type,identifier)); \
	if (list) \
	{ \
		if (list->index) \
		{ \
			/* search the index for the leaf node for the specified identifier */ \
			if (leaf_node=FIND_LEAF_NODE_IN_INDEX(object_type)(identifier, \
				list->index)) \
			{ \
				index=leaf_node->indices; \
				i=leaf_node->number_of_indices; \
				while ((i>0)&&(0<compare_function(identifier,(*index)->identifier))) \
				{ \
					i--; \
					index++; \
				} \
				if ((i>0)&&!compare_function(identifier,(*index)->identifier)) \
				{ \
					object= *index; \
				} \
				else \
				{ \
					object=(struct object_type *)NULL; \
				} \
			} \
			else \
			{ \
				object=(struct object_type *)NULL; \
			} \
		} \
		else \
		{ \
			object=(struct object_type *)NULL; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE,"FIND_BY_IDENTIFIER_IN_LIST(" #object_type \
			"," #identifier ").  Invalid argument"); \
		object=(struct object_type *)NULL; \
	} \
	LEAVE; \
\
	return (object); \
} /* FIND_BY_IDENTIFIER_IN_LIST(object_type,identifier) */

#define DECLARE_FIND_BY_IDENTIFIER_IN_INDEXED_LIST_FUNCTION_NOT_INDEX( \
	object_type , identifier , identifier_type , compare_function ) \
static int compare_ ## object_type(struct object_type *object, \
	void *identifier ## _void) \
{ \
	int return_code; \
	identifier_type identifier; \
\
	ENTER(compare_ ## object_type); \
	if (object&&(identifier=(identifier_type)identifier ## _void)) \
	{ \
		if (0==compare_function(identifier,object->identifier)) \
		{ \
			return_code=1; \
		} \
		else \
		{ \
			return_code=0; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"compare_" #object_type ".  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* compare_ ## object_type */ \
\
PROTOTYPE_FIND_BY_IDENTIFIER_IN_LIST_FUNCTION(object_type,identifier, \
	identifier_type) \
{ \
	struct object_type *object; \
\
	ENTER(FIND_BY_IDENTIFIER_IN_LIST(object_type,identifier)); \
	if (list) \
	{ \
		object=FIRST_OBJECT_IN_LIST_THAT(object_type)(compare_ ## object_type, \
			(void *)identifier,list); \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE,"FIND_BY_IDENTIFIER_IN_LIST(" #object_type \
			"," #identifier ").  Invalid argument"); \
		object=(struct object_type *)NULL; \
	} \
	LEAVE; \
\
	return (object); \
} /* FIND_BY_IDENTIFIER_IN_LIST(object_type,identifier) */

#define DECLARE_FIRST_OBJECT_IN_INDEXED_LIST_THAT_FUNCTION( object_type ) \
PROTOTYPE_FIRST_OBJECT_IN_LIST_THAT_FUNCTION(object_type) \
{ \
	struct object_type *object; \
\
	ENTER(FIRST_OBJECT_IN_LIST_THAT(object_type)); \
	/* check the arguments */ \
	if (list) \
	{ \
		if (list->index) \
		{ \
			object=FIRST_OBJECT_IN_INDEX_THAT(object_type)(conditional,user_data, \
				list->index); \
		} \
		else \
		{ \
			object=(struct object_type *)NULL; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"FIRST_OBJECT_IN_LIST_THAT(" #object_type ").  Invalid argument(s)"); \
		object=(struct object_type *)NULL; \
	} \
	LEAVE; \
\
	return (object); \
} /* FIRST_OBJECT_IN_LIST_THAT(object_type) */

#define DECLARE_FOR_EACH_OBJECT_IN_INDEXED_LIST_FUNCTION( object_type ) \
PROTOTYPE_FOR_EACH_OBJECT_IN_LIST_FUNCTION(object_type) \
{ \
	int return_code; \
\
	ENTER(FOR_EACH_OBJECT_IN_LIST(object_type)); \
	/* check the arguments */ \
	if (list&&iterator) \
	{ \
		if (list->index) \
		{ \
			return_code=FOR_EACH_OBJECT_IN_INDEX(object_type)(iterator,user_data, \
				list->index); \
		} \
		else \
		{ \
			return_code=1; \
		} \
	} \
	else \
	{ \
		display_message(ERROR_MESSAGE, \
			"FOR_EACH_OBJECT_IN_LIST(" #object_type ").  Invalid argument(s)"); \
		return_code=0; \
	} \
	LEAVE; \
\
	return (return_code); \
} /* FOR_EACH_OBJECT_IN_LIST(object_type) */

#define DECLARE_INDEXED_LIST_MODULE_FUNCTIONS( object_type , identifier , \
	identifier_type , compare_function ) \
DECLARE_CREATE_INDEX_NODE_FUNCTION(object_type) \
DECLARE_DESTROY_INDEX_NODE_FUNCTION(object_type) \
DECLARE_DUPLICATE_INDEX_NODE_FUNCTION(object_type) \
DECLARE_FIND_LEAF_NODE_IN_INDEX_FUNCTION(object_type,identifier, \
	identifier_type,compare_function) \
DECLARE_REMOVE_OBJECT_FROM_INDEX_FUNCTION(object_type,identifier, \
	compare_function) \
DECLARE_REMOVE_OBJECTS_FROM_INDEX_THAT_FUNCTION(object_type) \
DECLARE_ADD_INDEX_TO_NODE_PARENT_FUNCTION(object_type,identifier, \
	compare_function) \
DECLARE_ADD_OBJECT_TO_INDEX_FUNCTION(object_type,identifier,compare_function) \
DECLARE_IS_OBJECT_IN_INDEX_FUNCTION(object_type,identifier,compare_function) \
DECLARE_FIRST_OBJECT_IN_INDEX_THAT_FUNCTION(object_type) \
DECLARE_FOR_EACH_OBJECT_IN_INDEX_FUNCTION(object_type)

#define DECLARE_INDEXED_LIST_FUNCTIONS( object_type ) \
DECLARE_CREATE_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_DESTROY_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_COPY_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_REMOVE_OBJECT_FROM_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_REMOVE_OBJECTS_FROM_INDEXED_LIST_THAT_FUNCTION(object_type) \
DECLARE_REMOVE_ALL_OBJECTS_FROM_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_ADD_OBJECT_TO_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_NUMBER_IN_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_IS_OBJECT_IN_INDEXED_LIST_FUNCTION(object_type) \
DECLARE_FIRST_OBJECT_IN_INDEXED_LIST_THAT_FUNCTION(object_type) \
DECLARE_FOR_EACH_OBJECT_IN_INDEXED_LIST_FUNCTION(object_type)

#endif /* !defined (INDEXED_LIST_PRIVATE_H) */
