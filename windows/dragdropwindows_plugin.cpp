// CDropTarget.cpp
// Copyright (C) 2022 xuqiqiang
// This file is public domain software.
#include "include/dragdropwindows/dragdropwindows_plugin.h"
#include "include/dragdropwindows/CDropTarget.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

namespace {
HWND GetRootWindow(flutter::FlutterView* view) {
    return GetAncestor(view->GetNativeWindow(), GA_ROOT);
}
class DragdropwindowsPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  DragdropwindowsPlugin(flutter::PluginRegistrarWindows* registrar, std::unique_ptr<flutter::MethodChannel<>> channel);

  virtual ~DragdropwindowsPlugin();

 private:
  // The registrar for this plugin, for accessing the window.
  flutter::PluginRegistrarWindows* registrar_;
  std::unique_ptr<flutter::MethodChannel<>> channel_;
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  IDropTarget *pDropTarget;
};

// static
void DragdropwindowsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "dragdropwindows",
          &flutter::StandardMethodCodec::GetInstance());
  auto *channel_pointer = channel.get();
  auto plugin = std::make_unique<DragdropwindowsPlugin>(registrar, std::move(channel));

  channel_pointer->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

DragdropwindowsPlugin::DragdropwindowsPlugin(flutter::PluginRegistrarWindows* registrar, std::unique_ptr<flutter::MethodChannel<>> channel) {
  registrar_ = registrar;
  channel_ = std::move(channel);
  pDropTarget = NULL;
}

DragdropwindowsPlugin::~DragdropwindowsPlugin() {}

void DragdropwindowsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_call.method_name().compare("modifyWindowAcceptFiles") == 0) {
    HWND hWnd = GetRootWindow(registrar_->GetView());
    OleInitialize(NULL);

    channel_ =
          std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
              registrar_->messenger(), "dragdropwindows",
              &flutter::StandardMethodCodec::GetInstance());

    if (pDropTarget) pDropTarget->Release();
    pDropTarget = CDropTarget::CreateInstance(std::move(channel_));
    if (pDropTarget) {
        ::RegisterDragDrop(hWnd, pDropTarget);
    }
    result->Success();
  } else if (method_call.method_name().compare("resetWindowAcceptFiles") == 0) {
    HWND hWnd = GetRootWindow(registrar_->GetView());
    ::RevokeDragDrop(hWnd);
    if (pDropTarget) pDropTarget->Release();
    pDropTarget = NULL;
    OleUninitialize();
    result->Success();
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void DragdropwindowsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  DragdropwindowsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

CDropTarget::CDropTarget() : m_nRefCount(0)
{
}

CDropTarget::~CDropTarget()
{
}

IDropTarget *CDropTarget::CreateInstance(std::unique_ptr<flutter::MethodChannel<>> channel)
{
    CDropTarget *pDropTarget = new CDropTarget;
    if (pDropTarget)
    {
        pDropTarget->AddRef();
        pDropTarget->setChannel(std::move(channel));
        return pDropTarget;
    }
    return NULL;
}

ULONG STDMETHODCALLTYPE CDropTarget::setChannel(std::unique_ptr<flutter::MethodChannel<>> channel)
{
    m_channel = std::move(channel);
    return m_nRefCount;
}

HRESULT STDMETHODCALLTYPE
CDropTarget::QueryInterface(REFIID riid, void **ppvObject)
{
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget))
    {
        *ppvObject = static_cast<IDropTarget *>(this);
        AddRef();
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CDropTarget::AddRef()
{
    m_nRefCount++;
    return m_nRefCount;
}

ULONG STDMETHODCALLTYPE CDropTarget::Release()
{
    m_channel = NULL;
    m_nRefCount--;
    if (m_nRefCount > 0)
        return m_nRefCount;
    delete this;
    return 0;
}

HRESULT STDMETHODCALLTYPE
CDropTarget::DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
    // Indicates whether or not a drop can be accepted and the drop's effect if accepted.
    ::OutputDebugString(TEXT("CDropTarget::DragEnter\n"));
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
    // Provides target feedback to the user and communicates the drop's effect to the ::DoDragDrop function.
    // grfKeyState: MK_*
    // *pdwEffect: DROPEFFECT_*
    ::OutputDebugString(TEXT("CDropTarget::DragOver\n"));
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
CDropTarget::DragLeave()
{
    // Removes target feedback and releases the data object.
    ::OutputDebugString(TEXT("CDropTarget::DragLeave\n"));
    return S_OK;
}

std::string GBK_2_UTF8(std::string gbkStr)
{
    std::string outUtf8 = "";
	int n = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
	WCHAR *str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char *str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	outUtf8 = str2;
	delete[] str1;
	str1 = NULL;
	delete[] str2;
	str2 = NULL;
	return outUtf8;
}

std::string W_To_A(const std::wstring& wstr, unsigned int codepage = CP_ACP)
{
    int nwstrlen = WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (nwstrlen > 0) {
        std::string str(nwstrlen - 1, '\0');
        WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, (LPSTR)str.c_str(), nwstrlen, NULL, NULL);
        return std::move(str);
    }
    return ("");
}

char* dup_wchar_to_utf8(wchar_t* w) {
    size_t w_len = wcslen(w);
    wchar_t* wstr = (wchar_t*)malloc((w_len + 1) * sizeof(wchar_t*));
    // 去掉所有的'\r'
    if (wstr) {
        size_t j = 0;
        for (size_t i = 0; i < w_len; i++) {
            if (w[i] != '\r') {
                wstr[j++] = w[i];
            }
        }
        wstr[j] = '\0';
    }
    else {
        wstr = w;
    }

    char* s = NULL;
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, 0, 0, 0, 0);
    s = (char*)malloc(size + 1);
    if (s) {
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, s, size, 0, 0);
        s[size] = '\0';
    }

    if (wstr != w) {
        free(wstr);
    }
    return s;
}

HRESULT STDMETHODCALLTYPE
CDropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
    // Incorporates the source data into the target window, removes target feedback, and releases the data object.
    // grfKeyState: MK_*
    // *pdwEffect: DROPEFFECT_*
    //::OutputDebugString(TEXT("CDropTarget::Drop\n"));
    //::MessageBoxW(NULL, L"Dropped!", L"Dropped!", MB_ICONINFORMATION);

    FORMATETC fme;
    ZeroMemory(&fme, sizeof(fme));
    fme.cfFormat = CF_TEXT;
    fme.ptd = NULL;
    fme.dwAspect = DVASPECT_CONTENT;
    fme.lindex = -1;
    fme.tymed = TYMED_HGLOBAL;

    // The STGMEDIUM structure is a generalized global memory handle.
    STGMEDIUM stgm;
    ZeroMemory(&stgm, sizeof(stgm));

    if (SUCCEEDED(pDataObject->GetData(&fme, &stgm)))
    {
        char* pSrc = (char*)::GlobalLock(stgm.hGlobal);
        std::ostringstream strText;
        strText << "1";
        strText << pSrc;
        m_channel->InvokeMethod("onDragDrop", std::make_unique<flutter::EncodableValue>(strText.str()));
    } else {
        ZeroMemory(&fme, sizeof(fme));
        fme.cfFormat = CF_HDROP;
        fme.ptd = NULL;
        fme.dwAspect = DVASPECT_CONTENT;
        fme.lindex = -1;
        fme.tymed = TYMED_HGLOBAL;
        if (SUCCEEDED(pDataObject->GetData(&fme, &stgm)))
        {
          TCHAR wszName[MAX_PATH];
          std::wostringstream strPaths;
          HDROP hDrop = (HDROP)stgm.hGlobal;
          int count = DragQueryFile(hDrop, 0xFFFFFFFF, wszName, MAX_PATH);
          strPaths << L"2";
          for (int i = 0; i < count; i++)
          {
            DragQueryFile(hDrop, i, wszName, MAX_PATH);

            if (i > 0) strPaths << L"|";
            strPaths << wszName;
          }
          char* str = dup_wchar_to_utf8((wchar_t*)strPaths.str().c_str());
          m_channel->InvokeMethod("onDragDrop", std::make_unique<flutter::EncodableValue>(std::string(str)));
          DragFinish(hDrop);
        }
        else {
            ZeroMemory(&fme, sizeof(fme));
            fme.cfFormat = CF_UNICODETEXT;
            fme.ptd = NULL;
            fme.dwAspect = DVASPECT_CONTENT;
            fme.lindex = -1;
            fme.tymed = TYMED_HGLOBAL;

            if (SUCCEEDED(pDataObject->GetData(&fme, &stgm)))
            {
                wchar_t* pSrcw = (wchar_t*)::GlobalLock(stgm.hGlobal);
                std::wostringstream strText;
                strText << L"1";
                if (pSrcw != NULL)
                {
                    strText << pSrcw;
                }
                char* str = dup_wchar_to_utf8((wchar_t*)strText.str().c_str());
                m_channel->InvokeMethod("onDragDrop",
                    std::make_unique<flutter::EncodableValue>(std::string(str)));
            }
        }
    }
    return S_OK;
}
