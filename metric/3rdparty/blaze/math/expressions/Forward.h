//=================================================================================================
/*!
//  \file blaze/math/expressions/Forward.h
//  \brief Header file for all forward declarations for expression class templates
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

#ifndef _BLAZE_MATH_EXPRESSIONS_FORWARD_H_
#define _BLAZE_MATH_EXPRESSIONS_FORWARD_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "../../util/Types.h"


namespace blaze {

//=================================================================================================
//
//  ::blaze NAMESPACE FORWARD DECLARATIONS
//
//=================================================================================================

template< typename, bool > struct DenseMatrix;
template< typename, bool > struct DenseVector;
template< typename, bool > class DMatDeclDiagExpr;
template< typename, bool > class DMatDeclHermExpr;
template< typename, bool > class DMatDeclLowExpr;
template< typename, bool > class DMatDeclSymExpr;
template< typename, bool > class DMatDeclUppExpr;
template< typename, typename, bool > class DMatDMatAddExpr;
template< typename, typename, typename, bool > class DMatDMatMapExpr;
template< typename, typename, bool, bool, bool, bool > class DMatDMatMultExpr;
template< typename, typename, bool > class DMatDMatSchurExpr;
template< typename, typename, bool > class DMatDMatSubExpr;
template< typename, typename > class DMatDVecMultExpr;
template< typename, bool > class DMatEvalExpr;
template< typename, typename, bool > class DMatMapExpr;
template< typename, bool > class DMatInvExpr;
template< typename, typename, bool > class DMatScalarDivExpr;
template< typename, typename, bool > class DMatScalarMultExpr;
template< typename, bool > class DMatSerialExpr;
template< typename, typename, bool > class DMatSMatAddExpr;
template< typename, typename, bool, bool, bool, bool > class DMatSMatMultExpr;
template< typename, typename > class DMatSMatSchurExpr;
template< typename, typename, bool > class DMatSMatSubExpr;
template< typename, typename > class DMatSVecMultExpr;
template< typename, typename > class DMatTDMatAddExpr;
template< typename, typename, typename > class DMatTDMatMapExpr;
template< typename, typename, bool, bool, bool, bool > class DMatTDMatMultExpr;
template< typename, typename > class DMatTDMatSchurExpr;
template< typename, typename > class DMatTDMatSubExpr;
template< typename, bool > class DMatTransExpr;
template< typename, bool > class DMatTransposer;
template< typename, typename > class DMatTSMatAddExpr;
template< typename, typename, bool, bool, bool, bool > class DMatTSMatMultExpr;
template< typename, typename > class DMatTSMatSchurExpr;
template< typename, typename > class DMatTSMatSubExpr;
template< typename, typename, bool > class DVecDVecAddExpr;
template< typename, typename, bool > class DVecDVecCrossExpr;
template< typename, typename, bool > class DVecDVecDivExpr;
template< typename, typename, typename, bool > class DVecDVecMapExpr;
template< typename, typename, bool > class DVecDVecMultExpr;
template< typename, typename > class DVecDVecOuterExpr;
template< typename, typename, bool > class DVecDVecSubExpr;
template< typename, bool > class DVecEvalExpr;
template< typename, typename, bool > class DVecMapExpr;
template< typename, typename, bool > class DVecScalarDivExpr;
template< typename, typename, bool > class DVecScalarMultExpr;
template< typename, bool > class DVecSerialExpr;
template< typename, typename, bool > class DVecSVecAddExpr;
template< typename, typename, bool > class DVecSVecCrossExpr;
template< typename, typename, bool > class DVecSVecMultExpr;
template< typename, typename > class DVecSVecOuterExpr;
template< typename, typename, bool > class DVecSVecSubExpr;
template< typename, bool > class DVecTransExpr;
template< typename, bool > class DVecTransposer;
template< typename, bool > struct Matrix;
template< typename, bool > class SMatDeclDiagExpr;
template< typename, bool > class SMatDeclHermExpr;
template< typename, bool > class SMatDeclLowExpr;
template< typename, bool > class SMatDeclSymExpr;
template< typename, bool > class SMatDeclUppExpr;
template< typename, typename, bool, bool, bool, bool > class SMatDMatMultExpr;
template< typename, typename > class SMatDMatSchurExpr;
template< typename, typename, bool > class SMatDMatSubExpr;
template< typename, typename > class SMatDVecMultExpr;
template< typename, bool > class SMatEvalExpr;
template< typename, typename, bool > class SMatMapExpr;
template< typename, typename, bool > class SMatScalarDivExpr;
template< typename, typename, bool > class SMatScalarMultExpr;
template< typename, bool > class SMatSerialExpr;
template< typename, typename > class SMatSMatAddExpr;
template< typename, typename > class SMatSMatMultExpr;
template< typename, typename > class SMatSMatSchurExpr;
template< typename, typename > class SMatSMatSubExpr;
template< typename, typename > class SMatSVecMultExpr;
template< typename, typename, bool, bool, bool, bool > class SMatTDMatMultExpr;
template< typename, typename > class SMatTDMatSubExpr;
template< typename, bool > class SMatTransExpr;
template< typename, bool > class SMatTransposer;
template< typename, typename > class SMatTSMatAddExpr;
template< typename, typename > class SMatTSMatMultExpr;
template< typename, typename > class SMatTSMatSchurExpr;
template< typename, typename > class SMatTSMatSubExpr;
template< typename, bool > struct SparseMatrix;
template< typename, bool > struct SparseVector;
template< typename, typename, bool > class SVecDVecCrossExpr;
template< typename, typename, bool > class SVecDVecDivExpr;
template< typename, typename, bool > class SVecDVecMultExpr;
template< typename, typename > class SVecDVecOuterExpr;
template< typename, typename, bool > class SVecDVecSubExpr;
template< typename, bool > class SVecEvalExpr;
template< typename, typename, bool > class SVecMapExpr;
template< typename, typename, bool > class SVecScalarDivExpr;
template< typename, typename, bool > class SVecScalarMultExpr;
template< typename, bool > class SVecSerialExpr;
template< typename, typename, bool > class SVecSVecAddExpr;
template< typename, typename, bool > class SVecSVecCrossExpr;
template< typename, typename, bool > class SVecSVecMultExpr;
template< typename, typename > class SVecSVecOuterExpr;
template< typename, typename, bool > class SVecSVecSubExpr;
template< typename, bool > class SVecTransExpr;
template< typename, bool > class SVecTransposer;
template< typename, typename, bool, bool, bool, bool > class TDMatDMatMultExpr;
template< typename, typename > class TDMatDVecMultExpr;
template< typename, typename > class TDMatSMatAddExpr;
template< typename, typename, bool, bool, bool, bool > class TDMatSMatMultExpr;
template< typename, typename > class TDMatSMatSubExpr;
template< typename, typename > class TDMatSVecMultExpr;
template< typename, typename, bool, bool, bool, bool > class TDMatTDMatMultExpr;
template< typename, typename, bool, bool, bool, bool > class TDMatTSMatMultExpr;
template< typename, typename > class TDVecDMatMultExpr;
template< typename, typename > class TDVecSMatMultExpr;
template< typename, typename > class TDVecTDMatMultExpr;
template< typename, typename > class TDVecTSMatMultExpr;
template< typename, typename, bool, bool, bool, bool > class TSMatDMatMultExpr;
template< typename, typename > class TSMatDMatSchurExpr;
template< typename, typename > class TSMatDMatSubExpr;
template< typename, typename > class TSMatDVecMultExpr;
template< typename, typename > class TSMatSMatMultExpr;
template< typename, typename > class TSMatSMatSchurExpr;
template< typename, typename > class TSMatSMatSubExpr;
template< typename, typename > class TSMatSVecMultExpr;
template< typename, typename, bool, bool, bool, bool > class TSMatTDMatMultExpr;
template< typename, typename > class TSMatTSMatAddExpr;
template< typename, typename > class TSMatTSMatMultExpr;
template< typename, typename > class TSMatTSMatSchurExpr;
template< typename, typename > class TSMatTSMatSubExpr;
template< typename, typename > class TSVecDMatMultExpr;
template< typename, typename > class TSVecSMatMultExpr;
template< typename, typename > class TSVecTDMatMultExpr;
template< typename, typename > class TSVecTSMatMultExpr;
template< typename, bool > struct Vector;


template< typename VT1, typename VT2, bool TF >
decltype(auto) operator+( const DenseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator+( const DenseVector<VT1,TF>&, const SparseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator+( const SparseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator+( const SparseVector<VT1,TF>&, const SparseVector<VT2,TF>& );

template< typename MT1, typename MT2, bool SO >
decltype(auto) operator+( const DenseMatrix<MT1,SO>&, const DenseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const DenseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const DenseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2, bool SO >
decltype(auto) operator+( const DenseMatrix<MT1,SO>&, const SparseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const DenseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const DenseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2, bool SO >
decltype(auto) operator+( const SparseMatrix<MT1,SO>&, const DenseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const SparseMatrix<MT2,false>&, const DenseMatrix<MT1,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const SparseMatrix<MT2,true>&, const DenseMatrix<MT1,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator+( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,true>& );


template< typename MT1, typename MT2, bool SO >
decltype(auto) operator-( const DenseMatrix<MT1,SO>&, const DenseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const DenseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const DenseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2, bool SO >
decltype(auto) operator-( const DenseMatrix<MT1,SO>&, const SparseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const DenseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const DenseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2, bool SO >
decltype(auto) operator-( const SparseMatrix<MT1,SO>&, const DenseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const SparseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const SparseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator-( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,true>& );


template< typename VT1, typename VT2, bool TF >
decltype(auto) operator*( const DenseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const DenseVector<VT1,true>&, const DenseVector<VT2,false>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const DenseVector<VT1,false>&, const DenseVector<VT2,true>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator*( const DenseVector<VT1,TF>&, const SparseVector<VT2,TF>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const DenseVector<VT1,true>&, const SparseVector<VT2,false>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const DenseVector<VT1,false>&, const SparseVector<VT2,true>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator*( const SparseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const SparseVector<VT1,true>&, const DenseVector<VT2,false>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const SparseVector<VT1,false>&, const DenseVector<VT2,true>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator*( const SparseVector<VT1,TF>&, const SparseVector<VT2,TF>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const SparseVector<VT1,true>&, const SparseVector<VT2,false>& );

template< typename VT1, typename VT2 >
decltype(auto) operator*( const SparseVector<VT1,false>&, const SparseVector<VT2,true>& );

template< typename MT, typename VT >
decltype(auto) operator*( const DenseMatrix<MT,false>&, const DenseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const DenseMatrix<MT,true>&, const DenseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const DenseMatrix<MT,false>&, const SparseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const DenseMatrix<MT,true>&, const SparseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const SparseMatrix<MT,false>&, const DenseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const SparseMatrix<MT,true>&, const DenseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const SparseMatrix<MT,false>&, const SparseVector<VT,false>& );

template< typename MT, typename VT >
decltype(auto) operator*( const SparseMatrix<MT,true>&, const SparseVector<VT,false>& );

template< typename VT, typename MT >
decltype(auto) operator*( const DenseVector<VT,true>&, const DenseMatrix<MT,false>& );

template< typename VT, typename MT >
decltype(auto) operator*( const DenseVector<VT,true>&, const DenseMatrix<MT,true>& );

template< typename VT, typename MT >
decltype(auto) operator*( const DenseVector<VT,true>&, const SparseMatrix<MT,false>& );

template< typename VT, typename MT >
decltype(auto) operator*( const DenseVector<VT,true>&, const SparseMatrix<MT,true>& );

template< typename VT, typename MT >
decltype(auto) operator*( const SparseVector<VT,true>&, const DenseMatrix<MT,false>& );

template< typename VT, typename MT >
decltype(auto) operator*( const SparseVector<VT,true>&, const DenseMatrix<MT,true>& );

template< typename VT, typename MT >
decltype(auto) operator*( const SparseVector<VT,true>&, const SparseMatrix<MT,false>& );

template< typename VT, typename MT >
decltype(auto) operator*( const SparseVector<VT,true>&, const SparseMatrix<MT,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,false>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,true>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,false>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const DenseMatrix<MT1,true>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,false>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,true>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator*( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,true>& );


template< typename VT1, typename VT2, bool TF >
decltype(auto) operator/( const DenseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator/( const SparseVector<VT1,TF>&, const DenseVector<VT2,TF>& );


template< typename VT1, typename VT2, bool TF >
decltype(auto) operator%( const DenseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator%( const DenseVector<VT1,TF>&, const SparseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator%( const SparseVector<VT1,TF>&, const DenseVector<VT2,TF>& );

template< typename VT1, typename VT2, bool TF >
decltype(auto) operator%( const SparseVector<VT1,TF>&, const SparseVector<VT2,TF>& );


template< typename MT1, typename MT2, bool SO >
decltype(auto) operator%( const DenseMatrix<MT1,SO>&, const DenseMatrix<MT2,SO>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const DenseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const DenseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const DenseMatrix<MT1,false>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const DenseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const DenseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const DenseMatrix<MT1,true>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,false>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,false>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,true>&, const DenseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,true>&, const DenseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,false>&, const SparseMatrix<MT2,true>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,false>& );

template< typename MT1, typename MT2 >
decltype(auto) operator%( const SparseMatrix<MT1,true>&, const SparseMatrix<MT2,true>& );


template< typename VT, bool TF >
decltype(auto) trans( const DenseVector<VT,TF>& );

template< typename VT, bool TF >
decltype(auto) trans( const SparseVector<VT,TF>& );

template< typename MT, bool SO >
decltype(auto) trans( const DenseMatrix<MT,SO>& );

template< typename MT, bool SO >
decltype(auto) trans( const SparseMatrix<MT,SO>& );


template< bool TTF, typename VT, bool TF >
decltype(auto) transTo( const DenseVector<VT,TF>& );

template< bool TTF, typename VT, bool TF >
decltype(auto) transTo( const SparseVector<VT,TF>& );


template< typename VT, bool TF >
decltype(auto) eval( const DenseVector<VT,TF>& );

template< typename VT, bool TF >
decltype(auto) eval( const SparseVector<VT,TF>& );

template< typename MT, bool SO >
decltype(auto) eval( const DenseMatrix<MT,SO>& );

template< typename MT, bool SO >
decltype(auto) eval( const SparseMatrix<MT,SO>& );


template< typename VT, bool TF >
decltype(auto) serial( const DenseVector<VT,TF>& );

template< typename VT, bool TF >
decltype(auto) serial( const SparseVector<VT,TF>& );

template< typename MT, bool SO >
decltype(auto) serial( const DenseMatrix<MT,SO>& );

template< typename MT, bool SO >
decltype(auto) serial( const SparseMatrix<MT,SO>& );


template< typename MT, bool SO >
inline decltype(auto) inv( const DenseMatrix<MT,SO>& );


template< typename VT, bool TF, typename OP >
decltype(auto) map( const DenseVector<VT,TF>&, OP );

template< typename VT, bool TF, typename OP >
decltype(auto) map( const SparseVector<VT,TF>&, OP );

template< typename MT, bool SO, typename OP >
decltype(auto) map( const DenseMatrix<MT,SO>&, OP );

template< typename MT, bool SO, typename OP >
decltype(auto) map( const SparseMatrix<MT,SO>&, OP );

template< typename VT1, typename VT2, bool TF, typename OP >
decltype(auto) map( const DenseVector<VT1,TF>&, const DenseVector<VT2,TF>&, OP );

template< typename MT1, typename MT2, bool SO, typename OP >
decltype(auto) map( const DenseMatrix<MT1,SO>&, const DenseMatrix<MT2,SO>&, OP );

template< typename MT1, typename MT2, typename OP >
decltype(auto) map( const DenseMatrix<MT1,false>&, const DenseMatrix<MT2,true>&, OP );

template< typename MT1, typename MT2, typename OP >
decltype(auto) map( const DenseMatrix<MT1,true>&, const DenseMatrix<MT2,false>&, OP );


template< typename VT, bool TF, typename OP >
decltype(auto) reduce( const DenseVector<VT,TF>&, OP );

template< typename VT, bool TF, typename OP >
decltype(auto) reduce( const SparseVector<VT,TF>&, OP );

template< typename MT, bool SO, typename OP >
decltype(auto) reduce( const DenseMatrix<MT,SO>&, OP );

template< size_t RF, typename MT, bool SO, typename OP >
decltype(auto) reduce( const DenseMatrix<MT,SO>&, OP );

template< typename MT, bool SO, typename OP >
decltype(auto) reduce( const SparseMatrix<MT,SO>&, OP );

template< size_t RF, typename MT, bool SO, typename OP >
decltype(auto) reduce( const SparseMatrix<MT,SO>&, OP );


template< typename MT, bool SO >
inline decltype(auto) declsym( const DenseMatrix<MT,SO>& );

template< typename MT, bool SO >
inline decltype(auto) declsym( const SparseMatrix<MT,SO>& );


template< typename MT, bool SO >
inline decltype(auto) declherm( const DenseMatrix<MT,SO>& );

template< typename MT, bool SO >
inline decltype(auto) declherm( const SparseMatrix<MT,SO>& );


template< typename MT, bool SO >
inline decltype(auto) decllow( const DenseMatrix<MT,SO>& dm );

template< typename MT, bool SO >
inline decltype(auto) decllow( const SparseMatrix<MT,SO>& sm );


template< typename MT, bool SO >
inline decltype(auto) declupp( const DenseMatrix<MT,SO>& dm );

template< typename MT, bool SO >
inline decltype(auto) declupp( const SparseMatrix<MT,SO>& sm );


template< typename MT, bool SO >
inline decltype(auto) decldiag( const DenseMatrix<MT,SO>& dm );

template< typename MT, bool SO >
inline decltype(auto) decldiag( const SparseMatrix<MT,SO>& sm );

} // namespace blaze

#endif
