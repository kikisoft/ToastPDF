/**
	@file
	@brief 
*/

/*
 * CC PDF Converter: Windows PDF Printer with Creative Commons license support
 * Copyright (C) 2007 Guy Hachlili <hguy@cogniview.com>, Cogniview LTD.
 * 
 * This file is part of CC PDF Converter.
 * 
 * CC PDF Converter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CC PDF Converter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. * 
 */

#ifndef _PROGRESSINTERFACE_H_
#define _PROGRESSINTERFACE_H_

/**
    @brief Interface class for progress handling
*/
class ProgressInterface
{
public:
	/**
		@brief Called to test if the operation is to be canceled
		@return true to cancel the operation, false to continue (default)
	*/
	virtual bool	GetCancel() {return false;};
	/**
		@brief Called when the operation is finished
		@param bSuccess true if finished successfully, false if failed (or canceled by the user)
	*/
	virtual void	OnFinished(bool bSuccess) {};
};

/// Default progress interface; does nothing and allows the operation to finish
extern ProgressInterface gpiDefault;

#endif   //#define _PROGRESSINTERFACE_H_
