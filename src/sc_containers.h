/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2007,2008 Carsten Burstedde, Lucas Wilcox.

  The SC Library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the SC Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SC_CONTAINERS_H
#define SC_CONTAINERS_H

#ifndef SC_H
#error "sc.h should be included before this header file"
#endif

/* Hash macros from lookup3.c by Bob Jenkins, May 2006, public domain. */
#define sc_hash_rot(x,k) (((x) << (k)) | ((x) >> (32 - (k))))
#define sc_hash_mix(a,b,c) do {                         \
    a -= c; a ^= sc_hash_rot(c, 4); c += b;             \
    b -= a; b ^= sc_hash_rot(a, 6); a += c;             \
    c -= b; c ^= sc_hash_rot(b, 8); b += a;             \
    a -= c; a ^= sc_hash_rot(c,16); c += b;             \
    b -= a; b ^= sc_hash_rot(a,19); a += c;             \
    c -= b; c ^= sc_hash_rot(b, 4); b += a;             \
  } while (0)
#define sc_hash_final(a,b,c) do {                       \
    c ^= b; c -= sc_hash_rot(b,14);                     \
    a ^= c; a -= sc_hash_rot(c,11);                     \
    b ^= a; b -= sc_hash_rot(a,25);                     \
    c ^= b; c -= sc_hash_rot(b,16);                     \
    a ^= c; a -= sc_hash_rot(c, 4);                     \
    b ^= a; b -= sc_hash_rot(a,14);                     \
    c ^= b; c -= sc_hash_rot(b,24);                     \
  } while (0)

/** Function to compute a hash value of an object.
 * \param [in] v   The object to hash.
 * \param [in] u   Arbitrary user data.
 * \return Returns an unsigned integer.
 */
typedef unsigned    (*sc_hash_function_t) (const void *v, const void *u);

/** Function to check equality of two objects.
 * \param [in] u   Arbitrary user data.
 * \return Returns false if *v1 is unequal *v2 and true otherwise.
 */
typedef             bool (*sc_equal_function_t) (const void *v1,
                                                 const void *v2,
                                                 const void *u);

/** The sc_array object provides a large array of equal-size elements.
 * The array can be resized.
 * Elements are accessed by their 0-based index, their address may change.
 * The size (== elem_count) of the array can be changed by array_resize.
 * Elements can be sorted with array_sort.
 * If the array is sorted elements can be binary searched with array_bsearch.
 * A priority queue is implemented with pqueue_add and pqueue_pop.
 * Use sort and search whenever possible, they are faster than the pqueue.
 */
typedef struct sc_array
{
  /* interface variables */
  size_t              elem_size;        /* size of a single element */
  size_t              elem_count;       /* number of valid elements */

  /* implementation variables */
  size_t              byte_alloc;       /* number of allocated bytes */
  char               *array;    /* linear array to store elements */
}
sc_array_t;

/** Creates a new array structure with 0 elements.
 * \param [in] elem_size  Size of one array element in bytes.
 * \return Returns an allocated and initialized array.
 */
sc_array_t         *sc_array_new (size_t elem_size);

/** Destroys an array structure.
 * \param [in] array  The array to be destroyed.
 */
void                sc_array_destroy (sc_array_t * array);

/** Initializes an already allocated array structure.
 * \param [in,out]  array       Array structure to be initialized.
 * \param [in] elem_size  Size of one array element in bytes.
 */
void                sc_array_init (sc_array_t * array, size_t elem_size);

/** Sets the array count to zero and frees all elements.
 * \param [in,out]  array       Array structure to be resetted.
 * \note Calling sc_array_init, then any array operations,
 *       then sc_array_reset is memory neutral.
 */
void                sc_array_reset (sc_array_t * array);

/** Sets the element count to new_count.
 * Reallocation takes place only occasionally, so this function is usually fast.
 */
void                sc_array_resize (sc_array_t * array, size_t new_count);

/** Sorts the array in ascending order wrt. the comparison function.
 * \param [in] compar The comparison function to be used.
 */
void                sc_array_sort (sc_array_t * array,
                                   int (*compar) (const void *,
                                                  const void *));

/** Removed duplicate entries from a sorted array.
 * \param [in,out] array  The array size will be reduced as necessary.
 * \param [in] compar     The comparison function to be used.
 */
void                sc_array_uniq (sc_array_t * array,
                                   int (*compar) (const void *,
                                                  const void *));

/** Performs a binary search on an array. The array must be sorted.
 * \param [in] key     An element to be searched for.
 * \param [in] compar  The comparison function to be used.
 * \return Returns the index into array for the item found, or -1.
 */
ssize_t             sc_array_bsearch (sc_array_t * array,
                                      const void *key,
                                      int (*compar) (const void *,
                                                     const void *));

/** Computes the adler32 checksum of array data.
 * This is a faster checksum than crc32, and it works with zeros as data.
 * \param [in] first_elem  Index of the element to start with.
 *                         Can be between 0 and elem_count (inclusive).
 */
unsigned            sc_array_checksum (sc_array_t * array, size_t first_elem);

/** Adds an element to a priority queue.
 * The priority queue is implemented as a heap in ascending order.
 * A heap is a binary tree where the children are not less than their parent.
 * Assumes that elements [0]..[elem_count-2] form a valid heap.
 * Then propagates [elem_count-1] upward by swapping if necessary.
 * \param [in] temp    Pointer to unused allocated memory of elem_size.
 * \param [in] compar  The comparison function to be used.
 * \return Returns the number of swap operations.
 * \note  If the return value is zero for all elements in an array,
 *        the array is sorted linearly and unchanged.
 */
size_t              sc_array_pqueue_add (sc_array_t * array,
                                         void *temp,
                                         int (*compar) (const void *,
                                                        const void *));

/** Pops the smallest element from a priority queue.
 * This function assumes that the array forms a valid heap in ascending order.
 * \param [out] result  Pointer to unused allocated memory of elem_size.
 * \param [in]  compar  The comparison function to be used.
 * \return Returns the number of swap operations.
 * \note This function resizes the array to elem_count-1.
 */
size_t              sc_array_pqueue_pop (sc_array_t * array,
                                         void *result,
                                         int (*compar) (const void *,
                                                        const void *));

/** Returns a pointer to an array element.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index (sc_array_t * array, size_t iz)
{
  SC_ASSERT (iz < array->elem_count);

  return (void *) (array->array + (array->elem_size * iz));
}

/** Returns a pointer to an array element indexed by a plain int.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_int (sc_array_t * array, int i)
{
  SC_ASSERT (i >= 0 && (size_t) i < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) i));
}

/** Returns a pointer to an array element indexed by a plain long.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_long (sc_array_t * array, long l)
{
  SC_ASSERT (l >= 0 && (size_t) l < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) l));
}

/** Returns a pointer to an array element indexed by a ssize_t.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_ssize_t (sc_array_t * array, ssize_t is)
{
  SC_ASSERT (is >= 0 && (size_t) is < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) is));
}

/** Remove the last element from an array and return a pointer to it.
 *
 * \return                The pointer to the removed object.  Will be valid
 *                        as long as no other function is called on this array.
 */
static inline void *
sc_array_pop (sc_array_t * array)
{
  SC_ASSERT (array->elem_count > 0);

  return (void *) (array->array + (array->elem_size * --array->elem_count));
}

/** Enlarge an array by one.  Grows the array memory if necessary.
 * \return Returns a pointer to the uninitialized newly added element.
 */
static inline void *
sc_array_push (sc_array_t * array)
{
  if (array->elem_size * (array->elem_count + 1) > array->byte_alloc) {
    const size_t        old_count = array->elem_count;

    sc_array_resize (array, old_count + 1);
    return (void *) (array->array + (array->elem_size * old_count));
  }

  return (void *) (array->array + (array->elem_size * array->elem_count++));
}

/** The sc_mempool object provides a large pool of equal-size elements.
 * The pool grows dynamically for element allocation.
 * Elements are referenced by their address which never changes.
 * Elements can be freed (that is, returned to the pool)
 *    and are transparently reused.
 */
typedef struct sc_mempool
{
  /* interface variables */
  size_t              elem_size;        /* size of a single element */
  size_t              elem_count;       /* number of valid elements */

  /* implementation variables */
  struct obstack      obstack;  /* holds the allocated elements */
  sc_array_t          freed;    /* buffers the freed elements */
}
sc_mempool_t;

/** Creates a new mempool structure.
 * \param [in] elem_size  Size of one element in bytes.
 * \return Returns an allocated and initialized memory pool.
 */
sc_mempool_t       *sc_mempool_new (size_t elem_size);

/** Destroys a mempool structure.
 * All elements that are still in use are invalidated.
 */
void                sc_mempool_destroy (sc_mempool_t * mempool);

/** Invalidates all previously returned pointers, resets count to 0.
 */
void                sc_mempool_truncate (sc_mempool_t * mempool);

/** Allocate a single element.
 * Elements previously returned to the pool are recycled.
 * \return Returns a new or recycled element pointer.
 */
/*@unused@*/
static inline void *
sc_mempool_alloc (sc_mempool_t * mempool)
{
  void               *ret;
  sc_array_t         *freed = &mempool->freed;

  ++mempool->elem_count;

  if (freed->elem_count > 0) {
    ret = *(void **) sc_array_pop (freed);
  }
  else {
    ret = obstack_alloc (&mempool->obstack, (int) mempool->elem_size);
  }

#ifdef SC_DEBUG
  memset (ret, -1, mempool->elem_size);
#endif

  return ret;
}

/** Return a previously allocated element to the pool.
 * \param [in] elem  The element to be returned to the pool.
 */
/*@unused@*/
static inline void
sc_mempool_free (sc_mempool_t * mempool, void *elem)
{
  sc_array_t         *freed = &mempool->freed;

  SC_ASSERT (mempool->elem_count > 0);

#ifdef SC_DEBUG
  memset (elem, -1, mempool->elem_size);
#endif

  --mempool->elem_count;

  *(void **) sc_array_push (freed) = elem;
}

/** The sc_link structure is one link of a linked list.
 */
typedef struct sc_link
{
  void               *data;
  struct sc_link     *next;
}
sc_link_t;

/** The sc_list object provides a linked list.
 */
typedef struct sc_list
{
  /* interface variables */
  size_t              elem_count;
  sc_link_t          *first;
  sc_link_t          *last;

  /* implementation variables */
  int                 allocator_owned;
  sc_mempool_t       *allocator;        /* must allocate sc_link_t */
}
sc_list_t;

/** Allocate a linked list structure.
 * \param [in] allocator Memory allocator for sc_link_t, can be NULL.
 */
sc_list_t          *sc_list_new (sc_mempool_t * allocator);

/** Destroy a linked list structure in O(N).
 * \note If allocator was provided in sc_list_new, it will not be destroyed.
 */
void                sc_list_destroy (sc_list_t * list);

/** Initializes an already allocated list structure.
 * \param [in,out]  list       List structure to be initialized.
 * \param [in]      allocator  External memory allocator for sc_link_t.
 */
void                sc_list_init (sc_list_t * list, sc_mempool_t * allocator);

/** Removes all elements from a list in O(N).
 * \param [in,out]  list       List structure to be resetted.
 * \note Calling sc_list_init, then any list operations,
 *       then sc_list_reset is memory neutral.
 */
void                sc_list_reset (sc_list_t * list);

/** Unliks all list elements without returning them to the mempool.
 * This runs in O(1) but is dangerous because of potential memory leaks.
 * \param [in,out]  list       List structure to be unlinked.
 */
void                sc_list_unlink (sc_list_t * list);

void                sc_list_prepend (sc_list_t * list, void *data);
void                sc_list_append (sc_list_t * list, void *data);

/** Insert an element after a given position.
 * \param [in] pred The predecessor of the element to be inserted.
 */
void                sc_list_insert (sc_list_t * list,
                                    sc_link_t * pred, void *data);

/** Remove an element after a given position.
 * \param [in] pred  The predecessor of the element to be removed.
                     If \a pred == NULL, the first element is removed.
 * \return Returns the data of the removed element.
 */
void               *sc_list_remove (sc_list_t * list, sc_link_t * pred);

/** Remove an element from the front of the list.
 * \return Returns the data of the removed first list element.
 */
void               *sc_list_pop (sc_list_t * list);

/** The sc_hash implements a hash table.
 * It uses an array which has linked lists as elements.
 */
typedef struct sc_hash
{
  /* interface variables */
  size_t              elem_count;       /* total number of objects contained */

  /* implementation variables */
  sc_array_t         *slots;    /* the slot count is slots->elem_count */
  void               *user_data;        /* user data passed to hash function */
  sc_hash_function_t  hash_fn;
  sc_equal_function_t equal_fn;
  size_t              resize_checks, resize_actions;
  int                 allocator_owned;
  sc_mempool_t       *allocator;        /* must allocate sc_link_t */
}
sc_hash_t;

/** Create a new hash table.
 * The number of hash slots is chosen dynamically.
 * \param [in] hash_fn     Function to compute the hash value.
 * \param [in] equal_fn    Function to test two objects for equality.
 * \param [in] user_data   User data passed through to the hash function.
 * \param [in] allocator   Memory allocator for sc_link_t, can be NULL.
 */
sc_hash_t          *sc_hash_new (sc_hash_function_t hash_fn,
                                 sc_equal_function_t equal_fn,
                                 void *user_data, sc_mempool_t * allocator);

/** Destroy a hash table.
 *
 * If the allocator is owned, this runs in O(1), otherwise in O(N).
 * \note If allocator was provided in sc_hash_new, it will not be destroyed.
 */
void                sc_hash_destroy (sc_hash_t * hash);

/** Remove all entries from a hash table in O(N).
 *
 * If the allocator is owned, it calls sc_hash_unlink and sc_mempool_truncate.
 * Otherwise, it calls sc_list_reset on every hash slot which is slower.
 */
void                sc_hash_truncate (sc_hash_t * hash);

/** Unlink all hash elements without returning them to the mempool.
 *
 * If the allocator is not owned, this runs faster than sc_hash_truncate,
 *    but is dangerous because of potential memory leaks.
 * \param [in,out]  hash       Hash structure to be unlinked.
 */
void                sc_hash_unlink (sc_hash_t * hash);

/** Same effect as unlink and destroy, but in O(1).
 * This is dangerous because of potential memory leaks.
 * \param [in]  hash       Hash structure to be unlinked and destroyed.
 */
void                sc_hash_unlink_destroy (sc_hash_t * hash);

/** Check if an object is contained in the hash table.
 * \param [in]  v      The object to be looked up.
 * \param [out] found  If found != NULL, *found is set to the address of the
 *                     pointer to the already contained object if the object
 *                     is found.  You can assign to **found to override.
 * \return Returns true if object is found, false otherwise.
 */
bool                sc_hash_lookup (sc_hash_t * hash, void *v, void ***found);

/** Insert an object into a hash table if it is not contained already.
 * \param [in]  v      The object to be inserted.
 * \param [out] found  If found != NULL, *found is set to the address of the
 *                     pointer to the already contained, or if not present,
 *                     the new object.  You can assign to **found to override.
 * \return Returns true if object is added, false if it is already contained.
 */
bool                sc_hash_insert_unique (sc_hash_t * hash, void *v,
                                           void ***found);

/** Remove an object from a hash table.
 * \param [in]  v      The object to be removed.
 * \param [out] found  If found != NULL, *found is set to the object
                       that is removed if that exists.
 * \return Returns true if object is found, false if is not contained.
 */
bool                sc_hash_remove (sc_hash_t * hash, void *v, void **found);

/** Compute and print statistical information about the occupancy.
 */
void                sc_hash_print_statistics (int log_priority,
                                              sc_hash_t * hash);

typedef struct sc_hash_array_data
{
  sc_array_t         *pa;
  sc_hash_function_t  hash_fn;
  sc_equal_function_t equal_fn;
  void               *user_data;
  void               *current_item;
}
sc_hash_array_data_t;

/** The sc_hash_array implements an array backed up by a hash table.
 * This enables O(1) access for array elements.
 */
typedef struct sc_hash_array
{
  /* implementation variables */
  sc_array_t          a;
  sc_hash_array_data_t internal_data;
  sc_hash_t          *h;
}
sc_hash_array_t;

/** Create a new hash array.
 * \param [in] elem_size   Size of one array element in bytes.
 * \param [in] hash_fn     Function to compute the hash value.
 * \param [in] equal_fn    Function to test two objects for equality.
 */
sc_hash_array_t    *sc_hash_array_new (size_t elem_size,
                                       sc_hash_function_t hash_fn,
                                       sc_equal_function_t equal_fn,
                                       void *user_data);

/** Destroy a hash array.
 */
void                sc_hash_array_destroy (sc_hash_array_t * hash_array);

/** Remove all elements from the hash array.
 * \param [in,out] hash_array   Hash array to truncate.
 */
void                sc_hash_array_truncate (sc_hash_array_t * hash_array);

/** Insert an object into a hash array if it is not contained already.
 * The object is not copied into the array.  Use the return value for that.
 * New objects are guaranteed to be added at the end of the array.
 *
 * \param [in]  v          A pointer to the object.  Used for search only.
 * \param [out] position   If position != NULL, *position is set to the
 *                         array position of the already contained, or if
 *                         not present, the new object.
 * \return                 Returns NULL if the object is already contained.
 *                         Otherwise returns its new address in the array.
 */
void               *sc_hash_array_insert_unique (sc_hash_array_t * hash_array,
                                                 void *v, size_t * position);

/** Extract the array data from a hash array and destroy everything else.
 * \param [in] hash_array   The hash array is destroyed after extraction.
 * \param [in] rip          Array structure that will be overwritten.
 *                          All previous array data (if any) will be leaked.
 *                          The filled array can be freed with sc_array_reset.
 */
void                sc_hash_array_rip (sc_hash_array_t * hash_array,
                                       sc_array_t * rip);

/** The sc_recycle_array object provides an array of slots that can be reused.
 *
 * It keeps a list of free slots in the array which will be used for insertion
 * while available.  Otherwise, the array is grown.
 */
typedef struct sc_recycle_array
{
  /* interface variables */
  size_t              elem_count;       /* number of valid entries */

  /* implementation variables */
  sc_array_t          a;
  sc_array_t          f;
}
sc_recycle_array_t;

/** Initialize a recycle array.
 *
 * \param [in] elem_size   Size of the objects to be stored in the array.
 */
void                sc_recycle_array_init (sc_recycle_array_t * rec_array,
                                           size_t elem_size);

/** Reset a recycle array.
 *
 * As with all _reset functions, calling _init, then any array operations,
 * then _reset is memory neutral.
 */
void                sc_recycle_array_reset (sc_recycle_array_t * rec_array);

/** Insert an object into the recycle array.
 * The object is not copied into the array.  Use the return value for that.
 *
 * \param [out] position   If position != NULL, *position is set to the
 *                         array position of the inserted object.
 * \return                 Returns the new address of the object in the array.
 */
void               *sc_recycle_array_insert (sc_recycle_array_t * rec_array,
                                             size_t * position);

/** Remove an object from the recycle array.  It must be valid.
 *
 * \param [in] position   Index into the array for the object to remove.
 * \return                The pointer to the removed object.  Will be valid
 *                        as long as no other function is called
 *                        on this recycle array.
 */
void               *sc_recycle_array_remove (sc_recycle_array_t * rec_array,
                                             size_t position);

#endif /* !SC_CONTAINERS_H */