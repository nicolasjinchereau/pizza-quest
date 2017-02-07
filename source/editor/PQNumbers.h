/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_PQPROPVALS_H
#define INC_PQPROPVALS_H

#include "definitions.h"

/*****************************
PQNumber
*****************************/

class PQNumber
{
public:
	virtual ~PQNumber(){}
};

template<class T>
class PQNumber_t : public PQNumber
{
public:
	T val;

	PQNumber_t(){};
	PQNumber_t(T v){val = v;}
	PQNumber_t &operator=(T v){val = v; return *this;}
	operator T(){return val;}
};

typedef PQNumber_t<int8> PQInt8;
typedef PQNumber_t<Uint8> PQUint8;
typedef PQNumber_t<Int16> PQInt16;
typedef PQNumber_t<Uint16> PQUint16;
typedef PQNumber_t<int32> PQInt32;
typedef PQNumber_t<Uint32> PQUint32;
typedef PQNumber_t<float> PQFloat;
typedef PQNumber_t<double> PQDouble;

#endif /* INC_PQPROPVALS_H */