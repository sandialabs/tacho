// clang-format off
/* =====================================================================================
Copyright 2022 National Technology & Engineering Solutions of Sandia, LLC (NTESS).
Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains
certain rights in this software.

https://github.com/sandialabs/tacho

SCR#:2790.0

This file is part of Tacho. Tacho is open source software: you can redistribute it
and/or modify it under the terms of BSD 2-Clause License
(https://opensource.org/licenses/BSD-2-Clause). A copy of the licese is also
provided under the main directory

Questions? Kyungjoo Kim at <kyukim@sandia.gov,https://github.com/kyungjoo-kim>

Sandia National Laboratories, Albuquerque, NM, USA
===================================================================================== */
// clang-format on
#include "Tacho_CommandLineParser.hpp"
#include "Tacho_ExampleDeviceDenseCholesky.hpp"
#include "Tacho_ExampleDeviceDenseLDL.hpp"

int main(int argc, char *argv[]) {
  Tacho::CommandLineParser opts("This example program measure the Tacho on Kokkos::OpenMP");

  int m = 10;
  bool verbose = false;
  bool test_chol = false;
  bool test_ldl = false;
  // bool test_lu = false;

  opts.set_option<int>("m", "Dense problem size", &m);
  opts.set_option<bool>("verbose", "Flag for verbose printing", &verbose);
  opts.set_option<bool>("test-chol", "Flag for testing Cholesky", &test_chol);
  opts.set_option<bool>("test-ldl", "Flag for testing LDL", &test_ldl);
  //  opts.set_option<bool>("test-lu", "Flag for testing LU", &test_lu);

  const bool r_parse = opts.parse(argc, argv);
  if (r_parse)
    return 0; // print help return

  int r_val(0);
  Kokkos::initialize(argc, argv);
  {
    if (test_chol) {
      const int r_val_chol = driver_chol<double>(m, verbose);
      r_val += r_val_chol;
    }
    if (test_ldl) {
      const int r_val_ldl = driver_ldl<double>(m, verbose);
      r_val += r_val_ldl;
    }
  }
  Kokkos::finalize();

  return r_val;
}
