/* ======================================================================== *\
!
 !   Author(s): Tarek Hassan     09/2015 <mailto:thassan@ifae.es>
 !
 !   Copyright: CTA Software Development, 2015
 !
 \* ======================================================================== */

#include <fstream>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////
//
// GIRFUtils
// Useful functions used by GIRF
//
//////////////////////////////////////////////////////////////////////////////

using namespace std;


#include "GIRFUtils.h"
#include "GIRFRange.h"
#include <string.h>
#include <vector>


////////////////////////////////////////////////////////////////
//
// Set CHDU to last Axis HDU present within the fits file
//
void flexIRF::GIRFUtils::GoToLastAxisHDU(fitsfile* fptr) {

	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdutype = BINARY_TBL, hdunum, nkeys, ii;
	int lastID = 0, lasthdu, initialHDU;

	initialHDU = fptr->HDUposition;

	fits_get_num_hdus(fptr, &hdunum, &status);

	for (int hdupos = 1; hdupos <= hdunum; hdupos++) /* Main loop through each extension */
	{
		if (hdutype == BINARY_TBL) {
			if (!fits_read_key_str(fptr, "HDUCLAS2", card, NULL, &status)) {
				if (!strcmp(card, "AXIS")) {
					if (!fits_read_key_str(fptr, "HDUCLAS4", card, NULL, &status)) {
						if (atoi(card) > lastID) {
							lastID = atoi(card);
							lasthdu=fptr->HDUposition;
						}
					}
				}
			}
		}
		status = 0;
		fits_movabs_hdu(fptr, hdupos, &hdutype, &status);
		if (status)
			break;
	}
	fits_movabs_hdu(fptr, lasthdu + 1, NULL, &status);
}

////////////////////////////////////////////////////////////////
//
// Set CHDU to the Data HDU with ID equal to pdfID
//
void flexIRF::GIRFUtils::GoToPdfHDU(fitsfile* fptr, int pdfID) {

	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdutype = BINARY_TBL, hdunum, nkeys, ii;
	int lastID = 0, lasthdu, initialHDU;

	initialHDU = fptr->HDUposition;

	fits_get_num_hdus(fptr, &hdunum, &status);
	char axisIDkeyword[20];

	for (int hdupos = 1; hdupos <= hdunum; hdupos++) /* Main loop through each extension */
	{
		fits_movabs_hdu(fptr, hdupos, &hdutype, &status);
		if (hdutype == IMAGE_HDU) {
			if (!fits_read_key_str(fptr, "HDUCLAS2", card, NULL, &status)) {
				if (!strcmp(card, "DATA")) {
					if (!fits_read_key_str(fptr, "HDUCLAS4", card, NULL, &status)) {
						if (atoi(card) == pdfID){
							// Current HDU is the pdf we want.
							break;
						}
					}
				}
			}
		}
		if (status == KEY_NO_EXIST) status = 0;
		if (status) break;
	}
}



////////////////////////////////////////////////////////////////
//
// Check the last Axis ID present within the fits file
//
int flexIRF::GIRFUtils::GetLastAxisID(string filename) {

	fitsfile *fptr; /* FITS file pointer, defined in fitsio.h */
	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdupos, nkeys, ii;
	int lastID = 0;
	char errorCard[FLEN_ERRMSG]; /* max length of a FITSIO status text string */

	cout << "Opening file " << filename.data() << endl;
	if (!fits_open_file(&fptr, filename.data(), READONLY, &status)) {
		lastID = GetLastAxisID(fptr);
	} else {
		cout << "Failed to open file: " << filename.data() << endl;
		fits_get_errstatus(status, errorCard);
		cout << "Error code: " << errorCard << endl;
		return status;
	}
	if (fits_close_file(fptr, &status))
		cout << "GIRF::Write Error: cannot close file (error code: " << status
				<< ")" << endl;
	return lastID;
}


int flexIRF::GIRFUtils::GetLastAxisID(fitsfile* fptr) {

	int currenthdu = fptr->HDUposition;

	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdutype = BINARY_TBL, hdunum, nkeys, ii;
	int lastID = 0;
	fits_get_num_hdus(fptr, &hdunum, &status);

	for (int hdupos = 1; hdupos <= hdunum; hdupos++) /* Main loop through each extension */
	{
		fits_movabs_hdu(fptr, hdupos, &hdutype, &status);
		if (hdutype == BINARY_TBL) {
			if (!fits_read_key_str(fptr, "HDUCLAS2", card, NULL, &status)) {
				if (!strcmp(card, "AXIS")) {
					if (!fits_read_key_str(fptr, "HDUCLAS4", card, NULL,
							&status)) {
						if (atoi(card) > lastID) {
							lastID = atoi(card);
						}
					}
				}
			}
		}
		if (status == KEY_NO_EXIST) status = 0;
		if (status) break;

	}
	fits_movabs_hdu(fptr, currenthdu + 1, NULL, &status);
	return lastID;
}



////////////////////////////////////////////////////////////////
//
// Check the last Pdf ID present within the fits file
//
int flexIRF::GIRFUtils::GetLastPdfID(string filename) {

	fitsfile *fptr; /* FITS file pointer, defined in fitsio.h */
	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdupos, nkeys, ii;
	int lastID = 0;

	cout << "Opening file " << filename.data() << endl;
	if (!fits_open_file(&fptr, filename.data(), READONLY, &status)){
		lastID = GetLastPdfID(fptr);
	}
	if (fits_close_file(fptr, &status))
			cout << "GIRF::Write Error: cannot close file (error code: " << status
					<< ")" << endl;
	return lastID;
}

int flexIRF::GIRFUtils::GetLastPdfID(fitsfile* fptr) {

	int currenthdu = fptr->HDUposition;

	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdutype = IMAGE_HDU, hdunum, nkeys, ii;
	int lastID = 0;
	fits_get_num_hdus(fptr, &hdunum, &status);

	for (int hdupos = 1; hdupos <= hdunum; hdupos++) /* Main loop through each extension */
	{
		fits_movabs_hdu(fptr, hdupos, &hdutype, &status);
		if (hdutype == IMAGE_HDU) {						// First check if HDU extension
			if (!fits_read_key_str(fptr, "HDUCLAS2", card, NULL, &status)) {
				if (!strcmp(card, "DATA")) {
					if (!fits_read_key_str(fptr, "HDUCLAS4", card, NULL,
							&status)) {
						if (atoi(card) > lastID) {
							lastID = atoi(card);
						}
					}
				}
			}
		}
		status = 0;
		fits_movabs_hdu(fptr, hdupos, &hdutype, &status);
		if (status)
			break;
	}
	fits_movabs_hdu(fptr, currenthdu + 1, NULL, &status);
	return lastID;
}

vector<int> flexIRF::GIRFUtils::FindAxisRanges(string filename, std::vector<AxisRange> axisRanges){

	vector<int> axisIDs, foundIDs;


	fitsfile *fptr; /* FITS file pointer, defined in fitsio.h */
	int status = 0;   		// must be initialized (0 means ok)

	if (fits_open_file(&fptr, filename.data(), READONLY, &status)) {
		cout << "ERROR " << status << " while trying to open the FITS file." << endl;
	}
	for(std::vector<AxisRange>::iterator axisRange = axisRanges.begin(); axisRange != axisRanges.end(); ++axisRange) {
		foundIDs = FindAxisRange(fptr, *axisRange);
//		axisIDs.push_back(ID);
		cout << "axisRange->varType = " << axisRange->varType << ", lowRange = " << axisRange->lowRange << ", highRange = " << axisRange->highRange << endl;
		for(vector<int>::iterator foundID = foundIDs.begin(); foundID != foundIDs.end(); ++foundID){
			cout << "Found axis ID = " << *foundID << endl;
		}
	}

	return axisIDs;
}


////////////////////////////////////////////////////////////////
//
// 		Return all axis IDs matching AxisRange.
//
//
vector<int> flexIRF::GIRFUtils::FindAxisRange(fitsfile *fptr, AxisRange axisRange){
	vector<int> foundAxisID;

	int currenthdu = fptr->HDUposition;				//TODO: do we need to know the current position? I leave it just to make sure...

	int status = 0;   		// must be initialized (0 means ok)
	char card[FLEN_CARD]; /* Standard string lengths defined in fitsio.h */
	int single = 0, hdutype, hdunum;
	fits_get_num_hdus(fptr, &hdunum, &status);

	for (int hdupos = 1; hdupos <= hdunum; hdupos++) /* Main loop through each extension */
	{
		fits_movabs_hdu(fptr, hdupos, &hdutype, &status);
		if (hdutype == BINARY_TBL) {
			if (!fits_read_key_str(fptr, "HDUCLAS2", card, NULL, &status)) {
				if (!strcmp(card, "AXIS")) {
					if (!fits_read_key_str(fptr, "VARTYPE", card, NULL, &status)) {
						if (atoi(card) == axisRange.varType) {
							if (!fits_read_key_str(fptr, "HDUCLAS4", card, NULL, &status)) {
								foundAxisID.push_back(atoi(card));
							}
						}
					}
				}
			}
		}
		if (status == KEY_NO_EXIST) status = 0;
		if (status) break;

	}
	fits_movabs_hdu(fptr, currenthdu + 1, NULL, &status);

	return foundAxisID;
}















