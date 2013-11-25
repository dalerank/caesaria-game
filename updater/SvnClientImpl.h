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

#pragma once

#include "SvnClient.h"

#ifdef USE_LIBSVN

// Forward decl.
typedef struct apr_pool_t apr_pool_t;
typedef struct svn_client_ctx_t svn_client_ctx_t;

namespace tdm
{

// Implementation
class SvnClientImpl :
	public SvnClient
{
private:
	// SVN memory pool
	apr_pool_t* _pool;

	// The SVN client context
	svn_client_ctx_t* _context;

	bool _isActive;

public:
	SvnClientImpl();

	virtual ~SvnClientImpl();

	// Activate/Deacticate the client. Deactivated clients will return true in FileIsUnderVersionControl().
	virtual void SetActive(bool active);

	// Returns true if the given file is under version control, false in all other cases
	virtual bool FileIsUnderVersionControl(const fs::path& path);
};

} // namespace

#endif
