#pragma once
//encapsulate the OCC interface
namespace occ
{
    //operator overload
    inline TopoDS_Shape operator+(const TopoDS_Shape& object, const TopoDS_Shape& tool)
    {
        TopoDS_Shape shapeFuse = BRepAlgoAPI_Fuse(object, tool);
        return shapeFuse;
    }

    inline TopoDS_Shape operator-(const TopoDS_Shape& object, const TopoDS_Shape& tool)
    {
        TopoDS_Shape shapeCut = BRepAlgoAPI_Cut(object, tool);
        return shapeCut;
    }

    inline TopoDS_Shape operator^(const TopoDS_Shape& object, const TopoDS_Shape& tool)
    {
        TopoDS_Shape shapeCommon = BRepAlgoAPI_Common(object, tool);
        return shapeCommon;
    }

    //create new shape
    inline TopoDS_Shape operator*(const gp_Trsf& trsf, const TopoDS_Shape& shape)
    {
        return BRepBuilderAPI_Transform(shape, trsf).Shape();
    }
    inline TopoDS_Face operator*(const gp_Trsf& trsf, const TopoDS_Face& shape)
    {
        BRepBuilderAPI_Transform transform(shape, trsf);
        return TopoDS::Face(transform.Shape()); // point force convert
    }
    inline TopoDS_Wire operator*(const gp_Trsf& trsf, const TopoDS_Wire& shape)
    {
        BRepBuilderAPI_Transform transform(shape, trsf);
        return TopoDS::Wire(transform.Shape());
    }
    inline TopoDS_Edge operator*(const gp_Trsf& trsf, const TopoDS_Edge& shape)
    {
        BRepBuilderAPI_Transform transform(shape, trsf);
        return TopoDS::Edge(transform.Shape());
    }

    inline gp_Trsf trans(double x, double y, double z = 0)
    {
        gp_Trsf translation;
        translation.SetTranslation(gp_Vec(x, y, z));
        return translation;
    }

    inline gp_Trsf rotate(const gp_Pnt& point, const gp_Vec& vec, double theta = 0)
    {
        gp_Trsf rotation;
        gp_Ax1 axis(point, vec);
        rotation.SetRotation(axis, theta);
        return rotation;
    }

    inline gp_Trsf rotx(double theta)
    {
        return rotate(gp_Pnt(0, 0, 0), gp_Vec(1, 0, 0), theta);
    }

    inline gp_Trsf roty(double theta)
    {
        return rotate(gp_Pnt(0, 0, 0), gp_Vec(0, 1, 0), theta);
    }

    inline gp_Trsf rotz(double theta)
    {
        return rotate(gp_Pnt(0, 0, 0), gp_Vec(0, 0, 1), theta);
    }

    inline gp_Trsf scale(double s)
    {
        gp_Trsf scaling;
        scaling.SetScale(gp_Pnt(0.0, 0.0, 0.0), s);
        return scaling;
    }

    inline gp_GTrsf scale(double x, double y, double z = 1) //general transformation
    {
        //gp_GTrsf scaling;
        gp_Mat theM(
            x, 0, 0,
            0, y, 0,
            0, 0, z);
        gp_XYZ theV(0, 0, 0);
        return gp_GTrsf(theM, theV);
    }

}

namespace clash
{
    //inline std::string get_exe_path()
    //{
    //    char buffer[MAX_PATH];
    //    return std::string(_getcwd(buffer, sizeof(buffer))); //get current work directory
    //}

    //inline std::vector<std::string> string_split(const std::string& text, char delimiter)
    //{
    //    std::vector<std::string> tokens;
    //    std::string token;
    //    std::istringstream tokenStream(text);
    //    while (std::getline(tokenStream, token, delimiter))
    //    {
    //        tokens.push_back(token);
    //    }
    //    return tokens;
    //}

}