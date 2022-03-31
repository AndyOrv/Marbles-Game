#pragma once
#include "NavigationMap.h"
#include "../../Common/Plane.h"
#include <string>
#include <vector>
namespace NCL {
	namespace CSC8503 {
		struct NavTri {
			Plane   triPlane;
			Vector3 centroid;
			float	area;

			NavTri* parent;
			NavTri* neighbours[3];
			float costs[3];

			int indices[3];

			float f;
			float g;

			NavTri() {
				area = 0.0f;
				neighbours[0] = nullptr;
				neighbours[1] = nullptr;
				neighbours[2] = nullptr;

				indices[0] = -1;
				indices[1] = -1;
				indices[2] = -1;
			}
		};
		class NavigationMesh : public NavigationMap {

		public:
			NavigationMesh();
			NavigationMesh(std::string& filename);
			~NavigationMesh();

			//bool FindPath( Vector3& from,  Vector3& to, NavigationPath& outPath) override;
			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;

		protected:


			NavTri* GetTriForPosition( Vector3& pos) ;
			NavTri* RemoveBestTri(std::vector< NavTri*>& list) ;
			bool			TriInList( NavTri* n, std::vector< NavTri*>& list) ;
			float			Heuristic(NavTri* hTri,  NavTri* endTri) ;

			std::vector<NavTri>		allTris;
			std::vector<Vector3>	allVerts;
		};
	}
}