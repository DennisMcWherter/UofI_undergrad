/*
 * CS 241
 * The University of Illinois
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MEM_ALIGN (sizeof(int)/2)
#define _mem_metasize (sizeof(void*)+sizeof(size_t))

static void* _freeBase = NULL;

void *calloc(size_t nmemb, size_t size)
{
	void *ptr = malloc(nmemb * size);
	
	if (ptr)
		memset(ptr, 0x00, nmemb * size);

	return ptr;
}

void *malloc(size_t size)
{
  static unsigned long lastptr = 0;
  void* data = _freeBase;
  void* prev = NULL;
  size_t oldSize = 0;

  if(size == 0) /* malloc(0) == NULL */
    return NULL;

  /* Make sure we keep memory half_word-aligned */
  size += _mem_metasize;
  if(size % MEM_ALIGN)
    size = size + (MEM_ALIGN - (size % MEM_ALIGN));
  
  /* Find valid memory chunk if one exists */
  while(data != NULL) {
    if((oldSize = *((size_t*)(data+sizeof(void*)))) >= size) {
      size = oldSize;
      if(prev != NULL)
        *(unsigned long*)prev = *((unsigned long*)data);
      if(_freeBase == data)
        _freeBase = *((void**)data);
      break;
    }

    prev = data;
    data = *((void**)data);
  }
 
  /* If no chunk found, then make one */
  if(data == NULL)  
    if((data = sbrk(size)) == (void*)-1)
      return NULL; /* Error */
    //else /* Make sure memory initialized at least the first time */
     // memset(data, 0x00, size);

  /* Pack our data */
  *(unsigned long*)data = lastptr | 0x01;
  *(size_t*)(data+sizeof(void*)) = size;

  lastptr = (unsigned long)data;

  return data+_mem_metasize;
}

void free(void *ptr)
{
  size_t size = 0;

  /*
	 * According to the C standard:
	 *   If a null pointer is passed as argument, no action occurs.
	 */
	if (!ptr)
		return;

  ptr -= _mem_metasize;
  
  size = *((size_t*)(ptr+sizeof(void*)));

  if(!(*((unsigned long*)ptr) & 0x01))
    return;
  
  /* Clear valid bit */
  *((unsigned long*)ptr) &= ~0x01;

  /* Add memory to our free list */
  *((void**)ptr) = _freeBase;
  _freeBase = ptr;

  return;
}

void *realloc(void *ptr, size_t size)
{
  size_t value = 0;
  void* data = NULL;
	
  /*
	 * According to the C standard:
	 *   In case that ptr is NULL, the function behaves exactly as malloc,
	 *   assigning a new block of size bytes and returning a pointer to
	 *   the beginning of it.
	 */
	if (!ptr)
		return malloc(size);

	/*
	 * According to the C standard:
	 *   In case that the size is 0, the memory previously allocated in ptr
	 *   is deallocated as if a call to free was made, and a NULL pointer
	 *   is returned.
	 */
	if (!size)
	{
		free(ptr);
		return NULL;
	}
  
  ptr -= _mem_metasize;

  value = *((size_t*)(ptr+sizeof(void*)));

  if(value >= size+_mem_metasize) /* A little wasteful, but if we have enough use it */
    return ptr+_mem_metasize;

  data = malloc(size);

  if(data == NULL)
    return NULL;

  memcpy(data, ptr+_mem_metasize, value-_mem_metasize);
  
  free(ptr);

	return data;
}

