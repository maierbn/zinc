/***************************************************************************//**
 * FILE : fieldtypestime.hpp
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
 * The Original Code is libZinc.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2012
 * the Initial Developer. All Rights Reserved.cmgui
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
#ifndef __FIELD_TYPES_TIME_HPP__
#define __FIELD_TYPES_TIME_HPP__

extern "C" {
#include "api/cmiss_field_time.h"
}
#include "api++/field.hpp"
#include "api++/timekeeper.hpp"

namespace Zn
{

class FieldTimeLookup : public Field
{
public:

	FieldTimeLookup() : Field(NULL)
	{	}

	FieldTimeLookup(Cmiss_field_id field_id) : Field(field_id)
	{	}

};

class FieldTimeValue : public Field
{
public:

	FieldTimeValue() : Field(NULL)
	{	}

	FieldTimeValue(Cmiss_field_id field_id) : Field(field_id)
	{	}

};

inline FieldTimeLookup FieldModule::createTimeLookup(Field& sourceField, Field& timeField)
{
	return FieldTimeLookup(Cmiss_field_module_create_time_lookup(id,
		sourceField.getId(), timeField.getId()));
}

inline FieldTimeValue FieldModule::createTimeValue(TimeKeeper& timeKeeper)
{
	return FieldTimeValue(Cmiss_field_module_create_time_value(id, timeKeeper.getId()));
}

}  // namespace Zn

#endif /* __FIELD_TYPES_TIME_HPP__ */
