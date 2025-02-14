// Created on: 1993-10-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BRepAlgoAPI_BooleanOperation.hxx>

#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <BRepTools.hxx>

#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
#include <direct.h> //_getcwd
#include "../BOPAlgo/DataRecordSingleton.h"
#include "../BOPDS/BOPDS_DS.hxx"
#include "../TopoDS/TopoDS.hxx"
using namespace test;

//=======================================================================
//class : BRepAlgoAPI_DumpOper
//purpose : Dumps the arguments ad script to perform operation in DRAW
//=======================================================================
class BRepAlgoAPI_DumpOper {
 public:
  BRepAlgoAPI_DumpOper() :
    myIsDump(Standard_False),
    myIsDumpArgs(Standard_False),
    myIsDumpRes(Standard_False)  {
      OSD_Environment env("CSF_DEBUG_BOP");
      TCollection_AsciiString pathdump = env.Value();
      myIsDump = (!pathdump.IsEmpty() ? Standard_True: Standard_False);
      myPath=pathdump.ToCString();
  };
  //
  virtual ~BRepAlgoAPI_DumpOper() {
  };
  //
  void SetIsDump(const Standard_Boolean bFlag) { //extra
      myIsDump = bFlag;
  }
  Standard_Boolean IsDump()const {
    return myIsDump;
  };
  //
  void SetIsDumpArgs(const Standard_Boolean bFlag) { //checked before boolean
    myIsDumpArgs=bFlag;
  }
  //
  Standard_Boolean IsDumpArgs()const {
    return myIsDumpArgs;
  };
  //
  void SetIsDumpRes(const Standard_Boolean bFlag) { //checked after boolean
    myIsDumpRes=bFlag;
  };
  //
  Standard_Boolean IsDumpRes()const {
    return myIsDumpRes;
  };
  //
  void Dump(
            const TopoDS_Shape& theShape1,
            const TopoDS_Shape& theShape2,
            const TopoDS_Shape& theResult,
            BOPAlgo_Operation theOperation);
  //
 protected:
  Standard_Boolean myIsDump;
  Standard_Boolean myIsDumpArgs;
  Standard_Boolean myIsDumpRes;
  Standard_CString myPath;
};

//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation()
:
  BRepAlgoAPI_BuilderAlgo(),
  myOperation(BOPAlgo_UNKNOWN)
{
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation
  (const BOPAlgo_PaveFiller& thePF)
:
  BRepAlgoAPI_BuilderAlgo(thePF),
  myOperation(BOPAlgo_UNKNOWN)
{
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : obsolete
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation
  (const TopoDS_Shape& theS1,
   const TopoDS_Shape& theS2,
   const BOPAlgo_Operation theOp)
:
  BRepAlgoAPI_BuilderAlgo(),
  myOperation(theOp)
{
  myArguments.Append(theS1);
  myTools.Append(theS2);
#ifdef USING_OPENCASCADE_TEST
  //before do boolean(one-one) check
  if (DataRecordDashboard::getInstance().isOpenCheck())
  {
      DataRecordSingleton& instance = DataRecordSingleton::getInstance();
      BRepAlgoAPI_Check checkObsolete = BRepAlgoAPI_Check(theS1, theS2, theOp);
      Message_ProgressRange rangeObsolete;
      checkObsolete.Perform(rangeObsolete);
      instance.getShapeCheck().m_checkObsolete = checkObsolete.Result();
  }
#endif//USING_OPENCASCADE_TEST
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation
  (const TopoDS_Shape& theS1,
   const TopoDS_Shape& theS2,
   const BOPAlgo_PaveFiller& thePF,
   const BOPAlgo_Operation theOp)
:
  BRepAlgoAPI_BuilderAlgo(thePF),
  myOperation(theOp)
{
  myArguments.Append(theS1);
  myTools.Append(theS2);
}
//=======================================================================
//function : Build
//purpose  :
//=======================================================================
void BRepAlgoAPI_BooleanOperation::Build(const Message_ProgressRange& theRange)
{
  // Set Not Done status by default
  NotDone();
  // Clear from previous runs
  Clear();
  // Check for availability of arguments and tools
  // Both should be present
  if (myArguments.IsEmpty() || myTools.IsEmpty())
  {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  // Check if the operation is set
  if (myOperation == BOPAlgo_UNKNOWN)
  {
    AddError (new BOPAlgo_AlertBOPNotSet);
    return;
  }

  // DEBUG option for dumping shapes and scripts
  BRepAlgoAPI_DumpOper aDumpOper;
  {
    if (aDumpOper.IsDump()) {
      BRepAlgoAPI_Check aChekArgs(myArguments.First(), myTools.First(), myOperation);
      aDumpOper.SetIsDumpArgs(!aChekArgs.IsValid());
    }
  }
  
  TCollection_AsciiString aPSName;
  switch (myOperation)
  {
    case BOPAlgo_COMMON:
      aPSName = "Performing COMMON operation";
      break;
    case BOPAlgo_FUSE:
      aPSName = "Performing FUSE operation";
      break;
    case BOPAlgo_CUT:
    case BOPAlgo_CUT21:
      aPSName = "Performing CUT operation";
      break;
    case BOPAlgo_SECTION:
      aPSName = "Performing SECTION operation";
      break;
    default:
      return;
  }

  Message_ProgressScope aPS(theRange, aPSName, myIsIntersectionNeeded ? 100 : 30);
  // If necessary perform intersection of the argument shapes
  if (myIsIntersectionNeeded)
  {
    // Combine Objects and Tools into a single list for intersection
    TopTools_ListOfShape aLArgs = myArguments;
    for (TopTools_ListOfShape::Iterator it(myTools); it.More(); it.Next())
      aLArgs.Append(it.Value());

#ifdef USING_OPENCASCADE_TEST
    //before do boolean(multi-multi and one-one) check
    if (DataRecordDashboard::getInstance().isOpenCheck())
    {
        DataRecordSingleton& instance = DataRecordSingleton::getInstance();
        BOPAlgo_ListOfCheckResult resultBefore;
        for (const auto& iter : aLArgs)
        {
            BRepAlgoAPI_Check checkBefore = BRepAlgoAPI_Check(iter);
            Message_ProgressRange rangeBefore;
            checkBefore.Perform(rangeBefore);
            BOPAlgo_ListOfCheckResult temp = checkBefore.Result();
            resultBefore.Append(temp);
        }
        instance.getShapeCheck().m_checkBefore = resultBefore;
    }
#endif//USING_OPENCASCADE_TEST

    // Perform intersection
    IntersectShapes(aLArgs, aPS.Next(70));
    if (HasErrors())
    {
      if (aDumpOper.IsDump())
      {
        aDumpOper.SetIsDumpRes(Standard_False);
        aDumpOper.Dump(myArguments.First(), myTools.First(), TopoDS_Shape(), myOperation);
      }
      return;
    }
  }

  // Builder Initialization
  if (myOperation == BOPAlgo_SECTION)
  {
    myBuilder = new BOPAlgo_Section(myAllocator);
    myBuilder->SetArguments(myDSFiller->Arguments());
  }
  else
  {
    myBuilder = new BOPAlgo_BOP(myAllocator);
    myBuilder->SetArguments(myArguments);
    ((BOPAlgo_BOP*)myBuilder)->SetTools(myTools);
    ((BOPAlgo_BOP*)myBuilder)->SetOperation(myOperation);
  }

  // Build the result
  BuildResult(aPS.Next(30));
  if (HasErrors())
  {
    return;
  }
#ifdef USING_OPENCASCADE_TEST
  // after do boolean check
  DataRecordSingleton& instance = DataRecordSingleton::getInstance();
  if (DataRecordDashboard::getInstance().isOpenCheck())
  {
      BRepAlgoAPI_Check checkAfter = BRepAlgoAPI_Check(myShape);
      Message_ProgressRange rangeAfter;
      checkAfter.Perform(rangeAfter);
      instance.getShapeCheck().m_checkAfter = checkAfter.Result();
      instance.getShapeCheck().m_msgReport = GetReport(); //from parent class
  }
  std::vector<DataRecordSingleton::DataMap>& dataMap = instance.getData();
  if (!dataMap.empty())
  {
      dataMap.back().m_shape = std::make_shared<TopoDS_Shape>(myShape);
  }
  //output tcl file
  if (DataRecordDashboard::getInstance().isOpenOutput())
  {
      //force to output
      aDumpOper.SetIsDumpArgs(true);
      aDumpOper.SetIsDumpRes(true);
      aDumpOper.Dump(myArguments.First(), myTools.First(), myShape, myOperation);
  }
#endif//USING_OPENCASCADE_TEST

  if (aDumpOper.IsDump()) {
    Standard_Boolean isDumpRes = myShape.IsNull() ||
                                 !BRepAlgoAPI_Check(myShape).IsValid();
    aDumpOper.SetIsDumpRes(isDumpRes);
    aDumpOper.Dump(myArguments.First(), myTools.First(), myShape, myOperation);
  }
}

//=======================================================================
//function : Dump
//purpose  : DEBUG: Dumping the shapes and script of the operation
//=======================================================================
void BRepAlgoAPI_DumpOper::Dump(const TopoDS_Shape& theShape1,
                                const TopoDS_Shape& theShape2,
                                const TopoDS_Shape& theResult,
                                BOPAlgo_Operation theOperation)
{
  if (!(myIsDumpArgs && myIsDumpRes)) {
    return;
  }
  //
  TCollection_AsciiString aPath(myPath);
#ifdef USING_OPENCASCADE_TEST
  bool isForce = DataRecordDashboard::getInstance().isOpenOutput();
  if (isForce) //(aPath.IsEmpty())
  {
      aPath.Clear();
      char buffer[MAX_PATH];
      std::string path(_getcwd(buffer, sizeof(buffer)));
      aPath = TCollection_AsciiString(path.c_str());
      aPath += "/binFile"; //custom
  }
#endif//USING_OPENCASCADE_TEST

  aPath += "/";
  Standard_Integer aNumOper = 1;
  Standard_Boolean isExist = Standard_True;
  TCollection_AsciiString aFileName;
 
  while(isExist)
  {
    aFileName = aPath + "BO_" + TCollection_AsciiString(aNumOper) +".tcl";
    OSD_File aScript(aFileName);
    isExist = aScript.Exists(); //avoid file cover
    if(isExist)
      aNumOper++;
  }

  FILE* afile = fopen(aFileName.ToCString(), "w+");
  if(!afile)
    return;
  if(!myIsDumpArgs)
    fprintf(afile,"%s\n","# Arguments are invalid");

  TCollection_AsciiString aName1;
  TCollection_AsciiString aName2;
  TCollection_AsciiString aNameRes;
  if(!theShape1.IsNull())
  {
    aName1 = aPath +
      "Arg1_" + TCollection_AsciiString(aNumOper) + ".brep";
    BRepTools::Write(theShape1, aName1.ToCString());
  }
  else
    fprintf(afile,"%s\n","# First argument is Null ");
   
  if(!theShape2.IsNull())
  {
    aName2 =  aPath +
      "Arg2_"+ TCollection_AsciiString(aNumOper) + ".brep";

    BRepTools::Write(theShape2, aName2.ToCString());
  }
  else
    fprintf(afile,"%s\n","# Second argument is Null ");
   
   if(!theResult.IsNull())
  {
    aNameRes =  aPath +
      "Result_"+ TCollection_AsciiString(aNumOper) + ".brep";

    BRepTools::Write(theResult, aNameRes.ToCString());
  }
  else
    fprintf(afile,"%s\n","# Result is Null ");
  
  fprintf(afile, "%s %s %s\n","restore",  aName1.ToCString(), "arg1");
  fprintf(afile, "%s %s %s\n","restore",  aName2.ToCString(), "arg2");
  TCollection_AsciiString aBopString;
  switch (theOperation)
  {
    case BOPAlgo_COMMON : aBopString += "bcommon Res "; break;
    case BOPAlgo_FUSE   : aBopString += "bfuse Res "; break;
    case BOPAlgo_CUT    : 
    case BOPAlgo_CUT21  : aBopString += "bcut Res "; break;
    case BOPAlgo_SECTION : aBopString += "bsection Res "; break;
    default : break;
  };
  aBopString += ("arg1 arg2");
  if(theOperation == BOPAlgo_CUT21)
    aBopString += " 1";

  fprintf(afile, "%s\n",aBopString.ToCString());
  fclose(afile);
}
