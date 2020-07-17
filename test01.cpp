#include <stdio.h>
#include <iostream>
#include <vector>

#include "ber.h"
#include "mpz.h"

using namespace std;

static ber::Type read_type(unsigned char c)
{
	ber::Type type;
	type.cls = static_cast<ber::Class>((c & 0b11000000) >> 6);
	type.pc = static_cast<ber::PC>((c & 0b00100000) >> 5);
	type.tag = static_cast<ber::Tag>(c & 0b00011111);

	return type;

}

static int read_length(istream& is)
{
	unsigned char c;

	if (!(is >> noskipws >> c))
		throw "reached eof in read_length";

	if (c & 0b10000000)
	{
		vector<unsigned char> v;

		for (int i = 0, j = c&0b01111111; i < j; i++)
		{
			is >> c;
			printf ("%0x ", c);
			v.push_back(c);
		}

		return bnd2mpz(v.begin(), v.end()).get_si();
	} else
		return c;
}

vector<unsigned char> read_value(istream& is, int len)
{
	unsigned char c;
	vector<unsigned char> v;

	for (int i = 0; i < len; i++)
	{
		if (!(is >> noskipws >> c))
			throw "reached eof in read_value";

		v.push_back(c);
	}

	return v;
}


static Json::Value type_change(ber::Tag tag, vector<unsigned char> v)
{
	switch (tag)
	{
		case ber::EOC:
		case ber::BOOLEAN:
			return v[0] ? true : false;
		case ber::INTEGER:
		case ber::BIT_STRING:
		case ber::OCTET_STRING:
		case ber::NUMERIC_STRING:
		case ber::OBJECT_IDENTIFIER:
		case ber::OBJECT_DESCRIPTOR:
			{
				stringstream ss;

				for (auto a : v)
					ss << hex << setw(2) << setfill('0') << +a << ':';
			
				return ss.str();
			}
		case ber::NULL_TYPE:
			return "null";
		case ber::EXTERNAL:
		case ber::REAL:
			return *(float*)v.data();
		case ber::ENUMERATED:
		case ber::EMBEDDED_PDV:
		case ber::RELATIVE_OID:
		default:
			{
				stringstream ss;

				for (auto a: v) 
					ss << a;

				return ss.str();
			}
	}
}


int main()
{

	ber::Type  type;
	unsigned char c = 0b10101010;
	unsigned char s = 0b01111111;
	stringstream ss;

	printf ("%d\n", c);

	type = read_type(c);


	printf ("cls = %0x\n", type.cls);
	printf ("pc = %0x\n", type.pc);
	printf ("tag = %0x\n", type.tag);

	ss << s;
	printf ("%d\n", read_length(ss));



	return 0;
}
