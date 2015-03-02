#include <nan.h>
#include <node.h>
#include <node_object_wrap.h>

#include "leveldb/db.h"

using namespace leveldb;
using namespace v8;

class Database : public node::ObjectWrap {
  public:
    static void Initialize(Handle<Object> exports);

  private:
    static NAN_METHOD(New);
    static NAN_METHOD(Open);
    static NAN_METHOD(Close);
    static NAN_METHOD(Delete);
    static NAN_METHOD(Get);
    static NAN_METHOD(List);
    static NAN_METHOD(Set);

    static Persistent<FunctionTemplate> constructor;
    DB *database;
};

Persistent<FunctionTemplate> Database::constructor;

void Database::Initialize(Handle<Object> exports) {
  Local<FunctionTemplate> functionTemplate = NanNew<FunctionTemplate>(Database::New);
  NanAssignPersistent(constructor, functionTemplate);
  functionTemplate->SetClassName(NanNew("Database"));
  functionTemplate->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(functionTemplate, "open", Database::Open);
  NODE_SET_PROTOTYPE_METHOD(functionTemplate, "close", Database::Close);
  NODE_SET_PROTOTYPE_METHOD(functionTemplate, "delete", Database::Delete);
  NODE_SET_PROTOTYPE_METHOD(functionTemplate, "get", Database::Get);
  NODE_SET_PROTOTYPE_METHOD(functionTemplate, "list", Database::List);
  NODE_SET_PROTOTYPE_METHOD(functionTemplate, "set", Database::Set);

  exports->Set(NanNew("Database"), functionTemplate->GetFunction());
}

NAN_METHOD(Database::New) {
  NanScope();

  if (!args.IsConstructCall()) {
    NanThrowError("Cannot call Database::New as a plain function.");
    NanReturnUndefined();
  }

  Database *obj = new Database();
  obj->database = NULL;
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(Database::Open) {
  NanScope();

  if (args.Length() != 1) {
    NanThrowError("Exactly one argument expected: the database name.");
    NanReturnUndefined();
  }

  if (!args[0]->IsString()) {
    NanThrowError("Argument must be a string specifying the database name.");
    NanReturnUndefined();
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  if (self->database) {
    NanThrowError("The database must be closed before it can be re-opened.");
    NanReturnUndefined();
  }

  Options options;
  options.create_if_missing = true;
  Status status = DB::Open(options, std::string(*String::Utf8Value(args[0])), &self->database);
  if (!status.ok()) {
    NanThrowError(status.ToString().c_str());
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Database::Close) {
  NanScope();

  if (args.Length() != 0) {
    NanThrowError("Zero arguments expected to close().");
    NanReturnUndefined();
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  delete self->database;
  self->database = NULL;

  NanReturnValue(NanTrue());
}

NAN_METHOD(Database::Delete) {
  NanScope();

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  if (!self->database) {
    NanThrowError("The database must be opened before an item can be deleted.");
    NanReturnUndefined();
  }

  if (args.Length() != 1) {
    NanThrowError("One argument expected to delete(): (key)");
    NanReturnUndefined();
  }

  Status status = self->database->Delete(WriteOptions(), *String::Utf8Value(args[0]));
  if (!status.ok()) {
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Database::Get) {
  NanScope();

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  if (!self->database) {
    NanThrowError("The database must be opened before keys can be fetched.");
    NanReturnUndefined();
  }

  if (args.Length() != 1) {
    NanThrowError("One argument expected to get(): (key)");
    NanReturnUndefined();
  }

  std::string value;
  Status status = self->database->Get(ReadOptions(), *String::Utf8Value(args[0]), &value);
  if (!status.ok()) {
    NanReturnValue(NanNull());
  }

  NanReturnValue(NanNew<String>(value));
}

NAN_METHOD(Database::List) {
  NanScope();

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  if (!self->database) {
    NanThrowError("The database must be opened before it can be listed.");
    NanReturnUndefined();
  }

  int argCount = args.Length();
  if (argCount < 1 || argCount > 3) {
    NanThrowError("Between one and three arguments expected to list(): (opt_start, opt_end, callback)");
    NanReturnUndefined();
  }

  NanCallback *callback = new NanCallback(args[argCount - 1].As<Function>());
  Iterator *iterator = self->database->NewIterator(ReadOptions());

  for (argCount > 1 ? iterator->Seek(*String::Utf8Value(args[0])) : iterator->SeekToFirst(); iterator->Valid(); iterator->Next()) {
    Slice key = iterator->key();
    Slice value = iterator->value();

    if (argCount > 2 && key.compare(*String::Utf8Value(args[1])) > 0)
      break;

    Local<Value> callbackArgs[] = {
      NanNew<String>(key.ToString()),
      NanNew<String>(value.ToString())
    };

    callback->Call(2, callbackArgs);
  }

  delete iterator;
  delete callback;

  NanReturnUndefined();
}

NAN_METHOD(Database::Set) {
  NanScope();

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  if (!self->database) {
    NanThrowError("The database must be opened before keys can be set.");
    NanReturnUndefined();
  }

  if (args.Length() != 2) {
    NanThrowError("Two arguments expected to set(): (key, value)");
    NanReturnUndefined();
  }

  Status status = self->database->Put(WriteOptions(), *String::Utf8Value(args[0]), *String::Utf8Value(args[1]));
  if (!status.ok()) {
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

static void initialize(Handle<Object> exports) {
  Database::Initialize(exports);
}

NODE_MODULE(node_leveldb, initialize);
