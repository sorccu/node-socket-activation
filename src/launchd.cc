#include <nan.h>
#include <launch.h>

v8::Local<v8::Value> complain(const char *msg, const char *code) {
  v8::Local<v8::Value> ex = Nan::Error(msg);
  v8::Local<v8::Object> obj = ex.As<v8::Object>();
  Nan::Set(
    obj,
    Nan::New<v8::String>("code").ToLocalChecked(),
    Nan::New<v8::String>(code).ToLocalChecked()
  );
  return ex;
}

NAN_METHOD(Collect) {
  if (info.Length() != 1) {
    return Nan::ThrowError(Nan::TypeError("Must be given exactly 1 argument"));
  }

  Nan::Utf8String handleName(info[0]);
  int *fds;
  size_t cnt;

  int err = launch_activate_socket(*handleName, &fds, &cnt);
  switch (err) {
  case 0:
    break;
  case ENOENT: {
    v8::Local<v8::Value> ex = complain(
      "The socket name specified does not exist in the caller's launchd.plist",
      "ENOENT"
    );

    return Nan::ThrowError(ex);
  }
  case ESRCH: {
    v8::Local<v8::Value> ex = complain(
      "The calling process is not managed by launchd",
      "ESRCH"
    );

    return Nan::ThrowError(ex);
  }
  case EALREADY: {
    v8::Local<v8::Value> ex = complain(
      "The specified socket has already been activated",
      "EALREADY"
    );

    return Nan::ThrowError(ex);
  }
  default:
    return Nan::ThrowError(strerror(err));
  }

  v8::Local<v8::Array> result = Nan::New<v8::Array>(cnt);

  for (size_t i = 0; i < result->Length(); ++i) {
    Nan::Set(result, i, Nan::New<v8::Number>(fds[i]));
  }

  free(fds);

  info.GetReturnValue().Set(result);
};

NAN_MODULE_INIT(InitAll) {
  Nan::Set(
    target,
    Nan::New<v8::String>("collect").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(Collect)).ToLocalChecked()
  );
}

NODE_MODULE(socket_activation, InitAll)
