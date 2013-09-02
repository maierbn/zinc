/**
 * glyph_colour_bar.hpp
 *
 * Derived glyph type which creates a spectrum colour_bar which automatically
 * updates when the spectrum changes.
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
#ifndef GLYPH_COLOUR_BAR_HPP
#define GLYPH_COLOUR_BAR_HPP

#include "zinc/graphicsmaterial.h"
#include "graphics/glyph.hpp"

struct cmzn_spectrum;

struct cmzn_glyph_colour_bar : public cmzn_glyph
{
private:
	cmzn_spectrum *spectrum;
	GT_object *graphicsObject;
	double axis[3], centre[3], sideAxis[3];
	double extendLength;
	cmzn_graphics_material *labelMaterial;
	char *numberFormat;
	int labelDivisions;
	double tickLength;

	cmzn_glyph_colour_bar(cmzn_spectrum *spectrumIn);
	virtual ~cmzn_glyph_colour_bar();

	void invalidate()
	{
		if (this->graphicsObject)
		{
			DEACCESS(GT_object)(&this->graphicsObject);
		}
		this->changed();
	}

public:
	static cmzn_glyph_colour_bar *create(cmzn_spectrum *spectrumIn)
	{
		if (spectrumIn)
			return new cmzn_glyph_colour_bar(spectrumIn);
		return 0;
	}

	virtual GT_object *getGraphicsObject(cmzn_tessellation *tessellation,
		cmzn_graphics_material *material, cmzn_font *font);

	virtual bool usesFont()
	{
		return true;
	}

	int getAxis(int valuesCount, double *valuesOut) const;

	int setAxis(int valuesCount, const double *valuesIn);

	int getCentre(int valuesCount, double *valuesOut) const;

	int setCentre(int valuesCount, const double *valuesIn);

	double getExtendLength() const
	{
		return this->extendLength;
	}

	int setExtendLength(double extendLengthIn);

	int getLabelDivisions() const
	{
		return this->labelDivisions;
	}

	int setLabelDivisions(int labelDivisionsIn);

	cmzn_graphics_material_id getLabelMaterial() const
	{
		return labelMaterial ? cmzn_graphics_material_access(labelMaterial) : 0;
	}

	int setLabelMaterial(cmzn_graphics_material_id material);

	char *getNumberFormat() const;

	int setNumberFormat(const char *numberFormatIn);

	int getSideAxis(int valuesCount, double *valuesOut) const;

	int setSideAxis(int valuesCount, const double *valuesIn);

	double getTickLength() const
	{
		return this->tickLength;
	}

	int setTickLength(double tickLengthIn);

	virtual void fontChange();

	virtual void materialChange(struct MANAGER_MESSAGE(Graphical_material) *message);

	virtual void spectrumChange(struct MANAGER_MESSAGE(Spectrum) *message);

};

#endif // GLYPH_COLOUR_BAR_HPP
