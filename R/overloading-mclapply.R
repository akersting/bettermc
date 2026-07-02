#' Overload \code{\link[parallel:mclapply]{parallel::mclapply}} With
#' \code{\link[bettermc:mclapply]{bettermc::mclapply}}
#'
#' Enable the use of \code{\link[bettermc:mclapply]{bettermc::mclapply}} by
#' third-party packages originally using \code{mclapply} from the parallel
#' package, e.g. doMC or rstan. This is achieved by replacing the
#' \code{mclapply}-function in various environments.
#'
#' This feature violates CRAN policies and is hence only included in the
#' separate package \href{https://github.com/akersting/bettermcExt}{bettermcExt},
#' which you can install e.g. via
#' \preformatted{
#' remotes::install_github("akersting/bettermcExt", remotes::github_release())
#' }
#'
#' @name overloading-mclapply
NULL
