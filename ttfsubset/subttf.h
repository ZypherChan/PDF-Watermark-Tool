#ifndef SUBTTF_H
#define SUBTTF_H

#include <vector>

// get subsetted TrueType font data
std::vector<uint8_t> subsetTrueTypeFontData(
	const char* input_font_path,
	const wchar_t* extract_wchs);

#endif