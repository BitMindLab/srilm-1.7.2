/*
 * Map2.cc --
 *	Two-dimensional map implementation
 *
 */

#ifndef _Map2_cc_
#define _Map2_cc_

#ifndef lint
static char Map2_Copyright[] = "Copyright (c) 1999 SRI International.  All Rights Reserved.";
static char Map2_RcsId[] = "@(#)$Header: /home/srilm/devel/dstruct/src/RCS/Map2.cc,v 1.7 2000/08/05 19:47:08 stolcke Exp $";
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream.h>
#include <new.h>

#include "Map2.h"

#undef INSTANTIATE_MAP2

#ifdef USE_SARRAY_MAP2

# include "SArray.cc"

# define INSTANTIATE_MAP1(Key1T,Key2T,DataT) \
	typedef SArray<Key2T,DataT> Map2Type; \
	INSTANTIATE_SARRAY(Key1T, Map2Type ); \
	template class Map2< Key1T,Key2T,DataT>; \
	template class Map2Iter<Key1T,Key2T,DataT>; \
	template class Map2Iter2<Key1T,Key2T,DataT>

# define INSTANTIATE_MAP2(Key1T,Key2T,DataT) \
	INSTANTIATE_SARRAY(Key2T, DataT ); \
	INSTANTIATE_MAP1(Key1T,Key2T,DataT)

#else /* ! USE_SARRAY_MAP2 */

# include "LHash.cc"

# define INSTANTIATE_MAP1(Key1T,Key2T,DataT) \
	typedef LHash<Key2T,DataT> Map2Type; \
	INSTANTIATE_LHASH(Key1T, Map2Type ); \
	template class Map2< Key1T,Key2T,DataT>; \
	template class Map2Iter<Key1T,Key2T,DataT>; \
	template class Map2Iter2<Key1T,Key2T,DataT>

# define INSTANTIATE_MAP2(Key1T,Key2T,DataT) \
	INSTANTIATE_LHASH(Key2T, DataT ); \
	INSTANTIATE_MAP1(Key1T,Key2T,DataT)

#endif /* USE_SARRAY_MAP2 */
   
template <class Key1T, class Key2T, class DataT>
Map2<Key1T,Key2T,DataT>::Map2()
    : sub(0)
{
}

template <class Key1T, class Key2T, class DataT>
Map2<Key1T,Key2T,DataT>::~Map2()
{
    MAP2_ITER_T<Key1T, MAP2_INDEX_T<Key2T,DataT> > iter(sub);
    Key1T key;
    MAP2_INDEX_T<Key2T,DataT> *node;

    /*
     * destroy all second-level indices
     */
    while (node = iter.next(key)) {
#if __GNUC__ == 2 && __GNUC_MINOR__ == 8
	/* workaround for buggy gcc 2.8.1 */
	node->clear(0);
#else
	node->~MAP2_INDEX_T();
#endif
    }
}

/*
 * Dump contents of Map2 to cerr
 */
template <class Key1T, class Key2T, class DataT>
void
Map2<Key1T,Key2T,DataT>::dump() const
{
    MAP2_ITER_T<Key1T, MAP2_INDEX_T<Key2T,DataT> > iter(sub);
    Key1T key;
    MAP2_INDEX_T<Key2T,DataT> *row;

    while (row = iter.next(key)) {
	cerr << "Row Key = " << key << endl;
	row->dump();
    }
}

/*
 * Compute memory stats for Trie, including all children and grand-children
 */
template <class Key1T, class Key2T, class DataT>
void
Map2<Key1T,Key2T,DataT>::memStats(MemStats &stats) const
{
    stats.total += sizeof(*this) - sizeof(sub);
    sub.memStats(stats);

    MAP2_ITER_T<Key1T, MAP2_INDEX_T<Key2T,DataT> > iter(sub);
    Key1T key;
    MAP2_INDEX_T<Key2T,DataT> *row;

    while (row = iter.next(key)) {
	stats.total -= sizeof(*row);
	row->memStats(stats);
    }
}

template <class Key1T, class Key2T, class DataT>
DataT *
Map2<Key1T,Key2T,DataT>::find(Key1T key1, Key2T key2, Boolean &foundP) const
{
    MAP2_INDEX_T<Key2T,DataT> *row = sub.find(key1);

    if (row == 0) {
	foundP = false;
	return 0;
    } else {
	return row->find(key2, foundP);
    }
}

template <class Key1T, class Key2T, class DataT>
DataT *
Map2<Key1T,Key2T,DataT>::insert(Key1T key1, Key2T key2, Boolean &foundP)
{
    MAP2_INDEX_T<Key2T,DataT> *row = sub.insert(key1);

    return row->insert(key2, foundP);
}

template <class Key1T, class Key2T, class DataT>
DataT *
Map2<Key1T,Key2T,DataT>::remove(Key1T key1, Key2T key2, Boolean &foundP)
{
    MAP2_INDEX_T<Key2T,DataT> *row = sub.find(key1);

    if (row == 0) {
	foundP = false;
	return 0;
    } else{
	return row->remove(key2, foundP);
    }
}

template <class Key1T, class Key2T, class DataT>
Boolean
Map2<Key1T,Key2T,DataT>::remove(Key1T key1, Boolean &foundP)
{
    MAP2_INDEX_T<Key2T,DataT> *row = sub.find(key1, foundP);

    if (row != 0) {
	/*
	 * Destroy the row vector
	 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 8
	/* workaround for buggy gcc 2.8.1 */
	row->clear(0);
#else
	row->~MAP2_INDEX_T();
#endif
	sub.remove(key1);
    }
    return foundP;
}

#endif /* _Map2_cc_ */