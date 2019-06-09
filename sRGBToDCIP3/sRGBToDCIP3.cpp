// sRGBToDCIP3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <nvtt.h>

using std::cin;

enum ImageType {
	ImageType_RGB,
	ImageType_RGBA,
	ImageType_Normal,
	ImageType_HDR,
};

int main()
{
	std::cout << "Welcome to texture converter using NVTT." << std::endl 
		<< "Enter file path to texture for conversion:" << std::endl;

	std::string filePath;

	cin >> filePath;

	std::cout << "Converting " << filePath.c_str() << " to DCI-P3." << std::endl;

	nvtt::Surface img;

	// 0 = other
	// 1 = tga
	// 2 = dds
	int type = 0;

	if (!img.load(filePath.c_str()))
	{
		printf("Input image '%s' not found.\n", filePath.c_str());
		return EXIT_FAILURE;
	}

	const std::string fileName = filePath.substr(filePath.find_last_of("\\") + 1);
	const std::string extension = fileName.substr(fileName.find_last_of(".") + 1);

	if (strcmp("tga", extension.c_str()) == 0)
		type = 1;

	if (strcmp("dds", extension.c_str()) == 0)
		type = 2;

	std::cout << "Converting: " << fileName << std::endl;

	if (type == 1 || type == 2)		// Lightmap's alpha channel is coverage.
		img.setAlphaMode(nvtt::AlphaMode_Transparency);

	if (img.isNormalMap())
		img.normalizeNormalMap();

	img.toGamma(2.6f);

	static const float P3_r_x = 0.680f;
	static const float P3_r_y = 0.320f;
	static const float P3_r_z = 0.000f;
	static const float P3_g_x = 0.265f;
	static const float P3_g_y = 0.690f;
	static const float P3_g_z = 0.045f;
	static const float P3_b_x = 0.150f;
	static const float P3_b_y = 0.060f;
	static const float P3_b_z = 0.790f;
	static const float P3_w_x = 0.314f;
	static const float P3_w_y = 0.351f;
	static const float P3_w_z = 0.000f;
	static const float P3_offset_x = 0.f;
	static const float P3_offset_y = 0.f;
	static const float P3_offset_z = 0.f;

	img.transform(&P3_r_x, &P3_g_x, &P3_b_x, &P3_w_x, &P3_offset_x);
	img.transform(&P3_r_y, &P3_g_y, &P3_b_y, &P3_w_y, &P3_offset_y);
	img.transform(&P3_r_z, &P3_g_z, &P3_b_z, &P3_w_z, &P3_offset_z);

	if (!type)
		filePath.append(".dds");

	nvtt::OutputOptions outputOptions;
	outputOptions.setFileName(filePath.c_str());
	
	nvtt::CompressionOptions compressionOptions;

	nvtt::Context context;

	if (!context.outputHeader(img, 1, compressionOptions, outputOptions)) {
		fprintf(stderr, "Error writing file header.\n");
		return EXIT_FAILURE;
	}
	if (!context.compress(img, 0, 0, compressionOptions, outputOptions)) {
		fprintf(stderr, "Error compressing file.\n");
		return EXIT_FAILURE;
	}

	std::cout << "Conversion finished." << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
