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

#include "precomp.h"
#include "globals.h"
#include "CCLicensePropPage.h"

#include "CCLicenseWizard.h"

#include "PngImage.h"
#include "CCPrintLicenseInfo.h"
#include "CCLicenseLocationDlg.h"

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicensePropPage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Which message is this?
    switch (uMsg)
    {
		case WM_INITDIALOG:
			// Page creation:
			{
				// Initialize the combo box strings
				for (int i=0;i<LLNone;i++)
				{
					AddDlgComboString(IDC_LOCATION, i + IDS_LICENSELOCATION_TOP, i);
					AddDlgComboString(IDC_LOCATION_OTHER, i + IDS_LICENSELOCATION_TOP, i);
				}
			}
			// Reset the visible data
			InitControls();

			// Set the focus
			SetFocus(GetDlgItem(IDC_LOCATION));
			return FALSE;

        case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
					// Some button/box clicked:
                    switch(LOWORD(wParam))
                    {
						case IDC_SELECT_LICENSE:
							// The user wants to select a license:
							{
								// Show the license selection wizard:
								LicenseInfoLoader info(m_license);
								CCLicenseWizard wiz;
								if (wiz.DoModal(m_hDlg, m_pHelper, &info))
								{
									// Update license writing information from the web
									if (!info.LoadLicenseData(m_hDlg))
										break;

									// Set the license on the page:
									m_license = info;
									UpdateControls();
									SetChanged();
								}
							}
							break;
						case IDC_LICENSE_PROPERTIES:
							// The user changed the properties check box
							m_bSetProperties = GetDlgItemCheck(IDC_LICENSE_PROPERTIES);
							SetChanged();
							break;
						case IDC_STAMP:
							// The user changed the license stamping box:
							if (GetDlgItemCheck(IDC_STAMP))
							{
								// It's on, so make sure we use the correct location
								int nData = GetDlgComboSelData(IDC_LOCATION);
								if (nData == -1)
									m_location.eFirstPage = LLBottom;
								else
									m_location.eFirstPage = (LicenseLocation)nData;
							}
							else
								// It's off
								m_location.eFirstPage = LLNone;
							// Update the controls:
							UpdateLicenseLocationControls();
							SetChanged();
							break;
						case IDC_LOCATION:
							// Changed the location selection:
							m_location.eFirstPage = (LicenseLocation)GetDlgComboSelData(IDC_LOCATION);
							// Update the controls
							UpdateLicenseLocationControls();
							SetChanged();
							break;
						case IDC_STAMP_ALL:
							// Changed the license stamping on all pages box:
							if (GetDlgItemCheck(IDC_STAMP_ALL))
							{
								// OK, get the location for the other pages
								int nSel = GetCheckedRadioBtn(IDC_LOCATION_SAME, IDC_LOCATION_DIFFERENT);
								if (nSel != 1)
									m_location.eOtherPages = LLOther;
								else
								{
									int nData = GetDlgComboSelData(IDC_LOCATION_OTHER);
									if (nData == -1)
										m_location.eOtherPages = LLBottom;
									else
										m_location.eOtherPages = (LicenseLocation)nData;
								}
							}
							else
								m_location.eOtherPages = LLNone;
							// And update everything:
							UpdateLicenseLocationControls();
							SetChanged();
							break;
						case IDC_LOCATION_SAME:
						case IDC_LOCATION_DIFFERENT:
							// User changed when the other pages have stamps:
							if (GetCheckedRadioBtn(IDC_LOCATION_SAME, IDC_LOCATION_DIFFERENT) == 1)
							{
								int nData = GetDlgComboSelData(IDC_LOCATION_OTHER);
								if (nData == -1)
									m_location.eOtherPages = LLBottom;
								else
									m_location.eOtherPages = (LicenseLocation)nData;
							}
							else
							{
								m_location.eOtherPages = LLOther;
							}
							// Update it all:
							UpdateLicenseLocationControls();
							SetChanged();
							break;
						case IDC_LOCATION_USER:
						case IDC_LOCATION_OTHER_USER:
							{
								// The user wants to set the license location, so get current size and location
								SIZE szLicense = GetLicenseSize();
								POINT ptLocation = m_location.LocationForPage(LOWORD(wParam) == IDC_LOCATION_USER, GetPageSize(), szLicense);
								RECT rect;
								::SetRect(&rect, ptLocation.x, ptLocation.y, ptLocation.x + szLicense.cx, ptLocation.y + szLicense.cy);
								// Display the location selection dialog:
								CCLicenseLocationDlg dlg(GetPageSize(), rect);
								if (dlg.DoModal(m_hDlg) == IDOK)
								{
									// OK, get the new location
									if (LOWORD(wParam) == IDC_LOCATION_USER)
										m_location.ptFirstPage = dlg.GetLocation();
									else
										m_location.ptOtherPages = dlg.GetLocation();
									SetChanged();
								}
							}
							break;
					}
                    break;

				case CBN_SELCHANGE:
					// User has selected a combo box value:
					switch (LOWORD(wParam))
					{
						case IDC_LOCATION:
							// It's the first page location box, update the license location values
							if (LLUserDefined == (LicenseLocation)GetDlgComboSelData(IDC_LOCATION))
								m_location.ptFirstPage = m_location.LocationForPage(true, GetPageSize(), GetLicenseSize());
							m_location.eFirstPage = (LicenseLocation)GetDlgComboSelData(IDC_LOCATION);
							// And update the page controls
							UpdateLicenseLocationControls();
							SetChanged();
							break;
						case IDC_LOCATION_OTHER:
							// It's the first page location box, update the license location values
							if (LLUserDefined == (LicenseLocation)GetDlgComboSelData(IDC_LOCATION_OTHER))
								m_location.ptOtherPages = m_location.LocationForPage(false, GetPageSize(), GetLicenseSize());
							m_location.eOtherPages = (LicenseLocation)GetDlgComboSelData(IDC_LOCATION_OTHER);
							// And update the page controls
							UpdateLicenseLocationControls();
							SetChanged();
							break;
					}
					break;

                default:
					// Nothing we can handle
                    return FALSE;
            }
			// Got here: we handled it
            return TRUE;

        case WM_NOTIFY:
            {
				// Notification, what is it?
                switch (((LPNMHDR)lParam)->code)  // type of notification message
                {
                    case PSN_SETACTIVE:
						// Page made active, reload the data
						InitControls();
                        break;
    
                    case PSN_KILLACTIVE:
						// Moved to another page, remember the data
						m_pDevMode->location = m_location;
						m_pDevMode->info = m_license;
						m_pDevMode->bSetProperties = m_bSetProperties;
						return TRUE;

                    case PSN_APPLY:
						// Apply, we want to save the data
						m_pDevMode->location = m_location;
						m_pDevMode->info = m_license;
						m_pDevMode->bSetProperties = m_bSetProperties;
						// And notify the dialog that we have applied the changes
						(*m_pfnComPropSheet)(m_hComPropSheet, CPSFUNC_SET_RESULT, (LPARAM)m_hPage, CPSUI_OK);
						return TRUE;

                    case PSN_RESET:
						// Reset, reload the data from the passed properties
						InitControls();
                        break;
                }
            }
            break;
    }

	// Continue handling the message
    return FALSE;
}

/**
	
*/
void CCLicensePropPage::UpdateLicenseControls()
{
	// Set the license text
	SetDlgItemText(IDC_LICENSE, m_license.GetLicenseName().c_str());
	// Clean up the image, we'll reset it if necessary
	if (m_hBmp != NULL)
	{
		DeleteObject(m_hBmp);
		m_hBmp = NULL;
	}

	// Do we have an image for this license?
	UINT uID = m_license.GetLicenseImageID();
	if (uID != 0)
	{
		// Yes, load it from the resources
		PngImage png;
		if (png.LoadFromResource(uID, true, ghInstance))
		{
			HDC hDC = ::GetDC(GetDlgItem(IDC_LICENSE_IMAGE));
			m_hBmp = png.ToBitmap(hDC);
			::ReleaseDC(GetDlgItem(IDC_LICENSE_IMAGE), hDC);
		}
	}

	// Set the license image (works with NULL too)
	::SendMessage(GetDlgItem(IDC_LICENSE_IMAGE), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_hBmp);
}

/**
	
*/
void CCLicensePropPage::UpdateLicenseLocationControls()
{
	if ((m_license.m_eLicense == LicenseInfo::LTNone) || (m_license.m_eLicense == LicenseInfo::LTUnknown))
	{
		// Disable all if there's no license!
		EnableDlgItem(IDC_LICENSE_PROPERTIES, FALSE);
		EnableDlgItem(IDC_STAMP, FALSE);
		EnableDlgItem(IDC_LOCATION, FALSE);
		EnableDlgItem(IDC_STAMP_ALL, FALSE);
		EnableDlgItem(IDC_LOCATION_SAME, FALSE);
		EnableDlgItem(IDC_LOCATION_DIFFERENT, FALSE);
		EnableDlgItem(IDC_LOCATION_OTHER, FALSE);
		return;
	}

	// OK, enable and set the XMP properties box
	SetDlgItemCheck(IDC_LICENSE_PROPERTIES, m_bSetProperties);
	EnableDlgItem(IDC_LICENSE_PROPERTIES, TRUE);

	// Set the license stamping box
	BOOL bStamp = (m_location.eFirstPage != LLNone), bAllPages = FALSE, bDifferentLocation = FALSE;
	SetDlgItemCheck(IDC_STAMP, bStamp);
	EnableDlgItem(IDC_STAMP, TRUE);

	if (bStamp)
	{
		// We want to stamp it, so enable the location boxes
		SetDlgComboSel(IDC_LOCATION, m_location.eFirstPage);
		bAllPages = (m_location.eOtherPages != LLNone);
		SetDlgItemCheck(IDC_STAMP_ALL, bAllPages);
		if (bAllPages)
		{
			// We also want to stamp the other pages, so update them too
			bDifferentLocation = (m_location.eOtherPages != LLOther);
			SetDlgItemCheck(bDifferentLocation ? IDC_LOCATION_DIFFERENT : IDC_LOCATION_SAME, TRUE);
			if (bDifferentLocation)
				SetDlgComboSel(IDC_LOCATION_OTHER, (int)m_location.eOtherPages);

		}
	}

	// Now enable everything that should be enabled
	EnableDlgItem(IDC_LOCATION, bStamp);
	EnableDlgItem(IDC_LOCATION_USER, bStamp && (GetDlgComboSel(IDC_LOCATION) == LLUserDefined));
	EnableDlgItem(IDC_STAMP_ALL, bStamp);
	EnableDlgItem(IDC_LOCATION_SAME, bStamp && bAllPages);
	EnableDlgItem(IDC_LOCATION_DIFFERENT, bStamp && bAllPages);
	EnableDlgItem(IDC_LOCATION_OTHER, bStamp && bAllPages && bDifferentLocation);
	EnableDlgItem(IDC_LOCATION_OTHER_USER, bStamp && bAllPages && bDifferentLocation && (GetDlgComboSel(IDC_LOCATION_OTHER) == LLUserDefined));
}

/**
	
*/
void CCLicensePropPage::InitControls()
{
	// Make sure the image will be displayed
	::SetWindowLong(GetDlgItem(IDC_LICENSE_IMAGE), GWL_STYLE, (::GetWindowLong(GetDlgItem(IDC_LICENSE_IMAGE), GWL_STYLE)&~SS_TYPEMASK)|SS_BITMAP);

	// Read the license and location information
	m_license = m_pDevMode->info;
	m_location = m_pDevMode->location;
	m_bSetProperties = m_pDevMode->bSetProperties;

	// Update the page controls
	UpdateControls();
}

/**
	@return Size of the page (from the general printer structure)
*/
SIZE CCLicensePropPage::GetPageSize()
{
	SIZE szPage;

	// Is the page proprait?
	if (m_pPublicDM->dmOrientation == DMORIENT_PORTRAIT)
	{
		// Yes, we use the regular width and length
		szPage.cx = m_pPublicDM->dmPaperWidth;
		szPage.cy = m_pPublicDM->dmPaperLength;
	}
	else
	{
		// No, so the height is the width and visa versa
		szPage.cx = m_pPublicDM->dmPaperLength;
		szPage.cy = m_pPublicDM->dmPaperWidth;
	}
	return szPage;
}

/**
	@return The size of the license stamp
*/
SIZE CCLicensePropPage::GetLicenseSize()
{
	// Calculate the multiplication factor from teh general printer structure
	int nMulti = m_pPublicDM->dmPrintQuality > 0 ? m_pPublicDM->dmPrintQuality / 72 : 1;
	SIZE szLicenseSize;

	// All license images are 88x31, but multiple it first
	szLicenseSize.cx = 88 * nMulti;
	szLicenseSize.cy = 31 * nMulti;

	// That's it
	return szLicenseSize;
}
