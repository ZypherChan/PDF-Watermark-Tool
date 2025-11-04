#include "subttf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sfntly/font.h"
#include "subtly/character_predicate.h"
#include "subtly/stats.h"
#include "subtly/subsetter.h"
#include "subtly/utils.h"

std::vector<uint8_t> subsetTrueTypeFontData(const char* input_font_path, const wchar_t* extract_wchs)
{
	std::vector<uint8_t> subset;
	using namespace subtly;
	if (input_font_path == NULL || extract_wchs == NULL || wcslen(extract_wchs) == 0)
		return subset;

	FontPtr font;
	font.Attach(subtly::LoadFont(input_font_path));
	if (font->num_tables() == 0) {
		fprintf(stderr, "Could not load font %s.\n", input_font_path);
		return subset;
	}

	int32_t original_size = TotalFontSize(font);

	size_t size = wcslen(extract_wchs);
	IntegerSet* extract_set = new IntegerSet;
	for (size_t i = 0; i < size; i++) {
		extract_set->insert(extract_wchs[i]);
	}

	Ptr<CharacterPredicate> set_predicate =
		new AcceptSet(extract_set);
	Ptr<Subsetter> subsetter = new Subsetter(font, set_predicate);
	Ptr<Font> new_font;

	new_font.Attach(subsetter->Subset());
	if (!new_font) {
		fprintf(stdout, "%s Cannot create subset.\n", input_font_path);
		return subset;
	}

	subtly::SerializeFont(subset, new_font);

	subtly::PrintComparison(stdout, font, new_font);
	int32_t new_size = TotalFontSize(new_font);
	fprintf(stdout, "Went from %d to %d: %lf%% of original\n",
		original_size, new_size,
		static_cast<double>(new_size) / original_size * 100);

	if (subsetter->HasMissingGlyphs())
	{
		fprintf(stdout,  "Font has missing glyphs");
		return subset;
	}

	return subset;
}
