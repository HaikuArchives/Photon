#include "TextMessage.hh"

#include <fstream>
#ifdef __POWERPC__
#include <sstream>
#else
#include <strstream>
#endif

TextMessage::TextMessage(uint32 command) : BMessage(command)
{
}

/*
//incompatible with Maui (r4.6?)
TextMessage::TextMessage(BMessage* message) : BMessage(message)
{
}
*/

TextMessage::TextMessage(const BMessage& message) : BMessage(message)
{
}

TextMessage::TextMessage() : BMessage()
{
}

TextMessage::~TextMessage()
{
}

status_t
TextMessage::WriteToTextFile(const char* thePath, const BMessage* anotherMsg) const
{
	const BMessage* whereFrom = (anotherMsg == 0) ? this : anotherMsg;
	bool		b;
	int8		i8;
	int16		i16;
	int32		i32;
	int64		i64;
	float		f;
	double		d;
	const char*	s;

	char  *name;
	uint32  typeconstant;
	int32   count;
	ofstream outFile(thePath);
	outFile << "what = " << whereFrom->what << endl;
	for (int32 i = 0; whereFrom->GetInfo(B_ANY_TYPE, i, &name, &typeconstant, &count) == B_OK; i++)
	{
		string type;
		if (strstr(name, "  ") || strchr(name, '\t') || strstr(name, " = "))
			return B_ERROR;
		if ((type = myConvertTypeConstant(typeconstant)) != "")
		{
			for (int iterator=0; iterator<count; iterator++)
			{
				outFile << type<<" "<<name<<" = ";
				if (typeconstant == B_BOOL_TYPE)
					if (whereFrom->FindBool(name, iterator, &b) == B_OK)
						if (b)
							outFile << "True";
						else
							outFile << "False";
				if (typeconstant == B_INT8_TYPE)
					if (whereFrom->FindInt8(name, iterator, &i8) == B_OK)
						outFile << i8;
				if (typeconstant == B_INT16_TYPE)
					if (whereFrom->FindInt16(name, iterator, &i16) == B_OK)
						outFile << i16;
				if (typeconstant == B_INT32_TYPE)
					if (whereFrom->FindInt32(name, iterator, &i32) == B_OK)
						outFile << i32;
				if (typeconstant == B_INT64_TYPE)
					if (whereFrom->FindInt64(name, iterator, &i64) == B_OK)
						outFile << i64;
				if (typeconstant == B_FLOAT_TYPE)
					if (whereFrom->FindFloat(name, iterator, &f) == B_OK)
						outFile << f;
				if (typeconstant == B_DOUBLE_TYPE)
					if (whereFrom->FindDouble(name, iterator, &d) == B_OK)
						outFile << d;
				if (typeconstant == B_STRING_TYPE)
					if (whereFrom->FindString(name, iterator, &s) == B_OK)
					{
						if (strchr(s, '\r') || strchr(s, '\n'))
							return B_ERROR;
						outFile << s;
					}
				outFile << endl;
			}
		}
	}
	return B_OK;
}

void
TextMessage::ReadFromTextFile(const char* thePath, BMessage* anotherMsg)
{
	BMessage* whereTo = (anotherMsg == 0) ? this : anotherMsg;
	ifstream inFile(thePath);
	string temp;
	inFile >> temp; // read "what"
	inFile >> temp; // read "="
	uint32 what;
	inFile >> what; // read BMessage.what code
	whereTo->what = what;
	while(!inFile.eof())
	{
		getline(inFile, temp);
#ifdef __POWERPC__
		stringstream line;
#else
		strstream line;
#endif
		line << temp;
		string varName; // This can contain spaces
		string varType;

		line >> varType;
		line >> temp;
		varName += temp;
		line >> temp;
		// Note! For laziness purposes, all white-
		// spaces in the names will be truncated
		// to a single space:
		while( (temp != "=") && (!line.eof()))
		{
			varName += " ";
			varName += temp;
			line >> temp;
		}
		// Ok, has read the equals-sign.
		if (varType == "string")
		{
			string contents;
			getline(line, contents);
		
			//ADDED BY SLEMA TO REMOVE LEADING SPACE....
			char *a;  a= (char*) malloc(2048);
			sprintf(a,contents.c_str());  a++;
			//ADDED BY SLEMA TO REMOVE LEADING SPACE....
		
			whereTo->AddString(varName.c_str(), a); //contents.c_str()
		}
		else if (varType == "bool")
		{
			string boolVal;
			line >> boolVal;
			if (!strcasecmp(boolVal.c_str(), "true"))
				whereTo->AddBool(varName.c_str(), true);
			else
				whereTo->AddBool(varName.c_str(), false);
		}
		else if (varType == "int8")
		{
			int val;
			line >> val;
			whereTo->AddInt8(varName.c_str(), char(val));
		}
		else if (varType == "int16")
		{
			int16 val;
			line >> val;
			whereTo->AddInt16(varName.c_str(), val);
		}
		else if (varType == "int32")
		{
			int32 val;
			line >> val;
			whereTo->AddInt32(varName.c_str(), val);
		}
		else if (varType == "int64")
		{
			int64 val;
			line >> val;
			whereTo->AddInt64(varName.c_str(), val);
		}
		else if (varType == "float")
		{
			float val;
			line >> val;
			whereTo->AddFloat(varName.c_str(), val);
		}
		else if (varType == "double")
		{
			double val;
			line >> val;
			whereTo->AddDouble(varName.c_str(), val);
		}
	}
}

string
TextMessage::myConvertTypeConstant(const uint32 theTypeCode) const
{
	string ret;
	switch(theTypeCode)
	{
		case B_BOOL_TYPE: ret = "bool"; break;
		case B_INT8_TYPE: ret = "int8"; break;
		case B_INT16_TYPE: ret = "int16"; break;
		case B_INT32_TYPE: ret = "int32"; break;
		case B_INT64_TYPE: ret = "int64"; break;
		case B_FLOAT_TYPE: ret = "float"; break;
		case B_DOUBLE_TYPE: ret = "double"; break;
		case B_STRING_TYPE: ret = "string"; break;
		default: ret = "";
	}
	return ret;
}

uint32
TextMessage::myConvertTypeConstant(const string theTypeName) const
{
	if 		(theTypeName == "bool") 	return B_BOOL_TYPE;
	else if (theTypeName == "int8") 	return B_INT8_TYPE;
	else if (theTypeName == "int16")	return B_INT16_TYPE;
	else if (theTypeName == "int32") 	return B_INT32_TYPE;
	else if (theTypeName == "int64") 	return B_INT64_TYPE;
	else if (theTypeName == "float") 	return B_FLOAT_TYPE;
	else if (theTypeName == "double") 	return B_DOUBLE_TYPE;
	else if (theTypeName == "string") 	return B_STRING_TYPE;
	return B_ERROR;
}
