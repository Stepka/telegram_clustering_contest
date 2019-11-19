//=================================================================================================
/*!
//  \file blaze/math/dense/StaticMatrix.h
//  \brief Header file for the implementation of a fixed-size matrix
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

#ifndef _BLAZE_MATH_DENSE_STATICMATRIX_H_
#define _BLAZE_MATH_DENSE_STATICMATRIX_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <utility>
#include "../../math/Aliases.h"
#include "../../math/constraints/Diagonal.h"
#include "../../math/constraints/Symmetric.h"
#include "../../math/dense/DenseIterator.h"
#include "../../math/Exception.h"
#include "../../math/expressions/DenseMatrix.h"
#include "../../math/expressions/SparseMatrix.h"
#include "../../math/Forward.h"
#include "../../math/InitializerList.h"
#include "../../math/shims/Clear.h"
#include "../../math/shims/Conjugate.h"
#include "../../math/shims/IsDefault.h"
#include "../../math/shims/NextMultiple.h"
#include "../../math/shims/Serial.h"
#include "../../math/SIMD.h"
#include "../../math/traits/AddTrait.h"
#include "../../math/traits/ColumnsTrait.h"
#include "../../math/traits/DivTrait.h"
#include "../../math/traits/MapTrait.h"
#include "../../math/traits/MultTrait.h"
#include "../../math/traits/RowsTrait.h"
#include "../../math/traits/SchurTrait.h"
#include "../../math/traits/SubmatrixTrait.h"
#include "../../math/traits/SubTrait.h"
#include "../../math/typetraits/HasConstDataAccess.h"
#include "../../math/typetraits/HasMutableDataAccess.h"
#include "../../math/typetraits/HasSIMDAdd.h"
#include "../../math/typetraits/HasSIMDMult.h"
#include "../../math/typetraits/HasSIMDSub.h"
#include "../../math/typetraits/HighType.h"
#include "../../math/typetraits/IsAligned.h"
#include "../../math/typetraits/IsColumnMajorMatrix.h"
#include "../../math/typetraits/IsColumnVector.h"
#include "../../math/typetraits/IsContiguous.h"
#include "../../math/typetraits/IsDenseMatrix.h"
#include "../../math/typetraits/IsDiagonal.h"
#include "../../math/typetraits/IsLower.h"
#include "../../math/typetraits/IsMatrix.h"
#include "../../math/typetraits/IsPadded.h"
#include "../../math/typetraits/IsRowMajorMatrix.h"
#include "../../math/typetraits/IsRowVector.h"
#include "../../math/typetraits/IsSIMDCombinable.h"
#include "../../math/typetraits/IsSparseMatrix.h"
#include "../../math/typetraits/IsSquare.h"
#include "../../math/typetraits/IsStatic.h"
#include "../../math/typetraits/IsStrictlyLower.h"
#include "../../math/typetraits/IsStrictlyUpper.h"
#include "../../math/typetraits/IsSymmetric.h"
#include "../../math/typetraits/IsUpper.h"
#include "../../math/typetraits/LowType.h"
#include "../../math/typetraits/MaxSize.h"
#include "../../math/typetraits/Size.h"
#include "../../math/typetraits/StorageOrder.h"
#include "../../system/Inline.h"
#include "../../system/Optimizations.h"
#include "../../system/StorageOrder.h"
#include "../../system/TransposeFlag.h"
#include "../../util/algorithms/Max.h"
#include "../../util/algorithms/Min.h"
#include "../../util/AlignedArray.h"
#include "../../util/AlignmentCheck.h"
#include "../../util/Assert.h"
#include "../../util/constraints/Const.h"
#include "../../util/constraints/FloatingPoint.h"
#include "../../util/constraints/Pointer.h"
#include "../../util/constraints/Reference.h"
#include "../../util/constraints/Vectorizable.h"
#include "../../util/constraints/Volatile.h"
#include "../../util/DisableIf.h"
#include "../../util/EnableIf.h"
#include "../../util/FalseType.h"
#include "../../util/IntegralConstant.h"
#include "../../util/Memory.h"
#include "../../util/mpl/PtrdiffT.h"
#include "../../util/StaticAssert.h"
#include "../../util/TrueType.h"
#include "../../util/Types.h"
#include "../../util/typetraits/AlignmentOf.h"
#include "../../util/typetraits/IsNumeric.h"
#include "../../util/typetraits/IsSame.h"
#include "../../util/typetraits/IsVectorizable.h"
#include "../../util/typetraits/RemoveConst.h"
#include "../../util/Unused.h"


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup static_matrix StaticMatrix
// \ingroup dense_matrix
*/
/*!\brief Efficient implementation of a fixed-sized matrix.
// \ingroup static_matrix
//
// The StaticMatrix class template is the representation of a fixed-size matrix with statically
// allocated elements of arbitrary type. The type of the elements, the number of rows and columns
// and the storage order of the matrix can be specified via the four template parameters:

   \code
   template< typename Type, size_t M, size_t N, bool SO >
   class StaticMatrix;
   \endcode

//  - Type: specifies the type of the matrix elements. StaticMatrix can be used with any
//          non-cv-qualified, non-reference, non-pointer element type.
//  - M   : specifies the total number of rows of the matrix.
//  - N   : specifies the total number of columns of the matrix. Note that it is expected
//          that StaticMatrix is only used for tiny and small matrices.
//  - SO  : specifies the storage order (blaze::rowMajor, blaze::columnMajor) of the matrix.
//          The default value is blaze::rowMajor.
//
// Depending on the storage order, the matrix elements are either stored in a row-wise fashion
// or in a column-wise fashion. Given the 2x3 matrix

                          \f[\left(\begin{array}{*{3}{c}}
                          1 & 2 & 3 \\
                          4 & 5 & 6 \\
                          \end{array}\right)\f]\n

// in case of row-major order the elements are stored in the order

                          \f[\left(\begin{array}{*{6}{c}}
                          1 & 2 & 3 & 4 & 5 & 6. \\
                          \end{array}\right)\f]

// In case of column-major order the elements are stored in the order

                          \f[\left(\begin{array}{*{6}{c}}
                          1 & 4 & 2 & 5 & 3 & 6. \\
                          \end{array}\right)\f]

// The use of StaticMatrix is very natural and intuitive. All operations (addition, subtraction,
// multiplication, scaling, ...) can be performed on all possible combinations of row-major and
// column-major dense and sparse matrices with fitting element types. The following example gives
// an impression of the use of StaticMatrix:

   \code
   using blaze::StaticMatrix;
   using blaze::CompressedMatrix;
   using blaze::rowMajor;
   using blaze::columnMajor;

   StaticMatrix<double,2UL,3UL,rowMajor> A;   // Default constructed, non-initialized, row-major 2x3 matrix
   A(0,0) = 1.0; A(0,1) = 2.0; A(0,2) = 3.0;  // Initialization of the first row
   A(1,0) = 4.0; A(1,1) = 5.0; A(1,2) = 6.0;  // Initialization of the second row

   DynamicMatrix<float,2UL,3UL,columnMajor> B;  // Default constructed column-major single precision 2x3 matrix
   B(0,0) = 1.0; B(0,1) = 3.0; B(0,2) = 5.0;    // Initialization of the first row
   B(1,0) = 2.0; B(1,1) = 4.0; B(1,2) = 6.0;    // Initialization of the second row

   CompressedMatrix<float>     C( 2, 3 );  // Empty row-major sparse single precision matrix
   StaticMatrix<float,3UL,2UL> D( 4.0F );  // Directly, homogeneously initialized single precision 3x2 matrix

   StaticMatrix<double,2UL,3UL,rowMajor>    E( A );  // Creation of a new row-major matrix as a copy of A
   StaticMatrix<double,2UL,2UL,columnMajor> F;       // Creation of a default column-major matrix

   E = A + B;     // Matrix addition and assignment to a row-major matrix
   F = A - C;     // Matrix subtraction and assignment to a column-major matrix
   F = A * D;     // Matrix multiplication between two matrices of different element types

   A *= 2.0;      // In-place scaling of matrix A
   E  = 2.0 * B;  // Scaling of matrix B
   E  = B * 2.0;  // Scaling of matrix B

   E += A - B;    // Addition assignment
   E -= A + C;    // Subtraction assignment
   F *= A * D;    // Multiplication assignment
   \endcode
*/
template< typename Type                    // Data type of the matrix
        , size_t M                         // Number of rows
        , size_t N                         // Number of columns
        , bool SO = defaultStorageOrder >  // Storage order
class StaticMatrix
   : public DenseMatrix< StaticMatrix<Type,M,N,SO>, SO >
{
 private:
   //**********************************************************************************************
   //! The number of elements packed within a single SIMD vector.
   static constexpr size_t SIMDSIZE = SIMDTrait<Type>::size;

   //! Alignment adjustment.
   static constexpr size_t NN = ( usePadding ? nextMultiple( N, SIMDSIZE ) : N );

   //! Compilation switch for the choice of alignment.
   static constexpr bool align = ( usePadding || NN % SIMDSIZE == 0UL );
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   using This          = StaticMatrix<Type,M,N,SO>;   //!< Type of this StaticMatrix instance.
   using BaseType      = DenseMatrix<This,SO>;        //!< Base type of this StaticMatrix instance.
   using ResultType    = This;                        //!< Result type for expression template evaluations.
   using OppositeType  = StaticMatrix<Type,M,N,!SO>;  //!< Result type with opposite storage order for expression template evaluations.
   using TransposeType = StaticMatrix<Type,N,M,!SO>;  //!< Transpose type for expression template evaluations.
   using ElementType   = Type;                        //!< Type of the matrix elements.
   using SIMDType      = SIMDTrait_t<ElementType>;    //!< SIMD type of the matrix elements.
   using ReturnType    = const Type&;                 //!< Return type for expression template evaluations.
   using CompositeType = const This&;                 //!< Data type for composite expression templates.

   using Reference      = Type&;        //!< Reference to a non-constant matrix value.
   using ConstReference = const Type&;  //!< Reference to a constant matrix value.
   using Pointer        = Type*;        //!< Pointer to a non-constant matrix value.
   using ConstPointer   = const Type*;  //!< Pointer to a constant matrix value.

   using Iterator      = DenseIterator<Type,align>;        //!< Iterator over non-constant elements.
   using ConstIterator = DenseIterator<const Type,align>;  //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Rebind struct definition********************************************************************
   /*!\brief Rebind mechanism to obtain a StaticMatrix with different data/element type.
   */
   template< typename NewType >  // Data type of the other matrix
   struct Rebind {
      using Other = StaticMatrix<NewType,M,N,SO>;  //!< The type of the other StaticMatrix.
   };
   //**********************************************************************************************

   //**Resize struct definition********************************************************************
   /*!\brief Resize mechanism to obtain a StaticMatrix with different fixed dimensions.
   */
   template< size_t NewM    // Number of rows of the other matrix
           , size_t NewN >  // Number of columns of the other matrix
   struct Resize {
      using Other = StaticMatrix<Type,NewM,NewN,SO>;  //!< The type of the other StaticMatrix.
   };
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for SIMD optimization.
   /*! The \a simdEnabled compilation flag indicates whether expressions the matrix is involved
       in can be optimized via SIMD operations. In case the element type of the matrix is a
       vectorizable data type, the \a simdEnabled compilation flag is set to \a true, otherwise
       it is set to \a false. */
   static constexpr bool simdEnabled = IsVectorizable_v<Type>;

   //! Compilation flag for SMP assignments.
   /*! The \a smpAssignable compilation flag indicates whether the matrix can be used in SMP
       (shared memory parallel) assignments (both on the left-hand and right-hand side of the
       assignment). */
   static constexpr bool smpAssignable = false;
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline StaticMatrix();
   explicit inline StaticMatrix( const Type& init );
   explicit inline StaticMatrix( initializer_list< initializer_list<Type> > list );

   template< typename Other >
   explicit inline StaticMatrix( size_t m, size_t n, const Other* array );

   template< typename Other, size_t Rows, size_t Cols >
   explicit inline StaticMatrix( const Other (&array)[Rows][Cols] );

                                        inline StaticMatrix( const StaticMatrix& m );
   template< typename Other, bool SO2 > inline StaticMatrix( const StaticMatrix<Other,M,N,SO2>& m );
   template< typename MT   , bool SO2 > inline StaticMatrix( const Matrix<MT,SO2>& m );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j ) noexcept;
   inline ConstReference operator()( size_t i, size_t j ) const noexcept;
   inline Reference      at( size_t i, size_t j );
   inline ConstReference at( size_t i, size_t j ) const;
   inline Pointer        data  () noexcept;
   inline ConstPointer   data  () const noexcept;
   inline Pointer        data  ( size_t i ) noexcept;
   inline ConstPointer   data  ( size_t i ) const noexcept;
   inline Iterator       begin ( size_t i ) noexcept;
   inline ConstIterator  begin ( size_t i ) const noexcept;
   inline ConstIterator  cbegin( size_t i ) const noexcept;
   inline Iterator       end   ( size_t i ) noexcept;
   inline ConstIterator  end   ( size_t i ) const noexcept;
   inline ConstIterator  cend  ( size_t i ) const noexcept;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   inline StaticMatrix& operator=( const Type& set );
   inline StaticMatrix& operator=( initializer_list< initializer_list<Type> > list );

   template< typename Other, size_t Rows, size_t Cols >
   inline StaticMatrix& operator=( const Other (&array)[Rows][Cols] );

                                        inline StaticMatrix& operator= ( const StaticMatrix& rhs );
   template< typename Other, bool SO2 > inline StaticMatrix& operator= ( const StaticMatrix<Other,M,N,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator= ( const Matrix<MT,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator+=( const Matrix<MT,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator-=( const Matrix<MT,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator%=( const Matrix<MT,SO2>& rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   static inline constexpr size_t rows() noexcept;
   static inline constexpr size_t columns() noexcept;
   static inline constexpr size_t spacing() noexcept;
   static inline constexpr size_t capacity() noexcept;
          inline           size_t capacity( size_t i ) const noexcept;
          inline           size_t nonZeros() const;
          inline           size_t nonZeros( size_t i ) const;
          inline           void   reset();
          inline           void   reset( size_t i );
          inline           void   swap( StaticMatrix& m ) noexcept;
   //@}
   //**********************************************************************************************

   //**Numeric functions***************************************************************************
   /*!\name Numeric functions */
   //@{
   inline StaticMatrix& transpose();
   inline StaticMatrix& ctranspose();

   template< typename Other > inline StaticMatrix& scale( const Other& scalar );
   //@}
   //**********************************************************************************************

   //**Memory functions****************************************************************************
   /*!\name Memory functions */
   //@{
   static inline void* operator new  ( std::size_t size );
   static inline void* operator new[]( std::size_t size );
   static inline void* operator new  ( std::size_t size, const std::nothrow_t& );
   static inline void* operator new[]( std::size_t size, const std::nothrow_t& );

   static inline void operator delete  ( void* ptr );
   static inline void operator delete[]( void* ptr );
   static inline void operator delete  ( void* ptr, const std::nothrow_t& );
   static inline void operator delete[]( void* ptr, const std::nothrow_t& );
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        IsRowMajorMatrix_v<MT> );
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedAddAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        HasSIMDAdd_v< Type, ElementType_t<MT> > &&
        IsRowMajorMatrix_v<MT> &&
        !IsDiagonal_v<MT> );
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedSubAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        HasSIMDSub_v< Type, ElementType_t<MT> > &&
        IsRowMajorMatrix_v<MT> &&
        !IsDiagonal_v<MT> );
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedSchurAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        HasSIMDMult_v< Type, ElementType_t<MT> > &&
        IsRowMajorMatrix_v<MT> );
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Debugging functions*************************************************************************
   /*!\name Debugging functions */
   //@{
   inline bool isIntact() const noexcept;
   //@}
   //**********************************************************************************************

   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool canAlias ( const Other* alias ) const noexcept;
   template< typename Other > inline bool isAliased( const Other* alias ) const noexcept;

   static inline constexpr bool isAligned() noexcept;

   BLAZE_ALWAYS_INLINE SIMDType load ( size_t i, size_t j ) const noexcept;
   BLAZE_ALWAYS_INLINE SIMDType loada( size_t i, size_t j ) const noexcept;
   BLAZE_ALWAYS_INLINE SIMDType loadu( size_t i, size_t j ) const noexcept;

   BLAZE_ALWAYS_INLINE void store ( size_t i, size_t j, const SIMDType& value ) noexcept;
   BLAZE_ALWAYS_INLINE void storea( size_t i, size_t j, const SIMDType& value ) noexcept;
   BLAZE_ALWAYS_INLINE void storeu( size_t i, size_t j, const SIMDType& value ) noexcept;
   BLAZE_ALWAYS_INLINE void stream( size_t i, size_t j, const SIMDType& value ) noexcept;

   template< typename MT, bool SO2 >
   inline auto assign( const DenseMatrix<MT,SO2>& rhs ) -> DisableIf_t< VectorizedAssign_v<MT> >;

   template< typename MT, bool SO2 >
   inline auto assign( const DenseMatrix<MT,SO2>& rhs ) -> EnableIf_t< VectorizedAssign_v<MT> >;

   template< typename MT > inline void assign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void assign( const SparseMatrix<MT,!SO>& rhs );

   template< typename MT, bool SO2 >
   inline auto addAssign( const DenseMatrix<MT,SO2>& rhs ) -> DisableIf_t< VectorizedAddAssign_v<MT> >;

   template< typename MT, bool SO2 >
   inline auto addAssign( const DenseMatrix<MT,SO2>& rhs ) -> EnableIf_t< VectorizedAddAssign_v<MT> >;

   template< typename MT > inline void addAssign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void addAssign( const SparseMatrix<MT,!SO>& rhs );

   template< typename MT, bool SO2 >
   inline auto subAssign( const DenseMatrix<MT,SO2>& rhs ) -> DisableIf_t< VectorizedSubAssign_v<MT> >;

   template< typename MT, bool SO2 >
   inline auto subAssign( const DenseMatrix<MT,SO2>& rhs ) -> EnableIf_t< VectorizedSubAssign_v<MT> >;

   template< typename MT > inline void subAssign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void subAssign( const SparseMatrix<MT,!SO>& rhs );

   template< typename MT, bool SO2 >
   inline auto schurAssign( const DenseMatrix<MT,SO2>& rhs ) -> DisableIf_t< VectorizedSchurAssign_v<MT> >;

   template< typename MT, bool SO2 >
   inline auto schurAssign( const DenseMatrix<MT,SO2>& rhs ) -> EnableIf_t< VectorizedSchurAssign_v<MT> >;

   template< typename MT > inline void schurAssign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void schurAssign( const SparseMatrix<MT,!SO>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*! \cond BLAZE_INTERNAL */
   inline void transpose ( TrueType  );
   inline void transpose ( FalseType );
   inline void ctranspose( TrueType  );
   inline void ctranspose( FalseType );
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   //! Alignment of the data elements.
   static constexpr size_t Alignment =
      ( align ? AlignmentOf_v<Type> : std::alignment_of<Type>::value );

   //! Type of the aligned storage.
   using AlignedStorage = AlignedArray<Type,M*NN,Alignment>;
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   AlignedStorage v_;  //!< The statically allocated matrix elements.
                       /*!< Access to the matrix elements is gained via the function call
                            operator. In case of row-major order the memory layout of the
                            elements is
                            \f[\left(\begin{array}{*{5}{c}}
                            0            & 1             & 2             & \cdots & N-1         \\
                            N            & N+1           & N+2           & \cdots & 2 \cdot N-1 \\
                            \vdots       & \vdots        & \vdots        & \ddots & \vdots      \\
                            M \cdot N-N  & M \cdot N-N+1 & M \cdot N-N+2 & \cdots & M \cdot N-1 \\
                            \end{array}\right)\f]. */
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   BLAZE_STATIC_ASSERT( !usePadding || NN % SIMDSIZE == 0UL );
   BLAZE_STATIC_ASSERT( NN >= N );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The default constructor for StaticMatrix.
//
// All matrix elements are initialized to the default value (i.e. 0 for integral data types).
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix()
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   if( IsNumeric_v<Type> ) {
      for( size_t i=0UL; i<M*NN; ++i )
         v_[i] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a homogenous initialization of all elements.
//
// \param init Initial value for all matrix elements.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& init )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = init;

      for( size_t j=N; j<NN; ++j )
         v_[i*NN+j] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief List initialization of all matrix elements.
//
// \param list The initializer list.
// \exception std::invalid_argument Invalid setup of static matrix.
//
// This constructor provides the option to explicitly initialize the elements of the matrix by
// means of an initializer list:

   \code
   using blaze::rowMajor;

   blaze::StaticMatrix<int,3,3,rowMajor> A{ { 1, 2, 3 },
                                            { 4, 5 },
                                            { 7, 8, 9 } };
   \endcode

// The matrix elements are initialized by the values of the given initializer list. Missing values
// are initialized as default (as e.g. the value 6 in the example). Note that in case the size of
// the top-level initializer list does not match the number of rows of the matrix or the size of
// any nested list exceeds the number of columns, a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( initializer_list< initializer_list<Type> > list )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   if( list.size() != M || determineColumns( list ) > N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid setup of static matrix" );
   }

   size_t i( 0UL );

   for( const auto& rowList : list ) {
      std::fill( std::copy( rowList.begin(), rowList.end(), v_+i*NN ), v_+(i+1UL)*NN, Type() );
      ++i;
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Array initialization of all matrix elements.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
// \param array Dynamic array for the initialization.
//
// This constructor offers the option to directly initialize the elements of the matrix with
// a dynamic array:

   \code
   using blaze::rowMajor;

   int* array = new int[6];
   // ... Initialization of the dynamic array
   blaze::StaticMatrix<int,3,4,rowMajor> v( array, 2UL, 3UL );
   delete[] array;
   \endcode

// The matrix is initialized with the values from the given array. Missing values are initialized
// with default values. In case the specified number of rows and/or columns exceeds the maximum
// number of rows/column of the static matrix (i.e. \a m is larger than M or \a n is larger than
// N), a \a std::invalid_argument exception is thrown.\n
// Note that it is expected that the given \a array has at least \a m by \a n elements. Providing
// an array with less elements results in undefined behavior!
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the initialization array
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( size_t m, size_t n, const Other* array )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   if( m > M || n > N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid setup of static matrix" );
   }

   for( size_t i=0UL; i<m; ++i ) {
      for( size_t j=0UL; j<n; ++j )
         v_[i*NN+j] = array[i*n+j];

      if( IsNumeric_v<Type> ) {
         for( size_t j=n; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }

   if( IsNumeric_v<Type> ) {
      for( size_t i=m; i<M; ++i ) {
         for( size_t j=0UL; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Array initialization of all matrix elements.
//
// \param array \f$ M \times N \f$ dimensional array for the initialization.
//
// This constructor offers the option to directly initialize the elements of the matrix with
// a static array:

   \code
   using blaze::rowMajor;

   const int init[3][3] = { { 1, 2, 3 },
                            { 4, 5 },
                            { 7, 8, 9 } };
   blaze::StaticMatrix<int,3,3,rowMajor> A( init );
   \endcode

// The matrix is initialized with the values from the given array. Missing values are initialized
// with default values (as e.g. the value 6 in the example).
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N        // Number of columns
        , bool SO >       // Storage order
template< typename Other  // Data type of the initialization array
        , size_t Rows     // Number of rows of the initialization array
        , size_t Cols >   // Number of columns of the initialization array
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Other (&array)[Rows][Cols] )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );
   BLAZE_STATIC_ASSERT( Rows == M && Cols == N );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = array[i][j];

      for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief The copy constructor for StaticMatrix.
//
// \param m Matrix to be copied.
//
// The copy constructor is explicitly defined in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const StaticMatrix& m )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   for( size_t i=0UL; i<M*NN; ++i )
      v_[i] = m.v_[i];

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different StaticMatrix instances.
//
// \param m Matrix to be copied.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N        // Number of columns
        , bool SO >       // Storage order
template< typename Other  // Data type of the foreign matrix
        , bool SO2 >      // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const StaticMatrix<Other,M,N,SO2>& m )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = m(i,j);

      for( size_t j=N; j<NN; ++j )
         v_[i*NN+j] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different matrices.
//
// \param m Matrix to be copied.
// \exception std::invalid_argument Invalid setup of static matrix.
//
// This constructor initializes the static matrix from the given matrix. In case the size of
// the given matrix does not match the size of the static matrix (i.e. the number of rows is
// not M or the number of columns is not N), a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the foreign matrix
        , bool SO2 >     // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Matrix<MT,SO2>& m )
   : v_()  // The statically allocated matrix elements
{
   using blaze::assign;

   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || NN == N );

   if( (~m).rows() != M || (~m).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid setup of static matrix" );
   }

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=( IsSparseMatrix_v<MT> ? 0UL : N ); j<NN; ++j ) {
         v_[i*NN+j] = Type();
      }
   }

   assign( *this, ~m );

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference to the accessed value.
//
// This function only performs an index check in case BLAZE_USER_ASSERT() is active. In contrast,
// the at() function is guaranteed to perform a check of the given access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::Reference
   StaticMatrix<Type,M,N,SO>::operator()( size_t i, size_t j ) noexcept
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i*NN+j];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference-to-const to the accessed value.
//
// This function only performs an index check in case BLAZE_USER_ASSERT() is active. In contrast,
// the at() function is guaranteed to perform a check of the given access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstReference
   StaticMatrix<Type,M,N,SO>::operator()( size_t i, size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i*NN+j];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checked access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
// \exception std::out_of_range Invalid matrix access index.
//
// In contrast to the subscript operator this function always performs a check of the given
// access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::Reference
   StaticMatrix<Type,M,N,SO>::at( size_t i, size_t j )
{
   if( i >= M ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid row access index" );
   }
   if( j >= N ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid column access index" );
   }
   return (*this)(i,j);
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checked access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
// \exception std::out_of_range Invalid matrix access index.
//
// In contrast to the subscript operator this function always performs a check of the given
// access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstReference
   StaticMatrix<Type,M,N,SO>::at( size_t i, size_t j ) const
{
   if( i >= M ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid row access index" );
   }
   if( j >= N ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid column access index" );
   }
   return (*this)(i,j);
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the static matrix. Note that you
// can NOT assume that all matrix elements lie adjacent to each other! The static matrix may
// use techniques such as padding to improve the alignment of the data. Whereas the number of
// elements within a row/column are given by the \c rows() and \c columns() member functions,
// respectively, the total number of elements including padding is given by the \c spacing()
// member function.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::Pointer
   StaticMatrix<Type,M,N,SO>::data() noexcept
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the static matrix. Note that you
// can NOT assume that all matrix elements lie adjacent to each other! The static matrix may
// use techniques such as padding to improve the alignment of the data. Whereas the number of
// elements within a row/column are given by the \c rows() and \c columns() member functions,
// respectively, the total number of elements including padding is given by the \c spacing()
// member function.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstPointer
   StaticMatrix<Type,M,N,SO>::data() const noexcept
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements of row/column \a i.
//
// \param i The row/column index.
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage for the elements in row/column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::Pointer
   StaticMatrix<Type,M,N,SO>::data( size_t i ) noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return v_ + i*NN;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements of row/column \a i.
//
// \param i The row/column index.
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage for the elements in row/column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstPointer
   StaticMatrix<Type,M,N,SO>::data( size_t i ) const noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return v_ + i*NN;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first element of row/column \a i.
//
// This function returns a row/column iterator to the first element of row/column \a i. In case
// the storage order is set to \a rowMajor the function returns an iterator to the first element
// of row \a i, in case the storage flag is set to \a columnMajor the function returns an iterator
// to the first element of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::Iterator
   StaticMatrix<Type,M,N,SO>::begin( size_t i ) noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return Iterator( v_ + i*NN );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first element of row/column \a i.
//
// This function returns a row/column iterator to the first element of row/column \a i. In case
// the storage order is set to \a rowMajor the function returns an iterator to the first element
// of row \a i, in case the storage flag is set to \a columnMajor the function returns an iterator
// to the first element of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstIterator
   StaticMatrix<Type,M,N,SO>::begin( size_t i ) const noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return ConstIterator( v_ + i*NN );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first element of row/column \a i.
//
// This function returns a row/column iterator to the first element of row/column \a i. In case
// the storage order is set to \a rowMajor the function returns an iterator to the first element
// of row \a i, in case the storage flag is set to \a columnMajor the function returns an iterator
// to the first element of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstIterator
   StaticMatrix<Type,M,N,SO>::cbegin( size_t i ) const noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return ConstIterator( v_ + i*NN );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last element of row/column \a i.
//
// This function returns an row/column iterator just past the last element of row/column \a i.
// In case the storage order is set to \a rowMajor the function returns an iterator just past
// the last element of row \a i, in case the storage flag is set to \a columnMajor the function
// returns an iterator just past the last element of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::Iterator
   StaticMatrix<Type,M,N,SO>::end( size_t i ) noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return Iterator( v_ + i*NN + N );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last element of row/column \a i.
//
// This function returns an row/column iterator just past the last element of row/column \a i.
// In case the storage order is set to \a rowMajor the function returns an iterator just past
// the last element of row \a i, in case the storage flag is set to \a columnMajor the function
// returns an iterator just past the last element of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstIterator
   StaticMatrix<Type,M,N,SO>::end( size_t i ) const noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return ConstIterator( v_ + i*NN + N );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last element of row/column \a i.
//
// This function returns an row/column iterator just past the last element of row/column \a i.
// In case the storage order is set to \a rowMajor the function returns an iterator just past
// the last element of row \a i, in case the storage flag is set to \a columnMajor the function
// returns an iterator just past the last element of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::ConstIterator
   StaticMatrix<Type,M,N,SO>::cend( size_t i ) const noexcept
{
   BLAZE_USER_ASSERT( i < M, "Invalid dense matrix row access index" );
   return ConstIterator( v_ + i*NN + N );
}
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Homogenous assignment to all matrix elements.
//
// \param set Scalar value to be assigned to all matrix elements.
// \return Reference to the assigned matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const Type& set )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = set;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief List assignment to all matrix elements.
//
// \param list The initializer list.
// \exception std::invalid_argument Invalid assignment to static matrix.
//
// This assignment operator offers the option to directly assign to all elements of the matrix
// by means of an initializer list:

   \code
   using blaze::rowMajor;

   blaze::StaticMatrix<int,3,3,rowMajor> A;
   A = { { 1, 2, 3 },
         { 4, 5 },
         { 7, 8, 9 } };
   \endcode

// The matrix elements are assigned the values from the given initializer list. Missing values
// are initialized as default (as e.g. the value 6 in the example). Note that in case the size
// of the top-level initializer list does not match the number of rows of the matrix or the size
// of any nested list exceeds the number of columns, a \a std::invalid_argument exception is
// thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>&
   StaticMatrix<Type,M,N,SO>::operator=( initializer_list< initializer_list<Type> > list )
{
   if( list.size() != M || determineColumns( list ) > N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid assignment to static matrix" );
   }

   size_t i( 0UL );

   for( const auto& rowList : list ) {
      std::fill( std::copy( rowList.begin(), rowList.end(), v_+i*NN ), v_+(i+1UL)*NN, Type() );
      ++i;
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Array assignment to all matrix elements.
//
// \param array \f$ M \times N \f$ dimensional array for the assignment.
// \return Reference to the assigned matrix.
//
// This assignment operator offers the option to directly set all elements of the matrix:

   \code
   using blaze::rowMajor;

   const int init[3][3] = { { 1, 2, 3 },
                            { 4, 5 },
                            { 7, 8, 9 } };
   blaze::StaticMatrix<int,3UL,3UL,rowMajor> A;
   A = init;
   \endcode

// The matrix is assigned the values from the given array. Missing values are initialized with
// default values (as e.g. the value 6 in the example).
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N        // Number of columns
        , bool SO >       // Storage order
template< typename Other  // Data type of the initialization array
        , size_t Rows     // Number of rows of the initialization array
        , size_t Cols >   // Number of columns of the initialization array
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const Other (&array)[Rows][Cols] )
{
   BLAZE_STATIC_ASSERT( Rows == M && Cols == N );

   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = array[i][j];

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for StaticMatrix.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
//
// Explicit definition of a copy assignment operator for performance reasons.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const StaticMatrix& rhs )
{
   using blaze::assign;

   assign( *this, ~rhs );

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different StaticMatrix instances.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N        // Number of columns
        , bool SO >       // Storage order
template< typename Other  // Data type of the foreign matrix
        , bool SO2 >      // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,SO>&
   StaticMatrix<Type,M,N,SO>::operator=( const StaticMatrix<Other,M,N,SO2>& rhs )
{
   using blaze::assign;

   assign( *this, ~rhs );

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different matrices.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
// \exception std::invalid_argument Invalid assignment to static matrix.
//
// This constructor initializes the matrix as a copy of the given matrix. In case the
// number of rows of the given matrix is not M or the number of columns is not N, a
// \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const Matrix<MT,SO2>& rhs )
{
   using blaze::assign;

   using TT = decltype( trans( *this ) );
   using CT = decltype( ctrans( *this ) );
   using IT = decltype( inv( *this ) );

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid assignment to static matrix" );
   }

   if( IsSame_v<MT,TT> && (~rhs).isAliased( this ) ) {
      transpose( typename IsSquare<This>::Type() );
   }
   else if( IsSame_v<MT,CT> && (~rhs).isAliased( this ) ) {
      ctranspose( typename IsSquare<This>::Type() );
   }
   else if( !IsSame_v<MT,IT> && (~rhs).canAlias( this ) ) {
      StaticMatrix tmp( ~rhs );
      assign( *this, tmp );
   }
   else {
      if( IsSparseMatrix_v<MT> )
         reset();
      assign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Addition assignment operator for the addition of a matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix to be added to the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator+=( const Matrix<MT,SO2>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   if( (~rhs).canAlias( this ) ) {
      const ResultType_t<MT> tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix to be subtracted from the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator-=( const Matrix<MT,SO2>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   if( (~rhs).canAlias( this ) ) {
      const ResultType_t<MT> tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Schur product assignment operator for the multiplication of a matrix (\f$ A\circ=B \f$).
//
// \param rhs The right-hand side matrix for the Schur product.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator%=( const Matrix<MT,SO2>& rhs )
{
   using blaze::schurAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   if( (~rhs).canAlias( this ) ) {
      const ResultType_t<MT> tmp( ~rhs );
      schurAssign( *this, tmp );
   }
   else {
      schurAssign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns the current number of rows of the matrix.
//
// \return The number of rows of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline constexpr size_t StaticMatrix<Type,M,N,SO>::rows() noexcept
{
   return M;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the current number of columns of the matrix.
//
// \return The number of columns of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline constexpr size_t StaticMatrix<Type,M,N,SO>::columns() noexcept
{
   return N;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the spacing between the beginning of two rows.
//
// \return The spacing between the beginning of two rows.
//
// This function returns the spacing between the beginning of two rows, i.e. the total number
// of elements of a row.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline constexpr size_t StaticMatrix<Type,M,N,SO>::spacing() noexcept
{
   return NN;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the maximum capacity of the matrix.
//
// \return The capacity of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline constexpr size_t StaticMatrix<Type,M,N,SO>::capacity() noexcept
{
   return M*NN;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the current capacity of the specified row/column.
//
// \param i The index of the row/column.
// \return The current capacity of row/column \a i.
//
// This function returns the current capacity of the specified row/column. In case the
// storage order is set to \a rowMajor the function returns the capacity of row \a i,
// in case the storage flag is set to \a columnMajor the function returns the capacity
// of column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::capacity( size_t i ) const noexcept
{
   UNUSED_PARAMETER( i );

   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );

   return NN;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the total number of non-zero elements in the matrix
//
// \return The number of non-zero elements in the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::nonZeros() const
{
   size_t nonzeros( 0UL );

   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         if( !isDefault( v_[i*NN+j] ) )
            ++nonzeros;

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the specified row/column.
//
// \param i The index of the row/column.
// \return The number of non-zero elements of row/column \a i.
//
// This function returns the current number of non-zero elements in the specified row/column.
// In case the storage order is set to \a rowMajor the function returns the number of non-zero
// elements in row \a i, in case the storage flag is set to \a columnMajor the function returns
// the number of non-zero elements in column \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::nonZeros( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );

   const size_t jend( i*NN + N );
   size_t nonzeros( 0UL );

   for( size_t j=i*NN; j<jend; ++j )
      if( !isDefault( v_[j] ) )
         ++nonzeros;

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::reset()
{
   using blaze::clear;

   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         clear( v_[i*NN+j] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset the specified row/column to the default initial values.
//
// \param i The index of the row/column.
// \return void
//
// This function resets the values in the specified row/column to their default value. In case
// the storage order is set to \a rowMajor the function resets the values in row \a i, in case
// the storage order is set to \a columnMajor the function resets the values in column \a i.
// Note that the capacity of the row/column remains unchanged.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::reset( size_t i )
{
   using blaze::clear;

   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );
   for( size_t j=0UL; j<N; ++j )
      clear( v_[i*NN+j] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two static matrices.
//
// \param m The matrix to be swapped.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::swap( StaticMatrix& m ) noexcept
{
   using std::swap;

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         swap( v_[i*NN+j], m(i,j) );
      }
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  NUMERIC FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief In-place transpose of the matrix.
//
// \return Reference to the transposed matrix.
//
// This function transposes the static matrix in-place. Note that this function can only be used
// for square static matrices, i.e. if \a M is equal to N.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::transpose()
{
   using std::swap;

   BLAZE_STATIC_ASSERT( M == N );

   for( size_t i=1UL; i<M; ++i )
      for( size_t j=0UL; j<i; ++j )
         swap( v_[i*NN+j], v_[j*NN+i] );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the trans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the trans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::rowMajor> A;

   A = trans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::transpose( TrueType )
{
   transpose();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the trans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the trans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::rowMajor> A;

   A = trans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::transpose( FalseType )
{}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief In-place conjugate transpose of the matrix.
//
// \return Reference to the transposed matrix.
//
// This function transposes the static matrix in-place. Note that this function can only be used
// for square static matrices, i.e. if \a M is equal to N.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::ctranspose()
{
   BLAZE_STATIC_ASSERT( M == N );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<i; ++j ) {
         cswap( v_[i*NN+j], v_[j*NN+i] );
      }
      conjugate( v_[i*NN+i] );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the ctrans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the ctrans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::rowMajor> A;

   A = ctrans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::ctranspose( TrueType )
{
   ctranspose();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the ctrans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the ctrans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::rowMajor> A;

   A = ctrans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::ctranspose( FalseType )
{}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the matrix by the scalar value \a scalar (\f$ A*=s \f$).
//
// \param scalar The scalar value for the matrix scaling.
// \return Reference to the matrix.
//
// This function scales the matrix by applying the given scalar value \a scalar to each element
// of the matrix. For built-in and \c complex data types it has the same effect as using the
// multiplication assignment operator:

   \code
   blaze::StaticMatrix<int,2,3> A;
   // ... Initialization
   A *= 4;        // Scaling of the matrix
   A.scale( 4 );  // Same effect as above
   \endcode
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the scalar value
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::scale( const Other& scalar )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] *= scalar;

   return *this;
}
//*************************************************************************************************




//=================================================================================================
//
//  MEMORY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Class specific implementation of operator new.
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void* StaticMatrix<Type,M,N,SO>::operator new( std::size_t size )
{
   UNUSED_PARAMETER( size );

   BLAZE_INTERNAL_ASSERT( size == sizeof( StaticMatrix ), "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( 1UL );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of operator new[].
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void* StaticMatrix<Type,M,N,SO>::operator new[]( std::size_t size )
{
   BLAZE_INTERNAL_ASSERT( size >= sizeof( StaticMatrix )       , "Invalid number of bytes detected" );
   BLAZE_INTERNAL_ASSERT( size %  sizeof( StaticMatrix ) == 0UL, "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( size/sizeof(StaticMatrix) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of the no-throw operator new.
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void* StaticMatrix<Type,M,N,SO>::operator new( std::size_t size, const std::nothrow_t& )
{
   UNUSED_PARAMETER( size );

   BLAZE_INTERNAL_ASSERT( size == sizeof( StaticMatrix ), "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( 1UL );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of the no-throw operator new[].
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void* StaticMatrix<Type,M,N,SO>::operator new[]( std::size_t size, const std::nothrow_t& )
{
   BLAZE_INTERNAL_ASSERT( size >= sizeof( StaticMatrix )       , "Invalid number of bytes detected" );
   BLAZE_INTERNAL_ASSERT( size %  sizeof( StaticMatrix ) == 0UL, "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( size/sizeof(StaticMatrix) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of operator delete.
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::operator delete( void* ptr )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of operator delete[].
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::operator delete[]( void* ptr )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of no-throw operator delete.
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::operator delete( void* ptr, const std::nothrow_t& )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Class specific implementation of no-throw operator delete[].
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::operator delete[]( void* ptr, const std::nothrow_t& )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
//*************************************************************************************************




//=================================================================================================
//
//  DEBUGGING FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the invariants of the static matrix are intact.
//
// \return \a true in case the static matrix's invariants are intact, \a false otherwise.
//
// This function checks whether the invariants of the static matrix are intact, i.e. if its
// state is valid. In case the invariants are intact, the function returns \a true, else it
// will return \a false.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool StaticMatrix<Type,M,N,SO>::isIntact() const noexcept
{
   if( IsNumeric_v<Type> ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j ) {
            if( v_[i*NN+j] != Type() )
               return false;
         }
      }
   }

   return true;
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the matrix can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
//
// This function returns whether the given address can alias with the matrix. In contrast
// to the isAliased() function this function is allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the foreign expression
inline bool StaticMatrix<Type,M,N,SO>::canAlias( const Other* alias ) const noexcept
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the matrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
//
// This function returns whether the given address is aliased with the matrix. In contrast
// to the canAlias() function this function is not allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the foreign expression
inline bool StaticMatrix<Type,M,N,SO>::isAliased( const Other* alias ) const noexcept
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the matrix is properly aligned in memory.
//
// \return \a true in case the matrix is aligned, \a false if not.
//
// This function returns whether the matrix is guaranteed to be properly aligned in memory, i.e.
// whether the beginning and the end of each row/column of the matrix are guaranteed to conform
// to the alignment restrictions of the element type \a Type.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline constexpr bool StaticMatrix<Type,M,N,SO>::isAligned() noexcept
{
   return align;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Load of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded SIMD element.
//
// This function performs a load of a specific SIMD element of the dense matrix. The row index
// must be smaller than the number of rows and the column index must be smaller then the number
// of columns. Additionally, the column index (in case of a row-major matrix) or the row index
// (in case of a column-major matrix) must be a multiple of the number of values inside the
// SIMD element. This function must \b NOT be called explicitly! It is used internally for the
// performance optimized evaluation of expression templates. Calling this function explicitly
// might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE typename StaticMatrix<Type,M,N,SO>::SIMDType
   StaticMatrix<Type,M,N,SO>::load( size_t i, size_t j ) const noexcept
{
   if( align )
      return loada( i, j );
   else
      return loadu( i, j );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned load of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded SIMD element.
//
// This function performs an aligned load of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the column index (in case of a row-major matrix)
// or the row index (in case of a column-major matrix) must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE typename StaticMatrix<Type,M,N,SO>::SIMDType
   StaticMatrix<Type,M,N,SO>::loada( size_t i, size_t j ) const noexcept
{
   using blaze::loada;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j + SIMDSIZE <= NN, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( !usePadding || j % SIMDSIZE == 0UL, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( checkAlignment( &v_[i*NN+j] ), "Invalid alignment detected" );

   return loada( &v_[i*NN+j] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned load of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded SIMD element.
//
// This function performs an unaligned load of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the column index (in case of a row-major matrix)
// or the row index (in case of a column-major matrix) must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE typename StaticMatrix<Type,M,N,SO>::SIMDType
   StaticMatrix<Type,M,N,SO>::loadu( size_t i, size_t j ) const noexcept
{
   using blaze::loadu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j + SIMDSIZE <= NN, "Invalid column access index" );

   return loadu( &v_[i*NN+j] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs a store of a specific SIMD element of the dense matrix. The row index
// must be smaller than the number of rows and the column index must be smaller than the number
// of columns. Additionally, the column index (in case of a row-major matrix) or the row index
// (in case of a column-major matrix) must be a multiple of the number of values inside the
// SIMD element. This function must \b NOT be called explicitly! It is used internally for the
// performance optimized evaluation of expression templates. Calling this function explicitly
// might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,SO>::store( size_t i, size_t j, const SIMDType& value ) noexcept
{
   if( align )
      storea( i, j, value );
   else
      storeu( i, j, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs an aligned store of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the column index (in case of a row-major matrix)
// or the row index (in case of a column-major matrix) must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,SO>::storea( size_t i, size_t j, const SIMDType& value ) noexcept
{
   using blaze::storea;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j + SIMDSIZE <= NN, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( !usePadding || j % SIMDSIZE == 0UL, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( checkAlignment( &v_[i*NN+j] ), "Invalid alignment detected" );

   storea( &v_[i*NN+j], value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs an unaligned store of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the column index (in case of a row-major matrix)
// or the row index (in case of a column-major matrix) must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,SO>::storeu( size_t i, size_t j, const SIMDType& value ) noexcept
{
   using blaze::storeu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j + SIMDSIZE <= NN, "Invalid column access index" );

   storeu( &v_[i*NN+j], value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store of a specific SIMD element of the
// dense matrix. The row index must be smaller than the number of rows and the column index
// must be smaller than the number of columns. Additionally, the column index (in case of a
// row-major matrix) or the row index (in case of a column-major matrix) must be a multiple
// of the number of values inside the SIMD element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,SO>::stream( size_t i, size_t j, const SIMDType& value ) noexcept
{
   using blaze::stream;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j + SIMDSIZE <= NN, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( !usePadding || j % SIMDSIZE == 0UL, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( checkAlignment( &v_[i*NN+j] ), "Invalid alignment detected" );

   stream( &v_[i*NN+j], value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::assign( const DenseMatrix<MT,SO2>& rhs )
   -> DisableIf_t< VectorizedAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         v_[i*NN+j] = (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief SIMD optimized implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::assign( const DenseMatrix<MT,SO2>& rhs )
   -> EnableIf_t< VectorizedAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   const size_t jpos( ( remainder )?( N & size_t(-SIMDSIZE) ):( N ) );
   BLAZE_INTERNAL_ASSERT( !remainder || ( N - ( N % (SIMDSIZE) ) ) == jpos, "Invalid end calculation" );

   for( size_t i=0UL; i<M; ++i )
   {
      size_t j( 0UL );

      for( ; j<jpos; j+=SIMDSIZE ) {
         store( i, j, (~rhs).load(i,j) );
      }
      for( ; remainder && j<N; ++j ) {
         v_[i*NN+j] = (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::assign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::assign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::addAssign( const DenseMatrix<MT,SO2>& rhs )
   -> DisableIf_t< VectorizedAddAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
   {
      if( IsDiagonal_v<MT> )
      {
         v_[i*NN+i] += (~rhs)(i,i);
      }
      else
      {
         const size_t jbegin( ( IsUpper_v<MT> )
                              ?( IsStrictlyUpper_v<MT> ? i+1UL : i )
                              :( 0UL ) );
         const size_t jend  ( ( IsLower_v<MT> )
                              ?( IsStrictlyLower_v<MT> ? i : i+1UL )
                              :( N ) );
         BLAZE_INTERNAL_ASSERT( jbegin <= jend, "Invalid loop indices detected" );

         for( size_t j=jbegin; j<jend; ++j ) {
            v_[i*NN+j] += (~rhs)(i,j);
         }
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief SIMD optimized implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::addAssign( const DenseMatrix<MT,SO2>& rhs )
   -> EnableIf_t< VectorizedAddAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_DIAGONAL_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   for( size_t i=0UL; i<M; ++i )
   {
      const size_t jbegin( ( IsUpper_v<MT> )
                           ?( ( IsStrictlyUpper_v<MT> ? i+1UL : i ) & size_t(-SIMDSIZE) )
                           :( 0UL ) );
      const size_t jend  ( ( IsLower_v<MT> )
                           ?( IsStrictlyLower_v<MT> ? i : i+1UL )
                           :( N ) );
      BLAZE_INTERNAL_ASSERT( jbegin <= jend, "Invalid loop indices detected" );

      const size_t jpos( ( remainder )?( jend & size_t(-SIMDSIZE) ):( jend ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( jend - ( jend % (SIMDSIZE) ) ) == jpos, "Invalid end calculation" );

      size_t j( jbegin );

      for( ; j<jpos; j+=SIMDSIZE ) {
         store( i, j, load(i,j) + (~rhs).load(i,j) );
      }
      for( ; remainder && j<jend; ++j ) {
         v_[i*NN+j] += (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::addAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::addAssign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::subAssign( const DenseMatrix<MT,SO2>& rhs )
   -> DisableIf_t< VectorizedSubAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
   {
      if( IsDiagonal_v<MT> )
      {
         v_[i*NN+i] -= (~rhs)(i,i);
      }
      else
      {
         const size_t jbegin( ( IsUpper_v<MT> )
                              ?( IsStrictlyUpper_v<MT> ? i+1UL : i )
                              :( 0UL ) );
         const size_t jend  ( ( IsLower_v<MT> )
                              ?( IsStrictlyLower_v<MT> ? i : i+1UL )
                              :( N ) );
         BLAZE_INTERNAL_ASSERT( jbegin <= jend, "Invalid loop indices detected" );

         for( size_t j=jbegin; j<jend; ++j ) {
            v_[i*NN+j] -= (~rhs)(i,j);
         }
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief SIMD optimized implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::subAssign( const DenseMatrix<MT,SO2>& rhs )
   -> EnableIf_t< VectorizedSubAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_DIAGONAL_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   for( size_t i=0UL; i<M; ++i )
   {
      const size_t jbegin( ( IsUpper_v<MT> )
                           ?( ( IsStrictlyUpper_v<MT> ? i+1UL : i ) & size_t(-SIMDSIZE) )
                           :( 0UL ) );
      const size_t jend  ( ( IsLower_v<MT> )
                           ?( IsStrictlyLower_v<MT> ? i : i+1UL )
                           :( N ) );
      BLAZE_INTERNAL_ASSERT( jbegin <= jend, "Invalid loop indices detected" );

      const size_t jpos( ( remainder )?( jend & size_t(-SIMDSIZE) ):( jend ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( jend - ( jend % (SIMDSIZE) ) ) == jpos, "Invalid end calculation" );

      size_t j( jbegin );

      for( ; j<jpos; j+=SIMDSIZE ) {
         store( i, j, load(i,j) - (~rhs).load(i,j) );
      }
      for( ; remainder && j<jend; ++j ) {
         v_[i*NN+j] -= (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::subAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::subAssign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the Schur product assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::schurAssign( const DenseMatrix<MT,SO2>& rhs )
   -> DisableIf_t< VectorizedSchurAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         v_[i*NN+j] *= (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief SIMD optimized implementation of the Schur product assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,SO>::schurAssign( const DenseMatrix<MT,SO2>& rhs )
   -> EnableIf_t< VectorizedSchurAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   for( size_t i=0UL; i<M; ++i )
   {
      const size_t jpos( ( remainder )?( N & size_t(-SIMDSIZE) ):( N ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( N - ( N % (SIMDSIZE) ) ) == jpos, "Invalid end calculation" );

      size_t j( 0UL );

      for( ; j<jpos; j+=SIMDSIZE ) {
         store( i, j, load(i,j) * (~rhs).load(i,j) );
      }
      for( ; remainder && j<N; ++j ) {
         v_[i*NN+j] *= (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the Schur product assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::schurAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   const StaticMatrix tmp( serial( *this ) );

   reset();

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] = tmp.v_[i*NN+element->index()] * element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the Schur product assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::schurAssign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   const StaticMatrix tmp( serial( *this ) );

   reset();

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] = tmp.v_[element->index()*NN+j] * element->value();
}
//*************************************************************************************************








//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR COLUMN-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of StaticMatrix for column-major matrices.
// \ingroup static_matrix
//
// This specialization of StaticMatrix adapts the class template to the requirements of
// column-major matrices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
class StaticMatrix<Type,M,N,true>
   : public DenseMatrix< StaticMatrix<Type,M,N,true>, true >
{
 private:
   //**********************************************************************************************
   //! The number of elements packed within a single SIMD element.
   static constexpr size_t SIMDSIZE = SIMDTrait<Type>::size;

   //! Alignment adjustment.
   static constexpr size_t MM = ( usePadding ? nextMultiple( M, SIMDSIZE ) : M );

   //! Compilation switch for the choice of alignment.
   static constexpr bool align = ( usePadding || MM % SIMDSIZE == 0UL );
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   using This          = StaticMatrix<Type,M,N,true>;   //!< Type of this StaticMatrix instance.
   using BaseType      = DenseMatrix<This,true>;        //!< Base type of this StaticMatrix instance.
   using ResultType    = This;                          //!< Result type for expression template evaluations.
   using OppositeType  = StaticMatrix<Type,M,N,false>;  //!< Result type with opposite storage order for expression template evaluations.
   using TransposeType = StaticMatrix<Type,N,M,false>;  //!< Transpose type for expression template evaluations.
   using ElementType   = Type;                          //!< Type of the matrix elements.
   using SIMDType      = SIMDTrait_t<ElementType>;      //!< SIMD type of the matrix elements.
   using ReturnType    = const Type&;                   //!< Return type for expression template evaluations.
   using CompositeType = const This&;                   //!< Data type for composite expression templates.

   using Reference      = Type&;        //!< Reference to a non-constant matrix value.
   using ConstReference = const Type&;  //!< Reference to a constant matrix value.
   using Pointer        = Type*;        //!< Pointer to a non-constant matrix value.
   using ConstPointer   = const Type*;  //!< Pointer to a constant matrix value.

   using Iterator      = DenseIterator<Type,align>;        //!< Iterator over non-constant elements.
   using ConstIterator = DenseIterator<const Type,align>;  //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Rebind struct definition********************************************************************
   /*!\brief Rebind mechanism to obtain a StaticMatrix with different data/element type.
   */
   template< typename NewType >  // Data type of the other matrix
   struct Rebind {
      using Other = StaticMatrix<NewType,M,N,true>;  //!< The type of the other StaticMatrix.
   };
   //**********************************************************************************************

   //**Resize struct definition********************************************************************
   /*!\brief Resize mechanism to obtain a StaticMatrix with different fixed dimensions.
   */
   template< size_t NewM    // Number of rows of the other matrix
           , size_t NewN >  // Number of columns of the other matrix
   struct Resize {
      using Other = StaticMatrix<Type,NewM,NewN,true>;  //!< The type of the other StaticMatrix.
   };
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for SIMD optimization.
   /*! The \a simdEnabled compilation flag indicates whether expressions the matrix is involved
       in can be optimized via SIMD operations. In case the element type of the matrix is a
       vectorizable data type, the \a simdEnabled compilation flag is set to \a true, otherwise
       it is set to \a false. */
   static constexpr bool simdEnabled = IsVectorizable_v<Type>;

   //! Compilation flag for SMP assignments.
   /*! The \a smpAssignable compilation flag indicates whether the matrix can be used in SMP
       (shared memory parallel) assignments (both on the left-hand and right-hand side of the
       assignment). */
   static constexpr bool smpAssignable = false;
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline StaticMatrix();
   explicit inline StaticMatrix( const Type& init );
   explicit inline StaticMatrix( initializer_list< initializer_list<Type> > list );

   template< typename Other >
   explicit inline StaticMatrix( size_t m, size_t n, const Other* array );

   template< typename Other, size_t Rows, size_t Cols >
   explicit inline StaticMatrix( const Other (&array)[Rows][Cols] );

                                       inline StaticMatrix( const StaticMatrix& m );
   template< typename Other, bool SO > inline StaticMatrix( const StaticMatrix<Other,M,N,SO>&  m );
   template< typename MT   , bool SO > inline StaticMatrix( const Matrix<MT,SO>& m );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j ) noexcept;
   inline ConstReference operator()( size_t i, size_t j ) const noexcept;
   inline Reference      at( size_t i, size_t j );
   inline ConstReference at( size_t i, size_t j ) const;
   inline Pointer        data  () noexcept;
   inline ConstPointer   data  () const noexcept;
   inline Pointer        data  ( size_t j ) noexcept;
   inline ConstPointer   data  ( size_t j ) const noexcept;
   inline Iterator       begin ( size_t j ) noexcept;
   inline ConstIterator  begin ( size_t j ) const noexcept;
   inline ConstIterator  cbegin( size_t j ) const noexcept;
   inline Iterator       end   ( size_t j ) noexcept;
   inline ConstIterator  end   ( size_t j ) const noexcept;
   inline ConstIterator  cend  ( size_t j ) const noexcept;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   inline StaticMatrix& operator=( const Type& set );
   inline StaticMatrix& operator=( initializer_list< initializer_list<Type> > list );

   template< typename Other, size_t Rows, size_t Cols >
   inline StaticMatrix& operator=( const Other (&array)[Rows][Cols] );

                                       inline StaticMatrix& operator= ( const StaticMatrix& rhs );
   template< typename Other, bool SO > inline StaticMatrix& operator= ( const StaticMatrix<Other,M,N,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator= ( const Matrix<MT,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator+=( const Matrix<MT,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator-=( const Matrix<MT,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator%=( const Matrix<MT,SO>& rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   static inline constexpr size_t rows() noexcept;
   static inline constexpr size_t columns() noexcept;
   static inline constexpr size_t spacing() noexcept;
   static inline constexpr size_t capacity() noexcept;
          inline           size_t capacity( size_t j ) const noexcept;
          inline           size_t nonZeros() const;
          inline           size_t nonZeros( size_t j ) const;
          inline           void   reset();
          inline           void   reset( size_t i );
          inline           void   swap( StaticMatrix& m ) noexcept;
   //@}
   //**********************************************************************************************

   //**Numeric functions***************************************************************************
   /*!\name Numeric functions */
   //@{
   inline StaticMatrix& transpose();
   inline StaticMatrix& ctranspose();

   template< typename Other > inline StaticMatrix& scale( const Other& scalar );
   //@}
   //**********************************************************************************************

   //**Memory functions****************************************************************************
   /*!\name Memory functions */
   //@{
   static inline void* operator new  ( std::size_t size );
   static inline void* operator new[]( std::size_t size );
   static inline void* operator new  ( std::size_t size, const std::nothrow_t& );
   static inline void* operator new[]( std::size_t size, const std::nothrow_t& );

   static inline void operator delete  ( void* ptr );
   static inline void operator delete[]( void* ptr );
   static inline void operator delete  ( void* ptr, const std::nothrow_t& );
   static inline void operator delete[]( void* ptr, const std::nothrow_t& );
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        IsColumnMajorMatrix_v<MT> );
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedAddAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        HasSIMDAdd_v< Type, ElementType_t<MT> > &&
        IsColumnMajorMatrix_v<MT> &&
        !IsDiagonal_v<MT> );
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedSubAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        HasSIMDSub_v< Type, ElementType_t<MT> > &&
        IsColumnMajorMatrix_v<MT> &&
        !IsDiagonal_v<MT> );
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename MT >
   static constexpr bool VectorizedSchurAssign_v =
      ( useOptimizedKernels &&
        simdEnabled && MT::simdEnabled &&
        IsSIMDCombinable_v< Type, ElementType_t<MT> > &&
        HasSIMDMult_v< Type, ElementType_t<MT> > &&
        IsColumnMajorMatrix_v<MT> );
   //**********************************************************************************************

 public:
   //**Debugging functions*************************************************************************
   /*!\name Debugging functions */
   //@{
   inline bool isIntact() const noexcept;
   //@}
   //**********************************************************************************************

   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool canAlias ( const Other* alias ) const noexcept;
   template< typename Other > inline bool isAliased( const Other* alias ) const noexcept;

   static inline constexpr bool isAligned() noexcept;

   BLAZE_ALWAYS_INLINE SIMDType load ( size_t i, size_t j ) const noexcept;
   BLAZE_ALWAYS_INLINE SIMDType loada( size_t i, size_t j ) const noexcept;
   BLAZE_ALWAYS_INLINE SIMDType loadu( size_t i, size_t j ) const noexcept;

   BLAZE_ALWAYS_INLINE void store ( size_t i, size_t j, const SIMDType& value ) noexcept;
   BLAZE_ALWAYS_INLINE void storea( size_t i, size_t j, const SIMDType& value ) noexcept;
   BLAZE_ALWAYS_INLINE void storeu( size_t i, size_t j, const SIMDType& value ) noexcept;
   BLAZE_ALWAYS_INLINE void stream( size_t i, size_t j, const SIMDType& value ) noexcept;

   template< typename MT, bool SO >
   inline auto assign( const DenseMatrix<MT,SO>& rhs ) -> DisableIf_t< VectorizedAssign_v<MT> >;

   template< typename MT, bool SO >
   inline auto assign( const DenseMatrix<MT,SO>& rhs ) -> EnableIf_t< VectorizedAssign_v<MT> >;

   template< typename MT > inline void assign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void assign( const SparseMatrix<MT,false>& rhs );

   template< typename MT, bool SO >
   inline auto addAssign( const DenseMatrix<MT,SO>& rhs ) -> DisableIf_t< VectorizedAddAssign_v<MT> >;

   template< typename MT, bool SO >
   inline auto addAssign( const DenseMatrix<MT,SO>& rhs ) -> EnableIf_t< VectorizedAddAssign_v<MT> >;

   template< typename MT > inline void addAssign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void addAssign( const SparseMatrix<MT,false>& rhs );

   template< typename MT, bool SO >
   inline auto subAssign( const DenseMatrix<MT,SO>& rhs ) -> DisableIf_t< VectorizedSubAssign_v<MT> >;

   template< typename MT, bool SO >
   inline auto subAssign( const DenseMatrix<MT,SO>& rhs ) -> EnableIf_t< VectorizedSubAssign_v<MT> >;

   template< typename MT > inline void subAssign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void subAssign( const SparseMatrix<MT,false>& rhs );

   template< typename MT, bool SO >
   inline auto schurAssign( const DenseMatrix<MT,SO>& rhs ) -> DisableIf_t< VectorizedSchurAssign_v<MT> >;

   template< typename MT, bool SO >
   inline auto schurAssign( const DenseMatrix<MT,SO>& rhs ) -> EnableIf_t< VectorizedSchurAssign_v<MT> >;

   template< typename MT > inline void schurAssign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void schurAssign( const SparseMatrix<MT,false>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   inline void transpose ( TrueType  );
   inline void transpose ( FalseType );
   inline void ctranspose( TrueType  );
   inline void ctranspose( FalseType );
   //**********************************************************************************************

   //**********************************************************************************************
   //! Alignment of the data elements.
   static constexpr size_t Alignment =
      ( align ? AlignmentOf_v<Type> : std::alignment_of<Type>::value );

   //! Type of the aligned storage.
   using AlignedStorage = AlignedArray<Type,MM*N,Alignment>;
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   AlignedStorage v_;  //!< The statically allocated matrix elements.
                       /*!< Access to the matrix elements is gained via the function call operator. */
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   BLAZE_STATIC_ASSERT( !usePadding || MM % SIMDSIZE == 0UL );
   BLAZE_STATIC_ASSERT( MM >= M );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The default constructor for StaticMatrix.
//
// All matrix elements are initialized to the default value (i.e. 0 for integral data types).
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix()
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   if( IsNumeric_v<Type> ) {
      for( size_t i=0UL; i<MM*N; ++i )
         v_[i] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a homogenous initialization of all elements.
//
// \param init Initial value for all matrix elements.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& init )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = init;

      for( size_t i=M; i<MM; ++i )
         v_[i+j*MM] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief List initialization of all matrix elements.
//
// \param list The initializer list.
// \exception std::invalid_argument Invalid setup of static matrix.
//
// This constructor provides the option to explicitly initialize the elements of the matrix by
// means of an initializer list:

   \code
   using blaze::columnMajor;

   blaze::StaticMatrix<int,3,3,columnMajor> A{ { 1, 2, 3 },
                                               { 4, 5 },
                                               { 7, 8, 9 } };
   \endcode

// The matrix elements are initialized by the values of the given initializer list. Missing values
// are initialized as default (as e.g. the value 6 in the example). Note that in case the size of
// the top-level initializer list exceeds the number of rows or the size of any nested list exceeds
// the number of columns, a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( initializer_list< initializer_list<Type> > list )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   if( list.size() != M || determineColumns( list ) > N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid setup of static matrix" );
   }

   size_t i( 0UL );

   for( const auto& rowList : list ) {
      size_t j( 0UL );
      for( const auto& element : rowList ) {
         v_[i+j*MM] = element;
         ++j;
      }
      for( ; j<N; ++j ) {
         v_[i+j*MM] = Type();
      }
      ++i;
   }

   BLAZE_INTERNAL_ASSERT( i == M, "Invalid number of elements detected" );

   if( IsNumeric_v<Type> ) {
      for( ; i<MM; ++i ) {
         for( size_t j=0UL; j<N; ++j ) {
            v_[i+j*MM] = Type();
         }
      }
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Array initialization of all matrix elements.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
// \param array Dynamic array for the initialization.
//
// This constructor offers the option to directly initialize the elements of the matrix with
// a dynamic array:

   \code
   using blaze::columnMajor;

   int* array = new int[6];
   // ... Initialization of the dynamic array
   blaze::StaticMatrix<int,3,4,columnMajor> v( array, 2UL, 3UL );
   delete[] array;
   \endcode

// The matrix is initialized with the values from the given array. Missing values are initialized
// with default values. In case the specified number of rows and/or columns exceeds the maximum
// number of rows/column of the static matrix (i.e. \m is larger than M or \a n is larger than N),
// a \a std::invalid_argument exception is thrown.\n
// Note that it is expected that the given \a array has at least \a m by \a n elements. Providing
// an array with less elements results in undefined behavior!
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the initialization array
inline StaticMatrix<Type,M,N,true>::StaticMatrix( size_t m, size_t n, const Other* array )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   if( m > M || n > N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid setup of static matrix" );
   }

   for( size_t j=0UL; j<n; ++j ) {
      for( size_t i=0UL; i<m; ++i )
         v_[i+j*MM] = array[i+j*m];

      if( IsNumeric_v<Type> ) {
         for( size_t i=m; i<MM; ++i )
            v_[i+j*MM] = Type();
      }
   }

   if( IsNumeric_v<Type> ) {
      for( size_t j=n; j<N; ++j ) {
         for( size_t i=0UL; i<MM; ++i )
            v_[i+j*MM] = Type();
      }
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Array initialization of all matrix elements.
//
// \param array \f$ M \times N \f$ dimensional array for the initialization.
//
// This constructor offers the option to directly initialize the elements of the matrix with
// a static array:

   \code
   using blaze::columnMajor;

   const int init[3][3] = { { 1, 2, 3 },
                            { 4, 5 },
                            { 7, 8, 9 } };
   blaze::StaticMatrix<int,3,3,columnMajor> A( init );
   \endcode

// The matrix is initialized with the values from the given array. Missing values are initialized
// with default values (as e.g. the value 6 in the example).
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N >      // Number of columns
template< typename Other  // Data type of the initialization array
        , size_t Rows     // Number of rows of the initialization array
        , size_t Cols >   // Number of columns of the initialization array
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Other (&array)[Rows][Cols] )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );
   BLAZE_STATIC_ASSERT( Rows == M && Cols == N );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = array[i][j];

      for( size_t i=M; i<MM; ++i )
         v_[i+j*MM] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The copy constructor for StaticMatrix.
//
// \param m Matrix to be copied.
//
// The copy constructor is explicitly defined in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const StaticMatrix& m )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   for( size_t i=0UL; i<MM*N; ++i )
      v_[i] = m.v_[i];

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from different StaticMatrix instances.
//
// \param m Matrix to be copied.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N >      // Number of columns
template< typename Other  // Data type of the foreign matrix
        , bool SO >       // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const StaticMatrix<Other,M,N,SO>& m )
   : v_()  // The statically allocated matrix elements
{
   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = m(i,j);

      for( size_t i=M; i<MM; ++i )
         v_[i+j*MM] = Type();
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from different matrices.
//
// \param m Matrix to be copied.
// \exception std::invalid_argument Invalid setup of static matrix.
//
// This constructor initializes the static matrix from the given matrix. In case the size of
// the given matrix does not match the size of the static matrix (i.e. the number of rows is
// not M or the number of columns is not N), a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the foreign matrix
        , bool SO >      // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Matrix<MT,SO>& m )
   : v_()  // The statically allocated matrix elements
{
   using blaze::assign;

   BLAZE_STATIC_ASSERT( IsVectorizable_v<Type> || MM == M );

   if( (~m).rows() != M || (~m).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid setup of static matrix" );
   }

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=( IsSparseMatrix_v<MT> ? 0UL : M ); i<MM; ++i ) {
         v_[i+j*MM] = Type();
      }
   }

   assign( *this, ~m );

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference to the accessed value.
//
// This function only performs an index check in case BLAZE_USER_ASSERT() is active. In contrast,
// the at() function is guaranteed to perform a check of the given access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::Reference
   StaticMatrix<Type,M,N,true>::operator()( size_t i, size_t j ) noexcept
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i+j*MM];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference-to-const to the accessed value.
//
// This function only performs an index check in case BLAZE_USER_ASSERT() is active. In contrast,
// the at() function is guaranteed to perform a check of the given access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstReference
   StaticMatrix<Type,M,N,true>::operator()( size_t i, size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i+j*MM];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checked access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
// \exception std::out_of_range Invalid matrix access index.
//
// In contrast to the subscript operator this function always performs a check of the given
// access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::Reference
   StaticMatrix<Type,M,N,true>::at( size_t i, size_t j )
{
   if( i >= M ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid row access index" );
   }
   if( j >= N ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid column access index" );
   }
   return (*this)(i,j);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checked access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
// \exception std::out_of_range Invalid matrix access index.
//
// In contrast to the subscript operator this function always performs a check of the given
// access indices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstReference
   StaticMatrix<Type,M,N,true>::at( size_t i, size_t j ) const
{
   if( i >= M ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid row access index" );
   }
   if( j >= N ) {
      BLAZE_THROW_OUT_OF_RANGE( "Invalid column access index" );
   }
   return (*this)(i,j);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the static matrix. Note that you
// can NOT assume that all matrix elements lie adjacent to each other! The static matrix may
// use techniques such as padding to improve the alignment of the data. Whereas the number of
// elements within a column are given by the \c columns() member functions, the total number
// of elements including padding is given by the \c spacing() member function.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::Pointer
   StaticMatrix<Type,M,N,true>::data() noexcept
{
   return v_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the static matrix. Note that you
// can NOT assume that all matrix elements lie adjacent to each other! The static matrix may
// use techniques such as padding to improve the alignment of the data. Whereas the number of
// elements within a column are given by the \c columns() member functions, the total number
// of elements including padding is given by the \c spacing() member function.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstPointer
   StaticMatrix<Type,M,N,true>::data() const noexcept
{
   return v_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the matrix elements of column \a j.
//
// \param j The column index.
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage for the elements in column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::Pointer
   StaticMatrix<Type,M,N,true>::data( size_t j ) noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return v_ + j*MM;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the matrix elements of column \a j.
//
// \param j The column index.
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage for the elements in column \a j
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstPointer
   StaticMatrix<Type,M,N,true>::data( size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return v_ + j*MM;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of column \a j.
//
// \param j The column index.
// \return Iterator to the first element of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::Iterator
   StaticMatrix<Type,M,N,true>::begin( size_t j ) noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return Iterator( v_ + j*MM );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of column \a j.
//
// \param j The column index.
// \return Iterator to the first element of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstIterator
   StaticMatrix<Type,M,N,true>::begin( size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return ConstIterator( v_ + j*MM );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of column \a j.
//
// \param j The column index.
// \return Iterator to the first element of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstIterator
   StaticMatrix<Type,M,N,true>::cbegin( size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return ConstIterator( v_ + j*MM );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last element of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::Iterator
   StaticMatrix<Type,M,N,true>::end( size_t j ) noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return Iterator( v_ + j*MM + M );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last element of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstIterator
   StaticMatrix<Type,M,N,true>::end( size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return ConstIterator( v_ + j*MM + M );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last element of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::ConstIterator
   StaticMatrix<Type,M,N,true>::cend( size_t j ) const noexcept
{
   BLAZE_USER_ASSERT( j < N, "Invalid dense matrix column access index" );
   return ConstIterator( v_ + j*MM + M );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Homogenous assignment to all matrix elements.
//
// \param set Scalar value to be assigned to all matrix elements.
// \return Reference to the assigned matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const Type& set )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = set;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief List assignment to all matrix elements.
//
// \param list The initializer list.
// \exception std::invalid_argument Invalid assignment to static matrix.
//
// This assignment operator offers the option to directly assign to all elements of the matrix
// by means of an initializer list:

   \code
   using blaze::columnMajor;

   blaze::StaticMatrix<int,3,3,columnMajor> A;
   A = { { 1, 2, 3 },
         { 4, 5 },
         { 7, 8, 9 } };
   \endcode

// The matrix elements are assigned the values from the given initializer list. Missing values
// are initialized as default (as e.g. the value 6 in the example). Note that in case the size
// of the top-level initializer list exceeds the number of rows or the size of any nested list
// exceeds the number of columns, a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( initializer_list< initializer_list<Type> > list )
{
   if( list.size() != M || determineColumns( list ) > N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid assignment to static matrix" );
   }

   size_t i( 0UL );

   for( const auto& rowList : list ) {
      size_t j( 0UL );
      for( const auto& element : rowList ) {
         v_[i+j*MM] = element;
         ++j;
      }
      for( ; j<N; ++j ) {
         v_[i+j*MM] = Type();
      }
      ++i;
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Array assignment to all matrix elements.
//
// \param array \f$ M \times N \f$ dimensional array for the assignment.
// \return Reference to the assigned matrix.
//
// This assignment operator offers the option to directly set all elements of the matrix:

   \code
   using blaze::columnMajor;

   const int init[3][3] = { { 1, 2, 3 },
                            { 4, 5 },
                            { 7, 8, 9 } };
   blaze::StaticMatrix<int,3UL,3UL,columnMajor> A;
   A = init;
   \endcode

// The matrix is assigned the values from the given array. Missing values are initialized with
// default values (as e.g. the value 6 in the example).
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N >      // Number of columns
template< typename Other  // Data type of the initialization array
        , size_t Rows     // Number of rows of the initialization array
        , size_t Cols >   // Number of columns of the initialization array
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const Other (&array)[Rows][Cols] )
{
   BLAZE_STATIC_ASSERT( Rows == M && Cols == N );

   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = array[i][j];

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for StaticMatrix.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
//
// Explicit definition of a copy assignment operator for performance reasons.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const StaticMatrix& rhs )
{
   using blaze::assign;

   assign( *this, ~rhs );

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different StaticMatrix instances.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N >      // Number of columns
template< typename Other  // Data type of the foreign matrix
        , bool SO >       // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const StaticMatrix<Other,M,N,SO>& rhs )
{
   using blaze::assign;

   assign( *this, ~rhs );

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different matrices.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
// \exception std::invalid_argument Invalid assignment to static matrix.
//
// This constructor initializes the matrix as a copy of the given matrix. In case the
// number of rows of the given matrix is not M or the number of columns is not N, a
// \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator=( const Matrix<MT,SO>& rhs )
{
   using blaze::assign;

   using TT = decltype( trans( *this ) );
   using CT = decltype( ctrans( *this ) );
   using IT = decltype( inv( *this ) );

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid assignment to static matrix" );
   }

   if( IsSame_v<MT,TT> && (~rhs).isAliased( this ) ) {
      transpose( typename IsSquare<This>::Type() );
   }
   else if( IsSame_v<MT,CT> && (~rhs).isAliased( this ) ) {
      ctranspose( typename IsSquare<This>::Type() );
   }
   else if( !IsSame_v<MT,IT> && (~rhs).canAlias( this ) ) {
      StaticMatrix tmp( ~rhs );
      assign( *this, tmp );
   }
   else {
      if( IsSparseMatrix_v<MT> )
         reset();
      assign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix to be added to the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator+=( const Matrix<MT,SO>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   if( (~rhs).canAlias( this ) ) {
      const ResultType_t<MT> tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix to be subtracted from the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator-=( const Matrix<MT,SO>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   if( (~rhs).canAlias( this ) ) {
      const ResultType_t<MT> tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Schur product assignment operator for the multiplication of a matrix (\f$ A\circ=B \f$).
//
// \param rhs The right-hand side matrix for the Schur product.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator%=( const Matrix<MT,SO>& rhs )
{
   using blaze::schurAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   if( (~rhs).canAlias( this ) ) {
      const ResultType_t<MT> tmp( ~rhs );
      schurAssign( *this, tmp );
   }
   else {
      schurAssign( *this, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( isIntact(), "Invariant violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of rows of the matrix.
//
// \return The number of rows of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline constexpr size_t StaticMatrix<Type,M,N,true>::rows() noexcept
{
   return M;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of columns of the matrix.
//
// \return The number of columns of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline constexpr size_t StaticMatrix<Type,M,N,true>::columns() noexcept
{
   return N;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the spacing between the beginning of two columns.
//
// \return The spacing between the beginning of two columns.
//
// This function returns the spacing between the beginning of two column, i.e. the total number
// of elements of a column.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline constexpr size_t StaticMatrix<Type,M,N,true>::spacing() noexcept
{
   return MM;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum capacity of the matrix.
//
// \return The capacity of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline constexpr size_t StaticMatrix<Type,M,N,true>::capacity() noexcept
{
   return MM*N;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current capacity of the specified column.
//
// \param j The index of the column.
// \return The current capacity of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::capacity( size_t j ) const noexcept
{
   UNUSED_PARAMETER( j );

   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return MM;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the total number of non-zero elements in the matrix
//
// \return The number of non-zero elements in the dense matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::nonZeros() const
{
   size_t nonzeros( 0UL );

   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         if( !isDefault( v_[i+j*MM] ) )
            ++nonzeros;

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the specified column.
//
// \param j The index of the column.
// \return The number of non-zero elements of column \a j.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::nonZeros( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const size_t iend( j*MM + M );
   size_t nonzeros( 0UL );

   for( size_t i=j*MM; i<iend; ++i )
      if( !isDefault( v_[i] ) )
         ++nonzeros;

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::reset()
{
   using blaze::clear;

   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         clear( v_[i+j*MM] );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset the specified column to the default initial values.
//
// \param j The index of the column.
// \return void
//
// This function reset the values in the specified column to their default value. Note that
// the capacity of the column remains unchanged.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::reset( size_t j )
{
   using blaze::clear;

   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );
   for( size_t i=0UL; i<M; ++i )
      clear( v_[i+j*MM] );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Swapping the contents of two static matrices.
//
// \param m The matrix to be swapped.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::swap( StaticMatrix& m ) noexcept
{
   using std::swap;

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         swap( v_[i+j*MM], m(i,j) );
      }
   }
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  NUMERIC FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief In-place transpose of the matrix.
//
// \return Reference to the transposed matrix.
//
// This function transposes the static matrix in-place. Note that this function can only be used
// for square static matrices, i.e. if \a M is equal to N.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::transpose()
{
   using std::swap;

   BLAZE_STATIC_ASSERT( M == N );

   for( size_t j=1UL; j<N; ++j )
      for( size_t i=0UL; i<j; ++i )
         swap( v_[i+j*MM], v_[j+i*MM] );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the trans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the trans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::columnMajor> A;

   A = trans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::transpose( TrueType )
{
   transpose();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the trans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the trans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::columnMajor> A;

   A = trans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::transpose( FalseType )
{}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief In-place conjugate transpose of the matrix.
//
// \return Reference to the transposed matrix.
//
// This function transposes the static matrix in-place. Note that this function can only be used
// for square static matrices, i.e. if \a M is equal to N.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::ctranspose()
{
   BLAZE_STATIC_ASSERT( M == N );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<j; ++i ) {
         cswap( v_[i+j*MM], v_[j+i*MM] );
      }
      conjugate( v_[j+j*MM] );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the ctrans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the ctrans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::columnMajor> A;

   A = ctrans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::ctranspose( TrueType )
{
   ctranspose();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Helper function for self-transpose via the ctrans() function.
//
// \return void
//
// This function assists in the evaluation of self-transpose via the ctrans() function:

   \code
   blaze::StaticMatrix<int,3UL,3UL,blaze::columnMajor> A;

   A = ctrans( A );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::ctranspose( FalseType )
{}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling of the matrix by the scalar value \a scalar (\f$ A*=s \f$).
//
// \param scalar The scalar value for the matrix scaling.
// \return Reference to the matrix.
//
// This function scales the matrix by applying the given scalar value \a scalar to each element
// of the matrix. For built-in and \c complex data types it has the same effect as using the
// multiplication assignment operator:

   \code
   blaze::StaticMatrix<int,2,3> A;
   // ... Initialization
   A *= 4;        // Scaling of the matrix
   A.scale( 4 );  // Same effect as above
   \endcode
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the scalar value
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::scale( const Other& scalar )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] *= scalar;

   return *this;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MEMORY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of operator new.
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void* StaticMatrix<Type,M,N,true>::operator new( std::size_t size )
{
   UNUSED_PARAMETER( size );

   BLAZE_INTERNAL_ASSERT( size == sizeof( StaticMatrix ), "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( 1UL );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of operator new[].
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void* StaticMatrix<Type,M,N,true>::operator new[]( std::size_t size )
{
   BLAZE_INTERNAL_ASSERT( size >= sizeof( StaticMatrix )       , "Invalid number of bytes detected" );
   BLAZE_INTERNAL_ASSERT( size %  sizeof( StaticMatrix ) == 0UL, "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( size/sizeof(StaticMatrix) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of the no-throw operator new.
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void* StaticMatrix<Type,M,N,true>::operator new( std::size_t size, const std::nothrow_t& )
{
   UNUSED_PARAMETER( size );

   BLAZE_INTERNAL_ASSERT( size == sizeof( StaticMatrix ), "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( 1UL );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of the no-throw operator new[].
//
// \param size The total number of bytes to be allocated.
// \return Pointer to the newly allocated memory.
// \exception std::bad_alloc Allocation failed.
//
// This class-specific implementation of operator new provides the functionality to allocate
// dynamic memory based on the alignment restrictions of the StaticMatrix class template.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void* StaticMatrix<Type,M,N,true>::operator new[]( std::size_t size, const std::nothrow_t& )
{
   BLAZE_INTERNAL_ASSERT( size >= sizeof( StaticMatrix )       , "Invalid number of bytes detected" );
   BLAZE_INTERNAL_ASSERT( size %  sizeof( StaticMatrix ) == 0UL, "Invalid number of bytes detected" );

   return allocate<StaticMatrix>( size/sizeof(StaticMatrix) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of operator delete.
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::operator delete( void* ptr )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of operator delete[].
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::operator delete[]( void* ptr )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of no-throw operator delete.
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::operator delete( void* ptr, const std::nothrow_t& )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Class specific implementation of no-throw operator delete[].
//
// \param ptr The memory to be deallocated.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::operator delete[]( void* ptr, const std::nothrow_t& )
{
   deallocate( static_cast<StaticMatrix*>( ptr ) );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DEBUGGING FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the invariants of the static matrix are intact.
//
// \return \a true in case the static matrix's invariants are intact, \a false otherwise.
//
// This function checks whether the invariants of the static matrix are intact, i.e. if its
// state is valid. In case the invariants are intact, the function returns \a true, else it
// will return \a false.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline bool StaticMatrix<Type,M,N,true>::isIntact() const noexcept
{
   if( IsNumeric_v<Type> ) {
      for( size_t j=0UL; j<N; ++j ) {
         for( size_t i=M; i<MM; ++i ) {
            if( v_[i+j*MM] != Type() )
               return false;
         }
      }
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
//
// This function returns whether the given address can alias with the matrix. In contrast
// to the isAliased() function this function is allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the foreign expression
inline bool StaticMatrix<Type,M,N,true>::canAlias( const Other* alias ) const noexcept
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
//
// This function returns whether the given address is aliased with the matrix. In contrast
// to the canAlias() function this function is not allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the foreign expression
inline bool StaticMatrix<Type,M,N,true>::isAliased( const Other* alias ) const noexcept
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix is properly aligned in memory.
//
// \return \a true in case the matrix is aligned, \a false if not.
//
// This function returns whether the matrix is guaranteed to be properly aligned in memory, i.e.
// whether the beginning and the end of each column of the matrix are guaranteed to conform to
// the alignment restrictions of the element type \a Type.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline constexpr bool StaticMatrix<Type,M,N,true>::isAligned() noexcept
{
   return align;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Load of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded SIMD element.
//
// This function performs a load of a specific SIMD element of the dense matrix. The row index
// must be smaller than the number of rows and the column index must be smaller than the number
// of columns. Additionally, the row index must be a multiple of the number of values inside
// the SIMD element. This function must \b NOT be called explicitly! It is used internally
// for the performance optimized evaluation of expression templates. Calling this function
// explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE typename StaticMatrix<Type,M,N,true>::SIMDType
   StaticMatrix<Type,M,N,true>::load( size_t i, size_t j ) const noexcept
{
   if( align )
      return loada( i, j );
   else
      return loadu( i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned load of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded SIMD element.
//
// This function performs an aligned load of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the row index must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE typename StaticMatrix<Type,M,N,true>::SIMDType
   StaticMatrix<Type,M,N,true>::loada( size_t i, size_t j ) const noexcept
{
   using blaze::loada;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( i + SIMDSIZE <= MM, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( !usePadding || i % SIMDSIZE == 0UL, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( checkAlignment( &v_[i+j*MM] ), "Invalid alignment detected" );

   return loada( &v_[i+j*MM] );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Unaligned load of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded SIMD element.
//
// This function performs an unaligned load of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the row index must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE typename StaticMatrix<Type,M,N,true>::SIMDType
   StaticMatrix<Type,M,N,true>::loadu( size_t i, size_t j ) const noexcept
{
   using blaze::loadu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( i + SIMDSIZE <= MM, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );

   return loadu( &v_[i+j*MM] );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs a store of a specific SIMD element of the dense matrix. The row index
// must be smaller than the number of rows and the column index must be smaller then the number
// of columns. Additionally, the row index must be a multiple of the number of values inside the
// SIMD element. This function must \b NOT be called explicitly! It is used internally for the
// performance optimized evaluation of expression templates. Calling this function explicitly
// might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,true>::store( size_t i, size_t j, const SIMDType& value ) noexcept
{
   if( align )
      storea( i, j, value );
   else
      storeu( i, j, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs an aligned store of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the row index must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,true>::storea( size_t i, size_t j, const SIMDType& value ) noexcept
{
   using blaze::storea;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( i + SIMDSIZE <= MM, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( !usePadding || i % SIMDSIZE == 0UL, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( checkAlignment( &v_[i+j*MM] ), "Invalid alignment detected" );

   storea( &v_[i+j*MM], value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Unaligned store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs an unaligned store of a specific SIMD element of the dense matrix.
// The row index must be smaller than the number of rows and the column index must be smaller
// than the number of columns. Additionally, the row index must be a multiple of the number of
// values inside the SIMD element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,true>::storeu( size_t i, size_t j, const SIMDType& value ) noexcept
{
   using blaze::storeu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( i + SIMDSIZE <= MM, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );

   storeu( &v_[i+j*MM], value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned, non-temporal store of a SIMD element of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The SIMD element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store of a specific SIMD element of the
// dense matrix. The row index must be smaller than the number of rows and the column index
// must be smaller than the number of columns. Additionally, the row index must be a multiple
// of the number of values inside the SIMD element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
BLAZE_ALWAYS_INLINE void
   StaticMatrix<Type,M,N,true>::stream( size_t i, size_t j, const SIMDType& value ) noexcept
{
   using blaze::stream;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i < M, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( i + SIMDSIZE <= MM, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( !usePadding || i % SIMDSIZE == 0UL, "Invalid row access index" );
   BLAZE_INTERNAL_ASSERT( j < N, "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( checkAlignment( &v_[i+j*MM] ), "Invalid alignment detected" );

   stream( &v_[i+j*MM], value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::assign( const DenseMatrix<MT,SO>& rhs )
   -> DisableIf_t< VectorizedAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         v_[i+j*MM] = (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief SIMD optimized implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::assign( const DenseMatrix<MT,SO>& rhs )
   -> EnableIf_t< VectorizedAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   const size_t ipos( ( remainder )?( M & size_t(-SIMDSIZE) ):( M ) );
   BLAZE_INTERNAL_ASSERT( !remainder || ( M - ( M % (SIMDSIZE) ) ) == ipos, "Invalid end calculation" );

   for( size_t j=0UL; j<N; ++j )
   {
      size_t i( 0UL );

      for( ; i<ipos; i+=SIMDSIZE ) {
         store( i, j, (~rhs).load(i,j) );
      }
      for( ; remainder && i<M; ++i ) {
         v_[i+j*MM] = (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::assign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] = element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::assign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] = element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::addAssign( const DenseMatrix<MT,SO>& rhs )
   -> DisableIf_t< VectorizedAddAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
   {
      if( IsDiagonal_v<MT> )
      {
         v_[j+j*MM] += (~rhs)(j,j);
      }
      else
      {
         const size_t ibegin( ( IsLower_v<MT> )
                              ?( IsStrictlyLower_v<MT> ? j+1UL : j )
                              :( 0UL ) );
         const size_t iend  ( ( IsUpper_v<MT> )
                              ?( IsStrictlyUpper_v<MT> ? j : j+1UL )
                              :( M ) );
         BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

         for( size_t i=ibegin; i<iend; ++i ) {
            v_[i+j*MM] += (~rhs)(i,j);
         }
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief SIMD optimized implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::addAssign( const DenseMatrix<MT,SO>& rhs )
   -> EnableIf_t< VectorizedAddAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_DIAGONAL_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   for( size_t j=0UL; j<N; ++j )
   {
      const size_t ibegin( ( IsLower_v<MT> )
                           ?( ( IsStrictlyLower_v<MT> ? j+1UL : j ) & size_t(-SIMDSIZE) )
                           :( 0UL ) );
      const size_t iend  ( ( IsUpper_v<MT> )
                           ?( IsStrictlyUpper_v<MT> ? j : j+1UL )
                           :( M ) );
      BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

      const size_t ipos( ( remainder )?( iend & size_t(-SIMDSIZE) ):( iend ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( iend - ( iend % (SIMDSIZE) ) ) == ipos, "Invalid end calculation" );

      size_t i( ibegin );

      for( ; i<ipos; i+=SIMDSIZE ) {
         store( i, j, load(i,j) + (~rhs).load(i,j) );
      }
      for( ; remainder && i<iend; ++i ) {
         v_[i+j*MM] += (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::addAssign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::addAssign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::subAssign( const DenseMatrix<MT,SO>& rhs )
   -> DisableIf_t< VectorizedSubAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
   {
      if( IsDiagonal_v<MT> )
      {
         v_[j+j*MM] -= (~rhs)(j,j);
      }
      else
      {
         const size_t ibegin( ( IsLower_v<MT> )
                              ?( IsStrictlyLower_v<MT> ? j+1UL : j )
                              :( 0UL ) );
         const size_t iend  ( ( IsUpper_v<MT> )
                              ?( IsStrictlyUpper_v<MT> ? j : j+1UL )
                              :( M ) );
         BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

         for( size_t i=ibegin; i<iend; ++i ) {
            v_[i+j*MM] -= (~rhs)(i,j);
         }
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief SIMD optimized implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::subAssign( const DenseMatrix<MT,SO>& rhs )
   -> EnableIf_t< VectorizedSubAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_DIAGONAL_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   for( size_t j=0UL; j<N; ++j )
   {
      const size_t ibegin( ( IsLower_v<MT> )
                           ?( ( IsStrictlyLower_v<MT> ? j+1UL : j ) & size_t(-SIMDSIZE) )
                           :( 0UL ) );
      const size_t iend  ( ( IsUpper_v<MT> )
                           ?( IsStrictlyUpper_v<MT> ? j : j+1UL )
                           :( M ) );
      BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

      const size_t ipos( ( remainder )?( iend & size_t(-SIMDSIZE) ):( iend ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( iend - ( iend % (SIMDSIZE) ) ) == ipos, "Invalid end calculation" );

      size_t i( ibegin );

      for( ; i<ipos; i+=SIMDSIZE ) {
         store( i, j, load(i,j) - (~rhs).load(i,j) );
      }
      for( ; remainder && i<iend; ++i ) {
         v_[i+j*MM] -= (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::subAssign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] -= element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::subAssign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] -= element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the Schur product assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::schurAssign( const DenseMatrix<MT,SO>& rhs )
   -> DisableIf_t< VectorizedSchurAssign_v<MT> >
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         v_[i+j*MM] *= (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief SIMD optimized implementation of the Schur product assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline auto StaticMatrix<Type,M,N,true>::schurAssign( const DenseMatrix<MT,SO>& rhs )
   -> EnableIf_t< VectorizedSchurAssign_v<MT> >
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   constexpr bool remainder( !usePadding || !IsPadded_v<MT> );

   for( size_t j=0UL; j<N; ++j )
   {
      const size_t ipos( ( remainder )?( M & size_t(-SIMDSIZE) ):( M ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( M - ( M % (SIMDSIZE) ) ) == ipos, "Invalid end calculation" );

      size_t i( 0UL );

      for( ; i<ipos; i+=SIMDSIZE ) {
         store( i, j, load(i,j) * (~rhs).load(i,j) );
      }
      for( ; remainder && i<M; ++i ) {
         v_[i+j*MM] *= (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the Schur product assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::schurAssign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   const StaticMatrix tmp( serial( *this ) );

   reset();

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator_t<MT> element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] = tmp.v_[element->index()+j*MM] * element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the Schur product assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix for the Schur product.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::schurAssign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   const StaticMatrix tmp( serial( *this ) );

   reset();

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator_t<MT> element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] = tmp.v_[i+element->index()*MM] * element->value();
}
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  STATICMATRIX OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name StaticMatrix operators */
//@{
template< typename Type, size_t M, size_t N, bool SO >
inline void reset( StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline void reset( StaticMatrix<Type,M,N,SO>& m, size_t i );

template< typename Type, size_t M, size_t N, bool SO >
inline void clear( StaticMatrix<Type,M,N,SO>& m );

template< bool RF, typename Type, size_t M, size_t N, bool SO >
inline bool isDefault( const StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline bool isIntact( const StaticMatrix<Type,M,N,SO>& m ) noexcept;

template< typename Type, size_t M, size_t N, bool SO >
inline void swap( StaticMatrix<Type,M,N,SO>& a, StaticMatrix<Type,M,N,SO>& b ) noexcept;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given static matrix.
// \ingroup static_matrix
//
// \param m The matrix to be resetted.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void reset( StaticMatrix<Type,M,N,SO>& m )
{
   m.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset the specified row/column of the given static matrix.
// \ingroup static_matrix
//
// \param m The matrix to be resetted.
// \param i The index of the row/column to be resetted.
// \return void
//
// This function resets the values in the specified row/column of the given static matrix to
// their default value. In case the given matrix is a \a rowMajor matrix the function resets the
// values in row \a i, if it is a \a columnMajor matrix the function resets the values in column
// \a i. Note that the capacity of the row/column remains unchanged.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void reset( StaticMatrix<Type,M,N,SO>& m, size_t i )
{
   m.reset( i );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given static matrix.
// \ingroup static_matrix
//
// \param m The matrix to be cleared.
// \return void
//
// Clearing a static matrix is equivalent to resetting it via the reset() function.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void clear( StaticMatrix<Type,M,N,SO>& m )
{
   m.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given dynamic matrix is in default state.
// \ingroup dynamic_matrix
//
// \param m The matrix to be tested for its default state.
// \return \a true in case the given matrix's rows and columns are zero, \a false otherwise.
//
// This function checks whether the static matrix is in default (constructed) state. In case it
// is in default state, the function returns \a true, else it will return \a false. The following
// example demonstrates the use of the \a isDefault() function:

   \code
   blaze::StaticMatrix<double,3,5> A;
   // ... Initialization
   if( isDefault( A ) ) { ... }
   \endcode

// Optionally, it is possible to switch between strict semantics (blaze::strict) and relaxed
// semantics (blaze::relaxed):

   \code
   if( isDefault<relaxed>( A ) ) { ... }
   \endcode
*/
template< bool RF        // Relaxation flag
        , typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool isDefault( const StaticMatrix<Type,M,N,SO>& m )
{
   if( SO == rowMajor ) {
      for( size_t i=0UL; i<M; ++i )
         for( size_t j=0UL; j<N; ++j )
            if( !isDefault<RF>( m(i,j) ) ) return false;
   }
   else {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=0UL; i<M; ++i )
            if( !isDefault<RF>( m(i,j) ) ) return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the invariants of the given static matrix are intact.
// \ingroup static_matrix
//
// \param m The static matrix to be tested.
// \return \a true in case the given matrix's invariants are intact, \a false otherwise.
//
// This function checks whether the invariants of the static matrix are intact, i.e. if its
// state is valid. In case the invariants are intact, the function returns \a true, else it
// will return \a false. The following example demonstrates the use of the \a isIntact()
// function:

   \code
   blaze::StaticMatrix<double,3,5> A;
   // ... Initialization
   if( isIntact( A ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool isIntact( const StaticMatrix<Type,M,N,SO>& m ) noexcept
{
   return m.isIntact();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two static matrices.
// \ingroup static_matrix
//
// \param a The first matrix to be swapped.
// \param b The second matrix to be swapped.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void swap( StaticMatrix<Type,M,N,SO>& a, StaticMatrix<Type,M,N,SO>& b ) noexcept
{
   a.swap( b );
}
//*************************************************************************************************




//=================================================================================================
//
//  SIZE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, size_t M, size_t N, bool SO >
struct Size< StaticMatrix<T,M,N,SO>, 0UL >
   : public PtrdiffT<M>
{};

template< typename T, size_t M, size_t N, bool SO >
struct Size< StaticMatrix<T,M,N,SO>, 1UL >
   : public PtrdiffT<N>
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
template< typename T, size_t M, size_t N, bool SO >
struct MaxSize< StaticMatrix<T,M,N,SO>, 0UL >
   : public PtrdiffT<M>
{};

template< typename T, size_t M, size_t N, bool SO >
struct MaxSize< StaticMatrix<T,M,N,SO>, 1UL >
   : public PtrdiffT<N>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSQUARE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, size_t N, bool SO >
struct IsSquare< StaticMatrix<T,N,N,SO> >
   : public TrueType
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
template< typename T, size_t M, size_t N, bool SO >
struct HasConstDataAccess< StaticMatrix<T,M,N,SO> >
   : public TrueType
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
template< typename T, size_t M, size_t N, bool SO >
struct HasMutableDataAccess< StaticMatrix<T,M,N,SO> >
   : public TrueType
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTATIC SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, size_t M, size_t N, bool SO >
struct IsStatic< StaticMatrix<T,M,N,SO> >
   : public TrueType
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISALIGNED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, size_t M, size_t N, bool SO >
struct IsAligned< StaticMatrix<T,M,N,SO> >
   : public BoolConstant< StaticMatrix<T,M,N,SO>::isAligned() >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISCONTIGUOUS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, size_t M, size_t N, bool SO >
struct IsContiguous< StaticMatrix<T,M,N,SO> >
   : public TrueType
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISPADDED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, size_t M, size_t N, bool SO >
struct IsPadded< StaticMatrix<T,M,N,SO> >
   : public BoolConstant<usePadding>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ADDTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2 >
struct AddTraitEval2< T1, T2
                    , EnableIf_t< IsMatrix_v<T1> &&
                                  IsMatrix_v<T2> &&
                                  ( Size_v<T1,0UL> != DefaultSize_v ||
                                    Size_v<T2,0UL> != DefaultSize_v ) &&
                                  ( Size_v<T1,1UL> != DefaultSize_v ||
                                    Size_v<T2,1UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = max( Size_v<T1,0UL>, Size_v<T2,0UL> );
   static constexpr size_t N = max( Size_v<T1,1UL>, Size_v<T2,1UL> );

   static constexpr bool SO1 = StorageOrder_v<T1>;
   static constexpr bool SO2 = StorageOrder_v<T2>;

   static constexpr bool SO = ( IsDenseMatrix_v<T1> && IsDenseMatrix_v<T2>
                                ? ( IsSymmetric_v<T1> ^ IsSymmetric_v<T2>
                                    ? ( IsSymmetric_v<T1>
                                        ? SO2
                                        : SO1 )
                                    : SO1 && SO2 )
                                : ( IsDenseMatrix_v<T1>
                                    ? SO1
                                    : SO2 ) );

   using Type = StaticMatrix< AddTrait_t<ET1,ET2>, M, N, SO >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SUBTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2 >
struct SubTraitEval2< T1, T2
                    , EnableIf_t< IsMatrix_v<T1> &&
                                  IsMatrix_v<T2> &&
                                  ( Size_v<T1,0UL> != DefaultSize_v ||
                                    Size_v<T2,0UL> != DefaultSize_v ) &&
                                  ( Size_v<T1,1UL> != DefaultSize_v ||
                                    Size_v<T2,1UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = max( Size_v<T1,0UL>, Size_v<T2,0UL> );
   static constexpr size_t N = max( Size_v<T1,1UL>, Size_v<T2,1UL> );

   static constexpr bool SO1 = StorageOrder_v<T1>;
   static constexpr bool SO2 = StorageOrder_v<T2>;

   static constexpr bool SO = ( IsDenseMatrix_v<T1> && IsDenseMatrix_v<T2>
                                ? ( IsSymmetric_v<T1> ^ IsSymmetric_v<T2>
                                    ? ( IsSymmetric_v<T1>
                                        ? SO2
                                        : SO1 )
                                    : SO1 && SO2 )
                                : ( IsDenseMatrix_v<T1>
                                    ? SO1
                                    : SO2 ) );

   using Type = StaticMatrix< SubTrait_t<ET1,ET2>, M, N, SO >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SCHURTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2 >
struct SchurTraitEval2< T1, T2
                      , EnableIf_t< IsDenseMatrix_v<T1> &&
                                    IsDenseMatrix_v<T2> &&
                                    ( Size_v<T1,0UL> != DefaultSize_v ||
                                      Size_v<T2,0UL> != DefaultSize_v ) &&
                                    ( Size_v<T1,1UL> != DefaultSize_v ||
                                      Size_v<T2,1UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = max( Size_v<T1,0UL>, Size_v<T2,0UL> );
   static constexpr size_t N = max( Size_v<T1,1UL>, Size_v<T2,1UL> );

   static constexpr bool SO1 = StorageOrder_v<T1>;
   static constexpr bool SO2 = StorageOrder_v<T2>;

   static constexpr bool SO = ( IsSymmetric_v<T1> ^ IsSymmetric_v<T2>
                                ? ( IsSymmetric_v<T1>
                                    ? SO2
                                    : SO1 )
                                : SO1 && SO2 );

   using Type = StaticMatrix< MultTrait_t<ET1,ET2>, M, N, SO >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MULTTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2 >
struct MultTraitEval2< T1, T2
                     , EnableIf_t< IsMatrix_v<T1> &&
                                   IsNumeric_v<T2> &&
                                   ( Size_v<T1,0UL> != DefaultSize_v ) &&
                                   ( Size_v<T1,1UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;

   static constexpr size_t M = Size_v<T1,0UL>;
   static constexpr size_t N = Size_v<T1,1UL>;

   using Type = StaticMatrix< MultTrait_t<ET1,T2>, M, N, StorageOrder_v<T1> >;
};

template< typename T1, typename T2 >
struct MultTraitEval2< T1, T2
                     , EnableIf_t< IsNumeric_v<T1> &&
                                   IsMatrix_v<T2> &&
                                   ( Size_v<T2,0UL> != DefaultSize_v ) &&
                                   ( Size_v<T2,1UL> != DefaultSize_v ) > >
{
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = Size_v<T2,0UL>;
   static constexpr size_t N = Size_v<T2,1UL>;

   using Type = StaticMatrix< MultTrait_t<T1,ET2>, M, N, StorageOrder_v<T2> >;
};

template< typename T1, typename T2 >
struct MultTraitEval2< T1, T2
                     , EnableIf_t< IsColumnVector_v<T1> &&
                                   IsRowVector_v<T2> &&
                                   ( Size_v<T1,0UL> != DefaultSize_v ) &&
                                   ( Size_v<T2,0UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = Size_v<T1,0UL>;
   static constexpr size_t N = Size_v<T2,0UL>;

   using Type = StaticMatrix< MultTrait_t<ET1,ET2>, M, N, false >;
};

template< typename T1, typename T2 >
struct MultTraitEval2< T1, T2
                     , EnableIf_t< IsMatrix_v<T1> &&
                                   IsMatrix_v<T2> &&
                                   ( Size_v<T1,0UL> != DefaultSize_v ||
                                     ( IsSquare_v<T1> && Size_v<T2,0UL> != DefaultSize_v ) ) &&
                                   ( Size_v<T2,1UL> != DefaultSize_v ||
                                     ( IsSquare_v<T2> && Size_v<T1,1UL> != DefaultSize_v ) ) > >
{
   using ET1 = ElementType_t<T1>;
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = ( Size_v<T1,0UL> != DefaultSize_v ? Size_v<T1,0UL> : Size_v<T2,0UL> );
   static constexpr size_t N = ( Size_v<T2,1UL> != DefaultSize_v ? Size_v<T2,1UL> : Size_v<T1,1UL> );

   using Type = StaticMatrix< MultTrait_t<ET1,ET2>, M, N, StorageOrder_v<T1> >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DIVTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2 >
struct DivTraitEval2< T1, T2
                    , EnableIf_t< IsMatrix_v<T1> &&
                                  IsNumeric_v<T2> &&
                                  ( Size_v<T1,0UL> != DefaultSize_v ) &&
                                  ( Size_v<T1,1UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;

   static constexpr size_t M = Size_v<T1,0UL>;
   static constexpr size_t N = Size_v<T1,1UL>;

   using Type = StaticMatrix< DivTrait_t<ET1,T2>, M, N, StorageOrder_v<T1> >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MAPTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, typename OP >
struct UnaryMapTraitEval2< T, OP
                         , EnableIf_t< IsMatrix_v<T> &&
                                       Size_v<T,0UL> != DefaultSize_v &&
                                       Size_v<T,1UL> != DefaultSize_v > >
{
   using ET = ElementType_t<T>;

   using Type = StaticMatrix< MapTrait_t<ET,OP>, Size_v<T,0UL>, Size_v<T,1UL>, StorageOrder_v<T> >;
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2, typename OP >
struct BinaryMapTraitEval2< T1, T2, OP
                          , EnableIf_t< IsMatrix_v<T1> &&
                                        IsMatrix_v<T2> &&
                                        ( Size_v<T1,0UL> != DefaultSize_v ||
                                          Size_v<T2,0UL> != DefaultSize_v ) &&
                                        ( Size_v<T1,1UL> != DefaultSize_v ||
                                          Size_v<T2,1UL> != DefaultSize_v ) > >
{
   using ET1 = ElementType_t<T1>;
   using ET2 = ElementType_t<T2>;

   static constexpr size_t M = max( Size_v<T1,0UL>, Size_v<T2,0UL> );
   static constexpr size_t N = max( Size_v<T1,1UL>, Size_v<T2,1UL> );

   static constexpr bool SO1 = StorageOrder_v<T1>;
   static constexpr bool SO2 = StorageOrder_v<T2>;

   static constexpr bool SO = ( IsDenseMatrix_v<T1> && IsDenseMatrix_v<T2>
                                ? ( IsSymmetric_v<T1> ^ IsSymmetric_v<T2>
                                    ? ( IsSymmetric_v<T1>
                                        ? SO2
                                        : SO1 )
                                    : SO1 && SO2 )
                                : ( IsDenseMatrix_v<T1>
                                    ? SO1
                                    : SO2 ) );

   using Type = StaticMatrix< MapTrait_t<ET1,ET2,OP>, M, N, SO >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  HIGHTYPE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct HighType< StaticMatrix<T1,M,N,SO>, StaticMatrix<T2,M,N,SO> >
{
   using Type = StaticMatrix< typename HighType<T1,T2>::Type, M, N, SO >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  LOWTYPE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct LowType< StaticMatrix<T1,M,N,SO>, StaticMatrix<T2,M,N,SO> >
{
   using Type = StaticMatrix< typename LowType<T1,T2>::Type, M, N, SO >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SUBMATRIXTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, size_t I, size_t J, size_t M, size_t N >
struct SubmatrixTraitEval2< MT, I, J, M, N
                          , EnableIf_t< I != inf && J != inf && M != inf && N != inf &&
                                        IsDenseMatrix_v<MT> > >
{
   using Type = StaticMatrix< RemoveConst_t< ElementType_t<MT> >, M, N, StorageOrder_v<MT> >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ROWSTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, size_t M >
struct RowsTraitEval2< MT, M
                     , EnableIf_t< M != 0UL &&
                                   IsDenseMatrix_v<MT> &&
                                   Size_v<MT,1UL> != DefaultSize_v > >
{
   using Type = StaticMatrix< ElementType_t<MT>, M, Size_v<MT,1UL>, false >;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  COLUMNSTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, size_t N >
struct ColumnsTraitEval2< MT, N
                        , EnableIf_t< N != 0UL &&
                                      IsDenseMatrix_v<MT> &&
                                      Size_v<MT,0UL> != DefaultSize_v > >
{
   using Type = StaticMatrix< ElementType_t<MT>, Size_v<MT,0UL>, N, true >;
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
