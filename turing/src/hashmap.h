/***************************************************************************/
#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

/* forward declaration of opaque structure */
typedef struct sHashmap hashmap;

#define HASHMAP_ILLEGAL 0   
#define HASHMAP_INSERT 1    
#define HASHMAP_UPDATE 2    
typedef void(*fHashmapProc)(const char* key, const void* datum);

extern hashmap* newHashmap(unsigned int hint);

extern void deleteHashmap(hashmap* map);

extern int hashmapSet(hashmap* map, void* data, const char* key);

extern void* hashmapGet(const hashmap* map, const char* key);

extern void* hashmapRemove(hashmap* map, const char* key);

extern void hashmapProcess(const hashmap* map, fHashmapProc proc);

#endif