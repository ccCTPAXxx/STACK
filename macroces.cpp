//size_t hash_djb2(const StackValue *stack) {
//	size_t hash = 5381;
//	lli c;
//	while ((c = (size_t)*stack++)) {
//		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
//	}
//	return hash;
//}.
