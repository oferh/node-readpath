/*********************************************************************
 *
 ********************************************************************/

#include <nan.h>
#include <iostream>
#include <queue>
#include <string.h>
#include "async.h"
#include "readpath.h"
#include <unistd.h>

using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;
using Nan::Utf8String;
using std::string;
using std::queue;

class AsyncReadWorker : public AsyncWorker {
 public:
  explicit AsyncReadWorker(Callback *callback_, string path, Callback *entryHandler)
      : AsyncWorker(callback_), path_(path), entryHandler_(entryHandler) {
    async = new uv_async_t;
    uv_async_init(
        uv_default_loop()
      , async
      , AsyncRead_
    );
    async->data = this;

    uv_mutex_init(&async_lock);
  }

  virtual ~AsyncReadWorker() {
    uv_mutex_destroy(&async_lock);
  }

  virtual void HandleOKCallback() {
    WorkProgress();
    Local<Value> argv[] = {
       Null()
   };

   callback->Call(1, argv);
  }

  void WorkProgress() {
    while (!entries.empty()) {
      uv_mutex_lock(&async_lock);
      string entry = entries.front();
      entries.pop();
      uv_mutex_unlock(&async_lock);

      // Dont send progress events after we've already completed.
      // if (callback) {
      //     cout <<"handle progress" <<endl;
           HandleProgressCallback(entry);
      // }
    }
  }

  class ExecutionProgress {
    friend class AsyncReadWorker;
   public:
    // You could do fancy generics with templates here.
    void Send(string entry) const {
        that_->SendProgress_(entry);
    }

   private:
    explicit ExecutionProgress(AsyncReadWorker* that) : that_(that) {}
    NAN_DISALLOW_ASSIGN_COPY_MOVE(ExecutionProgress)
    AsyncReadWorker* const that_;
  };

  class EntryHandler {
    public:
      EntryHandler(ExecutionProgress* progress) : progress_(progress), count(0) {}
      void onEntry(string entry) {
        progress_->Send(entry);
        count++;
        if (count == 5) {
          usleep(0);
          count = 0;
        }
      }

    private:
      ExecutionProgress* const progress_;
      int count;

  };

  virtual void HandleProgressCallback(string entry) {
     HandleScope scope;

     Local<Value> argv[] = {
       New<v8::String>(entry.c_str()).ToLocalChecked()
     };

     entryHandler_->Call(1, argv);
  }

  virtual void Destroy() {
    uv_close(reinterpret_cast<uv_handle_t*>(async), AsyncClose_);
  }

  virtual void Execute() {
      int res = 0;

      ExecutionProgress progress(this);
      EntryHandler handler(&progress);

      Readpath<EntryHandler> reader(path_, &handler);
      res = reader.read();
      if (res != 0) {
        SetErrorMessage(strerror(res));
      }
  }

private:
  void SendProgress_(string entry) {

    uv_mutex_lock(&async_lock);
    entries.push(entry);
    uv_mutex_unlock(&async_lock);

    uv_async_send(async);
  }

  NAN_INLINE static NAUV_WORK_CB(AsyncRead_) {
    AsyncReadWorker *worker =
            static_cast<AsyncReadWorker*>(async->data);
    worker->WorkProgress();
  }

  NAN_INLINE static void AsyncClose_(uv_handle_t* handle) {
    AsyncReadWorker *worker =
            static_cast<AsyncReadWorker*>(handle->data);
    delete reinterpret_cast<uv_async_t*>(handle);
    delete worker;
  }

  uv_async_t *async;
  uv_mutex_t async_lock;
  queue<string> entries;
  string path_;
  Callback *entryHandler_;
};

// Asynchronous access to the `Estimate()` function
NAN_METHOD(readAsynch) {
  string path(*Utf8String(info[0]));
  Callback *entryHandler = new Callback(info[1].As<v8::Function>());
  Callback *callback = new Callback(info[2].As<v8::Function>());

  AsyncQueueWorker(new AsyncReadWorker(callback, path, entryHandler));
}
