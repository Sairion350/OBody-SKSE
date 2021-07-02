#pragma once

namespace stl
{
	bool contains(std::string const& a_text, std::string const& a_sub)
	{
		if (a_sub.length() > a_text.length())
			return false;

		auto it = std::search(
			a_text.begin(), a_text.end(),
			a_sub.begin(), a_sub.end(),
			[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); });

		return it != a_text.end();
	}

	bool contains(std::string const& a_text, std::vector<std::string> const& a_subs)
	{
		for (auto& sub : a_subs) {
			if (contains(a_text, sub))
				return true;
		}

		return false;
	}

	void files(const fs::path& a_path, std::vector<std::string>& a_files, const char* a_ext = 0)
	{
		for (const auto& entry : fs::directory_iterator(a_path)) {
			auto path = entry.path();
			if (a_ext) {
				auto ext = path.extension().string();
				if (ext != a_ext)
					continue;
			}

			a_files.push_back(path.string());
		}
	}

	bool cmp(std::string a_str1, std::string a_str2)
	{
		return std::equal(
			a_str1.begin(), a_str1.end(),
			a_str2.begin(), a_str2.end(),
			[](char a, char b) { return tolower(a) == tolower(b); });
	}

	bool cmp(const char* a_str1, const char* a_str2)
	{
		return cmp(std::string{ a_str1 }, std::string{ a_str2 });
	}

	float random(float a_val1, float a_val2)
	{
		// non-inclusive
		float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		float diff = a_val2 - a_val1;
		float r = random * diff;
		return a_val1 + r;
	}

	int random(int a_val1, int a_val2)
	{
		// non-inclusive
		int random = std::rand() / RAND_MAX;
		int diff = a_val2 - a_val1;
		int r = random * diff;
		return a_val1 + r;
	}

	bool chance(int a_chance)
	{
		float roll = random(0.0f, 99.0f);
		return roll <= static_cast<float>(a_chance);
	}
}
