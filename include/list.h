#ifndef LIST_H
#define LIST_H

#include <windows.h>

typedef struct _SRW_LIST_HEAD
{
    LIST_ENTRY head;
    DWORD dwLen;
    HANDLE hSem;
    SRWLOCK SRWLock;

} SRW_LIST_HEAD, *PSRW_LIST_HEAD;


typedef struct _LIST_ITEM
{
    LIST_ENTRY link;
    PVOID pData;
} LIST_ITEM, *PLIST_ITEM;

typedef PLIST_ENTRY (*PFOREACHFUNC)(PVOID);

/*
@brief Function that initializes a LIST_HEAD structure
@param pListHead PTR to a LIST_HEAD structure
@return VOID
*/
VOID ListInitialize(PLIST_ENTRY pListHead);

/*
@brief Function that initializes a LIST_HEAD structure (THREADSAFE)
@param pListHead PTR to a LIST_HEAD structure
@return VOID
*/
VOID SRWListInitialize(PSRW_LIST_HEAD pListHead);

/*
@brief Function that adds an entry to the end of the linked list
@param pListHead PTR to a LIST_HEAD structure
@param pEntry PTR to a LIST_ENTRY structure
@return VOID
*/
VOID ListAddTail(PLIST_ENTRY pListHead, PLIST_ENTRY pEntry);

/*
@brief Function that removes an entry from the front of a linked list
@param pListHead PTR to a LIST_HEAD structure
@return VOID
*/
PLIST_ENTRY ListRemoveHead(PLIST_ENTRY pListHead);

/*
@brief Function that checks if the list is empty
@param pListHead PTR to a LIST_HEAD structure
@return BOOLEAN TRUE if empty / FALSE if not-empty
*/
BOOLEAN ListIsEmpty(PLIST_ENTRY pListHead);

/*
@brief Function that iterates through a list performing a function on each node
@param pListHead PTR to a LIST_HEAD structure
@param pFunc PTR to a function
@return ForEachFunction has ability to return data
*/
PLIST_ENTRY ListForEach(PLIST_ENTRY pListHead, PFOREACHFUNC pFunc);

/*
@brief Function that initializes a LIST_HEAD structure (THREADSAFE)
@param pListHead PTR to a LIST_HEAD structure
@return VOID
*/
VOID SRWListInitialize(PSRW_LIST_HEAD pListHead);

/*
@brief Function that adds an entry to the end of the linked list  (THREADSAFE)
@param pListHead PTR to a LIST_HEAD structure
@param pEntry PTR to a LIST_ENTRY structure
@return VOID
*/
VOID SRWListAddTail(PSRW_LIST_HEAD pListHead, PLIST_ENTRY pEntry);

/*
@brief Function that removes an entry from the front of a linked list  (THREADSAFE)
@param pListHead PTR to a LIST_HEAD structure
@return VOID
*/
PLIST_ENTRY SRWListRemoveHead(PSRW_LIST_HEAD pListHead);

/*
@brief Function that checks if the list is empty  (THREADSAFE)
@param pListHead PTR to a LIST_HEAD structure
@return BOOLEAN TRUE if empty / FALSE if not-empty
*/
BOOLEAN SRWListIsEmpty(PSRW_LIST_HEAD pListHead);

/*
@brief Function that iterates through a list performing a function on each node  (THREADSAFE)
@param pListHead PTR to a LIST_HEAD structure
@param pFunc PTR to a function
@return ForEachFunction has ability to return data
*/
PLIST_ENTRY SRWListForEach(PSRW_LIST_HEAD pListHead, PFOREACHFUNC pFunc);

PLIST_ITEM ListItemCreate(PVOID pData);

#endif // LIST_H