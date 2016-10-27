#include <nan.h>
#include <launch.h>

NAN_METHOD(Collect) {
  if (info.Length() != 1) {
    return Nan::ThrowError(Nan::TypeError("Must be given exactly 1 argument"));
  }

  Nan::Utf8String handleName(info[0]->ToString());
  int *fds;
  size_t cnt;

  int err = launch_activate_socket(*handleName, &fds, &cnt);
  switch (err) {
  case 0:
    break;
  case ENOENT:
    return Nan::ThrowError("The socket name specified does not exist in the caller's launchd.plist");
  case ESRCH:
    return Nan::ThrowError("The calling process is not managed by launchd");
  case EALREADY:
    return Nan::ThrowError("The specified socket has already been activated");
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
