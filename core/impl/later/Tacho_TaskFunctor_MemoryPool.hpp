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
#ifndef __TACHO_TASKFUNCTOR_MEMORYPOOL_HPP__
#define __TACHO_TASKFUNCTOR_MEMORYPOOL_HPP__

/// \file Tacho_TaskFunctor_MemoryPool.hpp
/// \author Kyungjoo Kim (kyukim@sandia.gov)

#include "Tacho_Util.hpp"

namespace Tacho {

    template<typename SchedulerType>
    struct TaskFunctor_MemoryPool_Allocate {
      typedef SchedulerType scheduler_type;
      typedef typename scheduler_type::member_type member_type;

      typedef typename UseThisDevice<typename scheduler_type::execution_space>::type device_type;                                                                                                           

      typedef void* value_type; // functor return type
      typedef Kokkos::BasicFuture<void*,scheduler_type> future_type;
      
      // memory pool
      typedef Kokkos::MemoryPool<device_type> memory_pool_type;
      
    private:
      memory_pool_type _pool;
      size_type _bufsize;

    public:

      KOKKOS_INLINE_FUNCTION
      TaskFunctor_MemoryPool_Allocate() = delete;

      KOKKOS_INLINE_FUNCTION
      TaskFunctor_MemoryPool_Allocate(const memory_pool_type &pool,
                                      const size_type bufsize)
        : _pool(pool),
          _bufsize(bufsize) {}

      KOKKOS_INLINE_FUNCTION
      void operator()(member_type &member, value_type &r_val) {
        Kokkos::single(Kokkos::PerTeam(member), [&]() {
            if (_bufsize)
              r_val = (void*)_pool.allocate(_bufsize);
            else
              r_val = NULL;
          });
      }
    };
    
    template<typename SchedulerType>
    struct TaskFunctor_MemoryPool_Deallocate {
      typedef SchedulerType scheduler_type;
      typedef typename scheduler_type::member_type member_type;

      typedef typename UseThisDevice<typename scheduler_type::execution_space>::type device_type;                                                                                                           
      
      typedef void value_type; // functor return type
      typedef Kokkos::BasicFuture<void,scheduler_type> future_type;
      
      // memory pool
      typedef Kokkos::MemoryPool<device_type> memory_pool_type;
      typedef Kokkos::BasicFuture<void*,scheduler_type> future_ptr_type;
      
    private:
      memory_pool_type _pool;
      future_ptr_type _ptr;
      size_type _bufsize;

    public:

      KOKKOS_INLINE_FUNCTION
      TaskFunctor_MemoryPool_Deallocate() = delete;

      KOKKOS_INLINE_FUNCTION
      TaskFunctor_MemoryPool_Deallocate(const memory_pool_type &pool,
                                        const future_ptr_type &ptr,
                                        const size_type bufsize)
        : _pool(pool),
          _ptr(ptr),
          _bufsize(bufsize) {}

      KOKKOS_INLINE_FUNCTION
      void operator()(member_type &member) {
        Kokkos::single(Kokkos::PerTeam(member), [&]() {
            // if (_bufsize)
            //   _pool.deallocate((void*)_ptr.get(), _bufsize);
          });
      }
    };

    template<typename SchedulerType>
    struct TaskFunctor_MemoryPool_TestView {
      typedef SchedulerType scheduler_type;
      typedef typename scheduler_type::member_type member_type;

      typedef typename UseThisDevice<typename scheduler_type::execution_space>::type device_type;                                                                                                           

      typedef Kokkos::View<double**,Kokkos::LayoutLeft,device_type,Kokkos::MemoryUnmanaged> value_type;
      typedef Kokkos::BasicFuture<value_type,scheduler_type> future_type;

      typedef Kokkos::MemoryPool<device_type> memory_pool_type;
      typedef Kokkos::BasicFuture<void*,scheduler_type> future_ptr_type;

    private:
      memory_pool_type _pool;
      future_ptr_type _ptr;
      ordinal_type _m, _n;

    public:
      
      inline
      TaskFunctor_MemoryPool_TestView() = delete;
      
      inline
      TaskFunctor_MemoryPool_TestView(const memory_pool_type &pool,
                                      const future_ptr_type &ptr,
                                      const ordinal_type m,
                                      const ordinal_type n) 
        : _pool(pool),
          _ptr(ptr),
          _m(m),
          _n(n) {}
      
      inline
      void operator()(member_type &member, value_type &r_val) {
        Kokkos::single(Kokkos::PerTeam(member), [&]() {
            printf("TestView construct view in future\n");
            if (_m && _n) {
              value_type A((double*)_ptr.get(), _m, _n);
              ordinal_type cnt = 0;
              for (ordinal_type i=0;i<_m;++i)
                for (ordinal_type j=0;j<_n;++j)
                  A(i,j) = cnt++;
              r_val = A;
            } else {
              r_val = value_type();
            }
          });
      }
    };
    
    template<typename SchedulerType>
    struct TaskFunctor_MemoryPool_TestViewSee {
      typedef SchedulerType scheduler_type;
      typedef typename scheduler_type::member_type member_type;

      typedef typename UseThisDevice<typename scheduler_type::execution_space>::type device_type;                                                                                                           

      typedef void value_type;
      typedef Kokkos::BasicFuture<scheduler_type> future_type;
      
      typedef Kokkos::View<double**,Kokkos::LayoutLeft,device_type,Kokkos::MemoryUnmanaged> view_type;
      typedef Kokkos::BasicFuture<view_type,scheduler_type> future_view_type;
      
      // memory pool
      typedef Kokkos::MemoryPool<device_type> memory_pool_type;

    private:
      memory_pool_type _pool;
      future_view_type _A;

    public:

      inline
      TaskFunctor_MemoryPool_TestViewSee() = delete;

      inline
      TaskFunctor_MemoryPool_TestViewSee(const memory_pool_type &pool,
                                         const future_view_type &A)
        : _pool(pool),
          _A(A) {}
      
      inline
      void operator()(member_type &member) {
        Kokkos::single(Kokkos::PerTeam(member), [&]() {
            const auto A = _A.get();
            
            const ordinal_type m = A.extent(0);
            const ordinal_type n = A.extent(1);
            
            printf("A in TestViewSee: %lu\n", (long unsigned int)A.data());
            for (ordinal_type i=0;i<m;++i) {
              for (ordinal_type j=0;j<n;++j)
                printf(" %4d ", int(A(i,j)));
              printf("\n");
            }
          });
      }
    };

}

#endif
