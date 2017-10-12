/*
 * File:   tlist.h
 *
 * Created on July 23, 2006, 8:10 PM
 *
 * from:
 * http://www.oracle.com/technetwork/server-storage/solaris10/c-type-137127.html#Implementing%20a%20List%20Using%20typeof%20and%20Statement%20Expressions|outline
 */
#ifndef _tlist_H
#define _tlist_H
#include <stdlib.h>
#define TLIST(T,L)      \
  struct _List##L {   \
    struct  _List##L * _next; \
    struct  _List##L * _prev; \
    T          _data; \
  } L = {&(L), &(L)}
#define TLISTDEF(T, TL) \
  typedef struct _List##TL {   \
    struct  _List##TL * _next; \
    struct  _List##TL * _prev; \
    T          _data; \
  } TL
#define TLISTINIT(L)\
  (L) = (typeof(L)){&(L), &(L)}
#define TLISTBEGIN(L) ((L)._next)
#define TLISTEND(L) (&(L))
#define TLISTITER(L)  typeof((L)._next)
#define TLISTINC(I) ((I)->_next)
#define TLISTDEC(I) ((I)->_prev)
#define TLISTREF(I) ((I)->_data)
#define TLISTINSERT(I, V)\
({\
    typeof(I) __tmp, __n, __p;\
    __tmp = (typeof(I)) malloc(sizeof(*(I)));\
    __n = (I);\
    __p = __n->_prev;\
    if (__tmp != 0) {\
      __tmp->_data = V;\
      __tmp->_next = __n;\
      __tmp->_prev = __p;\
      __p->_next = __tmp;\
      __n->_prev = __tmp;\
    };\
    __tmp;\
})
#define TLISTPUSHFRONT(L,V) TLISTINSERT(TLISTBEGIN(L), V)
#define TLISTPUSHBACK(L,V) TLISTINSERT(TLISTEND(L), V)
#define TLISTERASE(I)\
({\
    typeof(I) __pos, __n, __p;\
    __pos = (I);\
    __n = __pos->_next;\
    __p = __pos->_prev;\
    __p->_next = __n;\
    __n->_prev = __p;\
    free(__pos);\
    __n;\
})
#define TLISTCLEAR(L)\
({\
    TLISTITER(L) __c = TLISTBEGIN(L);\
    while (__c != TLISTEND(L))\
    {\
        TLISTITER(L) __tmp = __c;\
        __c = TLISTINC(__c);\
        free(__tmp);\
    }\
    (L)._next = (L)._prev = &(L);\
})
#define FOR_EACH(I, first, last, inc, blk)\
({\
    typeof(first) I;\
    for (I=first; I != last; I=inc(I))\
      blk;\
    I;\
})
#define TLISTFOREACH(I, L, blk)\
  FOR_EACH(I, TLISTBEGIN(L), TLISTEND(L), TLISTINC, blk)
#define TLISTSIZE(L)\
({\
    int __n = 0;\
    TLISTFOREACH(i, L, ({++__n;}));\
    __n;\
})
#define TLISTFIND(I, L, blk)\
  TLISTFOREACH(I, L, ({if (blk) break;}))
#endif  /* _tlist_H */
