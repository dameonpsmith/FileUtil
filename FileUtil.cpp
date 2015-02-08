// FileDup.cpp : Defines the entry point for the console application.
//
//
#include "stdafx.h"
#include <map>
#include <vector>
#include <stack>
#include <boost/filesystem.hpp>
#include <iostream>
#include <algorithm>
#include <chrono>
namespace FileDupTest
{
	using namespace std;
	using namespace boost::filesystem;
	class FileUtil
	{
	public:
		static std::vector<char> ReadAllBytes(string filename)
		{
			ifstream ifs(filename, ios::binary | ios::ate);
			auto pos = ifs.tellg();
			std::vector<char>  result(pos);
			ifs.seekg(0, ios::beg);
			ifs.read(&result[0], pos);
			return result;
		}
		static map<uintmax_t, vector<string>> MapFilesBySize(vector<string> paths)
		{
			map<uintmax_t, vector<string>> results;
			stack<directory_iterator> estack;
			boost::system::error_code ec;
			for (auto path : paths)
			{
				try
				{
					estack.push(directory_iterator(path));
				}
				catch (exception e){ continue; }
				while (!estack.empty())
				{
					while ( estack.top() != directory_iterator() )
					{
						auto info = estack.top()->status();
						try
						{
							if (info.type() == file_type::directory_file)
							{
								estack.push(directory_iterator(estack.top()->path()));
							}
							else
							{
								results[file_size(estack.top()->path())].push_back(estack.top()->path().string());
								estack.top()++;
							}
						}
						catch (exception e){ if (!estack.empty())estack.top()++; continue; }
					}
					estack.pop();
					if (!estack.empty())estack.top()++;
				}
			}
			return results;
		}
		static vector<vector<string>> FindDuplicateFiles(vector<string> paths)
		{
			vector<vector<string>> results;
			auto sourceDictionary = MapFilesBySize(paths);
			vector<string> equivalentFiles;
			for (auto sourceFiles : sourceDictionary)
			{
				if (sourceFiles.second.size() > 2)
				{
					for (int i = 0; i < sourceFiles.second.size()-1; i++)
					{
						auto sourceFile = sourceFiles.second[i];
						if (sourceFile != string())
						{
							equivalentFiles.clear();
							vector<char> sourceData;
							try
							{
								sourceData = ReadAllBytes(sourceFile);
							}
							catch (exception e)
							{ continue; }
							for (int j = i + 1; j < sourceFiles.second.size(); j++)
							{
								auto sampleFile = sourceFiles.second[j];
								if (sampleFile != string())
								{
									vector<char> sampleData;
									try
									{
										sampleData = ReadAllBytes(sampleFile);
									}
									catch (exception e)
									{ break; }

									if (equal(sourceData.cbegin(), sourceData.cend(), sampleData.cbegin()))
									{
										equivalentFiles.push_back(sampleFile);
										sourceFiles.second[j] = string();
									}
								}
							}
							if (equivalentFiles.size() > 0)
							{
								equivalentFiles.push_back(sourceFile);
								results.push_back(equivalentFiles);
							}
						}
					}
				}
			}
			return results;
		}
	};
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::vector < std::string > input;
	input.push_back("C:\\program files (x86)");
	auto begin = std::chrono::system_clock::now();
	auto results = FileDupTest::FileUtil::FindDuplicateFiles(input);
	auto end = std::chrono::system_clock::now();
	int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	std::cout << duration;
	return 0;
}

