#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <string>

//return the size in bytes of the largest type in the group
template<typename F, typename... Ts> struct SizeofLargestType { static const size_t size = (sizeof(F) > SizeofLargestType <Ts...>::size ? sizeof(F) : SizeofLargestType <Ts...>::size); };
template<typename F> struct SizeofLargestType <F> { static const size_t size = sizeof(F); };

//Basic JSON IO. Limited Error handling. Supports all standard JSON types (Including integers and decimals separately)
namespace Json
{
    //All of the JSON types
    enum class Types
    {
        Invalid,
        Null,
        Boolean,
        Integer,
        Decimal,
        String,
        Object,
        Array
    };

    class Value;

    //A value representing one of the JSON types. (numbers (split into integers and decimals), null, bool, strings, objects, and arrays)
    class Value
    {
    public:

        using Char      = char;
        using Null      = std::nullptr_t;
        using Boolean   = bool;
        using Decimal   = double;
        using Integer   = long long;
        using String    = std::basic_string<Char>;
        using Array     = std::vector<Value>;
        using Object    = std::map<String, Value>;

        using IStream   = std::basic_istream<Char>;
        using OStream   = std::basic_ostream<Char>;
        using IFStream  = std::basic_ifstream<Char>;
        using OFStream  = std::basic_ofstream<Char>;

        Value() : type(Types::Invalid) { }

        //factory methods


        static Value Create(const Null& Val) { Value v; v.operator=(Val); return v; }
        static Value Create(const Boolean& Val) { Value v; v.operator=(Val); return v; }
        static Value Create(const Integer& Val) { Value v; v.operator=(Val); return v; }
        static Value Create(const Decimal& Val) { Value v; v.operator=(Val); return v; }
        static Value Create(const String& Val) { Value v; v.operator=(Val); return v; }
        static Value Create(const Object& Val) { Value v; v.operator=(Val); return v; }
        static Value Create(const Array& Val) { Value v; v.operator=(Val); return v; }

        static Value Create(const Char* Val) { Value v; v.operator=(String(Val)); return v; }

        inline Types Type() const { return type; }

        //Set operators

        Value& operator = (const Null& Value);
        Value& operator = (Boolean Value);
        Value& operator = (Integer Value);
        Value& operator = (Decimal Value);
        Value& operator = (const String& Value);
        Value& operator = (const Object& Value);
        Value& operator = (const Array& Value);

        inline Value& operator = (short Value) { return operator=(short(Value)); }
        inline Value& operator = (int Value) { return operator=(Integer(Value)); }
        inline Value& operator = (unsigned Value) { return operator=(Integer(Value)); }
        inline Value& operator = (float Value) { return operator=(float(Value)); }
        inline Value& operator = (const Char* Value) { return operator=(String(Value)); }

        //Get operators

        inline operator Null() const { return *(Null*)(value); }
        inline operator Boolean() const { return *(Boolean*)(value); }
        inline operator Integer () const { return *(Integer*)(value); }
        inline operator Decimal() const { return *(Decimal*)(value); }
        inline operator String() const { return *(String*)(value); }
        inline operator Object() const { return *(Object*)(value); }
        inline operator Array() const { return *(Array*)(value); }

        inline operator short() const { return (short)(*(Integer*)(value)); }
        inline operator int() const { return (int)(*(Integer*)(value)); }
        inline operator unsigned() const { return (unsigned)(*(Integer*)(value)); }
        inline operator float() const { return (float)(*(Decimal*)(value)); }
        inline operator const Char*() const { return ((String*)(value))->c_str(); }

        //Serializers

        void Read(IStream& Stream); //Create a value from a stream
        void Write(OStream& Stream) const; //Write a value to a stream

        static size_t DefaultStringReserveLength; //the default string reservation length (in chars) - Defaults to 32

        //Helper methods

        //Load a value automatically from a file
        static inline Value FromFile(const String& FileName)
        {
            IFStream fin;
            fin.open(FileName, std::ios::in);
            Value v;
            v.Read(fin);
            fin.close();
            return v;
        }
        inline Boolean WriteToFile(const String& FileName)
        {
            OFStream fout (FileName, std::ios::out);
            if (!fout.is_open())
                return false;

            Write(fout);

            fout.close();
            return true;
        }

    protected:
        Types type;
        char value[SizeofLargestType<Null, Boolean, Integer, Decimal, String, Object, Array>::size];

        //Delete any old values and reset it to the default
        void Reset();

        static Types GuessType(IStream& Stream); //Guess the type of object (Only looks based on the first character) - Assumes first character at stream pos is usable
        static void SkipWhitespace(IStream& Stream); //Skip any whitespace (Schema defined whitespace)
        static String EscapeQuotes(String String);
    };
}

inline Json::Value::IStream& operator >> (Json::Value::IStream& Stream, Json::Value& Value) { Value.Read(Stream); return Stream; }
inline Json::Value::OStream& operator << (Json::Value::OStream& Stream, const Json::Value& Value) { Value.Write(Stream); return Stream; }
