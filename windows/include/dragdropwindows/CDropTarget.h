#ifndef CDROPTARGET_HPP_
#define CDROPTARGET_HPP_

#include <oleidl.h>
#include <flutter/method_channel.h>

class CDropTarget : public IDropTarget
{
public:
    static IDropTarget *CreateInstance(std::unique_ptr<flutter::MethodChannel<>> channel);

private:
    CDropTarget();
    virtual ~CDropTarget();

    virtual ULONG STDMETHODCALLTYPE setChannel(std::unique_ptr<flutter::MethodChannel<>> channel);

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragLeave();
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

protected:
    LONG m_nRefCount;
    std::unique_ptr<flutter::MethodChannel<>> m_channel;
};

#endif  // ndef CDROPTARGET_HPP_
