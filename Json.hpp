#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

//return the size in bytes of the largest type in the group
template<typename F, typename... Ts> struct SizeofLargestType { static const size_t size = (sizeof(F) > SizeofLargestType <Ts...>::size ? sizeof(F) : SizeofLargestType <Ts...>::size); };
template<typename F> struct SizeofLargestType <F> { static const size_t size = sizeof(F); };

//Basic JSON IO. Limited Error handling. Supports all standard JSON types (Including Integers and Floats separately)
namespace Json
{
	//All of the JSON types
	enum class Types
	{
		Invalid,
		Null,
		Boolean,
		Integer,
		Floating,
		String,
		Object,
		Array
	};

	class Value;

	typedef long long fat;
	typedef std::vector<Value> Array;
	typedef std::map<std::string, Value> Object;

	//A value representing one of the JSON types. (numbers (split into integers and floats), null, bool, strings, objects, and arrays)
	class Value
	{
	public:

		Value() : type(Types::Invalid) { } 

		//factory methods

		template <typename T> static Value Create(const T& Val) { return Value(); }
		template <> static Value Create<nullptr_t>(const nullptr_t& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<bool>(const bool& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<fat>(const fat& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<double>(const double& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<std::string>(const std::string& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<Object>(const Object& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<Array>(const Array& Val) { Value v; v.operator=(Val); return v; }

		static Value Create(const char* Val) { Value v; v.operator=(std::string(Val)); return v; }

		inline Types Type() const { return type; }

		//Set operators

		Value& operator = (const std::nullptr_t& Value);
		Value& operator = (bool Value);
		Value& operator = (fat Value);
		Value& operator = (double Value);
		Value& operator = (const std::string& Value);
		Value& operator = (const Object& Value);
		Value& operator = (const Array& Value);

		inline Value& operator = (short Value) { return operator=(short(Value)); }
		inline Value& operator = (int Value) { return operator=(fat(Value)); }
		inline Value& operator = (unsigned Value) { return operator=(fat(Value)); }
		inline Value& operator = (float Value) { return operator=(float(Value)); }
		inline Value& operator = (const char* Value) { return operator=(std::string(Value)); }

		//Get operators

		inline operator std::nullptr_t() const { return *(std::nullptr_t*)(value); }
		inline operator bool() const { return *(bool*)(value); }
		inline operator fat () const { return *(fat*)(value); }
		inline operator double() const { return *(double*)(value); }
		inline operator std::string() const { return *(std::string*)(value); }
		inline operator Object() const { return *(Object*)(value); }
		inline operator Array() const { return *(Array*)(value); }

		inline operator short() const { return (short)(*(fat*)(value)); }
		inline operator int() const { return (int)(*(fat*)(value)); }
		inline operator unsigned() const { return (unsigned)(*(fat*)(value)); }
		inline operator float() const { return (float)(*(double*)(value)); }
		inline operator const char*() const { return ((std::string*)(value))->c_str(); }

		//Serializers

		void Read(std::istream& Stream); //Create a value from a stream
		void Write(std::ostream& Stream) const; //Write a value to a stream

		static size_t DefaultStringReserveLength; //the default string reservation length (in chars) - Defaults to 32

		friend std::istream& operator >> (std::istream& Stream, const Value& Value);
		friend std::ostream& operator << (std::ostream& Stream, const Value& Value);

	protected:
		Types type;
		char value[SizeofLargestType<std::nullptr_t, bool, fat, double, std::string, Object, Array>::size];

		//Delete any old values and reset it to the default
		void Reset();

		static Types GuessType(std::istream& Stream); //Guess the type of object (Only looks based on the first character) - Assumes first character at stream pos is usable
		static void SkipWhitespace(std::istream& Stream); //Skip any whitespace (Schema defined whitespace)
		static std::string EscapeQuotes(std::string String);
	};

	std::istream& operator >> (std::istream& Stream, Value& Value);
	std::ostream& operator << (std::ostream& Stream, const Value& Value);
}