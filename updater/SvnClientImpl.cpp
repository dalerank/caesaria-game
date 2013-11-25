/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5122 $ (Revision of last commit) 
 $Date: 2011-12-11 23:47:31 +0400 (Вс, 11 дек 2011) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#include "SvnClientImpl.h"

#ifdef USE_LIBSVN

#include <svn_client.h>
#include <svn_pools.h>
#include <svn_fs.h>
#include <svn_config.h>
#include <svn_cmdline.h>

#include "TraceLog.h"

namespace tdm
{

SvnClientImpl::SvnClientImpl() :
	_isActive(true)
{
	// Initialize the app.  Send all error messages to nothing
	if (svn_cmdline_init("minimal_client", NULL) != EXIT_SUCCESS)
	{
		throw std::runtime_error("Could not initialise SVN commandline.");
	}

	// Create our main pool
	_pool = svn_pool_create(NULL);

	/* Initialize the FS library. */
	svn_error_t* err = svn_fs_initialize(_pool);

	if (err)
	{
		throw std::runtime_error("Could not initialise SVN filesystem library.");
	}

	/* Make sure the ~/.subversion run-time config files exist */
	err = svn_config_ensure(NULL, _pool);

	if (err)
	{
		throw std::runtime_error("Could not initialise SVN configuration.");
	}

	// Initialize and allocate the client_ctx object. 
	if ((err = svn_client_create_context(&_context, _pool)))
	{
		throw std::runtime_error("Could not create SVN context.");
	}

	// Load the run-time config file into a hash
	if ((err = svn_config_get_config (&(_context->config), NULL, _pool)))
	{
		throw std::runtime_error("Could not load SVN config.");
	}

#ifdef WIN32
	// Set the working copy administrative directory name.
	if (getenv("SVN_ASP_DOT_NET_HACK"))
	{
		err = svn_wc_set_adm_dir("_svn", _pool);
		
		if (err)
		{
			throw std::runtime_error("Could not set SVN admin directory.");
		}
	}
#endif	
}

SvnClientImpl::~SvnClientImpl()
{
	svn_pool_destroy(_pool);
}

void SvnClientImpl::SetActive(bool active)
{
	if (!active)
	{
		TraceLog::WriteLine(LOG_STANDARD, "Deactivating SVN client checks.");
	}

	_isActive = active;
}

svn_error_t* info_receiver_dummy(void* baton, const char* path, const svn_info_t* info, apr_pool_t* pool)
{
	return SVN_NO_ERROR;
}

bool SvnClientImpl::FileIsUnderVersionControl(const fs::path& path)
{
	if (!_isActive) return true; // deactivated clients will return true on all cases

	apr_pool_t* subpool = svn_pool_create(_pool);

	svn_error_t* err = svn_client_info2(path.string().c_str(),
		NULL,	// peg_revision
		NULL,	// revision
		info_receiver_dummy, // receiver
		NULL,	// receiver_baton
		svn_depth_empty,
		NULL,	// changelists
		_context,
		subpool
	);

	bool isUnderVersionControl = (err == NULL);

	svn_pool_destroy(subpool);

	return isUnderVersionControl;
}

} // namespace

#endif