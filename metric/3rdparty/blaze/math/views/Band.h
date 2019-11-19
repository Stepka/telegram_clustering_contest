//=================================================================================================
/*!
//  \file blaze/math/views/Band.h
//  \brief Header file for the implementation of the Band view
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

#ifndef _BLAZE_MATH_VIEWS_BAND_H_
#define _BLAZE_MATH_VIEWS_BAND_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "../../math/expressions/DeclExpr.h"
#include "../../math/expressions/Forward.h"
#include "../../math/expressions/MatEvalExpr.h"
#include "../../math/expressions/MatMapExpr.h"
#include "../../math/expressions/MatMatAddExpr.h"
#include "../../math/expressions/MatMatMapExpr.h"
#include "../../math/expressions/MatMatSubExpr.h"
#include "../../math/expressions/Matrix.h"
#include "../../math/expressions/MatScalarDivExpr.h"
#include "../../math/expressions/MatScalarMultExpr.h"
#include "../../math/expressions/MatSerialExpr.h"
#include "../../math/expressions/MatTransExpr.h"
#include "../../math/expressions/SchurExpr.h"
#include "../../math/expressions/VecTVecMultExpr.h"
#include "../../math/shims/IsDefault.h"
#include "../../math/typetraits/HasConstDataAccess.h"
#include "../../math/typetraits/HasMutableDataAccess.h"
#include "../../math/typetraits/IsOpposedView.h"
#include "../../math/typetraits/IsRestricted.h"
#include "../../math/typetraits/IsSubmatrix.h"
#include "../../math/typetraits/MaxSize.h"
#include "../../math/typetraits/Size.h"
#include "../../math/views/band/BaseTemplate.h"
#include "../../math/views/band/Dense.h"
#include "../../math/views/band/Sparse.h"
#include "../../math/views/Check.h"
#include "../../math/views/Forward.h"
#include "../../math/views/subvector/SubvectorData.h"
#include "../../system/TransposeFlag.h"
#include "../../util/Assert.h"
#include "../../util/DisableIf.h"
#include "../../util/EnableIf.h"
#include "../../util/FunctionTrace.h"
#include "../../util/mpl/If.h"
#include "../../util/mpl/Minimum.h"
#include "../../util/mpl/PtrdiffT.h"
#include "../../util/TrueType.h"
#include "../../util/Types.h"


namespace blaze {

//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Creating a view on a specific band of the given matrix.
// \ingroup band
//
// \param matrix The matrix containing the band.
// \param args Optional band arguments.
// \return View on the specified band of the matrix.
// \exception std::invalid_argument Invalid band access index.
//
// This function returns an expression representing the specified band of the given matrix.

   \code
   using blaze::rowMajor;

   blaze::DynamicMatrix<double,rowMajor> D;
   blaze::CompressedMatrix<double,rowMajor> S;
   // ... Resizing and initialization

   // Creating a view on the upper secondary diagonal of the dense matrix D
   auto ub1 = band<1L>( D );

   // Creating a view on the lower secondary diagonal of the sparse matrix S
   auto lb1 = band<-1L>( S );
   \endcode

// By default, the provided band arguments are checked at runtime. // In case the band is not
// properly specified (i.e. if the specified index does not correspond to a valid band in the
// given matrix) a \a std::invalid_argument exception is thrown. The checks can be skipped by
// providing the optional \a blaze::unchecked argument.

   \code
   auto ub1 = band<1L>( D, unchecked );
   auto lb1 = band<-1L>( S, unchecked );
   \endcode
*/
template< ptrdiff_t I         // Band index
        , typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RBAs >  // Optional band arguments
inline decltype(auto) band( Matrix<MT,SO>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   using ReturnType = Band_<MT,I>;
   return ReturnType( ~matrix, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on a specific band of the given constant matrix.
// \ingroup band
//
// \param matrix The constant matrix containing the band.
// \param args Optional band arguments.
// \return View on the specified band of the matrix.
// \exception std::invalid_argument Invalid band access index.
//
// This function returns an expression representing the specified band of the given constant
// matrix.

   \code
   using blaze::rowMajor;

   const blaze::DynamicMatrix<double,rowMajor> D( ... );
   const blaze::CompressedMatrix<double,rowMajor> S( ... );

   // Creating a view on the upper secondary diagonal of the dense matrix D
   auto ub1 = band<1L>( D );

   // Creating a view on the lower secondary diagonal of the sparse matrix S
   auto lb1 = band<-1L>( S );
   \endcode

// By default, the provided band arguments are checked at runtime. // In case the band is not
// properly specified (i.e. if the specified index does not correspond to a valid band in the
// given matrix) a \a std::invalid_argument exception is thrown. The checks can be skipped by
// providing the optional \a blaze::unchecked argument.

   \code
   auto ub1 = band<1L>( D, unchecked );
   auto lb1 = band<-1L>( S, unchecked );
   \endcode
*/
template< ptrdiff_t I         // Band index
        , typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RBAs >  // Optional band arguments
inline decltype(auto) band( const Matrix<MT,SO>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   using ReturnType = const Band_<const MT,I>;
   return ReturnType( ~matrix, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on a specific band of the given temporary matrix.
// \ingroup band
//
// \param matrix The temporary matrix containing the band.
// \param args Optional band arguments.
// \return View on the specified band of the matrix.
// \exception std::invalid_argument Invalid band access index.
//
// This function returns an expression representing the specified band of the given temporary
// matrix. In case the band is not properly specified (i.e. if the specified index does not
// correspond to a valid band in the given matrix) a \a std::invalid_argument exception is
// thrown.
*/
template< ptrdiff_t I         // Band index
        , typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RBAs >  // Optional band arguments
inline decltype(auto) band( Matrix<MT,SO>&& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   using ReturnType = Band_<MT,I>;
   return ReturnType( ~matrix, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on a specific band of the given matrix.
// \ingroup band
//
// \param matrix The matrix containing the band.
// \param index The band index.
// \param args Optional band arguments.
// \return View on the specified band of the matrix.
// \exception std::invalid_argument Invalid band access index.
//
// This function returns an expression representing the specified band of the given matrix.

   \code
   using blaze::rowMajor;

   blaze::DynamicMatrix<double,rowMajor> D;
   blaze::CompressedMatrix<double,rowMajor> S;
   // ... Resizing and initialization

   // Creating a view on the upper secondary diagonal of the dense matrix D
   auto ub1 = band( D, 1L );

   // Creating a view on the lower secondary diagonal of the sparse matrix S
   auto lb1 = band( S, -1L );
   \endcode

// By default, the provided band arguments are checked at runtime. // In case the band is not
// properly specified (i.e. if the specified index does not correspond to a valid band in the
// given matrix) a \a std::invalid_argument exception is thrown. The checks can be skipped by
// providing the optional \a blaze::unchecked argument.

   \code
   auto ub1 = band( D, 1L, unchecked );
   auto lb1 = band( S, -1L, unchecked );
   \endcode
*/
template< typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RBAs >  // Optional band arguments
inline decltype(auto) band( Matrix<MT,SO>& matrix, ptrdiff_t index, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   using ReturnType = Band_<MT>;
   return ReturnType( ~matrix, index, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on a specific band of the given constant matrix.
// \ingroup band
//
// \param matrix The constant matrix containing the band.
// \param index The band index.
// \param args Optional band arguments.
// \return View on the specified band of the matrix.
// \exception std::invalid_argument Invalid band access index.
//
// This function returns an expression representing the specified band of the given constant
// matrix.

   \code
   using blaze::rowMajor;

   const blaze::DynamicMatrix<double,rowMajor> D( ... );
   const blaze::CompressedMatrix<double,rowMajor> S( ... );

   // Creating a view on the upper secondary diagonal of the dense matrix D
   auto lb1 = band( D, 1L );

   // Creating a view on the lower secondary diagonal of the sparse matrix S
   auto ub1 = band( S, -1L );
   \endcode

// By default, the provided band arguments are checked at runtime. // In case the band is not
// properly specified (i.e. if the specified index does not correspond to a valid band in the
// given matrix) a \a std::invalid_argument exception is thrown. The checks can be skipped by
// providing the optional \a blaze::unchecked argument.

   \code
   auto ub1 = band( D, 1L, unchecked );
   auto lb1 = band( S, -1L, unchecked );
   \endcode
*/
template< typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RBAs >  // Optional band arguments
inline decltype(auto) band( const Matrix<MT,SO>& matrix, ptrdiff_t index, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   using ReturnType = const Band_<const MT>;
   return ReturnType( ~matrix, index, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on a specific band of the given temporary matrix.
// \ingroup band
//
// \param matrix The temporary matrix containing the band.
// \param index The band index.
// \param args Optional band arguments.
// \return View on the specified band of the matrix.
// \exception std::invalid_argument Invalid band access index.
//
// This function returns an expression representing the specified band of the given temporary
// matrix. In case the band is not properly specified (i.e. if the specified index does not
// correspond to a valid band in the given matrix) a \a std::invalid_argument exception is
// thrown.
*/
template< typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RBAs >  // Optional band arguments
inline decltype(auto) band( Matrix<MT,SO>&& matrix, ptrdiff_t index, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   using ReturnType = Band_<MT>;
   return ReturnType( ~matrix, index, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on the diagonal of the given matrix.
// \ingroup band
//
// \param matrix The matrix containing the diagonal.
// \param args Optional diagonal arguments.
// \return View on the diagonal of the matrix.
//
// This function returns an expression representing the diagonal of the given matrix.

   \code
   using blaze::rowMajor;

   blaze::DynamicMatrix<double,rowMajor> D;
   blaze::CompressedMatrix<double,rowMajor> S;
   // ... Resizing and initialization

   // Creating a view on the diagonal of the dense matrix D
   auto diag1 = diagonal( D );

   // Creating a view on the diagonal of the sparse matrix S
   auto diag2 = diagonal( S );
   \endcode
*/
template< typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RDAs >  // Optional diagonal arguments
inline decltype(auto) diagonal( Matrix<MT,SO>& matrix, RDAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<0L>( ~matrix, unchecked, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on the diagonal of the given constant matrix.
// \ingroup band
//
// \param matrix The constant matrix containing the diagonal.
// \param args Optional diagonal arguments.
// \return View on the diagonal of the matrix.
//
// This function returns an expression representing the diagonal of the given constant matrix.

   \code
   using blaze::rowMajor;

   const blaze::DynamicMatrix<double,rowMajor> D( ... );
   const blaze::CompressedMatrix<double,rowMajor> S( ... );

   // Creating a view on the diagonal of the dense matrix D
   auto diag1 = diagonal( D );

   // Creating a view on the diagonal of the sparse matrix S
   auto diag2 = diagonal( S );
   \endcode
*/
template< typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RDAs >  // Optional diagonal arguments
inline decltype(auto) diagonal( const Matrix<MT,SO>& matrix, RDAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<0L>( ~matrix, unchecked, args... );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on the diagonal of the given temporary matrix.
// \ingroup band
//
// \param matrix The temporary matrix containing the diagonal.
// \param args Optional diagonal arguments.
// \return View on the diagonal of the matrix.
//
// This function returns an expression representing the diagonal of the given temporary matrix.
// In case the diagonal is not properly specified (i.e. in case the given matrix has zero rows
// or zero columns) a \a std::invalid_argument exception is thrown.
*/
template< typename MT         // Type of the matrix
        , bool SO             // Storage order
        , typename... RDAs >  // Optional diagonal arguments
inline decltype(auto) diagonal( Matrix<MT,SO>&& matrix, RDAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<0L>( ~matrix, unchecked, args... );
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL RESTRUCTURING FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix/matrix addition.
// \ingroup band
//
// \param matrix The constant matrix/matrix addition.
// \param args The runtime band arguments.
// \return View on the specified band of the addition.
//
// This function returns an expression representing the specified band of the given matrix/matrix
// addition.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatMatAddExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<CBAs...>( (~matrix).leftOperand(), args... ) +
          band<CBAs...>( (~matrix).rightOperand(), args... );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix/matrix subtraction.
// \ingroup band
//
// \param matrix The constant matrix/matrix subtraction.
// \param args The runtime band arguments.
// \return View on the specified band of the subtraction.
//
// This function returns an expression representing the specified band of the given matrix/matrix
// subtraction.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatMatSubExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<CBAs...>( (~matrix).leftOperand(), args... ) -
          band<CBAs...>( (~matrix).rightOperand(), args... );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given Schur product.
// \ingroup band
//
// \param matrix The constant Schur product.
// \param args The runtime band arguments.
// \return View on the specified band of the Schur product.
//
// This function returns an expression representing the specified band of the given Schur product.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const SchurExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<CBAs...>( (~matrix).leftOperand(), args... ) *
          band<CBAs...>( (~matrix).rightOperand(), args... );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given outer product.
// \ingroup band
//
// \param matrix The constant outer product.
// \return View on the specified band of the outer product.
//
// This function returns an expression representing the specified band of the given outer product.
*/
template< ptrdiff_t I    // Band index
        , typename MT >  // Type of the matrix
inline decltype(auto) band( const VecTVecMultExpr<MT>& matrix )
{
   BLAZE_FUNCTION_TRACE;

   decltype(auto) leftOperand ( (~matrix).leftOperand()  );
   decltype(auto) rightOperand( (~matrix).rightOperand() );

   const size_t row   ( I <  0L ? -I : 0UL );
   const size_t column( I >= 0L ?  I : 0UL );
   const size_t size  ( min( leftOperand.size() - row, rightOperand.size() - column ) );

   return transTo<defaultTransposeFlag>( subvector( leftOperand , row   , size ) ) *
          transTo<defaultTransposeFlag>( subvector( rightOperand, column, size ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given outer product.
// \ingroup band
//
// \param matrix The constant outer product.
// \param index The band index.
// \return View on the specified band of the outer product.
//
// This function returns an expression representing the specified band of the given outer product.
*/
template< typename MT >  // Type of the matrix
inline decltype(auto) band( const VecTVecMultExpr<MT>& matrix, ptrdiff_t index )
{
   BLAZE_FUNCTION_TRACE;

   decltype(auto) leftOperand ( (~matrix).leftOperand()  );
   decltype(auto) rightOperand( (~matrix).rightOperand() );

   const size_t row   ( index <  0L ? -index : 0UL );
   const size_t column( index >= 0L ?  index : 0UL );
   const size_t size  ( min( leftOperand.size() - row, rightOperand.size() - column ) );

   return transTo<defaultTransposeFlag>( subvector( leftOperand , row   , size ) ) *
          transTo<defaultTransposeFlag>( subvector( rightOperand, column, size ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix/scalar multiplication.
// \ingroup band
//
// \param matrix The constant matrix/scalar multiplication.
// \param args The runtime band arguments.
// \return View on the specified band of the multiplication.
//
// This function returns an expression representing the specified band of the given matrix/scalar
// multiplication.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatScalarMultExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<CBAs...>( (~matrix).leftOperand(), args... ) * (~matrix).rightOperand();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix/scalar division.
// \ingroup band
//
// \param matrix The constant matrix/scalar division.
// \param args The runtime band arguments.
// \return View on the specified band of the division.
//
// This function returns an expression representing the specified band of the given matrix/scalar
// division.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatScalarDivExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<CBAs...>( (~matrix).leftOperand(), args... ) / (~matrix).rightOperand();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given unary matrix map operation.
// \ingroup band
//
// \param matrix The constant unary matrix map operation.
// \param args The runtime band arguments.
// \return View on the specified band of the unary map operation.
//
// This function returns an expression representing the specified band of the given unary matrix
// map operation.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatMapExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return map( band<CBAs...>( (~matrix).operand(), args... ), (~matrix).operation() );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given binary matrix map operation.
// \ingroup band
//
// \param matrix The constant binary matrix map operation.
// \param args The runtime band arguments.
// \return View on the specified band of the binary map operation.
//
// This function returns an expression representing the specified band of the given binary matrix
// map operation.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatMatMapExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return map( band<CBAs...>( (~matrix).leftOperand(), args... ),
               band<CBAs...>( (~matrix).rightOperand(), args... ),
               (~matrix).operation() );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix evaluation operation.
// \ingroup band
//
// \param matrix The constant matrix evaluation operation.
// \param args The runtime band arguments.
// \return View on the specified band of the evaluation operation.
//
// This function returns an expression representing the specified band of the given matrix
// evaluation operation.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatEvalExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return eval( band<CBAs...>( (~matrix).operand(), args... ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix serialization operation.
// \ingroup band
//
// \param matrix The constant matrix serialization operation.
// \param args The runtime band arguments.
// \return View on the specified band of the serialization operation.
//
// This function returns an expression representing the specified band of the given matrix
// serialization operation.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatSerialExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return serial( band<CBAs...>( (~matrix).operand(), args... ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix declaration operation.
// \ingroup band
//
// \param matrix The constant matrix declaration operation.
// \param args The runtime band arguments.
// \return View on the specified band of the declaration operation.
//
// This function returns an expression representing the specified band of the given matrix
// declaration operation.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const DeclExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<CBAs...>( (~matrix).operand(), args... );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific band of the given matrix transpose operation.
// \ingroup band
//
// \param matrix The constant matrix transpose operation.
// \param args The runtime band arguments.
// \return View on the specified band of the transpose operation.
//
// This function returns an expression representing the specified band of the given matrix
// transpose operation.
*/
template< ptrdiff_t... CBAs   // Compile time band arguments
        , typename MT         // Type of the matrix
        , typename... RBAs >  // Runtime band arguments
inline decltype(auto) band( const MatTransExpr<MT>& matrix, RBAs... args )
{
   BLAZE_FUNCTION_TRACE;

   return band<-CBAs...>( (~matrix).operand(), -args... );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL RESTRUCTURING FUNCTIONS (SUBVECTOR)
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific subvector of the given band.
// \ingroup band
//
// \param b The band containing the subvector.
// \param args The optional subvector arguments.
// \return View on the specified subvector of the band.
//
// This function returns an expression representing the specified subvector of the given band.
*/
template< AlignmentFlag AF    // Alignment flag
        , size_t I1           // Index of the first subvector element
        , size_t N            // Size of the subvector
        , typename MT         // Type of the matrix
        , bool TF             // Transpose flag
        , bool DF             // Density flag
        , bool MF             // Multiplication flag
        , ptrdiff_t I2        // Band index
        , typename... RSAs >  // Optional subvector arguments
inline decltype(auto) subvector( Band<MT,TF,DF,MF,I2>& b, RSAs... args )
{
   BLAZE_FUNCTION_TRACE;

   constexpr size_t row   ( ( I2 >= 0L ? 0UL : -I2 ) + I1 );
   constexpr size_t column( ( I2 >= 0L ?  I2 : 0UL ) + I1 );

   return diagonal( submatrix<AF,row,column,N,N>( b.operand(), args... ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific subvector of the given constant band.
// \ingroup band
//
// \param b The constant band containing the subvector.
// \param args The optional subvector arguments.
// \return View on the specified subvector of the band.
//
// This function returns an expression representing the specified subvector of the given constant
// band.
*/
template< AlignmentFlag AF    // Alignment flag
        , size_t I1           // Index of the first subvector element
        , size_t N            // Size of the subvector
        , typename MT         // Type of the matrix
        , bool TF             // Transpose flag
        , bool DF             // Density flag
        , bool MF             // Multiplication flag
        , ptrdiff_t I2        // Band index
        , typename... RSAs >  // Optional subvector arguments
inline decltype(auto) subvector( const Band<MT,TF,DF,MF,I2>& b, RSAs... args )
{
   BLAZE_FUNCTION_TRACE;

   constexpr size_t row   ( ( I2 >= 0L ? 0UL : -I2 ) + I1 );
   constexpr size_t column( ( I2 >= 0L ?  I2 : 0UL ) + I1 );

   return diagonal( submatrix<AF,row,column,N,N>( b.operand(), args... ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific subvector of the given temporary band.
// \ingroup band
//
// \param b The temporary band containing the subvector.
// \param args The optional subvector arguments.
// \return View on the specified subvector of the band.
//
// This function returns an expression representing the specified subvector of the given temporary
// band.
*/
template< AlignmentFlag AF    // Alignment flag
        , size_t I1           // Index of the first subvector element
        , size_t N            // Size of the subvector
        , typename MT         // Type of the matrix
        , bool TF             // Transpose flag
        , bool DF             // Density flag
        , bool MF             // Multiplication flag
        , ptrdiff_t I2        // Band index
        , typename... RSAs >  // Optional subvector arguments
inline decltype(auto) subvector( Band<MT,TF,DF,MF,I2>&& b, RSAs... args )
{
   BLAZE_FUNCTION_TRACE;

   constexpr size_t row   ( ( I2 >= 0L ? 0UL : -I2 ) + I1 );
   constexpr size_t column( ( I2 >= 0L ?  I2 : 0UL ) + I1 );

   return diagonal( submatrix<AF,row,column,N,N>( b.operand(), args... ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific subvector of the given band.
// \ingroup band
//
// \param b The band containing the subvector.
// \param args The optional subvector arguments.
// \return View on the specified subvector of the band.
//
// This function returns an expression representing the specified subvector of the given band.
*/
template< AlignmentFlag AF    // Alignment flag
        , size_t... CSAs      // Compile time subvector arguments
        , typename MT         // Type of the matrix
        , bool TF             // Transpose flag
        , bool DF             // Density flag
        , bool MF             // Multiplication flag
        , ptrdiff_t... CBAs   // Compile time band arguments
        , typename... RSAs >  // Optional subvector arguments
inline decltype(auto) subvector( Band<MT,TF,DF,MF,CBAs...>& b, RSAs... args )
{
   BLAZE_FUNCTION_TRACE;

   const SubvectorData<CSAs...> sd( args... );

   const size_t row   ( b.row() + sd.offset() );
   const size_t column( b.column() + sd.offset() );

   return diagonal( submatrix<AF>( b.operand(), row, column, sd.size(), sd.size(), args... ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific subvector of the given constant band.
// \ingroup band
//
// \param b The constant band containing the subvector.
// \param args The optional subvector arguments.
// \return View on the specified subvector of the band.
//
// This function returns an expression representing the specified subvector of the given constant
// band.
*/
template< AlignmentFlag AF    // Alignment flag
        , size_t... CSAs      // Compile time subvector arguments
        , typename MT         // Type of the matrix
        , bool TF             // Transpose flag
        , bool DF             // Density flag
        , bool MF             // Multiplication flag
        , ptrdiff_t... CBAs   // Compile time band arguments
        , typename... RSAs >  // Optional subvector arguments
inline decltype(auto) subvector( const Band<MT,TF,DF,MF,CBAs...>& b, RSAs... args )
{
   BLAZE_FUNCTION_TRACE;

   const SubvectorData<CSAs...> sd( args... );

   const size_t row   ( b.row() + sd.offset() );
   const size_t column( b.column() + sd.offset() );

   return diagonal( submatrix<AF>( b.operand(), row, column, sd.size(), sd.size(), args... ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific subvector of the given temporary band.
// \ingroup band
//
// \param b The temporary band containing the subvector.
// \param args The optional subvector arguments.
// \return View on the specified subvector of the band.
//
// This function returns an expression representing the specified subvector of the given temporary
// band.
*/
template< AlignmentFlag AF    // Alignment flag
        , size_t... CSAs      // Compile time subvector arguments
        , typename MT         // Type of the matrix
        , bool TF             // Transpose flag
        , bool DF             // Density flag
        , bool MF             // Multiplication flag
        , ptrdiff_t... CBAs   // Compile time band arguments
        , typename... RSAs >  // Optional subvector arguments
inline decltype(auto) subvector( Band<MT,TF,DF,MF,CBAs...>&& b, RSAs... args )
{
   BLAZE_FUNCTION_TRACE;

   const SubvectorData<CSAs...> sd( args... );

   const size_t row   ( b.row() + sd.offset() );
   const size_t column( b.column() + sd.offset() );

   return diagonal( submatrix<AF>( b.operand(), row, column, sd.size(), sd.size(), args... ), unchecked );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  BAND OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Resetting the given band.
// \ingroup band
//
// \param band The band to be resetted.
// \return void
*/
template< typename MT          // Type of the matrix
        , bool TF              // Transpose flag
        , bool DF              // Density flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline void reset( Band<MT,TF,DF,MF,CBAs...>& band )
{
   band.reset();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Resetting the given temporary band.
// \ingroup band
//
// \param band The temporary band to be resetted.
// \return void
*/
template< typename MT          // Type of the matrix
        , bool TF              // Transpose flag
        , bool DF              // Density flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline void reset( Band<MT,TF,DF,MF,CBAs...>&& band )
{
   band.reset();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Clearing the given band.
// \ingroup band
//
// \param band The band to be cleared.
// \return void
//
// Clearing a band is equivalent to resetting it via the reset() function.
*/
template< typename MT          // Type of the matrix
        , bool TF              // Transpose flag
        , bool DF              // Density flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline void clear( Band<MT,TF,DF,MF,CBAs...>& band )
{
   band.reset();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Clearing the given temporary band.
// \ingroup band
//
// \param band The temporary band to be cleared.
// \return void
//
// Clearing a band is equivalent to resetting it via the reset() function.
*/
template< typename MT          // Type of the matrix
        , bool TF              // Transpose flag
        , bool DF              // Density flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline void clear( Band<MT,TF,DF,MF,CBAs...>&& band )
{
   band.reset();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the given dense band is in default state.
// \ingroup band
//
// \param band The dense band to be tested for its default state.
// \return \a true in case the given dense band is component-wise zero, \a false otherwise.
//
// This function checks whether the dense band is in default state. For instance, in case the
// band is instantiated for a built-in integral or floating point data type, the function returns
// \a true in case all band elements are 0 and \a false in case any band element is not 0. The
// following example demonstrates the use of the \a isDefault function:

   \code
   blaze::DynamicMatrix<int,rowMajor> A;
   // ... Resizing and initialization
   if( isDefault( band( A, 0UL ) ) ) { ... }
   \endcode

// Optionally, it is possible to switch between strict semantics (blaze::strict) and relaxed
// semantics (blaze::relaxed):

   \code
   if( isDefault<relaxed>( band( A, 0UL ) ) ) { ... }
   \endcode
*/
template< bool RF              // Relaxation flag
        , typename MT          // Type of the dense matrix
        , bool TF              // Transpose flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline bool isDefault( const Band<MT,TF,true,MF,CBAs...>& band )
{
   using blaze::isDefault;

   for( size_t i=0UL; i<band.size(); ++i )
      if( !isDefault<RF>( band[i] ) ) return false;
   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the given sparse band is in default state.
// \ingroup band
//
// \param band The sparse band to be tested for its default state.
// \return \a true in case the given band is component-wise zero, \a false otherwise.
//
// This function checks whether the sparse band is in default state. For instance, in case the
// band is instantiated for a built-in integral or floating point data type, the function returns
// \a true in case all band elements are 0 and \a false in case any band element is not 0. The
// following example demonstrates the use of the \a isDefault function:

   \code
   blaze::CompressedMatrix<int,rowMajor> A;
   // ... Resizing and initialization
   if( isDefault( band( A, 0UL ) ) ) { ... }
   \endcode

// Optionally, it is possible to switch between strict semantics (blaze::strict) and relaxed
// semantics (blaze::relaxed):

   \code
   if( isDefault<relaxed>( band( A, 0UL ) ) ) { ... }
   \endcode
*/
template< bool RF              // Relaxation flag
        , typename MT          // Type of the sparse matrix
        , bool TF              // Transpose flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline bool isDefault( const Band<MT,TF,false,MF,CBAs...>& band )
{
   using blaze::isDefault;

   for( const auto& element : band )
      if( !isDefault<RF>( element.value() ) ) return false;
   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the invariants of the given band are intact.
// \ingroup band
//
// \param band The band to be tested.
// \return \a true in case the given band's invariants are intact, \a false otherwise.
//
// This function checks whether the invariants of the band are intact, i.e. if its state is valid.
// In case the invariants are intact, the function returns \a true, else it will return \a false.
// The following example demonstrates the use of the \a isIntact() function:

   \code
   blaze::DynamicMatrix<int,rowMajor> A;
   // ... Resizing and initialization
   if( isIntact( band( A, 0UL ) ) ) { ... }
   \endcode
*/
template< typename MT          // Type of the matrix
        , bool TF              // Transpose flag
        , bool DF              // Density flag
        , bool MF              // Multiplication flag
        , ptrdiff_t... CBAs >  // Compile time band arguments
inline bool isIntact( const Band<MT,TF,DF,MF,CBAs...>& band ) noexcept
{
   const ptrdiff_t index( band.band() );

   return ( ( index >= 0L || size_t( -index ) < band.operand().rows()    ) &&
            ( index <= 0L || size_t(  index ) < band.operand().columns() ) &&
            isIntact( band.operand() ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Backend of the isSame() function for two regular bands.
// \ingroup band
//
// \param a The first band to be tested for its state.
// \param b The second band to be tested for its state.
// \return \a true in case the two bands share a state, \a false otherwise.
//
// This backend implementation of the isSame() function handles the special case of two regular
// bands. In case both bands represent the same observable state, the function returns \a true,
// otherwise it returns \a false.
*/
template< typename MT1          // Type of the matrix of the left-hand side band
        , bool TF               // Transpose flag
        , bool DF               // Density flag
        , bool MF               // Multiplication flag
        , ptrdiff_t... CBAs1    // Compile time band arguments of the left-hand side band
        , typename MT2          // Type of the matrix of the right-hand side band
        , ptrdiff_t... CBAs2 >  // Compile time band arguments of the right-hand side band
inline auto isSame_backend( const Band<MT1,TF,DF,MF,CBAs1...>& a,
                            const Band<MT2,TF,DF,MF,CBAs2...>& b ) noexcept
   -> DisableIf_t< IsSubmatrix_v<MT1> || IsSubmatrix_v<MT2>, bool >
{
   return ( isSame( a.operand(), b.operand() ) && ( a.band() == b.band() ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Backend of the isSame() function for the left band being a band on a submatrix.
// \ingroup band
//
// \param a The first band to be tested for its state.
// \param b The second band to be tested for its state.
// \return \a true in case the two bands share a state, \a false otherwise.
//
// This backend implementation of the isSame() function handles the special case of the left
// band being a band on a submatrix. In case both bands represent the same observable state,
// the function returns \a true, otherwise it returns \a false.
*/
template< typename MT1          // Type of the submatrix of the left-hand side band
        , bool TF               // Transpose flag
        , bool DF               // Density flag
        , bool MF               // Multiplication flag
        , ptrdiff_t... CBAs1    // Compile time band arguments of the left-hand side band
        , typename MT2          // Type of the matrix of the right-hand side band
        , ptrdiff_t... CBAs2 >  // Compile time band arguments of the right-hand side band
inline auto isSame_backend( const Band<MT1,TF,DF,MF,CBAs1...>& a,
                            const Band<MT2,TF,DF,MF,CBAs2...>& b ) noexcept
   -> EnableIf_t< IsSubmatrix_v<MT1> && !IsSubmatrix_v<MT2>, bool >
{
   return ( isSame( a.operand().operand(), b.operand() ) &&
            ( a.size() == b.size() ) &&
            ( a.row() + a.operand().row() == b.row() ) &&
            ( a.column() + a.operand().column() == b.column() ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Backend of the isSame() function for the right band being a band on a submatrix.
// \ingroup band
//
// \param a The first band to be tested for its state.
// \param b The second band to be tested for its state.
// \return \a true in case the two bands share a state, \a false otherwise.
//
// This backend implementation of the isSame() function handles the special case of the right
// band being a band on a submatrix. In case both bands represent the same observable state,
// the function returns \a true, otherwise it returns \a false.
*/
template< typename MT1          // Type of the matrix of the left-hand side band
        , bool TF               // Transpose flag
        , bool DF               // Density flag
        , bool MF               // Multiplication flag
        , ptrdiff_t... CBAs1    // Compile time band arguments of the left-hand side band
        , typename MT2          // Type of the submatrix of the right-hand side band
        , ptrdiff_t... CBAs2 >  // Compile time band arguments of the right-hand side band
inline auto isSame_backend( const Band<MT1,TF,DF,MF,CBAs1...>& a,
                            const Band<MT2,TF,DF,MF,CBAs2...>& b ) noexcept
   -> EnableIf_t< !IsSubmatrix_v<MT1> && IsSubmatrix_v<MT2>, bool >
{
   return ( isSame( a.operand(), b.operand().operand() ) &&
            ( a.size() == b.size() ) &&
            ( a.row() == b.row() + b.operand().row() ) &&
            ( a.column() == b.column() + b.operand().column() ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Backend of the isSame() function for two bands on submatrices.
// \ingroup band
//
// \param a The first band to be tested for its state.
// \param b The second band to be tested for its state.
// \return \a true in case the two bands share a state, \a false otherwise.
//
// This backend implementation of the isSame() function handles the special case of both bands
// being bands on submatrices. In case both bands represent the same observable state, the function
// returns \a true, otherwise it returns \a false.
*/
template< typename MT1          // Type of the submatrix of the left-hand side band
        , bool TF               // Transpose flag
        , bool DF               // Density flag
        , bool MF               // Multiplication flag
        , ptrdiff_t... CBAs1    // Compile time band arguments of the left-hand side band
        , typename MT2          // Type of the submatrix of the right-hand side band
        , ptrdiff_t... CBAs2 >  // Compile time band arguments of the right-hand side band
inline auto isSame_backend( const Band<MT1,TF,DF,MF,CBAs1...>& a,
                            const Band<MT2,TF,DF,MF,CBAs2...>& b ) noexcept
   -> EnableIf_t< IsSubmatrix_v<MT1> && IsSubmatrix_v<MT2>, bool >
{
   return ( isSame( a.operand().operand(), b.operand().operand() ) &&
            ( a.size() == b.size() ) &&
            ( a.row() + a.operand().row() == b.row() + b.operand().row() ) &&
            ( a.column() + a.operand().column() == b.column() + b.operand().column() ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the two given bands represent the same observable state.
// \ingroup band
//
// \param a The first band to be tested for its state.
// \param b The second band to be tested for its state.
// \return \a true in case the two bands share a state, \a false otherwise.
//
// This overload of the isSame function tests if the two given bands refer to exactly the same
// range of the same matrix. In case both bands represent the same observable state, the function
// returns \a true, otherwise it returns \a false.
*/
template< typename MT1          // Type of the matrix of the left-hand side band
        , bool TF               // Transpose flag
        , bool DF               // Density flag
        , bool MF               // Multiplication flag
        , ptrdiff_t... CBAs1    // Compile time band arguments of the left-hand side band
        , typename MT2          // Type of the matrix of the right-hand side band
        , ptrdiff_t... CBAs2 >  // Compile time band arguments of the right-hand side band
inline bool isSame( const Band<MT1,TF,DF,MF,CBAs1...>& a,
                    const Band<MT2,TF,DF,MF,CBAs2...>& b ) noexcept
{
   return isSame_backend( a, b );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by setting a single element of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the element to be set.
// \param value The value to be set to the element.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
inline bool trySet( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index < band.size(), "Invalid vector access index" );

   return trySet( band.operand(), band.row()+index, band.column()+index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by adding to a single element of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the element to be modified.
// \param value The value to be added to the element.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
inline bool tryAdd( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index < band.size(), "Invalid vector access index" );

   return tryAdd( band.operand(), band.row()+index, band.column()+index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by subtracting from a single element of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the element to be modified.
// \param value The value to be subtracted from the element.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
inline bool trySub( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index < band.size(), "Invalid vector access index" );

   return trySub( band.operand(), band.row()+index, band.column()+index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by scaling a single element of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the element to be modified.
// \param value The factor for the element.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
inline bool tryMult( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index < band.size(), "Invalid vector access index" );

   return tryMult( band.operand(), band.row()+index, band.column()+index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by scaling a range of elements of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the first element of the range to be modified.
// \param size The number of elements of the range to be modified.
// \param value The factor for the elements.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
BLAZE_ALWAYS_INLINE bool
   tryMult( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, size_t size, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index <= (~band).size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + size <= (~band).size(), "Invalid range size" );

   const size_t iend( index + size );

   for( size_t i=index; i<iend; ++i ) {
      if( !tryMult( band.operand(), band.row()+i, band.column()+i, value ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by scaling a single element of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the element to be modified.
// \param value The divisor for the element.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
inline bool tryDiv( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index < band.size(), "Invalid vector access index" );

   return tryDiv( band.operand(), band.row()+index, band.column()+index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by scaling a range of elements of a band.
// \ingroup band
//
// \param band The target band.
// \param index The index of the first element of the range to be modified.
// \param size The number of elements of the range to be modified.
// \param value The divisor for the elements.
// \return \a true in case the operation would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename ET >      // Type of the element
BLAZE_ALWAYS_INLINE bool
   tryDiv( const Band<MT,TF,DF,MF,CBAs...>& band, size_t index, size_t size, const ET& value )
{
   BLAZE_INTERNAL_ASSERT( index <= (~band).size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + size <= (~band).size(), "Invalid range size" );

   const size_t iend( index + size );

   for( size_t i=index; i<iend; ++i ) {
      if( !tryDiv( band.operand(), band.row()+i, band.column()+i, value ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by the assignment of a vector to a band.
// \ingroup band
//
// \param lhs The target left-hand side band.
// \param rhs The right-hand side vector to be assigned.
// \param index The index of the first element to be modified.
// \return \a true in case the assignment would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename VT >      // Type of the right-hand side vector
inline bool tryAssign( const Band<MT,TF,DF,MF,CBAs...>& lhs,
                       const Vector<VT,TF>& rhs, size_t index )
{
   BLAZE_INTERNAL_ASSERT( index <= lhs.size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + (~rhs).size() <= lhs.size(), "Invalid vector size" );

   return tryAssign( lhs.operand(), ~rhs, lhs.band(), lhs.row()+index, lhs.column()+index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by the addition assignment of a vector to a band.
// \ingroup band
//
// \param lhs The target left-hand side band.
// \param rhs The right-hand side vector to be added.
// \param index The index of the first element to be modified.
// \return \a true in case the assignment would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename VT >      // Type of the right-hand side vector
inline bool tryAddAssign( const Band<MT,TF,DF,MF,CBAs...>& lhs,
                          const Vector<VT,TF>& rhs, size_t index )
{
   BLAZE_INTERNAL_ASSERT( index <= lhs.size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + (~rhs).size() <= lhs.size(), "Invalid vector size" );

   return tryAddAssign( lhs.operand(), ~rhs, lhs.band(), lhs.row()+index, lhs.column()+index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by the subtraction assignment of a vector to a band.
// \ingroup band
//
// \param lhs The target left-hand side band.
// \param rhs The right-hand side vector to be subtracted.
// \param index The index of the first element to be modified.
// \return \a true in case the assignment would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename VT >      // Type of the right-hand side vector
inline bool trySubAssign( const Band<MT,TF,DF,MF,CBAs...>& lhs,
                          const Vector<VT,TF>& rhs, size_t index )
{
   BLAZE_INTERNAL_ASSERT( index <= lhs.size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + (~rhs).size() <= lhs.size(), "Invalid vector size" );

   return trySubAssign( lhs.operand(), ~rhs, lhs.band(), lhs.row()+index, lhs.column()+index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by the multiplication assignment of a vector to a band.
// \ingroup band
//
// \param lhs The target left-hand side band.
// \param rhs The right-hand side vector to be multiplied.
// \param index The index of the first element to be modified.
// \return \a true in case the assignment would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename VT >      // Type of the right-hand side vector
inline bool tryMultAssign( const Band<MT,TF,DF,MF,CBAs...>& lhs,
                           const Vector<VT,TF>& rhs, size_t index )
{
   BLAZE_INTERNAL_ASSERT( index <= lhs.size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + (~rhs).size() <= lhs.size(), "Invalid vector size" );

   return tryMultAssign( lhs.operand(), ~rhs, lhs.band(), lhs.row()+index, lhs.column()+index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Predict invariant violations by the division assignment of a vector to a band.
// \ingroup band
//
// \param lhs The target left-hand side band.
// \param rhs The right-hand side vector divisor.
// \param index The index of the first element to be modified.
// \return \a true in case the assignment would be successful, \a false if not.
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT        // Type of the matrix
        , bool TF            // Transpose flag
        , bool DF            // Density flag
        , bool MF            // Multiplication flag
        , ptrdiff_t... CBAs  // Compile time band arguments
        , typename VT >      // Type of the right-hand side vector
inline bool tryDivAssign( const Band<MT,TF,DF,MF,CBAs...>& lhs,
                          const Vector<VT,TF>& rhs, size_t index )
{
   BLAZE_INTERNAL_ASSERT( index <= lhs.size(), "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + (~rhs).size() <= lhs.size(), "Invalid vector size" );

   return tryDivAssign( lhs.operand(), ~rhs, lhs.band(), lhs.row()+index, lhs.column()+index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removal of all restrictions on the data access to the given band.
// \ingroup band
//
// \param b The band to be derestricted.
// \return Band without access restrictions.
//
// This function removes all restrictions on the data access to the given band. It returns a
// band object that does provide the same interface but does not have any restrictions on the
// data access.\n
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in the violation of invariants, erroneous results and/or in compilation errors.
*/
template< typename MT    // Type of the matrix
        , bool TF        // Transpose flag
        , bool DF        // Density flag
        , bool MF        // Multiplication flag
        , ptrdiff_t I >  // Band index
inline decltype(auto) derestrict( Band<MT,TF,DF,MF,I>& b )
{
   return band<I>( derestrict( b.operand() ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removal of all restrictions on the data access to the given temporary band.
// \ingroup band
//
// \param b The temporary band to be derestricted.
// \return Band without access restrictions.
//
// This function removes all restrictions on the data access to the given temporary band. It
// returns a band object that does provide the same interface but does not have any restrictions
// on the data access.\n
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in the violation of invariants, erroneous results and/or in compilation errors.
*/
template< typename MT    // Type of the matrix
        , bool TF        // Transpose flag
        , bool DF        // Density flag
        , bool MF        // Multiplication flag
        , ptrdiff_t I >  // Band index
inline decltype(auto) derestrict( Band<MT,TF,DF,MF,I>&& b )
{
   return band<I>( derestrict( b.operand() ), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removal of all restrictions on the data access to the given band.
// \ingroup band
//
// \param b The band to be derestricted.
// \return Band without access restrictions.
//
// This function removes all restrictions on the data access to the given band. It returns a
// band object that does provide the same interface but does not have any restrictions on the
// data access.\n
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in the violation of invariants, erroneous results and/or in compilation errors.
*/
template< typename MT  // Type of the matrix
        , bool TF      // Transpose flag
        , bool DF      // Density flag
        , bool MF >    // Multiplication flag
inline decltype(auto) derestrict( Band<MT,TF,DF,MF>& b )
{
   return band( derestrict( b.operand() ), b.band(), unchecked );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removal of all restrictions on the data access to the given temporary band.
// \ingroup band
//
// \param b The temporary band to be derestricted.
// \return Band without access restrictions.
//
// This function removes all restrictions on the data access to the given temporary band. It
// returns a band object that does provide the same interface but does not have any restrictions
// on the data access.\n
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in the violation of invariants, erroneous results and/or in compilation errors.
*/
template< typename MT  // Type of the matrix
        , bool TF      // Transpose flag
        , bool DF      // Density flag
        , bool MF >    // Multiplication flag
inline decltype(auto) derestrict( Band<MT,TF,DF,MF>&& b )
{
   return band( derestrict( b.operand() ), b.band(), unchecked );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SIZE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool TF, bool DF, bool MF, ptrdiff_t I >
struct Size< Band<MT,TF,DF,MF,I>, 0UL >
   : public If_t< ( Size_v<MT,0UL> >= 0L && Size_v<MT,1UL> >= 0L )
                , Minimum< PtrdiffT< Size_v<MT,0UL> - ( I >= 0L ? 0L : -I ) >
                         , PtrdiffT< Size_v<MT,1UL> - ( I >= 0L ? I : 0L ) > >
                , PtrdiffT<-1L> >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MAXSIZE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool TF, bool DF, bool MF, ptrdiff_t I >
struct MaxSize< Band<MT,TF,DF,MF,I>, 0UL >
   : public If_t< ( MaxSize_v<MT,0UL> >= 0L && MaxSize_v<MT,1UL> >= 0L )
                , Minimum< PtrdiffT< MaxSize_v<MT,0UL> - ( I >= 0L ? 0L : -I ) >
                         , PtrdiffT< MaxSize_v<MT,1UL> - ( I >= 0L ? I : 0L ) > >
                , PtrdiffT<-1L> >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISRESTRICTED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool TF, bool DF, bool MF, ptrdiff_t... CBAs >
struct IsRestricted< Band<MT,TF,DF,MF,CBAs...> >
   : public IsRestricted<MT>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  HASCONSTDATAACCESS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool TF, bool MF, ptrdiff_t... CBAs >
struct HasConstDataAccess< Band<MT,TF,true,MF,CBAs...> >
   : public HasConstDataAccess<MT>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  HASMUTABLEDATAACCESS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool TF, bool MF, ptrdiff_t... CBAs >
struct HasMutableDataAccess< Band<MT,TF,true,MF,CBAs...> >
   : public HasMutableDataAccess<MT>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISOPPOSEDVIEW SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool TF, bool DF, bool MF, ptrdiff_t... CBAs >
struct IsOpposedView< Band<MT,TF,DF,MF,CBAs...> >
   : public TrueType
{};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
