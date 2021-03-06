/*
 * openexr_mex.h
 *
 *  Created on: May 23, 2012
 *      Author: igkiou
 */

#ifndef OPENEXR_MEX_H_
#define OPENEXR_MEX_H_

#include <vector>
#include <string>
#include <stdio.h>
#include <stdarg.h>

#include "Iex.h"
#include "ImathBox.h"
#include "ImfArray.h"
#include "ImfAttribute.h"
#include "ImfBoxAttribute.h"
#include "ImfChannelList.h"
#include "ImfChannelListAttribute.h"
#include "ImfChromaticitiesAttribute.h"
#include "ImfCompressionAttribute.h"
#include "ImfEnvmapAttribute.h"
//#include "ImfExtraAttributes.h"
#include "ImfFloatAttribute.h"
#include "ImfFrameBuffer.h"
#include "ImfHeader.h"
#include "ImfInputFile.h"
#include "ImfLineOrderAttribute.h"
#include "ImfOutputFile.h"
#include "ImfPixelType.h"
#include "ImfRgba.h"
#include "ImfRgbaFile.h"
//#include "ImfStandardAttributes.h"
#include "ImfStringAttribute.h"
#include "ImfTestFile.h"
#include "ImfVecAttribute.h"
#include "ImfVectorAttribute.h"

#include "mex_utils.h"

namespace exr {

/*
 * TODO: Design: Everything that could be called from mexfile is using MxArray.
 * TODO: Maybe add support for preview images?
 * TODO: Add general support for alpha channel?
 */

typedef float FloatUsed;
const Imf::PixelType kEXRFloatUsed = Imf::FLOAT;

typedef enum EAttributeType {
	EAttributeChannelList = 0,
	EAttributeCompression,
	EAttributeLineOrder,
	EAttributeChromaticities,
	EAttributeEnvmap,
	EAttributeString,
	EAttributeBox2f,
	EAttributeBox2i,
	EAttributeV2f,
	EAttributeV2i,
	EAttributeVectorf,
	EAttributeVectori,
	EAttributeDouble,
	EAttributeFloat,
	EAttributeInt,
	EAttributeTypeLength,
	EAttributeTypeInvalid = -1
} EAttributeType;

inline mex::MxNumeric<bool> isOpenExrFile(const mex::MxString& fileName) {
	return mex::MxNumeric<bool>(Imf::isOpenExrFile(fileName.c_str()));
}

/*
 * TODO: Maybe replace remaining arguments of these functions to use directly
 * MxArrays?
 */
class EXRInputFile {
public:
	explicit EXRInputFile(const mex::MxString& fileName)
							: m_file(fileName.c_str()),
							  m_frameBuffer(),
							  m_pixelBuffer(),
							  m_foundChannel(),
							  m_createdFrameBuffer(false),
							  m_readFile(false) {
		mexAssert(Imf::isOpenExrFile(fileName.c_str()));
	}

	inline int getWidth() const {
		Imath::Box2i dw = m_file.header().dataWindow();
		return dw.max.x - dw.min.x + 1;
	}

	inline int getHeight() const {
		Imath::Box2i dw = m_file.header().dataWindow();
		return dw.max.y - dw.min.y + 1;
	}

	inline void getDimensions(int& width, int& height) const {
		Imath::Box2i dw = m_file.header().dataWindow();
		width = dw.max.x - dw.min.x + 1;
		height = dw.max.y - dw.min.y + 1;
	}

	inline int getNumberOfChannels() const {
		int numChannels = 0;
		for (Imf::ChannelList::ConstIterator
				channelIter = m_file.header().channels().begin(),
				channelEnd = m_file.header().channels().end();
				channelIter != channelEnd;
				++channelIter, ++numChannels) {
			continue;
		}
		return numChannels;
	}

	inline std::vector<std::string> getChannelNames() const {
		std::vector<std::string> channelNames(0);
		for (Imf::ChannelList::ConstIterator
				channelIter = m_file.header().channels().begin(),
				channelEnd = m_file.header().channels().end();
				channelIter != channelEnd;
				++channelIter) {
			channelNames.push_back(std::string(channelIter.name()));
		}
		return channelNames;
	}

	inline bool isComplete() const {
		return m_file.isComplete();
	}

	inline bool hasChannel(const std::string& channelName) const {
		return m_file.header().channels().findChannel(channelName.c_str()) != 0;
	}

	mex::MxArray getAttribute(const mex::MxString& attributeName) const;
	mex::MxArray getAttribute() const;

	void readChannelRGB();
	void readChannelY();
	void readChannel(const std::string& channelName);
	void readChannel(const std::vector<std::string>& channelNames);

	mex::MxNumeric<FloatUsed> readFile();

	~EXRInputFile() {
		if (m_createdFrameBuffer) {
			m_pixelBuffer.destroy();
		}
	}

private:
	mex::MxArray* getAttribute_sub(const mex::MxString& attributeName) const;
	mex::MxArray* getAttribute_sub() const;

	Imf::InputFile m_file;
	Imf::FrameBuffer m_frameBuffer;
	mex::MxNumeric<FloatUsed> m_pixelBuffer;
	std::vector<bool> m_foundChannel;
	bool m_createdFrameBuffer;
	bool m_readFile;
};

class EXROutputFile {
public:
	/*
	 * TODO: Should this be initialized directly from pixels MxNumeric? It may
	 * be desirable to create just the header. On the other hand, it would be
	 * safer in terms of passing width and height arguments to the
	 * initialization of the header.
	 */
	EXROutputFile(const int width, const int height)
				: m_header(width, height),
				  m_frameBuffer(),
				  m_pixelBuffer(),
				  m_createdFrameBuffer(false),
				  m_wroteFile(false) {	}

	inline int getWidth() const {
		Imath::Box2i dw = m_header.dataWindow();
		return dw.max.x - dw.min.x + 1;
	}

	inline int getHeight() const {
		Imath::Box2i dw = m_header.dataWindow();
		return dw.max.y - dw.min.y + 1;
	}

	inline void getDimensions(int& width, int& height) const {
		Imath::Box2i dw = m_header.dataWindow();
		width = dw.max.x - dw.min.x + 1;
		height = dw.max.y - dw.min.y + 1;
	}

	void setAttribute(const mex::MxArray& attribute,
					const mex::MxString& attributeName);
	void setAttribute(const mex::MxStruct& attributes);

	void writeChannelRGB(const mex::MxNumeric<FloatUsed>& rgbPixels);
	void writeChannelY(const mex::MxNumeric<FloatUsed>& yPixels);
	void writeChannel(const mex::MxNumeric<FloatUsed>& channelPixels,
						const std::string& channelName);
	void writeChannel(const mex::MxNumeric<FloatUsed>& channelPixels,
						const std::vector<std::string>& channelNames);

	void writeFile(const mex::MxString& fileName);

	~EXROutputFile() {
		if (m_createdFrameBuffer) {
			m_pixelBuffer.destroy();
		}
	}

private:
	Imf::Header m_header;
	Imf::FrameBuffer m_frameBuffer;
	mex::MxNumeric<FloatUsed> m_pixelBuffer;
	bool m_createdFrameBuffer;
	bool m_wroteFile;
};

}	/* namespace exr */

#endif /* OPENEXR_MEX_H_ */
