#ifndef RTWIMAGE_H
#define RTWIMAGE_H

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"
#include <cstdlib>
#include <iostream>

class rtwImage {
public:
	rtwImage() {}

	rtwImage(const char* image_filename) {
		//If we have env variable, only look there. Check parents.

		auto filename = std::string(image_filename);
		auto imagedir = getenv("RTW_IMAGES");

		// Hunt for the image file in some likely locations.
		if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
		if (load(filename)) return;
		if (load("images/" + filename)) return;
		if (load("../images/" + filename)) return;
		if (load("../../images/" + filename)) return;
		if (load("../../../images/" + filename)) return;
		if (load("../../../../images/" + filename)) return;
		if (load("../../../../../images/" + filename)) return;
		if (load("../../../../../../images/" + filename)) return;

		std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
	}

	~rtwImage() {
		delete[] bdata;
		STBI_FREE(fdata);
	}

	bool load(const std::string& filename) {
		//Loads linear image data from file. True if succeed.

		auto n = bytes_per_pixel;
		fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
		if (fdata == nullptr) return false;
		bytes_per_scanline = image_width * bytes_per_pixel;

		convertToBytes();
		return true;
	}

	int width() const { return (fdata == nullptr) ? 0 : image_width; }
	int height() const { return (fdata == nullptr) ? 0 : image_height; }

	const unsigned char* pixel_data(int x, int y) const {
		//Address of the three RGB bytes of pixel (x, y). Magenta default
		static unsigned char magenta[] = { 255, 0, 255 };
		if (bdata == nullptr) return magenta;
		x = clamp(x, 0, image_width);
		y = clamp(y, 0, image_height);

		return bdata + y*bytes_per_scanline + x*bytes_per_pixel;
	}

private:
	const int bytes_per_pixel = 3;
	float* fdata = nullptr; //Linear floating point pixel data
	unsigned char* bdata = nullptr; //Linear 8-bit pixel data
	int image_width = 0;
	int image_height = 0;
	int bytes_per_scanline = 0;

	static int clamp(int x, int low, int high) {
		if (x < low) return low;
		if (x < high) return x;
		return high - 1;
	}

	static unsigned char floatToByte(float value) {
		if (value <= 0.0)
			return 0;
		if (1.0 <= value)
			return 255;
		return static_cast<unsigned char>(256.0 * value);
	}

	void convertToBytes() {
		int totalBytes = image_width * image_height * bytes_per_pixel;
		bdata = new unsigned char[totalBytes];

		auto* bptr = bdata;
		auto* fptr = fdata;

		for (auto i = 0; i < totalBytes; i++, fptr++, bptr++) {
			*bptr = floatToByte(*fptr);
		}
	}
};

#ifdef _MSC_VER
	#pragma warning (pop)
#endif

#endif