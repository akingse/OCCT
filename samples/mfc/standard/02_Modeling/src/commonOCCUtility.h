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
        return TopoDS::Face(transform.Shape());
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
        gp_Trsf translation; // 平移变换
        translation.SetTranslation(gp_Vec(x, y, z));
        return translation;
    }

    inline gp_Trsf rotate(const gp_Pnt& point, const gp_Vec& vec, double theta = 0) // 旋转变换
    {
        gp_Trsf rotation;
        gp_Ax1 axis(point, vec); // 绕 z 轴旋转
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
        gp_Trsf scaling; // 缩放变换
        scaling.SetScale(gp_Pnt(0.0, 0.0, 0.0), s);
        return scaling;
    }

    //不支持不等轴缩放
    inline gp_Trsf scale(double x, double y, double z = 1)
    {
        gp_Trsf scaling; // 缩放变换
        //scaling.SetForm(gp_TrsfForm::gp_Scale);
        scaling.SetValues(
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0);
        return scaling;
    }

}