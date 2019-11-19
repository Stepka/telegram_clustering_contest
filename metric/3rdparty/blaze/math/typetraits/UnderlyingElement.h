//=================================================================================================
/*!
//  \file blaze/math/typetraits/UnderlyingElement.h
//  \brief Header file for the UnderlyingElement type trait
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

#ifndef _BLAZE_MATH_TYPETRAITS_UNDERLYINGELEMENT_H_
#define _BLAZE_MATH_TYPETRAITS_UNDERLYINGELEMENT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "../../util/mpl/If.h"
#include "../../util/typetraits/IsBuiltin.h"
#include "../../util/typetraits/IsComplex.h"


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Evaluation of the element type of a given data type.
// \ingroup math_type_traits
//
// Via this type trait it is possible to evaluate the element type of a given data type. Examples:

   \code
   using Type1 = double;                                    // Built-in data type
   using Type2 = complex<float>;                            // Complex data type
   using Type3 = StaticVector<int,3UL>;                     // Vector with built-in element type
   using Type4 = CompressedMatrix< DynamicVector<float> >;  // Matrix with vector element type

   blaze::UnderlyingElement< Type1 >::Type  // corresponds to double
   blaze::UnderlyingElement< Type2 >::Type  // corresponds to float
   blaze::UnderlyingElement< Type3 >::Type  // corresponds to int
   blaze::UnderlyingElement< Type4 >::Type  // corresponds to DynamicVector<float>
   \endcode

// Note that per default UnderlyingElement only supports fundamental/built-in data types, complex,
// and data types with the nested type definition \a ElementType. Support for other data types can
// be added by specializing the UnderlyingElement class template.
*/
template< typename T >
struct UnderlyingElement
{
 private:
   //**struct Builtin******************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename T2 >
   struct Builtin { using Type = T2; };
   /*! \endcond */
   //**********************************************************************************************

   //**struct Complex******************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename T2 >
   struct Complex { using Type = typename T2::value_type; };
   /*! \endcond */
   //**********************************************************************************************

   //**struct Other********************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename T2 >
   struct Other { using Type = typename T2::ElementType; };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   using Type = typename If_t< IsBuiltin_v<T>
                             , Builtin<T>
                             , If_t< IsComplex_v<T>
                                   , Complex<T>
                                   , Other<T> >
                             >::Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Auxiliary alias declaration for the UnderlyingElement type trait.
// \ingroup type_traits
//
// The UnderlyingElement_t alias declaration provides a convenient shortcut to access the
// nested \a Type of the UnderlyingElement class template. For instance, given the type \a T
// the following two type definitions are identical:

   \code
   using Type1 = typename blaze::UnderlyingElement<T>::Type;
   using Type2 = blaze::UnderlyingElement_t<T>;
   \endcode
*/
template< typename T >
using UnderlyingElement_t = typename UnderlyingElement<T>::Type;
//*************************************************************************************************

} // namespace blaze

#endif
