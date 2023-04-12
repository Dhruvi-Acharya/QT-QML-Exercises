#pragma once

#include <cstddef>
#include <string>
#include <cassert>
#include <stack>
#include <boost/core/ignore_unused.hpp>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

/**
\class Archiver
\brief Archiver concept

Archiver can be a reader or writer for serialization or deserialization respectively.

class Archiver {
public:
    /// \returns true if the archiver is in normal state. false if it has errors.
    operator bool() const;

    /// Starts an object
    Archiver& StartObject();
    
    /// After calling StartObject(), assign a member with a name
    Archiver& Member(const char* name);

    /// After calling StartObject(), check if a member presents
    bool HasMember(const char* name) const;

    /// Ends an object
    Archiver& EndObject();

    /// Starts an array
    /// \param size If Archiver::IsReader is true, the size of array is written.
    Archiver& StartArray(size_t* size = 0);

    /// Ends an array
    Archiver& EndArray();

    /// Read/Write primitive types.
    Archiver& operator&(bool& b);
    Archiver& operator&(unsigned& u);
    Archiver& operator&(int& i);
    Archiver& operator&(double& d);
    Archiver& operator&(std::string& s);

    /// Write primitive types.
    Archiver& SetNull();

    //! Whether it is a reader.
    static const bool IsReader;

    //! Whether it is a writer.
    static const bool IsWriter;
};
*/

using namespace rapidjson;

struct JsonReaderStackItem {
    enum State {
        BeforeStart,    //!< An object/array is in the stack but it is not yet called by StartObject()/StartArray().
        Started,        //!< An object/array is called by StartObject()/StartArray().
        Closed          //!< An array is closed after read all element, but before EndArray().
    };

    JsonReaderStackItem(const Value* value, State state) : value(value), state(state), index() {}

    const Value* value;
    State state;
    SizeType index;   // For array iteration
};

typedef std::stack<JsonReaderStackItem> JsonReaderStack;

#define DOCUMENT reinterpret_cast<Document*>(mDocument)
#define STACK (reinterpret_cast<JsonReaderStack*>(mStack))
#define TOP (STACK->top())
#define CURRENT (*TOP.value)

/// Represents a JSON reader which implements Archiver concept.
class JsonReader {
public:
    /// Constructor.
    /**
        \param json A non-const source json string for in-situ parsing.
        \note in-situ means the source JSON string will be modified after parsing.
    */
    JsonReader(const char* json)
        : mDocument(), mStack(), mError(false)
    {
        mDocument = new Document;
        DOCUMENT->Parse(json);
        if (DOCUMENT->HasParseError())
            mError = true;
        else {
            mStack = new JsonReaderStack;
            STACK->push(JsonReaderStackItem(DOCUMENT, JsonReaderStackItem::BeforeStart));
        }
    }

    /// Destructor.
    ~JsonReader()
    {
        delete DOCUMENT;
        delete STACK;
    }

    // Archive concept

    operator bool() const { return !mError; }

    JsonReader& StartObject()
    {
        if (!mError) {
            if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::BeforeStart)
                TOP.state = JsonReaderStackItem::Started;
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& Member(const char* name)
    {
        if (!mError) {
            if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started) {
                Value::ConstMemberIterator memberItr = CURRENT.FindMember(name);
                if (memberItr != CURRENT.MemberEnd())
                    STACK->push(JsonReaderStackItem(&memberItr->value, JsonReaderStackItem::BeforeStart));
                else
                    mError = true;
            }
            else
                mError = true;
        }
        return *this;
    }
    bool HasMember(const char* name) const
    {
        if (!mError && CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started)
            return CURRENT.HasMember(name);
        return false;
    }
    JsonReader& EndObject()
    {
        if (!mError) {
            if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started)
                Next();
            else
                mError = true;
        }
        return *this;
    }

    JsonReader& StartArray(size_t* size = 0)
    {
        if (!mError) {
            if (CURRENT.IsArray() && TOP.state == JsonReaderStackItem::BeforeStart) {
                TOP.state = JsonReaderStackItem::Started;
                if (size)
                    *size = CURRENT.Size();

                if (!CURRENT.Empty()) {
                    const Value* value = &CURRENT[TOP.index];
                    STACK->push(JsonReaderStackItem(value, JsonReaderStackItem::BeforeStart));
                }
                else
                    TOP.state = JsonReaderStackItem::Closed;
            }
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& EndArray()
    {
        if (!mError) {
            if (CURRENT.IsArray() && TOP.state == JsonReaderStackItem::Closed)
                Next();
            else
                mError = true;
        }
        return *this;
    }

    JsonReader& operator&(bool& b)
    {
        if (!mError) {
            if (CURRENT.IsBool()) {
                b = CURRENT.GetBool();
                Next();
            }else if(CURRENT.IsNull()){
                b = false;
                Next();
            }
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& operator&(unsigned& u)
    {
        if (!mError) {
            if (CURRENT.IsUint()) {
                u = CURRENT.GetUint();
                Next();
            }else if(CURRENT.IsNull()){
                u = 0;
                Next();
            }
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& operator&(int& i)
    {
        if (!mError) {
            if (CURRENT.IsInt()) {
                i = CURRENT.GetInt();
                Next();
            }else if(CURRENT.IsNull()){
                i = 0;
                Next();
            }
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& operator&(long long& ll)
    {
        if (!mError) {
            if (CURRENT.IsInt64()) {
                ll = CURRENT.GetInt64();
                Next();
            }else if(CURRENT.IsNull()){
                ll = 0;
                Next();
            }
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& operator&(double& d)
    {
        if (!mError) {
            if (CURRENT.IsNumber()) {
                d = CURRENT.GetDouble();
                Next();
            }else if(CURRENT.IsNull()){
                d = 0;
                Next();
            }
            else
                mError = true;
        }
        return *this;
    }
    JsonReader& operator&(std::string& s)
    {
        if (!mError) {
            if (CURRENT.IsString()) {
                s = CURRENT.GetString();
                Next();
            }else if(CURRENT.IsNull()){
                s = "";
                Next();
            }
            else
                mError = true;
        }
        return *this;
    }

    JsonReader& SetNull()
    {
        // This function is for JsonWriter only.
        mError = true;
        return *this;
    }

    static const bool IsReader = true;
    static const bool IsWriter = !IsReader;

private:
    JsonReader(const JsonReader&);
    JsonReader& operator=(const JsonReader&);

    void Next()
    {
        if (!mError) {
            assert(!STACK->empty());
            STACK->pop();

            if (!STACK->empty() && CURRENT.IsArray()) {
                if (TOP.state == JsonReaderStackItem::Started) { // Otherwise means reading array item pass end
                    if (TOP.index < CURRENT.Size() - 1) {
                        const Value* value = &CURRENT[++TOP.index];
                        STACK->push(JsonReaderStackItem(value, JsonReaderStackItem::BeforeStart));
                    }
                    else
                        TOP.state = JsonReaderStackItem::Closed;
                }
                else
                    mError = true;
            }
        }
    }

    // PIMPL
    void* mDocument;              ///< DOM result of parsing.
    void* mStack;                 ///< Stack for iterating the DOM
    bool mError;                  ///< Whether an error has occurred.
};

#undef DOCUMENT
#undef STACK
#undef TOP
#undef CURRENT

#define WRITER reinterpret_cast<PrettyWriter<StringBuffer>*>(mWriter)
#define STREAM reinterpret_cast<StringBuffer*>(mStream)

class JsonWriter {
public:
    /// Constructor.
    JsonWriter()
        : mWriter(), mStream()
    {
        mStream = new StringBuffer;
        mWriter = new PrettyWriter<StringBuffer>(*STREAM);
    }

    /// Destructor.
    ~JsonWriter()
    {
        delete WRITER;
        delete STREAM;
    }

    /// Obtains the serialized JSON string.
    const char* GetString() const
    {
        return STREAM->GetString();
    }

    // Archive concept

    operator bool() const { return true; }

    JsonWriter& StartObject()
    {
        WRITER->StartObject();
        return *this;
    }
    JsonWriter& Member(const char* name)
    {
        WRITER->String(name, static_cast<SizeType>(strlen(name)));
        return *this;
    }
    bool HasMember(const char* name) const
    {
        // This function is for JsonReader only.
        boost::ignore_unused(name);
        assert(false);
        return false;
    }
    JsonWriter& EndObject()
    {
        WRITER->EndObject();
        return *this;
    }

    JsonWriter& StartArray(size_t* size = 0)
    {
        boost::ignore_unused(size);
        WRITER->StartArray();
        return *this;
    }
    JsonWriter& EndArray()
    {
        WRITER->EndArray();
        return *this;
    }

    JsonWriter& operator&(bool& b)
    {
        WRITER->Bool(b);
        return *this;
    }
    JsonWriter& operator&(unsigned& u)
    {
        WRITER->Uint(u);
        return *this;
    }
    JsonWriter& operator&(int& i)
    {
        WRITER->Int(i);
        return *this;
    }
    JsonWriter& operator&(long long& ll)
    {
        WRITER->Int64(ll);
        return *this;
    }
    JsonWriter& operator&(double& d)
    {
        WRITER->Double(d);
        return *this;
    }
    JsonWriter& operator&(std::string& s)
    {
        WRITER->String(s.c_str(), static_cast<SizeType>(s.size()));
        return *this;
    }
    JsonWriter& SetNull()
    {
        WRITER->Null();
        return *this;
    }

    static const bool IsReader = false;
    static const bool IsWriter = !IsReader;

private:
    JsonWriter(const JsonWriter&);
    JsonWriter& operator=(const JsonWriter&);

    // PIMPL idiom
    void* mWriter;      ///< JSON writer.
    void* mStream;      ///< Stream buffer.
};

#undef STREAM
#undef WRITER
