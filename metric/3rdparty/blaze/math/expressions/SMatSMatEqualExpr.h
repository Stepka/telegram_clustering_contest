//=================================================================================================
/*!
//  \file blaze/math/expressions/SMatSMatEqualExpr.h
//  \brief Header file for the sparse matrix/sparse matrix equality comparison expression
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

#ifndef _BLAZE_MATH_EXPRESSIONS_SMATSMATEQUALEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_SMATSMATEQUALEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "../../math/Aliases.h"
#include "../../math/expressions/SparseMatrix.h"
#include "../../math/RelaxationFlag.h"
#include "../../math/shims/Equal.h"
#include "../../math/shims/IsDefault.h"
#include "../../util/Types.h"
#include "../../util/typetraits/RemoveReference.h"


namespace blaze {

//=================================================================================================
//
//  GLOBAL BINARY RELATIONAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Equality check of two row-major sparse matrices.
// \ingroup sparse_matrix
//
// \param lhs The left-hand side sparse matrix for the comparison.
// \param rhs The right-hand side sparse matrix for the comparison.
// \return \a true if the two matrices are equal, \a false if not.
//
// Equal function for the comparison of two sparse matrices. Due to the limited machine accuracy,
// a direct comparison of two floating point numbers should be avoided. This function offers the
// possibility to compare two floating-point matrices with a certain accuracy margin.
*/
template< bool RF         // Relaxation flag
        , typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side sparse matrix
inline bool equal( const SparseMatrix<MT1,false>& lhs, const SparseMatrix<MT2,false>& rhs )
{
   using CT1 = CompositeType_t<MT1>;
   using CT2 = CompositeType_t<MT2>;
   using LhsConstIterator = ConstIterator_t< RemoveReference_t<CT1> >;
   using RhsConstIterator = ConstIterator_t< RemoveReference_t<CT2> >;

   // Early exit in case the matrix sizes don't match
   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() )
      return false;

   // Evaluation of the two sparse matrix operands
   CT1 A( ~lhs );
   CT2 B( ~rhs );

   // In order to compare the two matrices, the data values of the lower-order data
   // type are converted to the higher-order data type within the equal function.
   for( size_t i=0UL; i<A.rows(); ++i )
   {
      const LhsConstIterator lend( A.end(i) );
      const RhsConstIterator rend( B.end(i) );

      LhsConstIterator lelem( A.begin(i) );
      RhsConstIterator relem( B.begin(i) );

      while( lelem != lend && relem != rend )
      {
         if( lelem->index() < relem->index() ) {
            if( !isDefault<RF>( lelem->value() ) )
               return false;
            ++lelem;
         }
         else if( lelem->index() > relem->index() ) {
            if( !isDefault<RF>( relem->value() ) )
               return false;
            ++relem;
         }
         else if( !equal<RF>( lelem->value(), relem->value() ) ) {
            return false;
         }
         else {
            ++lelem;
            ++relem;
         }
      }

      while( lelem != lend ) {
         if( !isDefault<RF>( lelem->value() ) )
            return false;
         ++lelem;
      }

      while( relem != rend ) {
         if( !isDefault<RF>( relem->value() ) )
            return false;
         ++relem;
      }
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Equality check of two column-major sparse matrices.
// \ingroup sparse_matrix
//
// \param lhs The left-hand side sparse matrix for the comparison.
// \param rhs The right-hand side sparse matrix for the comparison.
// \return \a true if the two matrices are equal, \a false if not.
//
// Equal function for the comparison of two sparse matrices. Due to the limited machine accuracy,
// a direct comparison of two floating point numbers should be avoided. This function offers the
// possibility to compare two floating-point matrices with a certain accuracy margin.
*/
template< bool RF         // Relaxation flag
        , typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side sparse matrix
inline bool equal( const SparseMatrix<MT1,true>& lhs, const SparseMatrix<MT2,true>& rhs )
{
   using CT1 = CompositeType_t<MT1>;
   using CT2 = CompositeType_t<MT2>;
   using LhsConstIterator = ConstIterator_t< RemoveReference_t<CT1> >;
   using RhsConstIterator = ConstIterator_t< RemoveReference_t<CT2> >;

   // Early exit in case the matrix sizes don't match
   if( (~lhs).rows() != (~rhs).rows() || (~lhs).columns() != (~rhs).columns() )
      return false;

   // Evaluation of the two sparse matrix operands
   CT1 A( ~lhs );
   CT2 B( ~rhs );

   // In order to compare the two matrices, the data values of the lower-order data
   // type are converted to the higher-order data type within the equal function.
   for( size_t j=0UL; j<A.columns(); ++j )
   {
      const LhsConstIterator lend( A.end(j) );
      const RhsConstIterator rend( B.end(j) );

      LhsConstIterator lelem( A.begin(j) );
      RhsConstIterator relem( B.begin(j) );

      while( lelem != lend && relem != rend )
      {
         if( lelem->index() < relem->index() ) {
            if( !isDefault<RF>( lelem->value() ) )
               return false;
            ++lelem;
         }
         else if( lelem->index() > relem->index() ) {
            if( !isDefault<RF>( relem->value() ) )
               return false;
            ++relem;
         }
         else if( !equal<RF>( lelem->value(), relem->value() ) ) {
            return false;
         }
         else {
            ++lelem;
            ++relem;
         }
      }

      while( lelem != lend ) {
         if( !isDefault<RF>( lelem->value() ) )
            return false;
         ++lelem;
      }

      while( relem != rend ) {
         if( !isDefault<RF>( relem->value() ) )
            return false;
         ++relem;
      }
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Equality check of two sparse matrices with different storage order.
// \ingroup sparse_matrix
//
// \param lhs The left-hand side sparse matrix for the comparison.
// \param rhs The right-hand side sparse matrix for the comparison.
// \return \a true if the two matrices are equal, \a false if not.
//
// Equal function for the comparison of two sparse matrices. Due to the limited machine accuracy,
// a direct comparison of two floating point numbers should be avoided. This function offers the
// possibility to compare two floating-point matrices with a certain accuracy margin.
*/
template< bool RF       // Relaxation flag
        , typename MT1  // Type of the left-hand side sparse matrix
        , typename MT2  // Type of the right-hand side sparse matrix
        , bool SO >     // Storage order
inline bool equal( const SparseMatrix<MT1,SO>& lhs, const SparseMatrix<MT2,!SO>& rhs )
{
   const OppositeType_t<MT2> tmp( ~rhs );
   return equal<RF>( ~lhs, tmp );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Equality operator for the comparison of two sparse matrices.
// \ingroup sparse_matrix
//
// \param lhs The left-hand side sparse matrix for the comparison.
// \param rhs The right-hand side sparse matrix for the comparison.
// \return \a true if the two sparse matrices are equal, \a false if not.
*/
template< typename MT1  // Type of the left-hand side sparse matrix
        , bool SO1      // Storage order of the left-hand side sparse matrix
        , typename MT2  // Type of the right-hand side sparse matrix
        , bool SO2 >    // Storage order of the right-hand side sparse matrix
inline bool operator==( const SparseMatrix<MT1,SO1>& lhs, const SparseMatrix<MT2,SO2>& rhs )
{
   return equal<relaxed>( lhs, rhs );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Inequality operator for the comparison of two sparse matrices.
// \ingroup sparse_matrix
//
// \param lhs The left-hand side sparse matrix for the comparison.
// \param rhs The right-hand side sparse matrix for the comparison.
// \return \a true if the two sparse matrices are not equal, \a false if they are equal.
*/
template< typename MT1  // Type of the left-hand side sparse matrix
        , bool SO1      // Storage order of the left-hand side sparse matrix
        , typename MT2  // Type of the right-hand side sparse matrix
        , bool SO2 >    // Storage order of the right-hand side sparse matrix
inline bool operator!=( const SparseMatrix<MT1,SO1>& lhs, const SparseMatrix<MT2,SO2>& rhs )
{
   return !equal<relaxed>( lhs, rhs );
}
//*************************************************************************************************

} // namespace blaze

#endif
