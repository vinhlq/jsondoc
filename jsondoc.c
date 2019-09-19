/*****************************************************************************
* File Name: jsondoc.c
*
* Version 1.00
*
* Description:
*   This file contains the declarations of all the high-level APIs.
*
* Note:
*   N/A
*
* Owner:
*   vinhlq
*
* Related Document:
*
* Hardware Dependency:
*   N/A
*
* Code Tested With:
*
******************************************************************************
* Copyright (2019), vinhlq.
******************************************************************************
* This software is owned by vinhlq and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* (vinhlq) hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* (vinhlq) Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a (vinhlq) integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of (vinhlq).
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* (vinhlq) reserves the right to make changes without further notice to the
* materials described herein. (vinhlq) does not assume any liability arising out
* of the application or use of any product or circuit described herein. (vinhlq)
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of (vinhlq)' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies (vinhlq) against all charges. Use may be
* limited by and subject to the applicable (vinhlq) software license agreement.
*****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#ifdef JSONDOC_USE_STD_SNPRINTF
#include <stdio.h>
#define json_snprintf snprintf
#else
#undef HAVE_SNPRINTF
#undef HAVE_CVT
#include "ap_snprintf.h"
#define json_snprintf ap_snprintf
#endif // #ifndef JSONDOC_USE_STD_SNPRINTF
#include "jsondoc.h"

#ifndef JSONDOC_NDIG
#define JSONDOC_NDIG 8
#endif

#define str(s) #s
#define xstr(s) str(s)

static inline double jsondoc_get_number_float(const JsonDocPrimitive_t *p)
{
	switch(p->size)
	{

	case sizeof(float):
	{
		float number = 0;
		memcpy
			(
				(void *)&number,
				(void *)p->pValue,
				sizeof(float)
			);
		return (double)number;
	}

	case sizeof(double):
	{
		double number = 0;
		memcpy
			(
				(void *)&number,
				(void *)p->pValue,
				sizeof(double)
			);
		return number;
	}

	default:
		return NAN;
	}
}

static int jsondoc_array_add_element
	(
		char *pJsonDocument, size_t maxSizeOfJsonDocument,
		const JsonDocArray_t *pJsonDocArray,
		int index
	);
static int jsondoc_object_add_element
	(
		char *pJsonDocument, size_t maxSizeOfJsonDocument,
		const JsonDocStruct_t *pJsonDocStruct
	)
{
	switch(pJsonDocStruct->type)
	{
	case JSONDOC_NUMBER_UINT:
	{
		unsigned long number;

		number = 0;
		memcpy
			(
				&number,
				pJsonDocStruct->primitive.pValue,
				pJsonDocStruct->primitive.size < sizeof(long) ? pJsonDocStruct->primitive.size:sizeof(long)
			);
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"\"%s\":%lu,"
				,
				pJsonDocStruct->pKey,
				number
			);
	}

	case JSONDOC_NUMBER_INT:
	{
		long number;

		number = 0;
		memcpy
			(
				&number,
				pJsonDocStruct->primitive.pValue,
				pJsonDocStruct->primitive.size < sizeof(long) ? pJsonDocStruct->primitive.size:sizeof(long)
			);
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"\"%s\":%ld,"
				,
				pJsonDocStruct->pKey,
				number
			);
	}

	case JSONDOC_NUMBER_FLOAT:
	{
		double number = jsondoc_get_number_float(&pJsonDocStruct->primitive);
		if(NAN == number)
		{
			return -1;
		}
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"\"%s\":" "%." xstr(JSONDOC_NDIG) "f,"
				,
				pJsonDocStruct->pKey,
				number
			);
	}

	case JSONDOC_BOOL:
	{
		long number;

		number = 0;
		memcpy
			(
				&number,
				pJsonDocStruct->primitive.pValue,
				pJsonDocStruct->primitive.size < sizeof(long) ? pJsonDocStruct->primitive.size:sizeof(long)
			);
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"\"%s\":%s,"
				,
				pJsonDocStruct->pKey,
				number ? "true":"false"
			);
	}

	case JSONDOC_STRING:
	{
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"\"%s\":\"%.*s\","
				,
				pJsonDocStruct->pKey,
				pJsonDocStruct->primitive.size, (char *)pJsonDocStruct->primitive.pValue
			);
	}

	case JSONDOC_OBJECT:
	{
		int n;
		int i;

		n = json_snprintf(pJsonDocument, maxSizeOfJsonDocument, "\"%s\":{", pJsonDocStruct->pKey);
		if(n < 1 || maxSizeOfJsonDocument < (n + 2))
		{
			return -1;
		}
		for
			(
				i = 0;
				i < pJsonDocStruct->object.count;
				i++
			)
		{
			n += jsondoc_object_add_element
					(
						&pJsonDocument[n], maxSizeOfJsonDocument-n,
						&pJsonDocStruct->object.pStruct[i]
					);
			if((n+2) >= maxSizeOfJsonDocument)
			{
				return -1;
			}
		}
		pJsonDocument[n-1] = '}';
		pJsonDocument[n] = ',';
		pJsonDocument[n+1] = '\0';
		return (n+1);
	}

	case JSONDOC_ARRAY:
	{
		int n;
		int i;

		n = json_snprintf(pJsonDocument, maxSizeOfJsonDocument, "\"%s\":[", pJsonDocStruct->pKey);
		if(n < 1 || maxSizeOfJsonDocument < (n + 2))
		{
			return -1;
		}
		for
			(
				i = 0;
				i < pJsonDocStruct->array.count;
				i++
			)
		{
			n += jsondoc_array_add_element
					(
						&pJsonDocument[n], maxSizeOfJsonDocument-n,
						&pJsonDocStruct->array,
						i
					);
			if((n+2) >= maxSizeOfJsonDocument)
			{
				return -1;
			}
		}
		pJsonDocument[n-1] = ']';
		pJsonDocument[n] = ',';
		pJsonDocument[n+1] = '\0';
		return (n+1);
	}

	default:
		return -1;
	}
}

static int jsondoc_array_add_element
	(
		char *pJsonDocument, size_t maxSizeOfJsonDocument,
		const JsonDocArray_t *pJsonDocArray,
		int index
	)
{
	switch(pJsonDocArray->type)
	{
	case JSONDOC_NUMBER_UINT:
	{
		unsigned long number;

		number = 0;
		memcpy
			(
				&number,
				pJsonDocArray->pPrimitive[index].pValue,
				pJsonDocArray->pPrimitive[index].size < sizeof(long) ? pJsonDocArray->pPrimitive[index].size:sizeof(long)
			);
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"%lu,"
				,
				number
			);
	}

	case JSONDOC_NUMBER_INT:
	{
		long number;

		number = 0;
		memcpy
			(
				&number,
				pJsonDocArray->pPrimitive[index].pValue,
				pJsonDocArray->pPrimitive[index].size < sizeof(long) ? pJsonDocArray->pPrimitive[index].size:sizeof(long)
			);
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"%ld,"
				,
				number
			);
	}

	case JSONDOC_NUMBER_FLOAT:
	{
		double number = jsondoc_get_number_float(&pJsonDocArray->pPrimitive[index]);
		if(NAN == number)
		{
			return -1;
		}
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"%." xstr(JSONDOC_NDIG) "f,"
				,
				number
			);
	}

	case JSONDOC_BOOL:
	{
		long number;

		number = 0;
		memcpy
			(
				&number,
				pJsonDocArray->pPrimitive[index].pValue,
				pJsonDocArray->pPrimitive[index].size < sizeof(long) ? pJsonDocArray->pPrimitive[index].size:sizeof(long)
			);
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"%s,"
				,
				number ? "true":"false"
			);
	}

	case JSONDOC_STRING:
	{
		return json_snprintf
			(
				pJsonDocument,
				maxSizeOfJsonDocument,
				"\"%.*s\","
				,
				pJsonDocArray->pPrimitive[index].size, (char *)pJsonDocArray->pPrimitive[index].pValue
			);
	}

	case JSONDOC_OBJECT:
	{
		int n;
		int i;

		n = json_snprintf(pJsonDocument, maxSizeOfJsonDocument, "{");
		if(n < 1 || maxSizeOfJsonDocument < (n + 2))
		{
			return -1;
		}
		for
			(
				i = 0;
				i < pJsonDocArray->pObject[index].count;
				i++
			)
		{
			n += jsondoc_object_add_element
					(
						&pJsonDocument[n], maxSizeOfJsonDocument-n,
						&pJsonDocArray->pObject[index].pStruct[i]
					);
			if((n+2) >= maxSizeOfJsonDocument)
			{
				return -1;
			}
		}
		pJsonDocument[n-1] = '}';
		pJsonDocument[n] = ',';
		pJsonDocument[n+1] = '\0';
		return (n+1);
	}

	case JSONDOC_ARRAY:
	{
		int n;
		int i;

		n = json_snprintf(pJsonDocument, maxSizeOfJsonDocument, "\"%s\":[", pJsonDocArray->pStruct[index].pKey);
		if(n < 1 || maxSizeOfJsonDocument < (n + 2))
		{
			return -1;
		}
		for
			(
				i = 0;
				i < pJsonDocArray->pStruct[index].array.count;
				i++
			)
		{
			n += jsondoc_array_add_element
					(
						&pJsonDocument[n], maxSizeOfJsonDocument-n,
						&pJsonDocArray->pStruct[index].array,
						i
					);
			if((n+2) >= maxSizeOfJsonDocument)
			{
				return -1;
			}
		}
		pJsonDocument[n-1] = ']';
		pJsonDocument[n] = ',';
		pJsonDocument[n+1] = '\0';
		return (n+1);
	}

	default:
		return -1;
	}
}

int jsondoc_array_add
	(
		char *pJsonDocument, size_t maxSizeOfJsonDocument,
		const JsonDocArray_t *pJsonDocArray
	)
{
	size_t n;
	int i, jsondoc_size;

	jsondoc_size = strlen(pJsonDocument);
	if(jsondoc_size < 1 || maxSizeOfJsonDocument < (jsondoc_size + 1))
	{
		return -1;
	}
	for
		(
			n = 0, i = 0;
			i < pJsonDocArray->count;
			i++
		)
	{
		jsondoc_size = jsondoc_array_add_element
				(
					&pJsonDocument[n], maxSizeOfJsonDocument-n,
					pJsonDocArray, i
				);
		if(jsondoc_size < 1)
		{
			return -1;
		}
		n += jsondoc_size;
	}
	return n;
}

int jsondoc_object_add_va_list
	(
		char *pJsonDocument, size_t maxSizeOfJsonDocument,
		uint8_t count, va_list args
	)
{
	size_t n;
	int jsondoc_size;
	JsonDocStruct_t *pJsonDocStruct;

	jsondoc_size = strlen(pJsonDocument);
	if(jsondoc_size < 1 || maxSizeOfJsonDocument < (jsondoc_size + 1))
	{
		return -1;
	}

	for
		(
			n = jsondoc_size;
			n < (maxSizeOfJsonDocument-1) && count;
			count--
		)
	{
		pJsonDocStruct = va_arg(args, JsonDocStruct_t *);
		if(!pJsonDocStruct)
		{
			return -1;
		}

		jsondoc_size = jsondoc_object_add_element(&pJsonDocument[n], maxSizeOfJsonDocument-n, pJsonDocStruct);
		if(jsondoc_size < 1)
		{
			return -1;
		}
		n += jsondoc_size;
	}

	pJsonDocument[n] = '\0';
	return n;
}

int jsondoc_object_add
	(
		char *pJsonDocument, size_t maxSizeOfJsonDocument,
		uint8_t count, const JsonDocStruct_t **pJsonDocStruct
	)
{
	size_t n;
	int jsondoc_size;

	jsondoc_size = strlen(pJsonDocument);
	if(jsondoc_size < 1 || maxSizeOfJsonDocument < (jsondoc_size + 1))
	{
		return -1;
	}

	for
		(
			n = jsondoc_size;
			n < (maxSizeOfJsonDocument-1) && count;
			count--
		)
	{
//		printf("object: %s\r\n", (*pJsonDocStruct)->pKey);
		jsondoc_size = jsondoc_object_add_element
				(
					&pJsonDocument[n], maxSizeOfJsonDocument-n,
					*pJsonDocStruct++
				);
		if(jsondoc_size < 1)
		{
			return -1;
		}
		n += jsondoc_size;
	}
	pJsonDocument[n] = '\0';
	return n;
}

int jsondoc_object_add_vargs(char *pJsonDocument, size_t maxSizeOfJsonDocument, uint8_t count, ...)
{
	va_list args;
	int n;

	va_start(args, count);
	n = jsondoc_object_add_va_list(pJsonDocument, maxSizeOfJsonDocument, count, args);
	va_end(args);
	return n;
}

int jsondoc_init(char *pJsonDocument, size_t maxSizeOfJsonDocument, const char *name)
{
	if(name)
	{
		return json_snprintf(pJsonDocument, maxSizeOfJsonDocument, "\"%s\":{", name);
	}
	else
	{
		return json_snprintf(pJsonDocument, maxSizeOfJsonDocument, "{");
	}
}

int jsondoc_finalize(char *pJsonDocument, size_t maxSizeOfJsonDocument, const char *name)
{
	size_t n=0;
	int jsondoc_size;
	char end;

	if(name)
	{
		n = strlen(name);
		if
			(
				n > maxSizeOfJsonDocument ||
				pJsonDocument[0] != '"' ||
				0 != strncmp(name, &pJsonDocument[1], n) ||
				pJsonDocument[n+1] != '"'
			)
		{
			return -1;
		}
		n += 2;
		pJsonDocument = pJsonDocument + n;
		maxSizeOfJsonDocument -= n;
	}

	if(pJsonDocument[0] == '{')
	{
		end = '}';
	}
	else
	{
		return -1;
	}

	jsondoc_size = strlen(pJsonDocument);
	if(jsondoc_size < 1 || maxSizeOfJsonDocument < (jsondoc_size + 1))
	{
		return -1;
	}
	n += jsondoc_size;
	pJsonDocument[n-1] = end;
	pJsonDocument[n] = '\0';
	return n + 1;
}


/*
 * EOF
 */

