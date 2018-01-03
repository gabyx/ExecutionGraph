#include "FileSchemeHandlerFactory.hpp"

FileSchemeHandlerFactory::FileSchemeHandlerFactory(std::string folderPath, std::string urlPrefix)
    : m_FolderPath(folderPath)
    , m_urlPrefix(urlPrefix)
{
}

CefRefPtr<CefResourceHandler> FileSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
    std::string requestUrl = request->GetURL().ToString();
    CefURLParts urlParts;
    if(CefParseURL(request->GetURL(), urlParts))
    {
        CefString path(urlParts.path.str);
        std::string resourceName = path.ToString();
        while(resourceName.find("/") == 0)
        {
            resourceName = resourceName.substr(1);
        }
        while(resourceName.find("\\") == 0)
        {
            resourceName = resourceName.substr(1);
        }

        if(m_urlPrefix == "" || resourceName.compare(0, m_urlPrefix.size(), m_urlPrefix) == 0)
        {
            if(m_urlPrefix != "")
            {
                resourceName = resourceName.substr(resourceName.find(m_urlPrefix) + m_urlPrefix.length());
                while(resourceName.find("/") == 0)
                {
                    resourceName = resourceName.substr(1);
                }
                while(resourceName.find("\\") == 0)
                {
                    resourceName = resourceName.substr(1);
                }
            }
            CefString sFilePath(m_FolderPath + "/" + resourceName);
            CefString sFileExtension(sFilePath.ToString().substr(sFilePath.ToString().find_last_of(".") + 1));
            CefString sMimeType(CefGetMimeType(sFileExtension));

            CefRefPtr<CefStreamReader> fileStream = CefStreamReader::CreateForFile(sFilePath);
            if(fileStream != nullptr)
            {
                return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(sMimeType, fileStream));
            }
        }
    }

    return nullptr;
}