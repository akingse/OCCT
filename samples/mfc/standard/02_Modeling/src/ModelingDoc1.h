#pragma once
namespace test
{
	using opencascade::handle;

	inline std::string getExePath()
	{
		char buffer[MAX_PATH];
		return std::string(_getcwd(buffer, sizeof(buffer))); //get current work directory
	}

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
		std::vector<std::string> m_errorMessage;
		TopoDS_Shape m_shape;

		void clear()
		{
			m_brepFaceVct.clear();
			m_brepEdgeVct.clear();
			m_brepVertexVct.clear();
			m_surfaceVct.clear();
			m_curveVct.clear();
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
			TopoDS_Shape result;
			if (BOPAlgo_Operation::BOPAlgo_COMMON == operation)
				result = BRepAlgoAPI_Common(object, tool);
			else if (BOPAlgo_Operation::BOPAlgo_FUSE == operation)
				result = BRepAlgoAPI_Fuse(object, tool);
			else if (BOPAlgo_Operation::BOPAlgo_CUT == operation)
				result = BRepAlgoAPI_Cut(object, tool);
			//m_shapeVct.push_back(result);
			m_shapeResult = result;
		}
		CsgTree(const TopTools_ListOfShape& objects, const TopTools_ListOfShape& tools, BOPAlgo_Operation operation)
		{
			m_isOneOne = false;
			m_node = std::make_shared<BoolNode>(operation);
			//链表，不支持索引
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
			DataRecordSingleton& instance = DataRecordSingleton::getInstance();
			instance.setOpenTime(false);
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
}
