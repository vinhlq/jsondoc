/*****************************************************************************
* File Name: jsondoc.h
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

/*******************************************************************************
* Included headers
*******************************************************************************/

/*******************************************************************************
* User defined Macros
*******************************************************************************/

#ifndef JSONDOC_H
#define JSONDOC_H



/*******************************************************************************
* Structure Definitions
*******************************************************************************/

typedef enum
{
	JSONDOC_NUMBER_UINT,
	JSONDOC_NUMBER_INT,
	JSONDOC_NUMBER_FLOAT,
	JSONDOC_BOOL,
	JSONDOC_STRING,
	JSONDOC_OBJECT,
	JSONDOC_ARRAY
} JsonDocPrimitiveType_t;

typedef struct JsonDocStruct JsonDocStruct_t;
typedef struct JsonDocArray JsonDocArray_t;

typedef struct
{
	const void *pValue;
	int size;
}JsonDocPrimitive_t;

typedef struct
{
	const JsonDocStruct_t *pStruct;
	int count;
}JsonDocObject_t;

struct JsonDocArray
{
	JsonDocPrimitiveType_t type;
	int count;

	union
	{
		const JsonDocPrimitive_t *pPrimitive;
		const JsonDocObject_t *pObject;
		const JsonDocStruct_t *pStruct;
	};
};

struct JsonDocStruct
{
	JsonDocPrimitiveType_t type;
	const char *pKey;
	union
	{
		JsonDocPrimitive_t primitive;
		JsonDocObject_t object;
		JsonDocArray_t array;
	};
};

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/** @brief jsondoc_init
 *
 *
 * @param pJsonDocument  			Ver.: always
 * @param maxSizeOfJsonDocument  	Ver.: always
 */
int jsondoc_init(char *pJsonDocument, size_t maxSizeOfJsonDocument, const char *name);

/** @brief jsondoc_finalize
 *
 *
 * @param pJsonDocument  			Ver.: always
 * @param maxSizeOfJsonDocument  	Ver.: always
 */
int jsondoc_finalize(char *pJsonDocument, size_t maxSizeOfJsonDocument, const char *name);

/** @brief jsondoc_object_add
 *
 *
 * @param pJsonDocument  			Ver.: always
 * @param maxSizeOfJsonDocument  	Ver.: always
 */
int jsondoc_object_add(char *pJsonDocument, size_t maxSizeOfJsonDocument, uint8_t count, const JsonDocStruct_t **pJsonDocStruct);

/** @brief jsondoc_object_add_va_list
 *
 *
 * @param pJsonDocument  			Ver.: always
 * @param maxSizeOfJsonDocument  	Ver.: always
 */
int jsondoc_object_add_va_list(char *pJsonDocument, size_t maxSizeOfJsonDocument, uint8_t count, va_list args);

/** @brief jsondoc_object_add_vargs
 *
 *
 * @param pJsonDocument  			Ver.: always
 * @param maxSizeOfJsonDocument  	Ver.: always
 */
int jsondoc_object_add_vargs(char *pJsonDocument, size_t maxSizeOfJsonDocument, uint8_t count, ...);

#endif
