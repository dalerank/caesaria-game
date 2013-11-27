
#pragma once

#include <list>
#include <fstream>
#include <boost/spirit/include/classic.hpp>
#include <boost/bind.hpp>

#include "PackageInstructions.h"
#include "SvnClient.h"

namespace bs = boost::spirit::classic;

namespace tdm
{

struct ManifestFile
{
	// The source file path relative to darkmod/
	fs::path sourceFile;

	// The destination path in the release, is almost always the same as sourceFile
	// except for some special cases (config.spec, DoomConfig.cfg, etc.)
	fs::path destFile;

	ManifestFile(const fs::path& sourceFile_) :
		sourceFile(sourceFile_),
		destFile(sourceFile_)
	{}

	ManifestFile(const fs::path& sourceFile_, const fs::path& destFile_) :
		sourceFile(sourceFile_),
		destFile(destFile_)
	{}

	// Comparison operator for sorting
	bool operator<(const ManifestFile& other) const
	{
		return this->sourceFile < other.sourceFile;
	}

	// Comparison operator for removing duplicates
	bool operator==(const ManifestFile& other) const
	{
		return this->sourceFile == other.sourceFile && this->destFile == other.destFile;
	}
};

/**
 * The manifest contains all the files which should go into a TDM release.
 * The list is unsorted, it doesn't matter if files are duplicate, as they
 * are re-sorted into PK4 mappings anyways, which resolves any double entries.
 *
 * File format:
 * Each line contains a single file, its path relative to darkmod/.
 * Comments lines start with the # character.
 * It's possible to move files to a different location by using the => syntax: 
 * e.g. ./devel/release/config.spec => ./config.spec
 */
class ReleaseManifest :
	public std::list<ManifestFile>
{
public:
	void LoadFromFile(const fs::path& manifestFile)
	{
		// Start parsing
		std::ifstream file(manifestFile.string().c_str());

		if (!file)
		{
			TraceLog::WriteLine(LOG_VERBOSE, "[ReleaseManifest]: Cannot open file " + manifestFile.string());
			return;
		}

		LoadFromStream(file);
	}

	void LoadFromStream(std::istream& stream)
	{
		// Read the whole stream into a string
		std::string buffer(std::istreambuf_iterator<char>(stream), (std::istreambuf_iterator<char>()));

		LoadFromString(buffer);
	}

	void LoadFromString(const std::string& str)
	{
		clear();

		// Comment starting character: #
		bs::rule<> char_start_comment = bs::ch_p('#');

		// Define blank characters
		bs::rule<> blanks_p = * bs::blank_p;

		// Define comment lines
		bs::rule<> l_comment = blanks_p >> char_start_comment >> *bs::print_p >> bs::eol_p; 

		// Define empty lines
		bs::rule<> l_empty = blanks_p >> bs::eol_p; 

		// A filename
		bs::rule<> filename_ident = +(bs::print_p - bs::ch_p('='));

		// The relocator => 
		bs::rule<> relocator_ident = bs::ch_p('=') >> bs::ch_p('>');

		// Define relocation rules
		bs::rule<> l_filename = 
					blanks_p >> 
					filename_ident[ boost::bind(&ReleaseManifest::AddSourceFile, this, _1, _2) ] >> 
					blanks_p >> 
					*relocator_ident >> 
					blanks_p >> 
					(*filename_ident) [ boost::bind(&ReleaseManifest::AddDestFile, this, _1, _2) ] >>
					blanks_p >> 
					bs::eol_p
		;

		bs::rule<> lines = l_comment | l_filename | l_empty;
		bs::rule<> manifestDef =  bs::lexeme_d [ * lines ] ;

		bs::parse_info<> info = bs::parse(str.c_str(), manifestDef);

		if (info.full)
		{
			TraceLog::WriteLine(LOG_VERBOSE, "Successfully parsed the whole manifest.");
		}
		else
		{
			TraceLog::WriteLine(LOG_VERBOSE, "Could not fully parse the manifest.");
		}
	}

	/**
	 * greebo: Collects all files from the SVN repository if they are meant to be included by
	 * the instructions given (which were loaded from darkmod_maps.txt).
	 * If @skipVersioningCheck is true no SVN client checks are performed. Still, .svn folders won't be included.
	 */
	void CollectFilesFromRepository(const fs::path& repositoryRoot, const PackageInstructions& instructions, 
									bool skipVersioningCheck = false)
	{
		// Acquire an SVN client implementation
		SvnClientPtr svnClient = SvnClient::Create();

		if (skipVersioningCheck)
		{
			// Deactivate the client class (won't perform checks, will always return true)
			svnClient->SetActive(false);
		}

		// Process the inclusion commands and exclude all files as instructed
		for (PackageInstructions::const_iterator instr = instructions.begin(); instr != instructions.end(); ++instr)
		{
			if (instr->type == PackageInstruction::Include)
			{
				ProcessInclusion(repositoryRoot, instructions, *instr, *svnClient);
			}
		}
	}

	void WriteToFile(const fs::path& manifestPath)
	{
		std::ofstream manifest(manifestPath.string().c_str());

		time_t rawtime;
		time(&rawtime);

		manifest << "#" << std::endl;
		manifest << "# This file was generated automatically:" << std::endl;
		manifest << "#   by tdm_package.exe" << std::endl;
		manifest << "#   at " << asctime(localtime(&rawtime)) << std::endl;
		manifest << "# Any modifications to this file will be lost, instead modify:" << std::endl;
		manifest << "#   devel/manifests/base.txt" << std::endl;
		manifest << "#" << std::endl;
		manifest << std::endl;

		for (const_iterator i = begin(); i != end(); ++i)
		{
			manifest << "./" << i->sourceFile;

			// Do we have a redirection?
			if (i->destFile != i->sourceFile)
			{
				manifest << " => ./" << i->destFile;
			}

			manifest << std::endl;
		}
	}

private:
	void ProcessInclusion(const fs::path& repositoryRoot, const PackageInstructions& instructions, 
						  const PackageInstruction& inclusion, SvnClient& svn)
	{
		TraceLog::WriteLine(LOG_STANDARD, "Processing inclusion: " + inclusion.value);

		// Construct the starting path
		fs::path inclusionPath = repositoryRoot;
		inclusionPath /= boost::algorithm::trim_copy(inclusion.value);

		// Add the inclusion path itself
		std::string relativeInclusionPath = inclusionPath.string().substr(repositoryRoot.string().length() + 1);

		// Cut off the trailing slash
		if (boost::algorithm::ends_with(relativeInclusionPath, "/"))
		{
			relativeInclusionPath = relativeInclusionPath.substr(0, relativeInclusionPath.length() - 1);
		}

		TraceLog::WriteLine(LOG_VERBOSE, "Adding inclusion path: " + relativeInclusionPath);
		push_back(ManifestFile(relativeInclusionPath));

		// Check for a file
		if (fs::is_regular_file(inclusionPath))
		{
			TraceLog::WriteLine(LOG_VERBOSE, "Investigating file: " + inclusionPath.string());

			// Add this item, if versioned
			if (!svn.FileIsUnderVersionControl(inclusionPath))
			{
				TraceLog::WriteLine(LOG_STANDARD, "Skipping unversioned file mentioned in INCLUDE statements: " + inclusionPath.string());
				return;
			}

			// Cut off the repository path to receive a relative path (cut off the trailing slash too)
			std::string relativePath = inclusionPath.string().substr(repositoryRoot.string().length() + 1);

			if (!instructions.IsExcluded(relativePath))
			{
				TraceLog::WriteLine(LOG_VERBOSE, "Adding file: " + relativePath);
				push_back(ManifestFile(relativePath));
			}
			else
			{
				TraceLog::WriteLine(LOG_VERBOSE, "File is excluded: " + relativePath);
				return;
			}
		}
		
		//TraceLog::WriteLine(LOG_PROGRESS, "Inclusion path: " + inclusionPath.string() + "---");

		if (!fs::is_directory(inclusionPath))
		{
			TraceLog::WriteLine(LOG_PROGRESS, "Skipping non-file and non-folder: " + inclusionPath.string());
			return;
		}

		ProcessDirectory(repositoryRoot, inclusionPath, instructions, svn);
	}

	// Adds stuff in the given directory (absolute path), adds stuff only if not excluded, returns the number of added items
	std::size_t ProcessDirectory(const fs::path& repositoryRoot, const fs::path& dir, 
								 const PackageInstructions& instructions, SvnClient& svn)
	{
		assert(fs::exists(dir));

		std::size_t itemsAdded = 0;

		// Traverse this folder
		for (fs::directory_iterator i = fs::directory_iterator(dir);
			 i != fs::directory_iterator(); ++i)
		{
			if (boost::algorithm::ends_with(i->path().string(), ".svn"))
			{
				// Prevent adding .svn folders
				continue;
			}

			// Ensure this item is under version control
			if (!svn.FileIsUnderVersionControl(*i))
			{
				TraceLog::WriteLine(LOG_PROGRESS, "Skipping unversioned item: " + i->path().string());
				continue;
			}

			TraceLog::WriteLine(LOG_VERBOSE, "Investigating item: " + i->path().string());

			// Cut off the repository path to receive a relative path (cut off the trailing slash too)
			std::string relativePath = i->path().string().substr(repositoryRoot.string().length() + 1);

			// Consider the exclusion list
			if (instructions.IsExcluded(relativePath))
			{
				TraceLog::WriteLine(LOG_VERBOSE, "Item is excluded: " + relativePath);
				continue;
			}
			
			if (fs::is_directory(*i))
			{
				// Versioned folder, enter recursion
				std::size_t folderItems = ProcessDirectory(repositoryRoot, *i, instructions, svn);

				if (folderItems > 0)
				{
					// Add an entry for this folder itself, it's not empty
					push_back(ManifestFile(relativePath));
				}

				itemsAdded += folderItems;
			}
			else
			{
				// Versioned file, add it
				TraceLog::WriteLine(LOG_VERBOSE, "Adding file: " + relativePath);
				push_back(ManifestFile(relativePath));

				itemsAdded++;
			}
		}

		return itemsAdded;
	}

	void AddSourceFile(char const* beg, char const* end)
	{
		if (end - beg < 2) return; // skip strings smaller than 2 chars

		if (beg[0] == '.' && beg[1] == '/') 
		{
			beg += 2; // skip leading ./
		}

		push_back(ManifestFile(boost::algorithm::trim_copy(std::string(beg, end))));
	}

	void AddDestFile(char const* beg, char const* end)
	{
		if (end - beg < 2) return; // skip strings smaller than 2 chars

		if (beg[0] == '.' && beg[1] == '/') 
		{
			beg += 2; // skip leading ./
		}

		// Set the destination on the last element
		assert(!empty());

		back().destFile = boost::algorithm::trim_copy(std::string(beg, end));
	}

};

} // namespace
