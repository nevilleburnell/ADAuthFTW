#pragma comment(lib, "node")

#include <node.h>
#include <v8.h>
#include <string>

#include "adauthftwHelper.h"

using namespace node;
using namespace v8;

class adauthftw: ObjectWrap
{
private:
    adauthftwHelper* _adauthftwHelper;
public:

    static Persistent<FunctionTemplate> s_ct;
    static void NODE_EXTERN Init(Handle<Object> target)
    {
        HandleScope scope;

        // set the constructor function
        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        // set the node.js/v8 class name
        s_ct = Persistent<FunctionTemplate>::New(t);
        s_ct->InstanceTemplate()->SetInternalFieldCount(1);
        s_ct->SetClassName(String::NewSymbol("adauthftw"));

        // registers a class member functions 
        NODE_SET_PROTOTYPE_METHOD(s_ct, "auth", auth);

        target->Set(String::NewSymbol("adauthftw"),
            s_ct->GetFunction());
    }

    adauthftw() 
    {
        _adauthftwHelper = adauthftwHelper::New();
    }

    ~adauthftw()
    {
        delete _adauthftwHelper;
    }

    static Handle<Value> New(const Arguments& args)
    {
        HandleScope scope;
        adauthftw* pm = new adauthftw();
        pm->Wrap(args.This());
        return args.This();
    }

    struct BatonAuth {
        uv_work_t request;
        adauthftwHelper* adauthftwHelper;
        Persistent<Function> callback;
		std::string path;
		std::string user;
		std::string pass;
        bool error;
        std::string error_message;
        std::string result;
    };

    static Handle<Value> auth(const Arguments& args)
    {
        HandleScope scope;

        if (!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
                String::New("First argument must be a string")));
        }

        if (!args[1]->IsString()) {
            return ThrowException(Exception::TypeError(
                String::New("Second argument must be a string")));
        }

		if (!args[2]->IsString()) {
            return ThrowException(Exception::TypeError(
                String::New("Third argument must be a string")));
        }

        if (!args[3]->IsFunction()) {
            return ThrowException(Exception::TypeError(
                String::New("Fourth argument must be a callback function")));
        }

        Local<String> path = Local<String>::Cast(args[0]);
        Local<String> user = Local<String>::Cast(args[1]);
        Local<String> pass = Local<String>::Cast(args[2]);
        // There's no ToFunction(), use a Cast instead.
        Local<Function> callback = Local<Function>::Cast(args[3]);

        adauthftw* so = ObjectWrap::Unwrap<adauthftw>(args.This());

        // create a state object
        BatonAuth* baton = new BatonAuth();
        baton->request.data = baton;
        baton->adauthftwHelper = so->_adauthftwHelper;
        baton->callback = Persistent<Function>::New(callback);
        baton->path = *v8::String::AsciiValue(path);
        baton->user = *v8::String::AsciiValue(user);
        baton->pass = *v8::String::AsciiValue(pass);

        // register a worker thread request
        uv_queue_work(uv_default_loop(), &baton->request,
            StartAuth, AfterAuth);

        return Undefined();

    }

    static void StartAuth(uv_work_t* req)
    {
        BatonAuth *baton = static_cast<BatonAuth*>(req->data);
        baton->error = baton->adauthftwHelper->auth(baton->path, baton->user, baton->pass, baton->error_message, baton->result);
    }

    static void AfterAuth(uv_work_t *req)
    {
        HandleScope scope;
        BatonAuth* baton = static_cast<BatonAuth*>(req->data);

        if (baton->error) 
        {
            Local<Value> err = Exception::Error(
                String::New(baton->error_message.c_str()));
            Local<Value> argv[] = { err };

            TryCatch try_catch;
            baton->callback->Call(
                Context::GetCurrent()->Global(), 1, argv);

            if (try_catch.HasCaught()) {
                node::FatalException(try_catch);
            }        
        } 
        else 
        {
            const unsigned argc = 2;
            Local<Value> argv[argc] = {
                Local<Value>::New(Null()),
                Local<Value>::New(String::New(baton->result.c_str()))
            };

            TryCatch try_catch;
            baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);

            if (try_catch.HasCaught()) {
                FatalException(try_catch);
            }
        }

        baton->callback.Dispose();
        delete baton;
    }

};

Persistent<FunctionTemplate> adauthftw::s_ct;

extern "C" {
    void NODE_EXTERN init (Handle<Object> target)
    {
        adauthftw::Init(target);
        LoadAssembly();
    }
    NODE_MODULE(adauthftw, init);
}