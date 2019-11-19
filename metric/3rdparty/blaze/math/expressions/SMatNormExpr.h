//=================================================================================================
/*!
//  \file blaze/math/expressions/SMatNormExpr.h
//  \brief Header file for the sparse matrix norm expression
//
//  Copyright (C) 2012-2018 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_EXPRESSIONS_SMATNORMEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_SMATNORMEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <utility>
#include "../../math/Aliases.h"
#include "../../math/expressions/SparseMatrix.h"
#include "../../math/functors/Abs.h"
#include "../../math/functors/Cbrt.h"
#include "../../math/functors/L1Norm.h"
#include "../../math/functors/L2Norm.h"
#include "../../math/functors/L3Norm.h"
#include "../../math/functors/L4Norm.h"
#include "../../math/functors/LpNorm.h"
#include "../../math/functors/Noop.h"
#include "../../math/functors/Pow2.h"
#include "../../math/functors/Pow3.h"
#include "../../math/functors/Pow4.h"
#include "../../math/functors/Qdrt.h"
#include "../../math/functors/Sqrt.h"
#include "../../math/functors/UnaryPow.h"
#include "../../math/shims/Evaluate.h"
#include "../../math/shims/Invert.h"
#include "../../math/shims/IsDefault.h"
#include "../../math/shims/IsZero.h"
#include "../../math/traits/MultTrait.h"
#include "../../math/typetraits/IsResizable.h"
#include "../../math/typetraits/IsRowMajorMatrix.h"
#include "../../math/typetraits/UnderlyingBuiltin.h"
#include "../../util/Assert.h"
#include "../../util/FunctionTrace.h"
#include "../../util/StaticAssert.h"
#include "../../util/TypeList.h"
#include "../../util/Types.h"
#include "../../util/typetraits/RemoveReference.h"


namespace blaze {

//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default backend implementation of the norm of a sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \param abs The functor for the abs operation.
// \param power The functor for the power operation.
// \param root The functor for the root operation.
// \return The norm of the given matrix.
//
// This function implements the performance optimized norm of a sparse matrix. Due to the
// explicit application of the SFINAE principle, this function can only be selected by the
// compiler in case vectorization cannot be applied.
*/
template< typename MT      // Type of the sparse matrix
        , bool SO          // Storage order
        , typename Abs     // Type of the abs operation
        , typename Power   // Type of the power operation
        , typename Root >  // Type of the root operation
inline decltype(auto) norm_backend( const SparseMatrix<MT,SO>& sm, Abs abs, Power power, Root root )
{
   using CT = CompositeType_t<MT>;
   using ET = ElementType_t<MT>;
   using RT = decltype( evaluate( root( std::declval<ET>() ) ) );

   using ConstIterator = ConstIterator_t< RemoveReference_t<CT> >;

   if( (~sm).rows() == 0UL || (~sm).columns() == 0UL ) return RT();

   CT tmp( ~sm );

   const size_t N( IsRowMajorMatrix_v<MT> ? tmp.rows(): tmp.columns() );

   ET norm{};

   for( size_t i=0UL; i<N; ++i )
   {
      const ConstIterator end( tmp.end(i) );
      for( ConstIterator element=tmp.begin(i); element!=end; ++element ) {
         if( IsResizable_v<ET> && isDefault( norm ) )
            norm = power( abs( element->value() ) );
         else
            norm += power( abs( element->value() ) );
      }
   }

   return evaluate( root( norm ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the L2 norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The L2 norm of the given sparse matrix.
//
// This function computes the L2 norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double l2 = norm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) norm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return norm_backend( ~sm, Noop(), Pow2(), Sqrt() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the squared L2 norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The squared L2 norm of the given sparse matrix.
//
// This function computes the squared L2 norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double l2 = sqrNorm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) sqrNorm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return norm_backend( ~sm, Noop(), Pow2(), Noop() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the L1 norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The L1 norm of the given sparse matrix.
//
// This function computes the L1 norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double l1 = l1Norm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) l1Norm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return norm_backend( ~sm, Abs(), Noop(), Noop() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the L2 norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The L2 norm of the given sparse matrix.
//
// This function computes the L2 norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double l2 = l2Norm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) l2Norm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return norm_backend( ~sm, Noop(), Pow2(), Sqrt() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the L3 norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The L3 norm of the given sparse matrix.
//
// This function computes the L3 norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double l3 = l3Norm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) l3Norm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return norm_backend( ~sm, Abs(), Pow3(), Cbrt() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the L4 norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The L4 norm of the given sparse matrix.
//
// This function computes the L4 norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double l4 = l4Norm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) l4Norm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return norm_backend( ~sm, Noop(), Pow4(), Qdrt() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the Lp norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \param p The norm parameter (p > 0).
// \return The Lp norm of the given sparse matrix.
//
// This function computes the Lp norm of the given sparse matrix, where the norm is specified by
// the runtime argument \a p:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double lp = lpNorm( A, 2.3 );
   \endcode

// \note The norm parameter \a p is expected to be larger than 0. This precondition is only checked
// by a user assertion.
*/
template< typename MT    // Type of the sparse matrix
        , bool SO        // Storage order
        , typename ST >  // Type of the norm parameter
decltype(auto) lpNorm( const SparseMatrix<MT,SO>& sm, ST p )
{
   BLAZE_FUNCTION_TRACE;

   BLAZE_USER_ASSERT( !isZero( p ), "Invalid p for Lp norm detected" );

   using ScalarType = MultTrait_t< UnderlyingBuiltin_t<MT>, decltype( inv( p ) ) >;
   return norm_backend( ~sm, Abs(), UnaryPow<ScalarType>( p ), UnaryPow<ScalarType>( inv( p ) ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the Lp norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The Lp norm of the given sparse matrix.
//
// This function computes the Lp norm of the given sparse matrix, where the norm is specified by
// the runtime argument \a P:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double lp = lpNorm<2>( A );
   \endcode

// \note The norm parameter \a P is expected to be larger than 0. A value of 0 results in a
// compile time error!.
*/
template< size_t P     // Compile time norm parameter
        , typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
inline decltype(auto) lpNorm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_STATIC_ASSERT_MSG( P > 0UL, "Invalid norm parameter detected" );

   using Norms = TypeList< L1Norm, L2Norm, L3Norm, L4Norm, LpNorm<P> >;
   using Norm  = typename TypeAt< Norms, min( P-1UL, 4UL ) >::Type;

   return Norm()( ~sm );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the maximum norm for the given sparse matrix.
// \ingroup sparse_matrix
//
// \param sm The given sparse matrix for the norm computation.
// \return The maximum norm of the given sparse matrix.
//
// This function computes the maximum norm of the given sparse matrix:

   \code
   blaze::CompressedMatrix<double> A;
   // ... Resizing and initialization
   const double max = maxNorm( A );
   \endcode
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
decltype(auto) maxNorm( const SparseMatrix<MT,SO>& sm )
{
   BLAZE_FUNCTION_TRACE;

   return max( abs( ~sm ) );
}
//*************************************************************************************************

} // namespace blaze

#endif
