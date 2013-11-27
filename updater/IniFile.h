/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5564 $ (Revision of last commit) 
 $Date: 2012-09-11 08:31:27 +0400 (Вт, 11 сен 2012) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include <string>
#include <map>
#include <set>
#include <istream>
#include <vector>
#include "core/smartptr.hpp"
#include "vfs/path.hpp"
#include "core/referencecounted.hpp"

namespace tdm
{

class IniFile;
typedef SmartPtr<IniFile> IniFilePtr;

class IniFile : public ReferenceCounted
{
public:
	struct Option { std::string key; std::string value; };

	typedef std::vector<Option> Options;
	typedef std::map<std::string, Options> Sections;

private:
	Sections _settings;

	// Private constructors
	IniFile();
	IniFile(const std::string& str);

public:
	// ---------- Named Constructors -----------------

	// Construct an INI file from the given filename
	// Can return NULL if the given file cannot be read
	static IniFilePtr ConstructFromFile(vfs::Path filename);

	// Construct an INI file from the given input stream
	// Always returns non-NULL
	static IniFilePtr ConstructFromStream(std::istream& stream);

	// Construct an INI file from the given string
	// Always returns non-NULL
	static IniFilePtr ConstructFromString(const std::string& str);

	// Creates an empty INI file
	static IniFilePtr Create();

	// ---------- Public Methods ---------------------

	// Returns TRUE if this file has no sections
	bool IsEmpty() const;

	/** 
	 * Add a named section. This won't do anything if the section already exists.
	 */
	void AddSection(const std::string& name);

	/** 
	 * Remove a named section including all keyvalue pairs belonging to it.
	 * Returns true if the section was found and removed, false otherwise.
	 */
	bool RemoveSection(const std::string& section);

	/**
	 * Returns a keyvalue from this INI file, from the given section,
	 * or an empty string if the key was not found.
	 */
	std::string GetValue(const std::string& section, const std::string& key) const;

	/** 
	 * Sets the given key in the given section to the given value. Always succeeds,
	 * existing values are overwritten.
	 */
	void SetValue(const std::string& section, const std::string& key, const std::string& value);

	/** 
	 * Removes the key from the given section.
	 * Returns TRUE if the key was present, FALSE otherwise
	 */
	bool RemoveKey(const std::string& section, const std::string& key);

	/**
	 * Returns all key/value pairs for the given section.
	 */
	Options GetAllKeyValues(const std::string& section) const;

	class SectionVisitor
	{
	public:
		virtual void VisitSection(const IniFile& iniFile, const std::string& sectionName) = 0;
	};

	/**
	 * Iterates over all sections, and invoking the given functor
	 */
	void ForeachSection(SectionVisitor& visitor) const;

	// Saves the contents into the given text file (target will be overwritten if existing)
	// One can pass a header string which will be prepended at the top of the file, in comment form.
	// The header string should be passed without the # character, these are added automatically.
	void ExportToFile(vfs::Path file, const std::string& headerComments = "") const;

private:
	void ParseFromString(const std::string& str);
};

} // namespace
