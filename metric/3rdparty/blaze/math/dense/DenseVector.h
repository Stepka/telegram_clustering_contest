//=================================================================================================
/*!
//  \file blaze/math/dense/DenseVector.h
//  \brief Header file for utility functions for dense vectors
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

#ifndef _BLAZE_MATH_DENSE_DENSEVECTOR_H_
#define _BLAZE_MATH_DENSE_DENSEVECTOR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "../../math/Aliases.h"
#include "../../math/expressions/DenseVector.h"
#include "../../math/Exception.h"
#include "../../math/shims/Equal.h"
#include "../../math/shims/IsDivisor.h"
#include "../../math/shims/IsNaN.h"
#include "../../math/shims/IsZero.h"
#include "../../math/shims/Pow2.h"
#include "../../math/shims/Sqrt.h"
#include "../../math/typetraits/IsRestricted.h"
#include "../../math/typetraits/IsUniform.h"
#include "../../util/Assert.h"
#include "../../util/constraints/Numeric.h"
#include "../../util/DecltypeAuto.h"
#include "../../util/EnableIf.h"
#include "../../util/Types.h"
#include "../../util/typetraits/IsNumeric.h"
#include "../../util/typetraits/RemoveReference.h"


namespace blaze {

//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name DenseVector operators */
//@{
template< typename T1, typename T2, bool TF >
inline auto operator==( const DenseVector<T1,TF>& vec, T2 scalar )
   -> EnableIf_t< IsNumeric_v<T2>, bool >;

template< typename T1, typename T2, bool TF >
inline auto operator==( T1 scalar, const DenseVector<T2,TF>& vec )
   -> EnableIf_t< IsNumeric_v<T1>, bool >;

template< typename T1, typename T2, bool TF >
inline auto operator!=( const DenseVector<T1,TF>& vec, T2 scalar )
   -> EnableIf_t< IsNumeric_v<T2>, bool >;

template< typename T1, typename T2, bool TF >
inline auto operator!=( T1 scalar, const DenseVector<T2,TF>& vec )
   -> EnableIf_t< IsNumeric_v<T1>, bool >;

template< typename VT, bool TF, typename ST >
inline auto operator*=( DenseVector<VT,TF>& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >;

template< typename VT, bool TF, typename ST >
inline auto operator*=( DenseVector<VT,TF>&& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >;

template< typename VT, bool TF, typename ST >
inline auto operator/=( DenseVector<VT,TF>& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >;

template< typename VT, bool TF, typename ST >
inline auto operator/=( DenseVector<VT,TF>&& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Equality operator for the comparison of a dense vector and a scalar value.
// \ingroup dense_vector
//
// \param vec The left-hand side dense vector for the comparison.
// \param scalar The right-hand side scalar value for the comparison.
// \return \a true if all elements of the vector are equal to the scalar, \a false if not.
//
// If all values of the vector are equal to the scalar value, the equality test returns \a true,
// otherwise \a false. Note that this function can only be used with built-in, numerical data
// types!
*/
template< typename T1  // Type of the left-hand side dense vector
        , typename T2  // Type of the right-hand side scalar
        , bool TF >    // Transpose flag
inline auto operator==( const DenseVector<T1,TF>& vec, T2 scalar )
   -> EnableIf_t< IsNumeric_v<T2>, bool >
{
   using CT1 = CompositeType_t<T1>;

   // Evaluation of the dense vector operand
   CT1 a( ~vec );

   // In order to compare the vector and the scalar value, the data values of the lower-order
   // data type are converted to the higher-order data type within the equal function.
   for( size_t i=0; i<a.size(); ++i )
      if( !equal( a[i], scalar ) ) return false;
   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Equality operator for the comparison of a scalar value and a dense vector.
// \ingroup dense_vector
//
// \param scalar The left-hand side scalar value for the comparison.
// \param vec The right-hand side dense vector for the comparison.
// \return \a true if all elements of the vector are equal to the scalar, \a false if not.
//
// If all values of the vector are equal to the scalar value, the equality test returns \a true,
// otherwise \a false. Note that this function can only be used with built-in, numerical data
// types!
*/
template< typename T1  // Type of the left-hand side scalar
        , typename T2  // Type of the right-hand side dense vector
        , bool TF >    // Transpose flag
inline auto operator==( T1 scalar, const DenseVector<T2,TF>& vec )
   -> EnableIf_t< IsNumeric_v<T1>, bool >
{
   return ( vec == scalar );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Inequality operator for the comparison of a dense vector and a scalar value.
// \ingroup dense_vector
//
// \param vec The left-hand side dense vector for the comparison.
// \param scalar The right-hand side scalar value for the comparison.
// \return \a true if at least one element of the vector is different from the scalar, \a false if not.
//
// If one value of the vector is inequal to the scalar value, the inequality test returns \a true,
// otherwise \a false. Note that this function can only be used with built-in, numerical data
// types!
*/
template< typename T1  // Type of the left-hand side dense vector
        , typename T2  // Type of the right-hand side scalar
        , bool TF >    // Transpose flag
inline auto operator!=( const DenseVector<T1,TF>& vec, T2 scalar )
   -> EnableIf_t< IsNumeric_v<T2>, bool >
{
   return !( vec == scalar );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Inequality operator for the comparison of a scalar value and a dense vector.
// \ingroup dense_vector
//
// \param scalar The left-hand side scalar value for the comparison.
// \param vec The right-hand side dense vector for the comparison.
// \return \a true if at least one element of the vector is different from the scalar, \a false if not.
//
// If one value of the vector is inequal to the scalar value, the inequality test returns \a true,
// otherwise \a false. Note that this function can only be used with built-in, numerical data
// types!
*/
template< typename T1  // Type of the left-hand side scalar
        , typename T2  // Type of the right-hand side vector
        , bool TF >    // Transpose flag
inline auto operator!=( T1 scalar, const DenseVector<T2,TF>& vec )
   -> EnableIf_t< IsNumeric_v<T1>, bool >
{
   return !( vec == scalar );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a dense vector and
//        a scalar value (\f$ \vec{a}*=s \f$).
// \ingroup dense_vector
//
// \param vec The left-hand side dense vector for the multiplication.
// \param scalar The right-hand side scalar value for the multiplication.
// \return Reference to the left-hand side dense vector.
// \exception std::invalid_argument Invalid scaling of restricted vector.
//
// In case the vector \a VT is restricted and the assignment would violate an invariant of the
// vector, a \a std::invalid_argument exception is thrown.
*/
template< typename VT    // Type of the left-hand side dense vector
        , bool TF        // Transpose flag
        , typename ST >  // Data type of the right-hand side scalar
inline auto operator*=( DenseVector<VT,TF>& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >
{
   if( IsRestricted_v<VT> ) {
      if( !tryMult( ~vec, 0UL, (~vec).size(), scalar ) ) {
         BLAZE_THROW_INVALID_ARGUMENT( "Invalid scaling of restricted vector" );
      }
   }

   BLAZE_DECLTYPE_AUTO( left, derestrict( ~vec ) );

   smpAssign( left, left * scalar );

   BLAZE_INTERNAL_ASSERT( isIntact( ~vec ), "Invariant violation detected" );

   return ~vec;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a temporary dense vector
//        and a scalar (\f$ v*=s \f$).
// \ingroup dense_vector
//
// \param vec The left-hand side temporary dense vector for the multiplication.
// \param scalar The right-hand side scalar value for the multiplication.
// \return Reference to the left-hand side dense vector.
// \exception std::invalid_argument Invalid scaling of restricted vector.
//
// In case the vector \a VT is restricted and the assignment would violate an invariant of the
// vector, a \a std::invalid_argument exception is thrown.
*/
template< typename VT    // Type of the left-hand side dense vector
        , bool TF        // Transpose flag
        , typename ST >  // Data type of the right-hand side scalar
inline auto operator*=( DenseVector<VT,TF>&& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >
{
   return operator*=( ~vec, scalar );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a dense vector by a scalar value
//        (\f$ \vec{a}/=s \f$).
// \ingroup dense_vector
//
// \param vec The left-hand side dense vector for the division.
// \param scalar The right-hand side scalar value for the division.
// \return Reference to the left-hand side dense vector.
// \exception std::invalid_argument Invalid scaling of restricted vector.
//
// In case the vector \a VT is restricted and the assignment would violate an invariant of the
// vector, a \a std::invalid_argument exception is thrown.
//
// \note A division by zero is only checked by an user assert.
*/
template< typename VT    // Type of the left-hand side dense vector
        , bool TF        // Transpose flag
        , typename ST >  // Data type of the right-hand side scalar
inline auto operator/=( DenseVector<VT,TF>& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >
{
   BLAZE_USER_ASSERT( !isZero( scalar ), "Division by zero detected" );

   if( IsRestricted_v<VT> ) {
      if( !tryDiv( ~vec, 0UL, (~vec).size(), scalar ) ) {
         BLAZE_THROW_INVALID_ARGUMENT( "Invalid scaling of restricted vector" );
      }
   }

   BLAZE_DECLTYPE_AUTO( left, derestrict( ~vec ) );

   smpAssign( left, left / scalar );

   BLAZE_INTERNAL_ASSERT( isIntact( ~vec ), "Invariant violation detected" );

   return ~vec;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a temporary dense vector by a scalar
//        value (\f$ \vec{a}/=s \f$).
// \ingroup dense_vector
//
// \param vec The left-hand side temporary dense vector for the division.
// \param scalar The right-hand side scalar value for the division.
// \return Reference to the left-hand side dense vector.
// \exception std::invalid_argument Invalid scaling of restricted vector.
//
// In case the vector \a VT is restricted and the assignment would violate an invariant of the
// vector, a \a std::invalid_argument exception is thrown.
//
// \note A division by zero is only checked by an user assert.
*/
template< typename VT    // Type of the left-hand side dense vector
        , bool TF        // Transpose flag
        , typename ST >  // Data type of the right-hand side scalar
inline auto operator/=( DenseVector<VT,TF>&& vec, ST scalar )
   -> EnableIf_t< IsNumeric_v<ST>, VT& >
{
   return operator/=( ~vec, scalar );
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name DenseVector functions */
//@{
template< typename VT, bool TF >
bool isnan( const DenseVector<VT,TF>& dv );

template< typename VT, bool TF >
bool isDivisor( const DenseVector<VT,TF>& dv );

template< typename VT, bool TF >
bool isUniform( const DenseVector<VT,TF>& dv );

template< typename VT, bool TF >
const ElementType_t<VT> sqrLength( const DenseVector<VT,TF>& dv );

template< typename VT, bool TF >
inline auto length( const DenseVector<VT,TF>& dv ) -> decltype( sqrt( sqrLength( ~dv ) ) );

template< typename VT, bool TF >
auto softmax( const DenseVector<VT,TF>& dv );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks the given dense vector for not-a-number elements.
// \ingroup dense_vector
//
// \param dv The vector to be checked for not-a-number elements.
// \return \a true if at least one element of the vector is not-a-number, \a false otherwise.
//
// This function checks the N-dimensional dense vector for not-a-number (NaN) elements. If at
// least one element of the vector is not-a-number, the function returns \a true, otherwise it
// returns \a false.

   \code
   blaze::DynamicVector<double> a;
   // ... Resizing and initialization
   if( isnan( a ) ) { ... }
   \endcode

// Note that this function only works for vectors with floating point elements. The attempt to
// use it for a vector with a non-floating point element type results in a compile time error.
*/
template< typename VT  // Type of the dense vector
        , bool TF >    // Transpose flag
bool isnan( const DenseVector<VT,TF>& dv )
{
   CompositeType_t<VT> a( ~dv );  // Evaluation of the dense vector operand

   for( size_t i=0UL; i<a.size(); ++i ) {
      if( isnan( a[i] ) ) return true;
   }
   return false;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given dense vector is a valid divisor.
// \ingroup dense_vector
//
// \param dv The dense vector to be tested.
// \return \a true in case the given vector is a valid divisor, \a false otherwise.
//
// This function checks if the given dense vector is a valid divisor. If all elements of the
// vector are valid divisors the function returns \a true, if at least one element of the vector
// is not a valid divisor, the function returns \a false.

   \code
   StaticVector<int,3UL> a{ 1, -1, 2 };  // isDivisor( a ) returns true
   StaticVector<int,3UL> b{ 1, -1, 0 };  // isDivisor( b ) returns false
   \endcode
*/
template< typename VT  // Type of the dense vector
        , bool TF >    // Transpose flag
bool isDivisor( const DenseVector<VT,TF>& dv )
{
   CompositeType_t<VT> a( ~dv );  // Evaluation of the dense vector operand

   for( size_t i=0UL; i<a.size(); ++i ) {
      if( !isDivisor( a[i] ) ) return false;
   }
   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks if the given dense vector is a uniform vector.
// \ingroup dense_vector
//
// \param dv The dense vector to be checked.
// \return \a true if the vector is a uniform vector, \a false if not.
//
// This function checks if the given dense vector is a uniform vector. The vector is considered
// to be uniform if all its elements are identical. The following code example demonstrates the
// use of the function:

   \code
   blaze::DynamicVector<int,blaze::columnVector> a, b;
   // ... Initialization
   if( isUniform( a ) ) { ... }
   \endcode

// It is also possible to check if a vector expression results in a uniform vector:

   \code
   if( isUniform( a + b ) ) { ... }
   \endcode

// However, note that this might require the complete evaluation of the expression, including
// the generation of a temporary vector.
*/
template< typename VT  // Type of the dense vector
        , bool TF >    // Transpose flag
bool isUniform( const DenseVector<VT,TF>& dv )
{
   using CT = CompositeType_t<VT>;
   using ConstReference = ConstReference_t< RemoveReference_t<CT> >;

   if( IsUniform_v<VT> || (~dv).size() < 2UL )
      return true;

   CT a( ~dv );  // Evaluation of the dense vector operand

   ConstReference cmp( a[0UL] );

   for( size_t i=1UL; i<a.size(); ++i ) {
      if( a[i] != cmp )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Calculation of the square length (magnitude) of the dense vector \f$|\vec{a}|^2\f$.
// \ingroup dense_vector
//
// \param dv The given dense vector.
// \return The square length (magnitude) of the dense vector.
//
// This function calculates the actual square length (magnitude) of the dense vector.
//
// \note This operation is only defined for numeric data types. In case the element type is
// not a numeric data type (i.e. a user defined data type or boolean) the attempt to use the
// sqrLength() function results in a compile time error!
*/
template< typename VT  // Type of the dense vector
        , bool TF >    // Transpose flag
const ElementType_t<VT> sqrLength( const DenseVector<VT,TF>& dv )
{
   using ElementType = ElementType_t<VT>;

   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( ElementType );

   ElementType sum( 0 );
   for( size_t i=0UL; i<(~dv).size(); ++i )
      sum += pow2( (~dv)[i] );
   return sum;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Calculation of the length (magnitude) of the dense vector \f$|\vec{a}|\f$.
// \ingroup dense_vector
//
// \param dv The given dense vector.
// \return The length (magnitude) of the dense vector.
//
// This function calculates the actual length (magnitude) of the dense vector. The return type
// of the length() function depends on the actual element type of the vector instance:
//
// <table border="0" cellspacing="0" cellpadding="1">
//    <tr>
//       <td width="250px"> \b Type </td>
//       <td width="100px"> \b LengthType </td>
//    </tr>
//    <tr>
//       <td>float</td>
//       <td>float</td>
//    </tr>
//    <tr>
//       <td>integral data types and double</td>
//       <td>double</td>
//    </tr>
//    <tr>
//       <td>long double</td>
//       <td>long double</td>
//    </tr>
//    <tr>
//       <td>complex<T></td>
//       <td>complex<T></td>
//    </tr>
// </table>
//
// \note This operation is only defined for numeric data types. In case the element type is
// not a numeric data type (i.e. a user defined data type or boolean) the attempt to use the
// sqrLength() function results in a compile time error!
*/
template< typename VT  // Type of the dense vector
        , bool TF >    // Transpose flag
inline auto length( const DenseVector<VT,TF>& dv ) -> decltype( sqrt( sqrLength( ~dv ) ) )
{
   return sqrt( sqrLength( ~dv ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Computes the softmax function for the given dense vector.
// \ingroup dense_vector
//
// \param dv The given dense vector for the softmax computation.
// \return The resulting dense vector.
//
// This function computes the softmax function (i.e. the normalized exponential function) for
// the given dense vector \a dv (see also https://en.wikipedia.org/wiki/Softmax_function). The
// resulting dense vector consists of real values in the range (0..1], which add up to 1.
*/
template< typename VT  // Type of the dense vector
        , bool TF >    // Transpose flag
auto softmax( const DenseVector<VT,TF>& dv )
{
   auto tmp( evaluate( exp( ~dv ) ) );
   const auto scalar( sum( ~tmp ) );
   tmp /= scalar;
   return tmp;
}
//*************************************************************************************************

} // namespace blaze

#endif
