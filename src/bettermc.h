#ifndef BETTERMC_H
#define BETTERMC_H

#define _POSIX_C_SOURCE 200112L

/* As of R 4.6.0 several long-available but non-API entry points are only
 * declared when ENABLE_LEGACY_NONAPI is defined (e.g. ATTRIB, SET_ATTRIB,
 * Rf_findVar). Defining it is harmless on older R, where the macro is simply
 * unused. See GitHub issue #9. */
#ifndef ENABLE_LEGACY_NONAPI
#define ENABLE_LEGACY_NONAPI
#endif

#include "Rinternals.h"

/* These two symbols are still exported by libR but are no longer declared in
 * Rinternals.h as of R 4.6.0: PRVALUE was removed entirely and the prototype
 * of Rf_allocVector3 was commented out. We declare them ourselves so that we
 * can keep using the custom-allocator (zero-copy shared memory) and
 * promise-inspection features. R_allocator_t is declared by Rinternals.h.
 * The #ifndef guard keeps this safe on older R where PRVALUE is a macro. */
#ifndef PRVALUE
extern SEXP PRVALUE(SEXP);
#endif
extern SEXP Rf_allocVector3(SEXPTYPE, R_xlen_t, R_allocator_t*);

SEXP copy2shm(SEXP, SEXP, SEXP, SEXP);
SEXP allocate_from_shm(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
SEXP unlink_all_shm(SEXP, SEXP);


SEXP is_altrep(SEXP);
SEXP is_allocated(SEXP);

SEXP char_map(SEXP);
SEXP char_map_long(SEXP);
SEXP set_attr(SEXP, SEXP);

SEXP semaphore_open(SEXP, SEXP, SEXP, SEXP);
SEXP semaphore_post(SEXP);
SEXP semaphore_wait(SEXP);
SEXP semaphore_close(SEXP);
SEXP semaphore_unlink(SEXP);
SEXP sigterm(SEXP);

SEXP semaphorev_open(SEXP);
SEXP semaphorev_post(SEXP, SEXP);
SEXP semaphorev_wait(SEXP, SEXP);
SEXP semaphorev_unlink(SEXP);

SEXP is_uneval_promise(SEXP, SEXP);
SEXP is_eval_promise_to_missing_arg(SEXP, SEXP);

SEXP set_timeout(SEXP, SEXP, SEXP);
SEXP disable_timeout(SEXP);

SEXP prio_queue_create(SEXP, SEXP, SEXP);
SEXP prio_queue_insert(SEXP, SEXP, SEXP);
SEXP prio_queue_release(SEXP);
SEXP prio_queue_destroy(SEXP, SEXP, SEXP);

#endif
