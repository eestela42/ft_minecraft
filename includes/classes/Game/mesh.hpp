#ifndef MESH_HPP
# define MESH_HPP

# include <iostream>
# include <string.h>
# include <sstream>
# include <fstream>
# include <math.h>
# include <chrono>
# include <iostream>
# include <string.h>
# include <sstream>
# include <fstream>
# include <vector>


#include <interfaces/Interface_Input.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <classes/vertexData.hpp>



typedef struct s_triangle
{
	unsigned int v[3];
}				t_triangle;

class mesh
{
public:

	std::vector<glm::vec3> base_vertexes;
	std::vector<std::vector<unsigned int>> base_faces;

	std::vector<glm::vec3> vertexes;
	std::vector<t_triangle> triangles;

	std::vector<std::vector<t_triangle>> base_triangles;
	std::vector<std::vector<unsigned int>> vertByFace;

	t_vertexData dataStruct;
	std::vector<u_int> shapeAssemblyData;

	~mesh();
	mesh();
	mesh(const mesh &copy);
	mesh(std::string file_name);

	void	parseFromFile(std::string file_name);


	std::vector<t_triangle> facesToTriangles();
	void	facesToTrianglesDupVertexes();
	void	addInfoToVertexesDup();




	void 					EcoDupVertex();
	t_triangle 				getSemioptiVerticesTriangle(t_triangle &triangle, std::size_t pos,  std::vector<glm::vec3> *new_vertexes,
												std::vector<t_triangle> *base_face_triangles, std::vector<t_triangle> *new_face_triangles);
	std::vector<t_triangle> createTriangleFromFace(std::vector<unsigned int> face);
	void 					addInfoToVertexesEco();


	void 	center_around_orgin();
	void	min_max_bounds(glm::vec3& min_bound, glm::vec3& max_bound);

	mesh&	operator=(const mesh &copy);
};



#endif