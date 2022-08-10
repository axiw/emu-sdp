#ifndef __LL_H__
#define __LL_H__

#include "ll.h"

struct LL {
	LL* next;
	LL* back;
};
typedef struct LL LL;

LL* ll_next(LL* cur);
LL* ll_back(LL* cur);
void ll_link_after(LL* root, LL* insert);
void ll_link_before(LL* root, LL* insert);
LL* ll_unlink(LL* cur);

#else

struct LL;
typedef struct LL LL;

#endif
