#include <stddef.h>
#include "ll.h"

LL* ll_next(LL* cur) {
	if (cur)
		return cur->next;
	return NULL;
}

LL* ll_back(LL* cur) {
	if (cur)
		return cur->back;
	return NULL;
}

void ll_link_after(LL* root, LL* insert) {
	if (root == NULL || insert == NULL)
		return;
	
	insert->next = root->next;
	insert->back = root;
	
	if (root->next)
		root->next->back = insert;
	root->next = insert;
}

void ll_link_before(LL* root, LL* insert) {
	if (root == NULL || insert == NULL)
		return;
	
	insert->next = root;
	insert->back = root->back;
	
	if (root->back)
		root->back->next = insert;
	root->back = insert;
}

// Unlinks the current ll and returns a pointer to the furthest back ll.
// Return value is useful when you are removing the root of a ll.
LL* ll_unlink(LL* cur) {
	if (cur == NULL)
		return NULL;
	
	if (cur->back)
		cur->back->next = cur->next;
	if (cur->next)
		cur->next->back = cur->back;
	
	if (cur->back)
		return cur->back;
	return cur->next;
}
