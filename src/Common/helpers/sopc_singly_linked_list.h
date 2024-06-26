/*
 * Licensed to Systerel under one or more contributor license
 * agreements. See the NOTICE file distributed with this work
 * for additional information regarding copyright ownership.
 * Systerel licenses this file to you under the Apache
 * License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/**
 * \file sopc_singly_linked_list.h
 *
 *  \brief A singly linked list based on elements with unique identifiers and dynamically allocated.
 */

#ifndef SOPC_SINGLY_LINKED_LIST_H_
#define SOPC_SINGLY_LINKED_LIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 *  \brief Singly linked list structure
 */
typedef struct SOPC_SLinkedList SOPC_SLinkedList;

typedef struct SOPC_SLinkedList_Elt SOPC_SLinkedList_Elt;

typedef SOPC_SLinkedList_Elt* SOPC_SLinkedListIterator;

/**
 *  \brief            Create and allocate a new singly linked list containing 0 elements with a size limit of the given
 *                    size.
 *
 *  \param sizeMax    The maximum number of elements allowed in the new linked list or 0 if no limit defined
 *  \return           Pointer to the newly allocated singly linked list
 */
SOPC_SLinkedList* SOPC_SLinkedList_Create(size_t sizeMax);

/**
 *  \brief          Add a new element (and allocate new list element) before head of the given linked list.
 *
 *  \param list     Pointer on the linked list in which new element must be added
 *  \param id       Unique identifier to associate with the element
 *                  (if not unique Prepend has LIFO behavior for Find and Remove)
 *  \param value    Pointer to the value or unsigned integer value of the element to prepend.
 *                  Value 0 is considered invalid.
 *
 *  \return         Pointer to the value or unsigned integer value prepended, provided as parameter,
 *                  if succeeded, 0 (NULL) otherwise
 */
uintptr_t SOPC_SLinkedList_Prepend(SOPC_SLinkedList* list, uint32_t id, uintptr_t value);

/**
 *  \brief          Add a new element (and allocate new list element) to the tail of the given linked list.
 *
 *  \param list     Pointer on the linked list in which new element must be added
 *  \param id       Unique identifier to associate with the element
 *                  (if not unique Append has FIFO behavior for Find and Remove)
 *  \param value    Pointer to the value or unsigned integer value of the element to append
 *                  Value 0 is considered invalid.
 *  \return         Pointer to the value or unsigned integer value appended, provided as parameter,
 *                  if succeeded, 0 (NULL) otherwise
 */
uintptr_t SOPC_SLinkedList_Append(SOPC_SLinkedList* list, uint32_t id, uintptr_t value);

/**
 * \brief           Insert element in sorted list in correct index regarding compare function.
 *
 *   The element will be inserted before the element for which the compare function return that new element is < to the
 *   existing element (compare returns -1 when new element is left operand and < to right operand).
 *
 * \note            Important: the provided list shall be sorted regarding the same compare function.
 *
 * \param list      Pointer to the linked list
 * \param id        Identifier of the given value
 * \param value     Value to insert in the sorted list.
 *                  Value 0 is considered invalid.
 * \param pCompFn   Compare function pointer returning a int8_t equals to -1 if left value < right value, 0 if left
 * value == right value and 1 if left value > right value
 *
 *  \return         Pointer to the value or unsigned integer value inserted, provided as parameter,
 *                  if succeeded, 0 (NULL) otherwise
 */
uintptr_t SOPC_SLinkedList_SortedInsert(SOPC_SLinkedList* list,
                                        uint32_t id,
                                        uintptr_t value,
                                        int8_t (*pCompFn)(uintptr_t left, uintptr_t right));

/**
 *  \brief          Get and remove the head element of the list
 *
 *  \param list     Pointer on the linked list from which head element must be returned and removed
 *
 *  \return         Pointer to the head element value of the list or 0 (NULL) if list is empty
 */
uintptr_t SOPC_SLinkedList_PopHead(SOPC_SLinkedList* list);

/**
 *  \brief          Get and remove the last element of the list
 *  \note           This function iterate on the whole list to remove the last element
 *
 *  \param list     Pointer on the linked list from which head element must be returned and removed
 *
 *  \return         Pointer to the last element value of the list or 0 (NULL) if list is empty
 */
uintptr_t SOPC_SLinkedList_PopLast(SOPC_SLinkedList* list);

/**
 *  \brief          Get the head element of the list without removing it.
 *                  Note: it shall not be freed.
 *
 *  \param list     Pointer on the linked list from which head element must be returned
 *
 *  \return         Pointer to the head element value of the list or 0 (NULL) if list is empty
 */
uintptr_t SOPC_SLinkedList_GetHead(SOPC_SLinkedList* list);

/**
 *  \brief          Get the tail element of the list without removing it.
 *                  Note: it shall not be freed.
 *
 *  \param list     Pointer on the linked list from which tail element must be returned
 *
 *  \return         Pointer to the tail element value of the list or 0 (NULL) if list is empty
 */
uintptr_t SOPC_SLinkedList_GetLast(SOPC_SLinkedList* list);

/**
 *  \brief          Find the first value associated to the given id in the linked list
 *                  (iterate from head to tail)
 *
 *  \param list     Pointer on the linked list in which element must be found
 *  \param id       Unique identifier associated with the element to find
 *
 *  \return         Pointer to the value or unsigned integer value found if succeeded, 0 (NULL) otherwise
 */
uintptr_t SOPC_SLinkedList_FindFromId(SOPC_SLinkedList* list, uint32_t id);

/**
 * \brief           Apply a function to the value of each element of the list.
 *
 *   An example is the SOPC_SLinkedList_EltGenericFree() function which frees the value of each element of the list.
 *
 * \param list      Pointer to the linked list
 * \param pFn       Function pointer which takes the id and the value of each element
 */
void SOPC_SLinkedList_Apply(SOPC_SLinkedList* list, void (*pFn)(uint32_t id, uintptr_t val));

/**
 *  \brief          Find and remove the first value associated to the given id in the linked list
 *                  (iterate from head to tail)
 *
 *  \param list     Pointer on the linked list in which element must be found
 *  \param id       Unique identifier associated with the element to remove
 *
 *  \return         Pointer to the value or unsigned integer value removed if succeeded, 0 (NULL) otherwise
 */
uintptr_t SOPC_SLinkedList_RemoveFromId(SOPC_SLinkedList* list, uint32_t id);

/**
 *  \brief          Find and remove the first value pointer equal in the linked list
 *                  (iterate from head to tail)
 *
 *  \param list     Pointer on the linked list in which element must be found
 *  \param value    Pointer to the value or unsigned integer value of the element to remove.
 *                  Value 0 is considered invalid.
 *
 *  \return         Pointer to the value or unsigned integer value removed if succeeded, 0 (NULL) otherwise
 */
uintptr_t SOPC_SLinkedList_RemoveFromValuePtr(SOPC_SLinkedList* list, uintptr_t value);

/**
 *  \brief          Delete all elements of the given linked list
 *
 *  \param list     Pointer to the list of elements to be deleted
 */
void SOPC_SLinkedList_Clear(SOPC_SLinkedList* list);

/**
 *  \brief          Delete and deallocate the given linked list
 *
 *  \param list     Pointer to the list to deallocate (pointer must not be used anymore after operation)
 */
void SOPC_SLinkedList_Delete(SOPC_SLinkedList* list);

/**
 * \brief           Frees the value of an element of the SOPC_SLinkedList.
 *
 *  \param id       Unique identifier associated with the element
 *  \param val      Element to be freed
 */
void SOPC_SLinkedList_EltGenericFree(uint32_t id, uintptr_t val);

/**
 * \brief           Get an iterator on a linked list to iterate on elements from head to tail
 *
 * \param list      Pointer to the list for which an iterator is requested
 *
 * \return          An iterator on the given linked list
 */
SOPC_SLinkedListIterator SOPC_SLinkedList_GetIterator(SOPC_SLinkedList* list);

/**
 * \brief           Return true if iterator has a non NULL value to provide on next iteration
 *                  (iterate from head to tail)
 *
 * \param it        An iterator on a linked list
 *
 * \return          true if iterator has a non NULL value to provide on next iteration, false otherwise
 */
bool SOPC_SLinkedList_HasNext(const SOPC_SLinkedListIterator* it);

/**
 * \brief           Return the next element pointed by iterator in the linked list
 *                  (iterate from head to tail)

 * \param it        An iterator on a linked list
 *
 * \return          Pointer to the value or unsigned integer value of the next element of the linked list
 */
uintptr_t SOPC_SLinkedList_Next(SOPC_SLinkedListIterator* it);

/**
 * \brief           Return the next element pointed by iterator in the linked list
 *                  (iterate from head to tail)
 *
 * \param it        An iterator on a linked list
 * \param pId       Pointer in which the next element id of the linked list is set
 *
 * \return          Pointer to the value or unsigned integer value of the next element of the linked list
 */
uintptr_t SOPC_SLinkedList_NextWithId(SOPC_SLinkedListIterator* it, uint32_t* pId);

/**
 * \brief           Get the number of elements in the linked list
 *
 * \param list      Pointer to the list
 *
 * \return          The number of elements in the list
 */
uint32_t SOPC_SLinkedList_GetLength(SOPC_SLinkedList* list);

/**
 * \brief           Get the maximum number of elements that can be contained in the linked list
 *
 * \param list      Pointer to the list
 *
 * \return          The capacity of the list
 */
uint32_t SOPC_SLinkedList_GetCapacity(SOPC_SLinkedList* list);

/**
 * \brief           Set the maximum number of elements that can be contained in the linked list.
 *                  It might be used to modify the value provided on ::SOPC_SLinkedList_Create.
 *                  Call to this function will fail if the current length is greater than provided value.
 *
 * \param list      Pointer to the list
 * \param sizeMax   The maximum number of elements allowed in the new linked list or 0 if no limit defined.
 *                  If the current list length is greater than the provided value, call will fail.
 *
 * \return          true if the list capacity has been set, false otherwise
 */
bool SOPC_SLinkedList_SetCapacity(SOPC_SLinkedList* list, size_t sizeMax);

#endif /* SOPC_SINGLE_LINKED_LIST_H_ */
