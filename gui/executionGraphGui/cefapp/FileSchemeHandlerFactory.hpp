
#ifndef FILE_SCHEME_HANDLER_FACTORY_H_
#define FILE_SCHEME_HANDLER_FACTORY_H_

#include <cef_scheme.h>
#include <string>

// Implementation of the factory for creating client request handlers.
class FileSchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
    FileSchemeHandlerFactory(std::string folderPath, std::string urlPrefix);

    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override;

private:
    std::string m_folderPath;
    std::string m_urlPrefix;

    IMPLEMENT_REFCOUNTING(FileSchemeHandlerFactory);
};

#endif  // FILE_SCHEME_HANDLER_FACTORY_H_
