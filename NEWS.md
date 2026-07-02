# bettermc 1.2.2.9000

## New Features
* new arguments `mc.timeout.elapsed` and `mc.timeout.cpu` to `mclapply()` (Linux
only): impose a limit on the elapsed/CPU time of each invocation of `FUN`. On
timeout, the signal given by the new argument `mc.timeout.signal` (one of
SIGTERM, SIGKILL, SIGABRT, SIGINT or any numeric signal) is sent to the child
process; depending on the signal this results in a fatal error (cf.
`mc.allow.fatal`) or, for SIGINT, a non-fatal error (cf. `mc.allow.error`)
* support for priority queues to control the *total* number and order of running
child processes in settings of (complex) nested parallelization: new
(experimental) exported function `prio_queue_create()` (together with
`prio_queue_insert()`, `prio_queue_release()` and `prio_queue_destroy()`) and
new arguments `mc.prio.queue` and `mc.priority` to `mclapply()` (not supported on
Windows)
* new exported function `gc_reset_triggers()`, which repeatedly triggers full
garbage collections until the GC trigger thresholds no longer decrease; this
helps to keep the memory usage of fork-based parallelization in check after
removing huge objects. The new argument `mc.gc.reset.triggers` to `mclapply()`
(default `FALSE`) calls it before forking

## Bug Fixes
* avoid truncation of long tracebacks from `etry()` (and hence also from
`mclapply()`) by printing them as a separate message
* fix installation on R (>= 4.6.0): several non-API entry points used by the C
code (`ATTRIB`, `SET_ATTRIB`, `Rf_findVar`, `DATAPTR`, `PRVALUE` and
`Rf_allocVector3`) are no longer declared in `Rinternals.h`. The package now
enables R's `ENABLE_LEGACY_NONAPI` compatibility flag, replaces `DATAPTR` with
the API-stable `DATAPTR_RO`/typed accessors, and itself declares the two
remaining symbols which are still exported by R but no longer in any header
(`PRVALUE` and `Rf_allocVector3`) (#9)

# bettermc 1.2.2

## Bug Fixes
* avoid the potential failure of a test of the `mc.system.time` argument and also skip other related tests, which are sensitive to timings, on CRAN

# bettermc 1.2.1

## New Features
* add support for overloading `parallel::mclapply()` with `bettermc::mclapply()` via the [bettermcExt](https://github.com/akersting/bettermcExt)-package on GitHub;
this enables the use of `bettermc::mclapply()` by third-party packages originally using `mclapply()` from the *parallel*-package, e.g. *doMC* or *rstan*
* when applying over a character vector, that vector is now - by default - used to name the (otherwise unnamed) result (argument `mc.use.names`)
* tracebacks from `etry()` (and hence also from `mclapply()`) now contain an overview of the local variables and their values (essentially the output of `ls.str()` applied to all the frames on the call stack)
* new argument `mc.system.time` to `mclapply()`: if `TRUE` measure and return the CPU (and other) times used by the invocations of `FUN`
* the formal arguments `mc.allow.fatal` and `mc.allow.error` to `mclapply()` gain a new valid value: `NA`, which is the same as `TRUE`, but suppresses the warnings which would normally signal that there was a (fatal) error

## Bug Fixes
* fix test failure if suggested *progress*-package is not available
* avoid progress bar job blocking main process

## Misc
* make the format of this very file *NEWS.md* compatible with *news()*
* avoid a superfluous clang -Wuninitialized as requested by CRAN
* avoid a CRAN warning regarding "unlikely file names for src files"

# bettermc 1.1.2

## Misc
* work around an issue in base R w.r.t. source references and lazy loading as discussed in <https://stat.ethz.ch/pipermail/r-devel/2021-April/080599.html>
* prepare for upcoming changes to *Rinternals.h*
  * do no longer assign to `ATTRIB()` and `OBJECT()`
  * do no longer depend on `SEXPREC_ALIGN`

# bettermc 1.1.1

## New Features

### Windows Support
This package can now also be installed on Windows, where it offers a reduced set of features.
Most notably, `bettermc::mclapply()` (just like `parallel::mclapply()`) falls back on serial execution, since forking is not available on Windows.
Still, this allows code employing `bettermc::mclapply()`, which was originally developed for Linux or macOS, to be run on Windows.

### Seeding
* restore the state of the random number generator (RNG) at the end of `mclapply()` to what it was before; this makes the RNG state of the parent process immune to changes to arguments such as `mc.cores` or `mc.force.fork`
* new argument `mc.retry.fixed.seed` to `mclapply()`: `TRUE` invokes `FUN()` for a particular element of `X` with the same fixed seed on every retry; default is `FALSE`

### Misc
* the environment variable "BMC_RETRY" now indicates the current retry ("0" means first try, a value of "1" first *re*try, etc.)
* new argument `mc.retry.silent` to `mclapply()`: `TRUE` suppresses the messages indicating failures during intermediate retires; default is `FALSE`

## Bug Fixes
* on retires, output, messages etc. might have been prefixed with the wrong index w.r.t. `X`
* on the first call of `mclapply()` in a session, the environment variable MC_CORES was not respected
* fix wrong length of affinity.list if `mc.force.fork == TRUE && length(X) == 1`
* fix tiny memory leaks reported by Valgrind in `allocate_from_shm()`
* fix invalid write reported by Valgrind in `allocate_from_shm()`
* mark memory returned by custom allocator in `allocate_from_shm()` for Valgrind as defined (cf. <https://stat.ethz.ch/pipermail/r-devel/2021-March/080572.html>)
* fix test failure on macOS with a page size of more than 4 KiB

# bettermc 1.0.1
* initial CRAN release
