# bettermc

[![R build
status](https://github.com/akersting/bettermc/workflows/R-CMD-check/badge.svg)](https://github.com/akersting/bettermc/actions?workflow=R-CMD-check)

The `bettermc` package provides a wrapper around the
`parallel::mclapply` function for better performance, error handling,
seeding and UX.

## Installation of the Development Version

``` r
# install.packages("remotes")
remotes::install_github("akersting/bettermc")
```

## Supported Platforms

`bettermc` was originally developed for 64-bit Linux. By now it should
also compile and run on 32-bit systems, and on macOS and Solaris.
However, as stated in the respective help pages, not all features are
supported on macOS. Porting to other POSIX-compliant Unix flavors should
be fairly straightforward. The Windows support is very limited and
mainly provided for compatibility reasons only, i.e. to allow the
*serial* execution of code using `bettermc::mclapply`, which was
originally developed for Linux or macOS.

## Features

Here is a short overview on its main features …

### Progress Bar

<figure>
<img src="progress.png" alt="progress bar" />
<figcaption aria-hidden="true">progress bar</figcaption>
</figure>

### Error Handling, Tracebacks and Crashdumps

By default, crashdumps and full tracebacks are generated on errors in
child processes:

``` r
g <- function(x) x + 1
f <- function(x) g(as.character(x))
bettermc::mclapply(1:2, f, mc.dumpto = "last.dump")
```

    ## crash dump saved to object 'last.dump' in environment 'bettermc::crash_dumps'; for debugging the first error, use:
    ##   'utils::debugger(attr(bettermc::crash_dumps[["last.dump"]][[1]], "dump.frames"))'

    ## 
    ## Error: non-numeric argument to binary operator
    ##   
    ##  Traceback with variables:
    ##   57: g(as.character(x))
    ##       
    ##       Local variables:
    ##         x :  chr "1"
    ##   56: FUN(X, ...) at mclapply.R#683
    ##       
    ...

    ## Error in bettermc::mclapply(1:2, f, mc.dumpto = "last.dump"): error(s) occured during mclapply; first original message:
    ##   
    ##   Error: non-numeric argument to binary operator

``` r
# in an interactive session use debugger() instead of print() for actual debugging
print(attr(bettermc::crash_dumps[["last.dump"]][[1]], "dump.frames"))
```

    ## $`mclapply.R#683: etry(withCallingHandlers(list(FUN(X, ...)), warning = whand`
    ## <environment: 0x55699569f828>
    ## 
    ## $`etry.R#41: tryCatch(withCallingHandlers(expr, error = function(e) {\n    if (`
    ## <environment: 0x55699569f908>
    ## 
    ## $`tryCatchList(expr, classes, parentenv, handlers)`
    ## <environment: 0x55699554f7e0>
    ## 
    ## $`tryCatchOne(expr, names, parentenv, handlers[[1]])`
    ## <environment: 0x556995541988>
    ## 
    ## $`doTryCatch(return(expr), name, parentenv, handler)`
    ## <environment: 0x5569955178d0>
    ## 
    ## $`etry.R#41: withCallingHandlers(expr, error = function(e) {\n    if ("max.line`
    ## <environment: 0x5569955117b8>
    ## 
    ## $`mclapply.R#683: withCallingHandlers(list(FUN(X, ...)), warning = whandler, `
    ## <environment: 0x5569951c14c0>
    ## 
    ## $`mclapply.R#683: FUN(X, ...)`
    ## <environment: 0x556994f2ccf8>
    ## 
    ## $`g(as.character(x))`
    ## <environment: 0x556994f2cf60>
    ## 
    ## attr(,"error.message")
    ## [1] "non-numeric argument to binary operator\n\n"
    ## attr(,"class")
    ## [1] "dump.frames"

As shown in the example above, `bettermc` by default fails if there are
errors in child processes. This behavior can be changed to merely warn
about both fatal and non-fatal error:

``` r
ret <- bettermc::mclapply(1:4, function(i) {
  if (i == 1L)
    stop(i)
  else if (i == 4L)
    system(paste0("kill ", Sys.getpid()))
  NULL
}, mc.allow.fatal = TRUE, mc.allow.error = TRUE, mc.preschedule = FALSE)
```

    ## Warning in bettermc::mclapply(1:4, function(i) {: at least one scheduled core
    ## did not return results; maybe it was killed (by the Linux Out of Memory Killer
    ## ?) or there was a fatal error in the forked process(es)

    ## Warning in bettermc::mclapply(1:4, function(i) {: error(s) occured during mclapply; first original message:
    ##   
    ##   Error: 1

Also in this case, full tracebacks and crash dumps are available:

``` r
stopifnot(inherits(ret[[1]], "try-error"))
names(attributes(ret[[1L]]))
```

    ## [1] "class"       "condition"   "traceback"   "locals"      "dump.frames"

Additionally, results affected by fatal errors are clearly indicated and
can be differentiated from legitimate `NULL` values:

``` r
lapply(ret, class)
```

    ## [[1]]
    ## [1] "etry-error" "try-error" 
    ## 
    ## [[2]]
    ## [1] "NULL"
    ## 
    ## [[3]]
    ## [1] "NULL"
    ## 
    ## [[4]]
    ## [1] "fatal-error" "try-error"

You can use `mc.allow.fatal = NULL` to instead return `NULL` on fatal
errors as does `parallel::mclapply`.

### Output, Messages and Warnings

In contrast to `parallel::mclapply`, neither output nor messages nor
warnings from the child processes are lost. All of these can be
forwarded to the parent process and are prefixed with the index of the
function invocation from which they originate:

``` r
f <- function(i) {
  if (i == 1) message(letters[i])
  else if (i == 2) warning(letters[i])
  else print(letters[i])
  
  i
}
ret <- bettermc::mclapply(1:4, f)
```

    ## 0/3: [1] "c"
    ## 0/4: [1] "d"

    ## Warning in FUN(X, ...): 0/2: b

    ## 0/1: a

Similarly, other conditions can also be re-signaled in the parent
process.

### Reproducible Seeding

By default, `bettermc` reproducibly seeds all function calls:

``` r
set.seed(538)
a <- bettermc::mclapply(1:4, function(i) runif(1), mc.cores = 3)
set.seed(538)
b <- bettermc::mclapply(1:4, function(i) runif(1), mc.cores = 1)
a
```

    ## [[1]]
    ## [1] 0.02134061
    ## 
    ## [[2]]
    ## [1] 0.7456995
    ## 
    ## [[3]]
    ## [1] 0.4223595
    ## 
    ## [[4]]
    ## [1] 0.6265811

``` r
stopifnot(identical(a, b))
```

Compare with `parallel`:

``` r
set.seed(594)
a <- parallel::mclapply(1:4, function(i) runif(1), mc.cores = 3)
set.seed(594)
b <- parallel::mclapply(1:4, function(i) runif(1), mc.cores = 3)
stopifnot(identical(a, b))
```

    ## Error: identical(a, b) is not TRUE

### POSIX Shared Memory

Many types of objects can be returned from the child processes using
POSIX shared memory. This includes e.g. logical, numeric, complex and
raw vectors and arrays as well as factors. In doing so, the overhead of
getting larger results back into the parent R process is reduced:

``` r
X <- data.frame(
  x = runif(3e7),
  y = sample(c(TRUE, FALSE), 3e7, TRUE),
  z = 1:3e7
)
f <- function(i) X

microbenchmark::microbenchmark(
  bettermc1 = bettermc::mclapply(1:2, f, mc.share.copy = FALSE),
  bettermc2 = bettermc::mclapply(1:2, f),
  bettermc3 = bettermc::mclapply(1:2, f, mc.share.vectors = FALSE),
  bettermc4 = bettermc::mclapply(1:2, f, mc.share.vectors = FALSE, mc.shm.ipc = FALSE),
  parallel = parallel::mclapply(1:2, f),
  times = 10, setup = gc()
)
```

    ## Unit: milliseconds
    ##       expr       min        lq      mean    median        uq       max neval
    ##  bettermc1  258.2423  260.8886  266.0044  265.3691  270.6897  278.9562    10
    ##  bettermc2  554.0544  554.9732  563.5756  562.0964  567.1360  588.7047    10
    ##  bettermc3  997.2268 1081.1928 1100.8035 1100.4903 1125.7399 1184.1736    10
    ##  bettermc4 1183.1943 1207.1514 1363.6663 1278.0570 1562.7704 1636.4377    10
    ##   parallel 1272.5414 1417.2813 1574.7550 1483.6165 1548.4324 2474.5669    10

In examples `bettermc1` and `bettermc2`, the child processes place the
columns of the return value `X` in shared memory. The object which needs
to be serialized for transfer from child to parent processes hence
becomes:

``` r
X_shm <- bettermc:::vectors2shm(X, name_prefix = "/bettermc_README_")
str(X_shm)
```

    ## 'data.frame':    30000000 obs. of  3 variables:
    ##  $ x:List of 6
    ##   ..$ name      : chr "/bettermc_README_1"
    ##   ..$ type      : int 14
    ##   ..$ length    : num 3e+07
    ##   ..$ size      : num 2.4e+08
    ##   ..$ attributes: NULL
    ##   ..$ copy      : logi TRUE
    ##   ..- attr(*, "class")= chr "shm_obj"
    ##  $ y:List of 6
    ##   ..$ name      : chr "/bettermc_README_2"
    ##   ..$ type      : int 10
    ##   ..$ length    : num 3e+07
    ##   ..$ size      : num 1.2e+08
    ##   ..$ attributes: NULL
    ##   ..$ copy      : logi TRUE
    ##   ..- attr(*, "class")= chr "shm_obj"
    ##  $ z: int  1 2 3 4 5 6 7 8 9 10 ...

Column `z` is an `ALTREP` and, because it can be serialized efficiently,
is left alone by default. The parent process can recover the original
object from `X_shm`:

``` r
Y <- bettermc:::shm2vectors(X_shm)
stopifnot(identical(X, Y))
```

The internal functions `vectors2shm()` and `shm2vectors()` recursively
walk the return value and apply the exported functions `copy2shm()` and
`allocate_from_shm()`, respectively.

In `bettermc1`, the shared memory objects are used directly by the
parent process. In `bettermc2`, which is the default, new vectors are
allocated in the parent process and the data is merely copied from the
shared memory objects, which are freed afterwards. See `?copy2shm` for
more details on this topic and why the slower `mc.share.copy = TRUE`
might be a sensible default.

In `bettermc3`, the original `X` is serialized and the resulting raw
vector is placed in shared memory, from where it is deserialized in the
parent process.

`bettermc4` does not involve any POSIX shared memory and hence is
equivalent to `parallel`, i.e. the original `X` is serialized and
transferred to the parent process using pipes.

### Character Compression

In practice, character vectors often contain a substantial amount of
duplicated values. This is exploited by `bettermc` to speed up the
returning of larger character vectors from child processes:

``` r
X <- rep(as.character(runif(1e6)), 30)
f <- function(i) X
microbenchmark::microbenchmark(
  bettermc1 = bettermc::mclapply(1:2, f),
  parallel =  parallel::mclapply(1:2, f),
  times = 1, setup = gc()
)
```

    ## Unit: seconds
    ##       expr       min        lq      mean    median        uq       max neval
    ##  bettermc1  3.477804  3.477804  3.477804  3.477804  3.477804  3.477804     1
    ##   parallel 28.348915 28.348915 28.348915 28.348915 28.348915 28.348915     1

By default, `bettermc` replaces character vectors with objects of type
`char_map` before returning them to the parent process:

``` r
X_comp <- bettermc::compress_chars(X)
str(X_comp)
```

    ## List of 3
    ##  $ chars     : chr [1:999882] "0.691763624548912" "0.428920124191791" "0.944636366795748" "0.9647259155754" ...
    ##  $ idx       : int [1:30000000] 198507 198506 198504 198503 198468 199015 199048 199049 199010 198505 ...
    ##  $ attributes: NULL
    ##  - attr(*, "class")= chr "char_map"

The important detail here is the length of the `chars` vector, which
just contains the unique elements of `X` and hence is significantly
faster to (de)serialize than the original vector. The parent process can
recover the original character vectors:

``` r
Y <- bettermc::uncompress_chars(X_comp)
stopifnot(identical(X, Y))
```

The functions `compress_chars()` and `uncompress_chars()` recursively
walk the return value and apply the functions `char_map()` and
`map2char()`, respectively.

`char_map()` is implemented using a radix sort, which makes it very
efficient:

``` r
microbenchmark::microbenchmark(
  char_map = bettermc::char_map(X),
  match = {chars <- unique(X); idx <- match(X, chars)},
  times = 3, setup = gc()
)
```

    ## Unit: milliseconds
    ##      expr       min        lq      mean    median        uq       max neval
    ##  char_map  914.0587  915.9847  923.4248  917.9106  928.1079  938.3052     3
    ##     match 4512.7746 4582.5145 4614.0304 4652.2544 4664.6583 4677.0622     3

### Retries

`bettermc` supports automatic retries on both fatal and non-fatal
errors. `mc.force.fork` ensures that `FUN` is called in a child process,
even if `X` is of length 1. This is useful if `FUN` might encounter a
fatal error and we want to protect the parent process against it. With
retires, `length(X)` might drop to 1 if all other values could already
be processed. This is also why we need `mc.force.fork` in the following
example:

``` r
set.seed(456)
res <-
  bettermc::mclapply(1:20, function(i) {
    r <- runif(1)
    if (r < 0.25)
      system(paste0("kill ", Sys.getpid()))
    else if (r < 0.5)
      stop(i)
    else
      i
  }, mc.retry = 50, mc.cores = 10, mc.force.fork = TRUE)
```

    ## 0: at least one scheduled core did not return results; maybe it was killed (by the Linux Out of Memory Killer ?) or there was a fatal error in the forked process(es)

    ## 0: error(s) occured during mclapply; first original message:
    ##   
    ##   
    ## Error: 5
    ##   
    ##  Traceback with variables:
    ##   57: stop(i)
    ##       
    ##       Local variables:
    ##         ..1 : <promise> (i)
    ...

    ## 1: at least one scheduled core did not return results; maybe it was killed (by the Linux Out of Memory Killer ?) or there was a fatal error in the forked process(es)

    ## 1: error(s) occured during mclapply; first original message:
    ##   
    ##   
    ## Error: 20
    ##   
    ##  Traceback with variables:
    ##   57: stop(i)
    ##       
    ##       Local variables:
    ##         ..1 : <promise> (i)
    ...

    ## 2: error(s) occured during mclapply; first original message:
    ##   
    ##   
    ## Error: 2
    ##   
    ##  Traceback with variables:
    ##   57: stop(i)
    ##       
    ##       Local variables:
    ##         ..1 : <promise> (i)
    ...

    ## 3: error(s) occured during mclapply; first original message:
    ##   
    ##   
    ## Error: 12
    ##   
    ##  Traceback with variables:
    ##   57: stop(i)
    ##       
    ##       Local variables:
    ##         ..1 : <promise> (i)
    ...

``` r
stopifnot(identical(res, as.list(1:20)))
```

Additionally, it is possible to automatically decrease the number of
cores with every retry by specifying a negative value for `mc.retry`.
This is useful if we expect failures to be caused simply by too many
concurrent processes, e.g. if system load or the size of input data is
unpredictable and might lead to the Linux Out Of Memory Killer stepping
in. In such a case it makes sense to retry using fewer concurrent
processes:

``` r
ppid <- Sys.getpid()
res <-
  bettermc::mclapply(1:20, function(i) {
    Sys.sleep(0.25)  # wait for the other child processes
    number_of_child_processes <- length(system(paste0("pgrep -P ", ppid), intern = TRUE))
    if (number_of_child_processes >= 5) system(paste0("kill ", Sys.getpid()))
    i
  }, mc.retry = -3, mc.cores = 10, mc.force.fork = TRUE)
```

    ## 0: at least one scheduled core did not return results; maybe it was killed (by the Linux Out of Memory Killer ?) or there was a fatal error in the forked process(es)

    ## 1: at least one scheduled core did not return results; maybe it was killed (by the Linux Out of Memory Killer ?) or there was a fatal error in the forked process(es)

``` r
stopifnot(identical(res, as.list(1:20)))
```

If there are still errors after the retries, we regularly fail:

``` r
set.seed(123)
res <-
  bettermc::mclapply(1:20, function(i) {
    r <- runif(1)
    if (r < 0.25)
      system(paste0("kill ", Sys.getpid()))
    else if (r < 0.5)
      stop(i)
    else
      i
  }, mc.retry = 1, mc.cores = 10, mc.force.fork = TRUE, mc.dumpto = "last.dump")
```

    ## 0: at least one scheduled core did not return results; maybe it was killed (by the Linux Out of Memory Killer ?) or there was a fatal error in the forked process(es)

    ## 0: error(s) occured during mclapply; first original message:
    ##   
    ##   
    ## Error: 3
    ##   
    ##  Traceback with variables:
    ##   57: stop(i)
    ##       
    ##       Local variables:
    ##         ..1 : <promise> (i)
    ...

    ## crash dump saved to object 'last.dump' in environment 'bettermc::crash_dumps'; for debugging the first error, use:
    ##   'utils::debugger(attr(bettermc::crash_dumps[["last.dump"]][[3]], "dump.frames"))'

    ## 
    ## Error: 3
    ##   
    ##  Traceback with variables:
    ##   57: stop(i)
    ##       
    ##       Local variables:
    ##         ..1 : <promise> (i)
    ##         call. :  logi TRUE
    ##         domain :  NULL
    ...

    ## Error in bettermc::mclapply(1:20, function(i) {: at least one scheduled core did not return results; maybe it was killed (by the Linux Out of Memory Killer ?) or there was a fatal error in the forked process(es)
    ##   
    ##   --- AND ---
    ##   
    ##   error(s) occured during mclapply; first original message:
    ##   
    ##   Error: 3

### Timeouts

`bettermc` can kill child processes after a certain time has elapsed:

``` r
bettermc::mclapply(X = 1:4, function(i) {
  Sys.sleep(i * 2)
  i
},
  mc.preschedule = FALSE, mc.allow.fatal = NA,
  mc.timeout.elapsed = 5, mc.force.fork = TRUE
)
```

    ## [[1]]
    ## [1] 1
    ## 
    ## [[2]]
    ## [1] 2
    ## 
    ## [[3]]
    ## [1] "child process did not return any results"
    ## attr(,"class")
    ## [1] "fatal-error" "try-error"  
    ## attr(,"condition")
    ## <simpleError: child process did not return any results>
    ## 
    ## [[4]]
    ## [1] "child process did not return any results"
    ## attr(,"class")
    ## [1] "fatal-error" "try-error"  
    ## attr(,"condition")
    ## <simpleError: child process did not return any results>

It is recommended to use `mc.force.fork = TRUE` to not accidentally kill
the main R process if `X` is of length 1.

In contrast to `base::setTimeLimit()` and hence
`R.utils::withTimeout()`, it does *not* suffer from the following
limitation:

> Time limits are checked whenever a user interrupt could occur. This
> will happen frequently in R code and during Sys.sleep(\*), but only at
> points in compiled C and Fortran code identified by the code author.
