/*
	LLIST.H
	Copyright (c) 1995  Shawn Halpenny
	All rights reserved.

	Singly-linked list template class.  Only stores pointers.  Additions
	go at end of list.
*/

#ifndef __LLIST_H
#define __LLIST_H

#ifndef ___DEFS_H
#include <_defs.h>
#endif

template <class T> struct TListNode
{
	T _FAR *pData;
    TListNode<T> _FAR *pNext;
};

template <class T> class TListIterator;
template <class T> class TList
{
	friend TListIterator<T>;
	TListNode<T> _FAR *pHead, _FAR *pLast;

public:
	TList();
	~TList();
	void Add(T _FAR *);
	void Remove(T _FAR *);
	void RemoveAll(BOOL); 
	BOOL IsEmpty() { return pHead == NULL; }
	T _FAR * nth(int);
	T _FAR * Peek();
	void ForEach(void (*iterFunc)(T _FAR *, void _FAR *), void _FAR *);
};

template <class T> class TListIterator
{
	const TList<T> _FAR *pList;
	TListNode<T> _FAR *pCur;

public:
	TListIterator(const TList<T> _FAR &);
	operator int();
	T _FAR * Current();
	T _FAR * operator++(int);
	T _FAR * operator++();
	void Restart();
};

/*
	Init list to empty.
*/
template <class T> TList<T>::TList()
{
	pHead = pLast = NULL;
}	//TList::TList

/*
	Destroy all nodes and data.
*/
template <class T> TList<T>::~TList()
{
	RemoveAll(TRUE);
}	//TList::~TList

/* Add p to list */
template <class T> void TList<T>::Add(T _FAR *p)
{
	TListNode<T> *pNewNode;

	pNewNode = new TListNode<T>;
	pNewNode->pData = p;
	pNewNode->pNext = NULL;

	if (pHead == NULL)
		pHead = pNewNode;
	else
		pLast->pNext = pNewNode;

	pLast = pNewNode;
}	//TList::Add

/* Remove p from list.  p is not destroyed. */
template <class T> void TList<T>::Remove(T _FAR *p)
{
	TListNode<T> *pNode = pHead;
	TListNode<T> *pPrev = NULL;

	while (pNode)
	{
		if (pNode->pData == p)
		{
			if (pHead == pNode)
				pHead = pHead->pNext;

			if (pLast == pNode)
			{
				pLast = pPrev;
                if (pLast != NULL)	//list not empty
					pLast->pNext = NULL;
			}
			else
			if (pPrev)
				pPrev->pNext = pNode->pNext;

			delete pNode;
			break;
		}

		pPrev = pNode;
        pNode = pNode->pNext;
    }
}	//TList::Remove

/*
	Empty the list.  If bAll, destroy all data too.
*/
template <class T> void TList<T>::RemoveAll(BOOL bAll)
{
	TListNode<T> *pNode = pHead, *pTemp;

	while (pNode)
	{
		pTemp = pNode->pNext;
		if (bAll)
			delete pNode->pData;

		delete pNode;
        pNode = pTemp;
	}

    pHead = pLast = NULL;
}	//TList::RemoveAll

/*
	Returns data in nth node of list (0-based).
*/
template <class T> T* TList<T>::nth(int n)
{
	TListNode<T> *pNode = pHead;
	int i = 0;

	while (pNode)
	{
		if (i == n)
			break;

		i++;
		pNode = pNode->pNext;
	}

    return pNode->pData;
}	//TList::nth

template <class T> T* TList<T>::Peek()
{
	return pHead->pData;
}	//TList::Peek

template <class T> void TList<T>::ForEach(void (*iterFunc)(T _FAR *, void _FAR *), void _FAR *p)
{
	TListIterator<T> Iter(*this);

	while (int(Iter))
		iterFunc(Iter++, p);
}	//TList::ForEach

template <class T> TListIterator<T>::TListIterator(const TList<T> _FAR &List)
{
	pList = &List;
	Restart();
}	//TListIterator::TListIterator

/*
	Returns 0 if at end of list.
*/
template <class T> TListIterator<T>::operator int()
{
    return (pCur != NULL);
}	//TListIterator::operator int

/*
	Returns data at current pos.
*/
template <class T> T* TListIterator<T>::Current()
{
	return pCur->pData;
}	//TListIterator::Current

/*
	Postfix incrementer.
*/
template <class T> T* TListIterator<T>::operator++(int)
{
	TListNode<T> *pTemp = pCur;

	pCur = pCur->pNext;
    return pTemp->pData;
}	//TListIterator::operator++

/*
	Prefix incrementer.
*/
template <class T> T* TListIterator<T>::operator++()
{
    pCur = pCur->pNext;
	return pCur->pData;
}	//TListIterator::operator++

/*
	Sets current pos to head of list.
*/
template <class T> void TListIterator<T>::Restart()
{
	pCur = pList->pHead;
}	//TListIterator::Restart

#endif	//__LIST_H
