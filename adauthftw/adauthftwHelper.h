#include <node.h>
#include <v8.h>
#include <string>

using namespace v8;

void LoadAssembly();

// this is a shim class which mediates between managed and unmanaged code
class adauthftwHelper
{
protected:
    adauthftwHelper() {};
public:
    virtual bool auth(std::string& path, std::string& user, std::string& pass, std::string& err, std::string& result) = 0;
	
    static adauthftwHelper* New();
};
