#pragma once
namespace test
{
	using opencascade::handle;

	struct TopoInfoRecord
	{
		std::vector<const BRep_TFace*> m_brepFaceVct;
		std::vector<const BRep_TEdge*> m_brepEdgeVct;
		std::vector<const BRep_TVertex*> m_brepVertexVct;
		std::vector<handle<Geom_Surface>> m_surfaceVct;
		std::vector<BRep_ListOfCurveRepresentation> m_curveListVct;//vector<BRep_CurveRepresentation>
		//std::set<BRep_CurveRepresentation*> m_curvePtr;
		std::vector<handle<BRep_CurveRepresentation>> m_curveVct; //remove repeat
		std::vector<std::string> m_curveType;
		std::vector<gp_Pnt> m_pointVct;
		std::vector<TopoDS_Face> m_topoFaceVct;
		std::vector<TopoDS_Edge> m_topoEdgeVct;
		std::vector<TopoDS_Vertex> m_topoVertexVct;
		//data
		//std::vector<std::string> m_errorMessage;
		TopoDS_Shape m_shape;

		void clear()
		{
			m_brepFaceVct.clear();
			m_brepEdgeVct.clear();
			m_brepVertexVct.clear();
			m_surfaceVct.clear();
			m_curveListVct.clear();
			m_curveVct.clear();
			m_curveType.clear();
			m_pointVct.clear();
			m_topoFaceVct.clear();
			m_topoEdgeVct.clear();
			m_topoVertexVct.clear();
		}

        void TraverseShape(const TopoDS_Shape& shape)
		{
			//if (shape.IsNull())
			//	shape = m_shape;
			for (TopoDS_Iterator it(shape); it.More(); it.Next())
			{
				TopoDS_Shape subShape = it.Value();
				std::string typeName;
				if (!subShape.TShape().IsNull())
					typeName = typeid(*subShape.TShape()).name();
				const TopAbs_ShapeEnum shapeType = subShape.ShapeType();
				//switch-case
				if (shapeType == TopAbs_COMPOUND) {
					const TopoDS_Compound& compound = TopoDS::Compound(subShape);
					TraverseShape(compound);
				}
				else if (shapeType == TopAbs_COMPSOLID) {
					const TopoDS_CompSolid& compsolid = TopoDS::CompSolid(subShape);
					TraverseShape(compsolid);
				}
				else if (shapeType == TopAbs_SOLID) {
					const TopoDS_Solid& solid = TopoDS::Solid(subShape);
					TraverseShape(solid);
				}
				else if (shapeType == TopAbs_SHELL) {
					const TopoDS_Shell& shell = TopoDS::Shell(subShape);
					TraverseShape(shell);
				}
				else if (shapeType == TopAbs_FACE) {
					const TopoDS_Face& face = TopoDS::Face(subShape);
					BRep_TFace* brepFace = dynamic_cast<BRep_TFace*>(face.TShape().get());
					if (brepFace != nullptr) //no repeat
					{
						m_brepFaceVct.push_back(brepFace);
					}
					TraverseShape(face);
				}
				else if (shapeType == TopAbs_WIRE) {
					const TopoDS_Wire& wire = TopoDS::Wire(subShape);
					TraverseShape(wire);
				}
				else if (shapeType == TopAbs_EDGE) {
					const TopoDS_Edge& edge = TopoDS::Edge(subShape);
					BRep_TEdge* brepEdge = dynamic_cast<BRep_TEdge*>(edge.TShape().get());
					//shared_ptr<BRep_TEdge> ptr = shared_ptr<BRep_TEdge>(brepEdge);//cause crash
					if (brepEdge != nullptr &&
						std::find(m_brepEdgeVct.begin(), m_brepEdgeVct.end(), brepEdge) == m_brepEdgeVct.end())
					{
						m_brepEdgeVct.push_back(brepEdge);
					}
					TraverseShape(edge);
				}
				else if (shapeType == TopAbs_VERTEX) {
					const TopoDS_Vertex& vertex = TopoDS::Vertex(subShape);
					BRep_TVertex* brepVertex = dynamic_cast<BRep_TVertex*>(vertex.TShape().get());
					//shared_ptr<const BRep_TVertex> ptr = shared_ptr<const BRep_TVertex>(brepVertex);//cause crash
					if (brepVertex != nullptr &&
						std::find(m_brepVertexVct.begin(), m_brepVertexVct.end(), brepVertex) == m_brepVertexVct.end())
					{
						m_brepVertexVct.push_back(brepVertex);
					}
				}
				//else
			}
		}

		void getGeomAndShape()
		{
			for (int i = 0; i < m_brepFaceVct.size(); i++)
			{
				if (m_brepFaceVct[i] == nullptr) //dual safe
					continue;
				const handle<Geom_Surface>& surface = m_brepFaceVct[i]->Surface();
				m_surfaceVct.push_back(surface);
				m_topoFaceVct.push_back(BRepBuilderAPI_MakeFace(surface, Precision::Confusion()).Face());
			}
			for (int i = 0; i < m_brepEdgeVct.size(); i++)
			{
				if (m_brepEdgeVct[i] == nullptr) //dual safe
					continue;
				const BRep_ListOfCurveRepresentation& curves = m_brepEdgeVct[i]->Curves();
				if (!curves.IsEmpty())
					m_curveListVct.push_back(curves);
				for (const auto& iter : curves)
				{
                    //  if (m_curvePtr.find(iter.get()) != m_curvePtr.end())
                    if (iter.IsNull() || std::find(m_curveVct.begin(), m_curveVct.end(), iter) != m_curveVct.end())
						continue;
					//m_curvePtr.insert(iter.get());
					m_curveVct.push_back(iter);
					std::string typeName = typeid(*iter.get()).name();
					if (typeName == "class BRep_Curve3D")//typeid(BRep_Curve3D))
					{
						const handle<Geom_Curve>& Curve3D = iter->Curve3D();
						if (Curve3D.IsNull())
							continue;
						m_topoEdgeVct.push_back(BRepBuilderAPI_MakeEdge(Curve3D).Edge());
						typeName += std::string("-") + typeid(*Curve3D.get()).name();
					}
					else if (typeName == "class BRep_CurveOnSurface" || typeName == "class BRep_CurveOnClosedSurface")
					{
						const handle<Geom_Surface>& Surface = iter->Surface();
						const handle<Geom2d_Curve>& PCurve = iter->PCurve();
                        if (!PCurve.IsNull() && !Surface.IsNull())
							m_topoEdgeVct.push_back(BRepBuilderAPI_MakeEdge(PCurve, Surface).Edge());
					}
					m_curveType.push_back(typeName);
				}
			}
			for (int i = 0; i < m_brepVertexVct.size(); i++)
			{
				if (m_brepVertexVct[i] == nullptr) //dual safe
					continue;
				const gp_Pnt& pnt = m_brepVertexVct[i]->Pnt();
				m_pointVct.push_back(pnt);
				m_topoVertexVct.push_back(BRepBuilderAPI_MakeVertex(pnt).Vertex());
			}
		}

		//从成员数据中获取指定类型的TopoDS_Shape
		std::vector<TopoDS_Shape> getShapeVct(TopAbs_ShapeEnum topoType) const
		{
			std::vector<TopoDS_Shape> shapeRes;
			if (TopAbs_ShapeEnum::TopAbs_FACE == topoType)
			{
				for (int i = 0; i < m_topoFaceVct.size(); i++)
					shapeRes.push_back(m_topoFaceVct[i]);
			}
			else if (TopAbs_ShapeEnum::TopAbs_EDGE == topoType)
			{
				for (int i = 0; i < m_topoEdgeVct.size(); i++)
					shapeRes.push_back(m_topoEdgeVct[i]);
			}
			else if (TopAbs_ShapeEnum::TopAbs_VERTEX == topoType)
			{
				for (int i = 0; i < m_topoVertexVct.size(); i++)
					shapeRes.push_back(m_topoVertexVct[i]);
			}
			return shapeRes;
		}
	};

	class BoolNode
	{
	public:
		BoolNode() = default;
		BoolNode(BOPAlgo_Operation operation) { m_operation = operation; };
		std::shared_ptr<BoolNode> m_leftNode;
		std::shared_ptr<BoolNode> m_rightNode;
		BOPAlgo_Operation m_operation = BOPAlgo_Operation::BOPAlgo_UNKNOWN; //
	};

	class CsgTree
	{
	public:
		std::vector<TopoDS_Shape> m_shapeArgument; //boolean before
		TopoDS_Shape m_shapeResult;//boolean after
		std::shared_ptr<BoolNode> m_node = nullptr;
		bool m_isOneOne = true;
		CsgTree() = default;
		CsgTree(const TopoDS_Shape& object, const TopoDS_Shape& tool, BOPAlgo_Operation operation)
		{
			m_isOneOne = true;
			m_shapeArgument.push_back(object);
			m_shapeArgument.push_back(tool);
			m_node = std::make_shared<BoolNode>(operation);
			BRepAlgoAPI_BooleanOperation result;
			result.SetUseOBB(true); //inherit from
			result.SetFuzzyValue(1e-6);

			if (BOPAlgo_Operation::BOPAlgo_COMMON == operation)
				result = BRepAlgoAPI_Common(object, tool);
			else if (BOPAlgo_Operation::BOPAlgo_FUSE == operation)
				result = BRepAlgoAPI_Fuse(object, tool);
			else if (BOPAlgo_Operation::BOPAlgo_CUT == operation)
				result = BRepAlgoAPI_Cut(object, tool);
			m_shapeResult = result.Shape();//BRepBuilderAPI_MakeShape
		}
		CsgTree(const TopTools_ListOfShape& objects, const TopTools_ListOfShape& tools, BOPAlgo_Operation operation)
		{
			m_isOneOne = false;
			m_node = std::make_shared<BoolNode>(operation);
			//NCollection_List, nonsupport using index
			for (const auto& iter : objects)
				m_shapeArgument.push_back(iter);
			for (const auto& iter : tools)
				m_shapeArgument.push_back(iter);
			//doboolean
			BRepAlgoAPI_BooleanOperation boolOp;
			boolOp.SetArguments(objects);
			boolOp.SetTools(tools);
			boolOp.SetOperation(operation);
			boolOp.Build();
			m_shapeResult = boolOp.Shape();
		}

		void checkTopology() const
		{
			//close
			DataRecordDashboard::getInstance().setOpenTime(false);
			if (m_shapeArgument.empty())
				return;
			if (m_isOneOne && m_shapeArgument.size() == 2)
			{
				BRepAlgoAPI_Check checkBefore = BRepAlgoAPI_Check(m_shapeArgument[0], m_shapeArgument[1], m_node->m_operation);
				Message_ProgressRange rangeBefore;
				checkBefore.Perform(rangeBefore);
				BOPAlgo_ListOfCheckResult resultBefore = checkBefore.Result();
			}
			else
			{
				for (int i = 0; i < m_shapeArgument.size(); i++)
				{
					BRepAlgoAPI_Check checkBefore = BRepAlgoAPI_Check(m_shapeArgument[i]);
					Message_ProgressRange rangeBefore;
					checkBefore.Perform(rangeBefore);
					BOPAlgo_ListOfCheckResult resultBefore = checkBefore.Result();
				}
			}
			BRepAlgoAPI_Check checkAfter = BRepAlgoAPI_Check(m_shapeResult);
			Message_ProgressRange rangeAfter;
			checkAfter.Perform(rangeAfter);
			BOPAlgo_ListOfCheckResult resultAfter = checkAfter.Result();
			return;
		}

	};

	//输出耗时信息
	inline void writeTimeDataToCsv()
	{
		DataRecordSingleton& instance = DataRecordSingleton::getInstance();
		const std::vector<DataRecordSingleton::DataMap>& datas = instance.getData();
		std::string filename = clash::get_exe_path();
		//windows系统函数，用于获取自系统启动以来所经过的毫秒数
		filename += "\\..\\csv\\DataCount_" + std::to_string(GetTickCount()) + ".csv";
		//instance.writeToCsvInOne(filename);
		instance.writeToCsvInOne({});
		instance.clear();
	}

	//输出shape.brep二进制数据
	inline void writeShapeDataToTxt()
	{
		DataRecordSingleton& instance = DataRecordSingleton::getInstance();
		const std::vector<DataRecordSingleton::DataMap>& data = instance.getData();
		std::string path = clash::get_exe_path();
		//read
		std::string filenameStd = path + "\\binFile\\shape_std_0.brep";
		TopoDS_Shape shapeStd = instance.readBinToShape(filenameStd);
		//compare
		TopoDS_Shape shapeTest = *(data.back().m_shape);
		std::string filenameTest = path + "\\binFile\\shape_0.brep";
		BRepTools::Write(shapeTest, filenameTest.c_str());
		//compare
		std::string str_shape0 = instance.readBinToString(filenameStd);
		std::string str_shape1 = instance.readBinToString(filenameTest);
		std::vector<int> cmpRes = instance.compareBRepFormat();
		//isEqual
		CString cs = str_shape0 == str_shape1 ? L"true" : L"false";
		AfxMessageBox(cs);
		instance.clear();
	}

	inline CsgTree readBooleanFromTclFile(const std::string& filename)
	{
		std::ifstream infile(filename);
		if (!infile)
			return CsgTree();
		///get current directory
		std::vector<std::string> partsDir = clash::string_split(filename, '/');
		partsDir.pop_back();
		std::string dirCurrent;// dirVct[0];
		for (int i = 0; i < partsDir.size(); i++)
			dirCurrent += partsDir[i] + "/";
		TopoDS_Shape shape1, shape2;
		BOPAlgo_Operation theOperation = BOPAlgo_Operation::BOPAlgo_UNKNOWN;
		std::string line;
		while (std::getline(infile, line))
		{
			std::vector<std::string> partsSpace = clash::string_split(line, ' ');
			if (partsSpace.size() < 3)
				continue;
			if (partsSpace[0] == "#")
				continue;
            std::vector<std::string> partsSlash = clash::string_split(partsSpace[1], '/');
			if (partsSpace[0] == "restore" && partsSpace[2] == "arg1")
			{
                shape1 = DataRecordSingleton::readBinToShape(partsSpace[1]);
				if (shape1.IsNull())
                    shape1 = DataRecordSingleton::readBinToShape(dirCurrent + partsSlash.back());
			}
			else if (partsSpace[0] == "restore" && partsSpace[2] == "arg2")
			{
				shape2 = DataRecordSingleton::readBinToShape(partsSpace[1]);
				if (shape2.IsNull())
					shape2 = DataRecordSingleton::readBinToShape(dirCurrent + partsSlash.back());
			}
			else if (partsSpace[1] == "Res")
			{
				std::string aBopString = partsSpace[0];
				if (aBopString == "bcommon")
					theOperation = BOPAlgo_Operation::BOPAlgo_COMMON;
				else if (aBopString == "bfuse")
					theOperation = BOPAlgo_Operation::BOPAlgo_FUSE;
				else if (aBopString == "bcut")
					theOperation = BOPAlgo_Operation::BOPAlgo_CUT;
			}
		}
		infile.close();
		CsgTree csgtree = CsgTree(shape1, shape2, theOperation);
		return csgtree;
	}

}
