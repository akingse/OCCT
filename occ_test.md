分别统计求交和构建的耗时



### TKBO布尔

BRepAlgoAPI/BRepAlgoAPI_BooleanOperation 

```c
BRepAlgoAPI_BooleanOperation::Build
	BRepAlgoAPI_BuilderAlgo::IntersectShapes
		BOPAlgo_PaveFiller::Perform
			BOPAlgo_PaveFiller::PerformInternal
			//BOPAlgo\BOPAlgo_PaveFiller.cxx
	BRepAlgoAPI_BuilderAlgo::BuildResult
        BOPAlgo_Builder::PerformWithFiller
            BOPAlgo_Builder::PerformInternal //try-catch
                BOPAlgo_BOP::PerformInternal1
                //BOPAlgo\BOPAlgo_BOP.cxx
                    
```



