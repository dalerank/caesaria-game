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

/**
 * greebo: I copied the main part of this auxiliary header 
 * directly from the TortoiseSVN repository.
 */

// helper:
// declares and defines stuff which is not available in the Vista SDK or
// which isn't available in the Win7 SDK but not unless NTDDI_VERSION is
// set to NTDDI_WIN7
#pragma once
#if (NTDDI_VERSION < 0x06010000)

/*
* Message filter info values (CHANGEFILTERSTRUCT.ExtStatus)
*/
#define MSGFLTINFO_NONE                         (0)
#define MSGFLTINFO_ALREADYALLOWED_FORWND        (1)
#define MSGFLTINFO_ALREADYDISALLOWED_FORWND     (2)
#define MSGFLTINFO_ALLOWED_HIGHER               (3)

typedef struct tagCHANGEFILTERSTRUCT {
    DWORD cbSize;
    DWORD ExtStatus;
} CHANGEFILTERSTRUCT, *PCHANGEFILTERSTRUCT;

/*
* Message filter action values (action parameter to ChangeWindowMessageFilterEx)
*/
#define MSGFLT_RESET                            (0)
#define MSGFLT_ALLOW                            (1)
#define MSGFLT_DISALLOW                         (2)

#ifdef __cplusplus

// Define operator overloads to enable bit operations on enum values that are
// used to define flags. Use DEFINE_ENUM_FLAG_OPERATORS(YOUR_TYPE) to enable these
// operators on YOUR_TYPE.

// Moved here from objbase.w.

#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
    extern "C++" { \
    inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) | ((int)b)); } \
    inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) |= ((int)b)); } \
    inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) & ((int)b)); } \
    inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) &= ((int)b)); } \
    inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((int)a)); } \
    inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) ^ ((int)b)); } \
    inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) ^= ((int)b)); } \
}
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for objectarray.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __objectarray_h__
#define __objectarray_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifndef __IObjectArray_FWD_DEFINED__
#define __IObjectArray_FWD_DEFINED__
typedef interface IObjectArray IObjectArray;
#endif  /* __IObjectArray_FWD_DEFINED__ */


#ifndef __IObjectCollection_FWD_DEFINED__
#define __IObjectCollection_FWD_DEFINED__
typedef interface IObjectCollection IObjectCollection;
#endif  /* __IObjectCollection_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif


    /* interface __MIDL_itf_shobjidl_0000_0093 */
    /* [local] */

#ifdef MIDL_PASS
    typedef IUnknown *HIMAGELIST;

#endif
    typedef /* [v1_enum] */
        enum THUMBBUTTONFLAGS
    {   THBF_ENABLED    = 0,
    THBF_DISABLED   = 0x1,
    THBF_DISMISSONCLICK = 0x2,
    THBF_NOBACKGROUND   = 0x4,
    THBF_HIDDEN = 0x8,
    THBF_NONINTERACTIVE = 0x10
    }   THUMBBUTTONFLAGS;

    DEFINE_ENUM_FLAG_OPERATORS(THUMBBUTTONFLAGS)
        typedef /* [v1_enum] */
        enum THUMBBUTTONMASK
    {   THB_BITMAP  = 0x1,
    THB_ICON    = 0x2,
    THB_TOOLTIP = 0x4,
    THB_FLAGS   = 0x8
    }   THUMBBUTTONMASK;

    DEFINE_ENUM_FLAG_OPERATORS(THUMBBUTTONMASK)
#include <pshpack8.h>
        typedef struct THUMBBUTTON
    {
        THUMBBUTTONMASK dwMask;
        UINT iId;
        UINT iBitmap;
        HICON hIcon;
        WCHAR szTip[ 260 ];
        THUMBBUTTONFLAGS dwFlags;
    }   THUMBBUTTON;

    typedef struct THUMBBUTTON *LPTHUMBBUTTON;

#include <poppack.h>
#define THBN_CLICKED        0x1800


    extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0000_0093_v0_0_c_ifspec;
    extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0000_0093_v0_0_s_ifspec;

#ifndef __ITaskbarList3_INTERFACE_DEFINED__
#define __ITaskbarList3_INTERFACE_DEFINED__

    /* interface ITaskbarList3 */
    /* [object][uuid] */

    typedef /* [v1_enum] */
        enum TBPFLAG
    {   TBPF_NOPROGRESS = 0,
    TBPF_INDETERMINATE  = 0x1,
    TBPF_NORMAL = 0x2,
    TBPF_ERROR  = 0x4,
    TBPF_PAUSED = 0x8
    }   TBPFLAG;

    DEFINE_ENUM_FLAG_OPERATORS(TBPFLAG)

        EXTERN_C const IID IID_ITaskbarList3;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
ITaskbarList3 : public ITaskbarList2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetProgressValue(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ ULONGLONG ullCompleted,
            /* [in] */ ULONGLONG ullTotal) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetProgressState(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ TBPFLAG tbpFlags) = 0;

        virtual HRESULT STDMETHODCALLTYPE RegisterTab(
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndMDI) = 0;

        virtual HRESULT STDMETHODCALLTYPE UnregisterTab(
            /* [in] */ __RPC__in HWND hwndTab) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetTabOrder(
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndInsertBefore) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetTabActive(
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndMDI,
            /* [in] */ DWORD dwReserved) = 0;

        virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ UINT cButtons,
            /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;

        virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ UINT cButtons,
            /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;

        virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in_opt HIMAGELIST himl) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in HICON hIcon,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszDescription) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip(
            /* [in] */ __RPC__in HWND hwnd,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTip) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip(
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in RECT *prcClip) = 0;

    };

#else   /* C style interface */

    typedef struct ITaskbarList3Vtbl
    {
        BEGIN_INTERFACE

            HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */
            __RPC__deref_out  void **ppvObject);

            ULONG ( STDMETHODCALLTYPE *AddRef )(
                __RPC__in ITaskbarList3 * This);

            ULONG ( STDMETHODCALLTYPE *Release )(
                __RPC__in ITaskbarList3 * This);

            HRESULT ( STDMETHODCALLTYPE *HrInit )(
                __RPC__in ITaskbarList3 * This);

            HRESULT ( STDMETHODCALLTYPE *AddTab )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd);

            HRESULT ( STDMETHODCALLTYPE *DeleteTab )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd);

            HRESULT ( STDMETHODCALLTYPE *ActivateTab )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd);

            HRESULT ( STDMETHODCALLTYPE *SetActiveAlt )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd);

            HRESULT ( STDMETHODCALLTYPE *MarkFullscreenWindow )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ BOOL fFullscreen);

            HRESULT ( STDMETHODCALLTYPE *SetProgressValue )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ ULONGLONG ullCompleted,
                /* [in] */ ULONGLONG ullTotal);

            HRESULT ( STDMETHODCALLTYPE *SetProgressState )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ TBPFLAG tbpFlags);

            HRESULT ( STDMETHODCALLTYPE *RegisterTab )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwndTab,
                /* [in] */ __RPC__in HWND hwndMDI);

            HRESULT ( STDMETHODCALLTYPE *UnregisterTab )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwndTab);

            HRESULT ( STDMETHODCALLTYPE *SetTabOrder )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwndTab,
                /* [in] */ __RPC__in HWND hwndInsertBefore);

            HRESULT ( STDMETHODCALLTYPE *SetTabActive )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwndTab,
                /* [in] */ __RPC__in HWND hwndMDI,
                /* [in] */ DWORD dwReserved);

            HRESULT ( STDMETHODCALLTYPE *ThumbBarAddButtons )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ UINT cButtons,
                /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton);

            HRESULT ( STDMETHODCALLTYPE *ThumbBarUpdateButtons )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ UINT cButtons,
                /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton);

            HRESULT ( STDMETHODCALLTYPE *ThumbBarSetImageList )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ __RPC__in_opt HIMAGELIST himl);

            HRESULT ( STDMETHODCALLTYPE *SetOverlayIcon )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ __RPC__in HICON hIcon,
                /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszDescription);

            HRESULT ( STDMETHODCALLTYPE *SetThumbnailTooltip )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTip);

            HRESULT ( STDMETHODCALLTYPE *SetThumbnailClip )(
                __RPC__in ITaskbarList3 * This,
                /* [in] */ __RPC__in HWND hwnd,
                /* [in] */ __RPC__in RECT *prcClip);

        END_INTERFACE
    } ITaskbarList3Vtbl;

    interface ITaskbarList3
    {
        CONST_VTBL struct ITaskbarList3Vtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define ITaskbarList3_QueryInterface(This,riid,ppvObject)   \
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )

#define ITaskbarList3_AddRef(This)  \
    ( (This)->lpVtbl -> AddRef(This) )

#define ITaskbarList3_Release(This) \
    ( (This)->lpVtbl -> Release(This) )


#define ITaskbarList3_HrInit(This)  \
    ( (This)->lpVtbl -> HrInit(This) )

#define ITaskbarList3_AddTab(This,hwnd) \
    ( (This)->lpVtbl -> AddTab(This,hwnd) )

#define ITaskbarList3_DeleteTab(This,hwnd)  \
    ( (This)->lpVtbl -> DeleteTab(This,hwnd) )

#define ITaskbarList3_ActivateTab(This,hwnd)    \
    ( (This)->lpVtbl -> ActivateTab(This,hwnd) )

#define ITaskbarList3_SetActiveAlt(This,hwnd)   \
    ( (This)->lpVtbl -> SetActiveAlt(This,hwnd) )


#define ITaskbarList3_MarkFullscreenWindow(This,hwnd,fFullscreen)   \
    ( (This)->lpVtbl -> MarkFullscreenWindow(This,hwnd,fFullscreen) )


#define ITaskbarList3_SetProgressValue(This,hwnd,ullCompleted,ullTotal) \
    ( (This)->lpVtbl -> SetProgressValue(This,hwnd,ullCompleted,ullTotal) )

#define ITaskbarList3_SetProgressState(This,hwnd,tbpFlags)  \
    ( (This)->lpVtbl -> SetProgressState(This,hwnd,tbpFlags) )

#define ITaskbarList3_RegisterTab(This,hwndTab,hwndMDI) \
    ( (This)->lpVtbl -> RegisterTab(This,hwndTab,hwndMDI) )

#define ITaskbarList3_UnregisterTab(This,hwndTab)   \
    ( (This)->lpVtbl -> UnregisterTab(This,hwndTab) )

#define ITaskbarList3_SetTabOrder(This,hwndTab,hwndInsertBefore)    \
    ( (This)->lpVtbl -> SetTabOrder(This,hwndTab,hwndInsertBefore) )

#define ITaskbarList3_SetTabActive(This,hwndTab,hwndMDI,dwReserved) \
    ( (This)->lpVtbl -> SetTabActive(This,hwndTab,hwndMDI,dwReserved) )

#define ITaskbarList3_ThumbBarAddButtons(This,hwnd,cButtons,pButton)    \
    ( (This)->lpVtbl -> ThumbBarAddButtons(This,hwnd,cButtons,pButton) )

#define ITaskbarList3_ThumbBarUpdateButtons(This,hwnd,cButtons,pButton) \
    ( (This)->lpVtbl -> ThumbBarUpdateButtons(This,hwnd,cButtons,pButton) )

#define ITaskbarList3_ThumbBarSetImageList(This,hwnd,himl)  \
    ( (This)->lpVtbl -> ThumbBarSetImageList(This,hwnd,himl) )

#define ITaskbarList3_SetOverlayIcon(This,hwnd,hIcon,pszDescription)    \
    ( (This)->lpVtbl -> SetOverlayIcon(This,hwnd,hIcon,pszDescription) )

#define ITaskbarList3_SetThumbnailTooltip(This,hwnd,pszTip) \
    ( (This)->lpVtbl -> SetThumbnailTooltip(This,hwnd,pszTip) )

#define ITaskbarList3_SetThumbnailClip(This,hwnd,prcClip)   \
    ( (This)->lpVtbl -> SetThumbnailClip(This,hwnd,prcClip) )

#endif /* COBJMACROS */


#endif  /* C style interface */

#endif  /* __ITaskbarList3_INTERFACE_DEFINED__ */

#ifdef __cplusplus
}
#endif

#endif

#endif /* (NTDDI_VERSION < NTDDI_WIN7) */
