/*
 * mex_utils.h
 *
 *  Created on: Jan 30, 2013
 *      Author: igkiou
 */

#ifndef MEX_UTILS_H_
#define MEX_UTILS_H_

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <string.h>

/*
 * TODO: Maybe include these inside mex namespace, to make sure mex:: needs to
 * be used? Check if this actually works.
 */
#include "mex.h"
#include "matrix.h"

/*
 * TODO: Add struct array support.
 * TODO: Is giving access to a data element with [] im MxCell and MxStruct
 * equivalent to using mxSetCell and mxSetField, respectively?
 * TODO: Provide cast from base MxArray to other types?
 * TODO: Add constructor of derived classes from base MxArray?
 * TODO: Provide templated version of MxArray that uses mxClassID instead of
 * C/C++ intrinsic types?
 */
namespace mex {

#ifdef NDEBUG
#define Assert(cond) ((void) 0)
#define AssertEx(cond, explanation) ((void) 0)
#else
#define Assert(cond) do { \
		if (!(cond)) \
		mexErrMsgIdAndTxt("MATLAB:mex", "Assertion \"%s\" failed in %s:%i\n", \
		#cond, __FILE__, __LINE__); \
	} while (0)

#define AssertEx(cond, explanation) do { \
		if (!(cond)) \
		mexErrMsgIdAndTxt(\
		"MATLAB:mex", "Assertion \"%s\" failed in %s:%i (" explanation ")\n", \
		#cond, __FILE__, __LINE__); \
	} while (0)
#endif

template <typename T, typename U>
class ConstMap {
public:
	ConstMap(const T& key, const U& val) {
		m_map[key] = val;
	}

	ConstMap<T, U>& operator()(const T& key, const U& val) {
		m_map[key] = val;
		return *this;
	}

	operator std::map<T, U>() { return m_map; }

	inline U operator[] (const T& key) const {
		typename std::map<T, U>::const_iterator it = m_map.find(key);
		AssertEx(it == m_map.end(), "Value not found.");
		return (*it).second;
	}

private:
	std::map<T, U> m_map;
};

template <typename T>
class MxClassId {
public:
	MxClassId()
			: m_class(mxUNKNOWN_CLASS) {	}

	MxClassId(const MxClassId& mxClassId)
			: m_class(mxClassId.m_kClass) {	}

	inline const mxClassID get_class() const {
		return m_class;
	}

	inline mxClassID get_class() {
		return m_class;
	}

	inline const mxClassID operator()() const {
		return m_class;
	}

	inline mxClassID operator()() {
		return m_class;
	}

	inline operator const mxClassID() const {
		return m_class;
	}

	inline operator mxClassID() {
		return m_class;
	}

private:
	mxClassID m_class;
};

template <> MxClassId<unsigned char>::MxClassId() : m_class(mxUINT8_CLASS) {	}
template <> MxClassId<char>::MxClassId() : m_class(mxINT8_CLASS) {	}
template <> MxClassId<short>::MxClassId() : m_class(mxINT16_CLASS) {	}
template <> MxClassId<unsigned short>::MxClassId() : m_class(mxUINT16_CLASS) {	}
template <> MxClassId<int>::MxClassId() : m_class(mxINT32_CLASS) {	}
template <> MxClassId<unsigned int>::MxClassId() : m_class(mxUINT32_CLASS) {	}
template <> MxClassId<long>::MxClassId() : m_class(mxINT64_CLASS) {	}
template <> MxClassId<unsigned long>::MxClassId() : m_class(mxUINT64_CLASS) {	}
template <> MxClassId<float>::MxClassId() : m_class(mxSINGLE_CLASS) {	}
template <> MxClassId<double>::MxClassId() : m_class(mxDOUBLE_CLASS) {	}
template <> MxClassId<bool>::MxClassId() : m_class(mxLOGICAL_CLASS) {	}

typedef mxArray MxArrayNative;
typedef MxArrayNative* PMxArrayNative;

class MxArray {
public:
	static const mxClassID m_kClass = mxUNKNOWN_CLASS;

	MxArray()
		: m_array() {	}

	explicit MxArray(const MxArray& other)
					: m_array(other.m_array) {	}

	MxArray& operator=(const MxArray& other) {
		if (this != &other) {
			this->m_array = other.m_array;
		}
		return *this;
	}

	explicit MxArray(const PMxArrayNative array)
					: m_array(array) {	}

	virtual ~MxArray() {}

	inline mxClassID mxClass() {
		return m_kClass;
	}

	inline const mxClassID mxClass() const {
		return m_kClass;
	}

	/*
 	 * TODO: These were removed as unsafe versions of specialized versions of
 	 * the same in derived classes.
 	 */
//	inline const void * data() const {
//		return (void *) mxGetData(m_pArray);
//	}
//
//	inline void * data() {
//		return (void *) mxGetData(m_pArray);
//	}

	/* getter */
	inline const PMxArrayNative get_array() const {
		return m_array;
	}

	inline PMxArrayNative get_array() {
		return m_array;
	}

	/*
	 * TODO: These were removed because they are not very safe.
	 */
	/* parenthesis operator */
//	inline const PMxArrayNative operator()() const {
//		return m_array;
//	}
//
//	inline PMxArrayNative operator()() {
//		return m_array;
//	}

	inline int size() const {
		return mxGetNumberOfElements(m_array);
	}

	inline int numel() const {
		return mxGetNumberOfElements(m_array);
	}

	inline int getNumberOfElements() const {
		return mxGetNumberOfElements(m_array);
	}

	inline int M() const {
		return mxGetM(m_array);
	}

	inline int getNumberOfRows() const {
		return mxGetM(m_array);
	}

	inline int N() const {
		return mxGetN(m_array);
	}

	inline int getNumberOfColumns() const {
		return mxGetN(m_array);
	}

	inline int numdims() const {
		return mxGetNumberOfDimensions(m_array);
	}

	inline int getNumberOfDimensions() const {
		return mxGetNumberOfDimensions(m_array);
	}

	inline std::vector<int> dims() const {
		const mwSize *tempDims = mxGetDimensions(m_array);
		std::vector<int> retArg;
		for (int iter = 0, numdims = getNumberOfDimensions(); iter < numdims;
			++iter) {
			retArg.push_back(tempDims[iter]);
		}
		return retArg;
	}

	inline void destroy() {
		mxDestroyArray(m_array);
	}

protected:
	PMxArrayNative m_array;
};

template <typename T>
class MxNumeric : public MxArray {
public:
	static const mxClassID m_kClass = MxClassId<T>();

	MxNumeric()
			: MxArray() {	}

	explicit MxNumeric(const MxNumeric<T>& mxNumeric)
					: MxArray(mxNumeric) {	}

	MxNumeric<T>& operator=(const MxNumeric<T>& mxNumeric) {
		if (this != &mxNumeric) {
			this->m_array = mxNumeric.m_array;
		}
		return *this;
	}

	explicit MxNumeric(const PMxArrayNative array)
					: MxArray(array) {
		Assert(m_kClass == mxGetClassID(array));
	}

	MxNumeric(const int numRows, const int numColumns)
			: MxArray() {
		m_array = mxCreateNumericMatrix(numRows, numColumns, m_kClass, mxREAL);
	}

	/*
	 * TODO: This was removed because of a conflict with the vector initializer
	 * in the case of T = int.
	 */
//	MxNumeric(const std::vector<int> &dims)
//		: MxArray(), \
//		  m_mxClassID(MxClassID<T>()) {
//		const int numDims = dims.size();
//		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
//		for (int iter = 0; iter < numDims; ++iter) {
//			tempDims[iter] = dims[iter];
//		}
//		m_pArray = mxCreateNumericArray(numDims, tempDims, m_kClass, mxREAL);
//		mxFree(tempDims);
//	}

	MxNumeric(const int numDims, const int *dims)
			: MxArray() {
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateNumericArray(numDims, tempDims, m_kClass, mxREAL);
		mxFree(tempDims);
	}

	MxNumeric(const int numDims, const mwSize *dims)
			: MxArray() {
		m_array = mxCreateNumericArray(numDims, dims, m_kClass, mxREAL);
	}

	explicit MxNumeric(const T scalarVar)
					: MxArray() {
		m_array = mxCreateNumericMatrix(1, 1, m_kClass, mxREAL);
		T *val = (T *) mxGetData(m_array);
		*val = (T) scalarVar;
	}

	explicit MxNumeric(const std::vector<T>& vecVar)
					: MxArray() {
		m_array = mxCreateNumericMatrix(vecVar.size(), 1, m_kClass, mxREAL);
		T *val = (T *) mxGetData(m_array);
		std::copy(vecVar.begin(), vecVar.end(), val);
	}

	MxNumeric(const T* arrVar, const int numRows, const int numColumns)
			: MxArray() {
		m_array = mxCreateNumericMatrix(numRows, numColumns, m_kClass, mxREAL);
		T *val = (T *) mxGetData(m_array);
		memcpy(val, arrVar, numRows * numColumns * sizeof(T));
	}

	MxNumeric(const T* arrVar, const std::vector<int>& dims)
			: MxArray() {
		const int numDims = dims.size();
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateNumericArray(numDims, tempDims, m_kClass, mxREAL);
		mxFree(tempDims);
		int numElements = 1;
		for (int iter = 0; iter < numDims; ++iter) {
			numElements *= dims[iter];
		}
		T *val = (T *) mxGetData(m_array);
		memcpy(val, arrVar, numElements * sizeof(T));
	}

	MxNumeric(const T* arrVar, const int numDims, const int *dims)
			: MxArray() {
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateNumericArray(numDims, tempDims, m_kClass, mxREAL);
		mxFree(tempDims);
		int numElements = 1;
		for (int iter = 0; iter < numDims; ++iter) {
			numElements *= dims[iter];
		}
		T *val = (T *) mxGetData(m_array);
		memcpy(val, arrVar, numElements * sizeof(T));
	}

	MxNumeric(const T* arrVar, const int numDims, const mwSize *dims)
			: MxArray() {
		m_array = mxCreateNumericArray(numDims, dims, m_kClass, mxREAL);
		int numElements = 1;
		for (int iter = 0; iter < numDims; ++iter) {
			numElements *= dims[iter];
		}
		T *val = (T *) mxGetData(m_array);
		memcpy(val, arrVar, numElements * sizeof(T));
	}

	inline T& operator[] (int i) {
		T* temp = (T *) mxGetData(m_array);
		return temp[i];
	}

	inline const T& operator[] (int i) const {
		T* temp = (T *) mxGetData(m_array);
		return temp[i];
	}

	inline const T* data() const {
		return (T *) mxGetData(m_array);
	}

	inline T* data() {
		return (T *) mxGetData(m_array);
	}

	inline std::vector<T> vectorize() {
		int numel = getNumberOfElements();
		const T* pData = data();
		return std::vector<T>(pData, pData + numel);
	}

	inline const std::vector<T> vectorize() const {
		int numel = getNumberOfElements();
		const T* pData = data();
		return std::vector<T>(pData, pData + numel);
	}
};

class MxString: public MxArray {
public:
	static const mxClassID m_kClass = mxCHAR_CLASS;

	explicit MxString(const MxString& mxString)
					: MxArray(mxString) {	}

	MxString& operator=(const MxString& mxString) {
		if (this != &mxString) {
			this->m_array = mxString.m_array;
		}
		return *this;
	}

	MxString(const PMxArrayNative array)
		: MxArray(array) {
		Assert(m_kClass == mxGetClassID(array));
	}

	MxString(const std::string& strVar) {
		m_array = mxCreateString(strVar.c_str());
	}

	MxString(const char* charVar) {
		m_array = mxCreateString(charVar);
	}

	inline std::string string() {
		char *temp = mxArrayToString(m_array);
		std::string retArg(temp);
		mxFree(temp);
		return retArg;
	}

	inline const std::string string() const {
		char *temp = mxArrayToString(m_array);
		std::string retArg(temp);
		mxFree(temp);
		return retArg;
	}

	inline char* c_str() {
		return mxArrayToString(m_array);
	}

	inline const char* c_str() const {
		return mxArrayToString(m_array);
	}

	inline int length() const {
		return mxGetNumberOfElements(m_array);
	}
};

class MxCell : public MxArray {
public:
	static const mxClassID m_kClass = mxCELL_CLASS;

	explicit MxCell(const MxCell& mxCell)
				: MxArray(mxCell) {	}

	MxCell& operator=(const MxCell& mxCell) {
		if (this != &mxCell) {
			this->m_array = mxCell.m_array;
		}
		return *this;
	}

	explicit MxCell(const PMxArrayNative array)
				: MxArray() {
		if (m_kClass == mxGetClassID(array)) {
			m_array = array;
		} else {
			m_array = mxCreateCellMatrix(1, 1);
			mxSetCell(m_array, 0, array);
		}
	}

	MxCell(const int numRows, const int numColumns)
		: MxArray() {
		m_array = mxCreateCellMatrix(numRows, numColumns);
	}

	MxCell(const std::vector<int> &dims)
		: MxArray() {
		const int numDims = dims.size();
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateCellArray(numDims, tempDims);
		mxFree(tempDims);
	}

	MxCell(const int numDims, const int *dims)
		: MxArray() {
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateCellArray(numDims, tempDims);
		mxFree(tempDims);
	}

	MxCell(const int numDims, const mwSize *dims)
		: MxArray() {
		m_array = mxCreateCellArray(numDims, dims);
	}

	/*
	 * TODO: Remember to change this if I add constructor from MxArray to all
	 * derived classes of MxArray.
	 */
	MxCell(const MxArray& scalarVar)
		: MxArray() {
		m_array = mxCreateCellMatrix(1, 1);
		mxSetCell(m_array, 0, scalarVar.get_array());
	}

	MxCell(const std::vector<PMxArrayNative>& vecVar)
		: MxArray() {
		m_array = mxCreateCellMatrix(vecVar.size(), 1);
		for (int iter = 0, numElements = vecVar.size(); iter < numElements; ++iter) {
			mxSetCell(m_array, iter, vecVar[iter]);
		}
	}

	MxCell(const std::vector<MxArray *>& vecVar)
		: MxArray() {
		m_array = mxCreateCellMatrix(vecVar.size(), 1);
		for (int iter = 0, numElements = vecVar.size(); iter < numElements; ++iter) {
			mxSetCell(m_array, iter, vecVar[iter]->get_array());
		}
	}

	MxCell(const std::vector<const MxArray *>& vecVar)
		: MxArray() {
		m_array = mxCreateCellMatrix(vecVar.size(), 1);
		for (int iter = 0, numElements = vecVar.size(); iter < numElements; ++iter) {
			mxSetCell(m_array, iter, vecVar[iter]->get_array());
		}
	}

	MxCell(const PMxArrayNative* arrVar, const int numRows, const int numColumns)
		: MxArray() {
		m_array = mxCreateCellMatrix(numRows, numColumns);
		int numElements = numRows * numColumns;
		for (int iter = 0; iter < numElements; ++iter) {
			mxSetCell(m_array, iter, arrVar[iter]);
		}
	}

	MxCell(const PMxArrayNative* arrVar, const std::vector<int>& dims)
		: MxArray() {
		const int numDims = dims.size();
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateCellArray(numDims, tempDims);
		mxFree(tempDims);
		int numElements = 1;
		for (int iter = 0; iter < numDims; ++iter) {
			numElements *= dims[iter];
		}
		for (int iter = 0; iter < numElements; ++iter) {
			mxSetCell(m_array, iter, arrVar[iter]);
		}
	}

	MxCell(const PMxArrayNative* arrVar, const int numDims, const int *dims)
		: MxArray() {
		mwSize *tempDims = (mwSize *) malloc(numDims * sizeof(mwSize));
		for (int iter = 0; iter < numDims; ++iter) {
			tempDims[iter] = dims[iter];
		}
		m_array = mxCreateCellArray(numDims, tempDims);
		mxFree(tempDims);
		int numElements = 1;
		for (int iter = 0; iter < numDims; ++iter) {
			numElements *= dims[iter];
		}
		for (int iter = 0; iter < numElements; ++iter) {
			mxSetCell(m_array, iter, arrVar[iter]);
		}
	}

	MxCell(const PMxArrayNative* arrVar, const int numDims, const mwSize *dims)
		: MxArray() {
		m_array = mxCreateCellArray(numDims, dims);
		int numElements = 1;
		for (int iter = 0; iter < numDims; ++iter) {
			numElements *= dims[iter];
		}
		for (int iter = 0; iter < numElements; ++iter) {
			mxSetCell(m_array, iter, arrVar[iter]);
		}
	}

	inline PMxArrayNative operator[] (int i) {
		return mxGetCell(m_array, i);
	}

	inline const PMxArrayNative operator[] (int i) const {
		return mxGetCell(m_array, i);
	}

	inline const PMxArrayNative* data() const {
		return (PMxArrayNative *) mxGetData(m_array);
	}

	inline PMxArrayNative* data() {
		return (PMxArrayNative *) mxGetData(m_array);
	}

	inline const std::vector<PMxArrayNative> vectorize() const {
		std::vector<PMxArrayNative> retArg;
		for (int iter = 0, numel = getNumberOfElements(); iter < numel; ++iter) {
			retArg.push_back((mxGetCell(m_array, iter)));
		}
		return retArg;
	}

	inline std::vector<PMxArrayNative> vectorize() {
		std::vector<PMxArrayNative> retArg;
		for (int iter = 0, numel = getNumberOfElements(); iter < numel; ++iter) {
			retArg.push_back((mxGetCell(m_array, iter)));
		}
		return retArg;
	}
};

class MxStruct : public MxArray {
public:
	static const mxClassID m_kClass = mxSTRUCT_CLASS;

	explicit MxStruct(const MxStruct& mxStruct)
		: MxArray(mxStruct) {	}

	MxStruct& operator=(const MxStruct& mxStruct) {
		if (this != &mxStruct) {
			this->m_array = mxStruct.m_array;
		}
		return *this;
	}

	explicit MxStruct(const PMxArrayNative array)
					: MxArray(array) {
		Assert(m_kClass == mxGetClassID(array));
	}

	MxStruct(const std::string& scalarName, const PMxArrayNative& scalarVar)
			: MxArray() {
		const char* temp = scalarName.c_str();
		m_array = mxCreateStructMatrix(1, 1, 1, &(temp));
		mxSetField(m_array, 0, scalarName.c_str(), scalarVar);
	}

	MxStruct(const std::string& scalarName, const MxArray& scalarVar)
			: MxArray() {
		const char* temp = scalarName.c_str();
		m_array = mxCreateStructMatrix(1, 1, 1, &(temp));
		mxSetField(m_array, 0, scalarName.c_str(), scalarVar.get_array());
	}

	MxStruct(const std::vector<std::string>& vecName,
			const std::vector<PMxArrayNative>& vecVar)
			: MxArray() {
		int numFields = vecName.size();
		Assert(vecVar.size() == numFields);
		if (numFields > 0) {
			const char* temp = vecName[0].c_str();
			m_array = mxCreateStructMatrix(1, 1, 1, &(temp));
			mxSetField(m_array, 0, vecName[0].c_str(), vecVar[0]);
			for (int iter = 1; iter < numFields; ++iter) {
				mxAddField(m_array, vecName[iter].c_str());
				mxSetField(m_array, 0, vecName[iter].c_str(), vecVar[iter]);
			}
		}
	}

	MxStruct(const std::vector<std::string>& vecName,
			const std::vector<MxArray *>& vecVar)
			: MxArray() {
		int numFields = vecName.size();
		Assert(vecVar.size() == numFields);
		if (numFields > 0) {
			const char* temp = vecName[0].c_str();
			m_array = mxCreateStructMatrix(1, 1, 1, &(temp));
			mxSetField(m_array, 0, vecName[0].c_str(), vecVar[0]->get_array());
			for (int iter = 1; iter < numFields; ++iter) {
				mxAddField(m_array, vecName[iter].c_str());
				mxSetField(m_array, 0, vecName[iter].c_str(),
						vecVar[iter]->get_array());
			}
		}
	}

	void addField(const std::string& scalarName,
				const PMxArrayNative& scalarVar) {
		mxAddField(m_array, scalarName.c_str());
		mxSetField(m_array, 0, scalarName.c_str(), scalarVar);
	}

	void addField(const std::string& scalarName, const MxArray& scalarVar) {
		mxAddField(m_array, scalarName.c_str());
		mxSetField(m_array, 0, scalarName.c_str(), scalarVar.get_array());
	}

	void addField(const std::vector<std::string>& vecName,
				const std::vector<PMxArrayNative>& vecVar) {
		Assert(vecVar.size() == numFields);
		for (int iter = 0, numFields = vecName.size(); iter < numFields;
			++iter) {
			mxAddField(m_array, vecName[iter].c_str());
			mxSetField(m_array, 0, vecName[iter].c_str(), vecVar[iter]);
		}
	}

	void addField(const std::vector<std::string>& vecName,
			const std::vector<MxArray *>& vecVar) {
		Assert(vecVar.size() == numFields);
		for (int iter = 0, numFields = vecName.size(); iter < numFields;
			++iter) {
			mxAddField(m_array, vecName[iter].c_str());
			mxSetField(m_array, 0, vecName[iter].c_str(),
					vecVar[iter]->get_array());
		}
	}

	bool isField(const std::string& name) {
		return (mxGetFieldNumber(m_array, name.c_str()) != -1);
	}

	inline int numFields() const {
		return mxGetNumberOfFields(m_array);
	}

	inline PMxArrayNative operator[] (int i) {
		return mxGetFieldByNumber(m_array, 0, i);
	}

	inline const PMxArrayNative operator[] (int i) const {
		return mxGetFieldByNumber(m_array, 0, i);
	}

	inline PMxArrayNative operator[] (const std::string& name) {
		return mxGetField(m_array, 0, name.c_str());
	}

	inline const PMxArrayNative operator[] (const std::string& name) const {
		return mxGetField(m_array, 0, name.c_str());
	}

	inline int getFieldNumber(const std::string& name) const {
		return mxGetFieldNumber(m_array, name.c_str());
	}

	/*
	 * TODO: Check that here mxGetFieldNameByNumber does not allocation and
	 * copy, so no dealloc is needed.
	 */
	inline std::string getFieldName(const int i) const {
		const char* temp = mxGetFieldNameByNumber(m_array, i);
		std::string retArg(temp);
		return retArg;
	}

	inline const PMxArrayNative* data() const {
		return (PMxArrayNative *) mxGetData(m_array);
	}

	inline PMxArrayNative* data() {
		return (PMxArrayNative *) mxGetData(m_array);
	}

	inline void vectorize(std::vector<std::string>& vecName,
						std::vector<PMxArrayNative>& vecVar ) const {
		vecName.resize(0);
		vecVar.resize(0);
		for (int iter = 0, numf = numFields(); iter < numf; ++iter) {
			const char* tempName = mxGetFieldNameByNumber(m_array, iter);
			vecName.push_back(std::string(tempName));
			PMxArrayNative tempVar = mxGetFieldByNumber(m_array, 0, iter);
			vecVar.push_back(tempVar);
		}
	}
};

} /* name space mex */

#endif /* MEX_UTILS_H_ */
