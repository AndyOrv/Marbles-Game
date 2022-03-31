#include "NavigationMesh.h"
#include "../../Common/Assets.h"
#include "../../Common/Maths.h"
#include <fstream>

using namespace NCL;
using namespace CSC8503;
using namespace std;

NavigationMesh::NavigationMesh()
{
}

NavigationMesh::NavigationMesh(  std::string& filename)//formatting 
{
	ifstream file(Assets::DATADIR + filename);

	int numVertices = 0;//points
	int numIndices = 0;//lines

	file >> numVertices;
	file >> numIndices;

	for (int i = 0; i < numVertices; ++i) {
		Vector3 vert;
		file >> vert.x;
		file >> vert.y;
		file >> vert.z;

		allVerts.emplace_back(vert);
		std::cout << "vert placed" << std::endl;
	}

	allTris.resize(numIndices / 3);

	for (int i = 0; i < allTris.size(); ++i) {
		NavTri* tri = &allTris[i];
		file >> tri->indices[0];
		file >> tri->indices[1];
		file >> tri->indices[2];

		tri->centroid = allVerts[tri->indices[0]] +
			allVerts[tri->indices[1]] +
			allVerts[tri->indices[2]];

		tri->centroid = allTris[i].centroid / 3.0f;

		tri->triPlane = Plane::PlaneFromTri(allVerts[tri->indices[0]],
			allVerts[tri->indices[1]],
			allVerts[tri->indices[2]]);

		tri->area = Maths::CrossAreaOfTri(allVerts[tri->indices[0]], allVerts[tri->indices[1]], allVerts[tri->indices[2]]);
	}
	for (int i = 0; i < allTris.size(); ++i) {
		NavTri* tri = &allTris[i];
		for (int j = 0; j < 3; ++j) {//add cost here I guess
			int index = 0;
			file >> index;
			if (index != -1) {
				tri->neighbours[j] = &allTris[index];
				tri->costs[j] = Heuristic(tri, tri->neighbours[j]);//length between tri and neighbour
			}
		}
	}
}

NavigationMesh::~NavigationMesh()
{
}
//parent and cost still not set 
bool NavigationMesh::FindPath(const Vector3& from,const  Vector3& to, NavigationPath& outPath) {

	std::cout << "Let's find it" << std::endl;

	Vector3 f = from;
	Vector3 t = to;

	NavTri* start = GetTriForPosition(f);//find the triangle that contains enemy ball
	NavTri* end = GetTriForPosition(t);//find the triangle that contains the player ball

	//Check to see if both are withing the same Nav
	//check to see if either fall outisde of the NavMesh

	if (!start || !end) {
		std::cout << "not in mesh" << std::endl;
		return false; //object positions are not present in navmesh
	}
	if (start == end) {
		std::cout << "same triangle" << std::endl;
		return false; //objects are present in same navmesh triangle idk what to do,
	}

	std::vector<  NavTri*>  openList;//place all adjacent triangles on open list
	std::vector<  NavTri*>  closedList;


	openList.push_back(start);

	start->f = 0;
	start->g = 0;
	start->parent = nullptr;

	  NavTri* bestTri = nullptr;

	while (!openList.empty()) {
		bestTri = RemoveBestTri(openList);
		std::cout << "trying shit" << std::endl;

		if (bestTri == end) {//found
			//need to set up navigation path here
			std::cout << "path found" << std::endl;
			  NavTri* node = end;
			while (node != nullptr) {
				outPath.PushWaypoint(node->centroid);
				node = node->parent;
			}
		}
		else {
			for (int i = 0; i < 3; i++) {
				NavTri* neigh = bestTri->neighbours[i];
				if (!neigh) {
					continue;
				}

				bool inClosed = TriInList(neigh, closedList);
				if (inClosed) {
					continue;//already discarded this neighbour
				}

				float h = Heuristic(neigh, end);
				float g = bestTri->g + bestTri->costs[i];
				float f = h + g;
				std::cout << "doing math" << std::endl;

				bool inOpen = TriInList(neigh, openList);
				if (!inOpen || f < neigh->f) {//might be a better route to this neighbour
					neigh->parent = bestTri;
					neigh->f = f;
					neigh->g = g;
				}

			}
			closedList.emplace_back(bestTri);
		}

	}

	std::cout << "didn't work" << std::endl;
	return false;
}


  NavTri* NavigationMesh::GetTriForPosition(  Vector3& pos)   {
	for (  NavTri& t : allTris) {
		Vector3 planePoint = t.triPlane.ProjectPointOntoPlane(pos);

		float ta = Maths::CrossAreaOfTri(allVerts[t.indices[0]], allVerts[t.indices[1]], planePoint);
		float tb = Maths::CrossAreaOfTri(allVerts[t.indices[1]], allVerts[t.indices[2]], planePoint);
		float tc = Maths::CrossAreaOfTri(allVerts[t.indices[2]], allVerts[t.indices[0]], planePoint);

		float areaSum = ta + tb + tc;

		if (abs(areaSum - t.area) > 0.001f) { //floating points are annoying! Are we more or less inside the triangle?
			continue;
		}
		std::cout << "found" << std::endl;
		return &t;
	}
	std::cout << "not found" << std::endl;
	return nullptr;
}

  NavTri* NavigationMesh::RemoveBestTri(std::vector<  NavTri*>& list)  
{
	std::vector<  NavTri*>::iterator bestI = list.begin();

	  NavTri* bestTri = *list.begin();

	for (auto i = list.begin(); i != list.end(); ++i) {
		if ((*i)->indices < bestTri->indices) {
			bestTri = (*i);
			bestI = i;
		}
	}
	list.erase(bestI);

	return bestTri;
}

bool NavigationMesh::TriInList(  NavTri* n, std::vector<  NavTri*>& list)  
{
	std::vector<  NavTri*>::iterator i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

//how far center of triangle is away from center of triangle containing the end point
float NavigationMesh::Heuristic(NavTri* hTri,   NavTri* endTri)  
{
	return (hTri->centroid - endTri->centroid).Length();
}
