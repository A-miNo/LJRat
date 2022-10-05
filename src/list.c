#include <list.h>
#include <debug.h>

VOID ListInitialize(PLIST_ENTRY pListHead)
{
	pListHead->Flink = pListHead->Blink = pListHead;
}

VOID SRWListInitialize(PSRW_LIST_HEAD pListHead)
{
	InitializeSRWLock(&(pListHead->SRWLock));
	ListInitialize(&(pListHead->head));
}

VOID SRWListAddTail(PSRW_LIST_HEAD pListHead, PLIST_ENTRY pEntry)
{
	AcquireSRWLockExclusive(&(pListHead->SRWLock));
	ListAddTail(&(pListHead->head), pEntry);
	ReleaseSRWLockExclusive(&(pListHead->SRWLock));
}

VOID ListAddTail(PLIST_ENTRY pListHead, PLIST_ENTRY pEntry)
{
	PLIST_ENTRY pBlink;

	pBlink = pListHead->Blink;
	pEntry->Flink = pListHead;
	pEntry->Blink = pBlink;
	pBlink->Flink = pEntry;
	pListHead->Blink = pEntry;

	return;
}

PLIST_ENTRY SRWListRemoveHead(PSRW_LIST_HEAD pListHead)
{
	PLIST_ENTRY pEntry = NULL;

	AcquireSRWLockExclusive(&(pListHead->SRWLock));
	pEntry = ListRemoveHead(&(pListHead->head));
	ReleaseSRWLockExclusive(&(pListHead->SRWLock));

	return pEntry;
}

PLIST_ENTRY ListRemoveHead(PLIST_ENTRY pListHead)
{
	PLIST_ENTRY pFlink;
	PLIST_ENTRY pEntry;

	pEntry = pListHead->Flink;
	pFlink = pEntry->Flink;
	pListHead->Flink = pFlink;
	pFlink->Blink = pListHead;
	return pEntry;
}

BOOLEAN SRWListIsEmpty(PSRW_LIST_HEAD pListHead)
{
	BOOLEAN bRetVal = FALSE;
	AcquireSRWLockShared(&(pListHead->SRWLock));
	bRetVal = ListIsEmpty(&(pListHead->head));
	ReleaseSRWLockShared(&(pListHead->SRWLock));

	return bRetVal;
}

BOOLEAN ListIsEmpty(PLIST_ENTRY pListHead)
{
	return (BOOLEAN)(pListHead->Flink == pListHead);
}

PLIST_ENTRY SRWListForEach(PSRW_LIST_HEAD pListHead, PFOREACHFUNC pFunc)
{
	PLIST_ENTRY pEntry = NULL;

	AcquireSRWLockExclusive(&(pListHead->SRWLock));
	pEntry = ListForEach(&(pListHead->head), pFunc);
	ReleaseSRWLockExclusive(&(pListHead->SRWLock));

	return pEntry;
}

PLIST_ENTRY ListForEach(PLIST_ENTRY pListHead, PFOREACHFUNC pFunc)
{
	PLIST_ENTRY pTemp = pListHead;
	PLIST_ENTRY pRetEntry = NULL;

	while (pTemp->Flink != pListHead)
	{
		pTemp = pTemp->Flink;
		pRetEntry = pFunc(pTemp);
	}

	return pRetEntry;
}

PLIST_ITEM ListItemCreate(PVOID pData)
{
    PLIST_ITEM pItem = NULL;

    if (NULL == pData)
    {
        goto cleanup;
    }

    pItem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LIST_ITEM));
    if (NULL == pItem)
    {
        goto cleanup;
    }

    pItem->pData = pData;

    cleanup:
return pItem;

}