/*
 * pfmread.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: igkiou
 */

#include "mex_utils.h"
#include "pfm_mex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	/* Check number of input arguments */
	if (nrhs != 1) {
		mexErrMsgTxt("Exactly one input argument is required.");
	}

	/* Check number of output arguments */
	if (nlhs > 2) {
		mexErrMsgTxt("Too many output arguments.");
	}

	pfm::PFMInputFile file(mex::MxString(const_cast<mxArray*>(prhs[0])));
	file.readHeader();
	plhs[0] = file.readFile().get_array();
	if (nlhs >= 2) {
		plhs[1] = file.get_header().toMxArray().get_array();
	}
}
