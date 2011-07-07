#define volatileAccess(v) *((volatile typeof((v)) *) &(v))
