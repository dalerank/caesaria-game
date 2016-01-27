// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __CAESARIA_PROGRESSHANDLER_H_INLCUDE__
#define __CAESARIA_PROGRESSHANDLER_H_INLCUDE__

#include "updater.hpp"
#include "updateview.hpp"

namespace updater
{

class ProgressHandler :
	public updater::Updater::DownloadProgress,
	public updater::Updater::FileOperationProgress
{
private:
	updater::CurDownloadInfo _info;

	IUpdateView& _view;

	double _recentDownloadSpeed;

  ProgressHandler(IUpdateView& view) :
    _view(view)
  {}

public:
  static SmartPtr<ProgressHandler> create( IUpdateView& view )
  {
    SmartPtr<ProgressHandler> ret( new ProgressHandler( view ) );
    ret->drop();

    return ret;
  }

	void OnOverallProgress(const OverallDownloadProgressInfo& info)
	{
		ProgressInfo progress;

		switch (info.updateType)
		{
		case OverallDownloadProgressInfo::Full:
			progress.type = ProgressInfo::FullUpdateDownload;
			break;
		case OverallDownloadProgressInfo::Differential:
			progress.type = ProgressInfo::DifferentialPackageDownload;
			break;
		default:
			return; // ignore unknown cases
		};

		progress.progressFraction = info.progressFraction > 1.0 ? 1.0 : info.progressFraction;
    progress.connection.downloaded = info.downloadedBytes;
    progress.connection.need = info.totalDownloadSize;
		progress.filesToDownload = info.filesToDownload;

		_view.onProgressChange(progress);
	}

	void OnDownloadProgress(const CurDownloadInfo& info)
	{
		ProgressInfo progress;

		progress.type = ProgressInfo::FileDownload;
		progress.file = info.file;
		progress.progressFraction = info.progressFraction > 1.0 ? 1.0 : info.progressFraction;
		progress.mirrorDisplayName = info.mirrorDisplayName;
    progress.connection.speed = info.downloadSpeed;
    progress.connection.downloaded = info.downloadedBytes;
    progress.connection.need = 0;
		progress.filesToDownload = 1;

		_view.onProgressChange(progress);

    _recentDownloadSpeed = progress.connection.speed;
	}

	void onDownloadFinish()
	{
		ProgressInfo progress;

		progress.type = ProgressInfo::FileDownload;
		progress.progressFraction = 1.0;
    progress.connection.speed = _recentDownloadSpeed;

		_view.onProgressChange(progress);
	}

	void onFileOperationProgress(const CurFileInfo& info)
	{
		ProgressInfo progress;

		progress.type = ProgressInfo::FileOperation;
		progress.file = info.file;
		progress.progressFraction = info.progressFraction > 1.0 ? 1.0 : info.progressFraction;
		progress.operationType = GetOperationTypeForFileInfo(info);

		_view.onProgressChange(progress);
	}

	void OnFileOperationFinish()
	{
		ProgressInfo progress;

		progress.type = ProgressInfo::FileOperation;
		progress.progressFraction = 1.0;
		progress.operationType = ProgressInfo::Unspecified;

		_view.onProgressChange(progress);
	}

private:
	ProgressInfo::FileOperationType GetOperationTypeForFileInfo(const CurFileInfo& info)
	{
		switch (info.operation)
		{
			case CurFileInfo::Check: return ProgressInfo::Check;
			case CurFileInfo::Delete: return ProgressInfo::Remove;
			case CurFileInfo::Replace: return ProgressInfo::Replace;
			case CurFileInfo::Add: return ProgressInfo::Add;
			default: return ProgressInfo::Unspecified;
		};
	}
};
typedef SmartPtr<ProgressHandler> ProgressHandlerPtr;

} // namespace

#endif //__CAESARIA_PROGRESSHANDLER_H_INLCUDE__
