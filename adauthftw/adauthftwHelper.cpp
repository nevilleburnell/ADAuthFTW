#include "adauthftwHelper.h"
#using <mscorlib.dll>
#using <C:\Git\adauthftw\adauthftwLib\bin\Debug\adauthftwLib.dll>
#include <gcroot.h>

using namespace System::Runtime::InteropServices;
using namespace System::Reflection;
using namespace adauthftw;

// we need to look for the assembly in the current directory
// node will search for it next to the node.exe binary
System::Reflection::Assembly ^OnAssemblyResolve(System::Object ^obj, System::ResolveEventArgs ^args)
{
    System::String ^path = System::Environment::CurrentDirectory;
    array<System::String^>^ assemblies =
        System::IO::Directory::GetFiles(path, "*.dll");
    for (long ii = 0; ii < assemblies->Length; ii++) {
        AssemblyName ^name = AssemblyName::GetAssemblyName(assemblies[ii]);
        if (AssemblyName::ReferenceMatchesDefinition(gcnew AssemblyName(args->Name), name)) {
            return Assembly::Load(name);
        }
    }
    return nullptr;
}

// register a custom assembly load handler
void LoadAssembly()
{
    System::AppDomain::CurrentDomain->AssemblyResolve +=
        gcnew System::ResolveEventHandler(OnAssemblyResolve);
}

class adauthftwWrapper : public adauthftwHelper {
private:
    gcroot<adauthftwClass^> _adauthftwClass;

public:
    adauthftwWrapper()
    {
        _adauthftwClass = gcnew adauthftwClass();
    }

    virtual bool auth(std::string& path, std::string& user, std::string& pass, std::string& err, std::string& result)
    {
        try
        {
			System::String^ data = "";
            if (_adauthftwClass->Authenticate(gcnew System::String(path.c_str()), gcnew System::String(user.c_str()), gcnew System::String(pass.c_str()))) data = "true";

            System::IntPtr p = Marshal::StringToHGlobalAnsi(data);
            result = static_cast<char*>(p.ToPointer());
            Marshal::FreeHGlobal(p);
            return false;
        }
        catch(System::Exception^ e)
        {
            System::IntPtr p = Marshal::StringToHGlobalAnsi(e->Message);
            err = static_cast<char*>(p.ToPointer());
            Marshal::FreeHGlobal(p);
            return true;
        }
    }


};

adauthftwHelper* adauthftwHelper::New()
{
    return new adauthftwWrapper();
}

