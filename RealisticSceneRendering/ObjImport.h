#ifndef OBJIMPORT_H
#define OBJIMPORT_H


#include <string>

#include "Mesh.h"


class ObjImport 
{
public:
    static Mesh* ParseObj(const string& fileName)
    { 
        // Vertex Attributes
        vector<glm::vec3> vertex_positions;
        vector<glm::vec2> vertex_texcoords;
        vector<glm::vec3> vertex_normals;

        //Face vectors
        vector<GLint> vertex_position_indices;
        vector<GLint> vertex_texcoord_indices;
        vector<GLint> vertex_normal_indices;

        //Result
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;


        //Reding variables
        stringstream ss;
        ifstream inFile(fileName);
        string line = "";
        string prefix = "";

        //String variable templates
        glm::vec3 temp_vertex_position;
        glm::vec2 temp_vertex_texcoord;
        glm::vec3 temp_vertex_normal;
        GLint temp_glint;

        // Open the input
        if (!inFile.is_open())
        {
            std::cout << "File could not open!";
            exit(1);
        }

        while (std::getline(inFile, line))
        {
            // Get the prefix
            ss.clear();
            ss.str(line);
            ss >> prefix;

            if (prefix == "#")
            {

            }
            else if (prefix == "o")
            {

            }
            else if (prefix == "s")
            {

            }
            else if (prefix == "use_mtl")
            {

            }
            else if (prefix == "v")
            {
                ss >> temp_vertex_position.x >> temp_vertex_position.y >> temp_vertex_position.z;
                vertex_positions.push_back(temp_vertex_position);
            }
            else if (prefix == "vt")
            {
                ss >> temp_vertex_texcoord.x >> temp_vertex_texcoord.y;
                vertex_texcoords.push_back(temp_vertex_texcoord);
            }
            else if (prefix == "vn")
            {
                ss >> temp_vertex_normal.x >> temp_vertex_normal.y >> temp_vertex_normal.z;
                vertex_normals.push_back(temp_vertex_normal);
            }
            else if (prefix == "f")
            {
                int counter = 0;
                while (ss >> temp_glint)
                {
                    //Pushing indices
                    if (counter == 0)
                    {
                        vertex_position_indices.push_back(temp_glint);
                    }
                    else if (counter == 1)
                    {
                        vertex_texcoord_indices.push_back(temp_glint);
                    }
                    else if (counter == 2)
                    {
                        vertex_normal_indices.push_back(temp_glint);
                    }

                    //Handling characters
                    if (ss.peek() == '/')
                    {
                        counter++;
                        ss.ignore(1, '/');
                    }
                    else if (ss.peek() == ' ')
                    {
                        counter++;
                        ss.ignore(1, ' ');
                    }

                    //Reset counter
                    if (counter > 2)
                    {
                        counter = 0;
                    }
                }
            }
        }

        inFile.close();

        //Create Vertex structure and push it to vertices array
        for (size_t i = 0; i < vertex_position_indices.size(); i++)
        {
            glm::vec3 pos = vertex_positions[vertex_position_indices[i] - 1];
            glm::vec2 texcoord = vertex_texcoords[vertex_texcoord_indices[i] - 1];
            glm::vec3 normal = vertex_normals[vertex_normal_indices[i] - 1];
            vertices.push_back(Vertex(pos, normal, texcoord));
        }

        for (size_t i = 0; i < vertex_position_indices.size(); i++)
        {
            indices.push_back(i);
        }

        cout << "Vert Size: " << vertices.size() << "Ind Size: " << indices.size() << endl;

        Mesh* resultMesh = new Mesh(vertices, indices, textures);

        return resultMesh;
    }
};

#endif