#ifndef INSTANCIA_MISC_ENTRY_H_
#define INSTANCIA_MISC_ENTRY_H_

typedef struct {
	int		id;
	size_t	size;
} entry_t;

entry_t*	entry_new(int id, size_t size);

bool		entry_is_atomic(entry_t* entry, size_t atomic_size);

void		entry_show(char* key, entry_t* entry);

void		entry_destroy(entry_t* entry);

#endif
