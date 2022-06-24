#ifndef OBJIMPORT_H
#define OBJIMPORT_H


#include <string>

#include "Mesh.h"


class ObjImport 
{
public:
    static Mesh* ParseObj(const string& fileName)
    {
        fstream myfile;
        vector<glm::vec2> vTexCoords;
        vector<glm::vec3> vNormals;
        vector<glm::vec3> vPositions;
        vector<unsigned int> vIndices;

        // Open the input 
        myfile.open(fileName.c_str(), std::ios::in);

        if (myfile.is_open())
        {
            string curLine;

            while (getline(myfile, curLine))
            {
                stringstream str(curLine);
                GLfloat c1, c2, c3;
                GLuint index[9];
                string tmp;

                if (curLine.length() >= 2)
                {
                    if (curLine[0] == 'v')
                    {
                        if (curLine[1] == 't') // texture
                        {
                            str >> tmp; // consume "vt"
                            str >> c1 >> c2;
                            vTexCoords.push_back(glm::vec2(c1, c2));
                        }
                        else if (curLine[1] == 'n') // normal
                        {
                            str >> tmp; // consume "vn"
                            str >> c1 >> c2 >> c3;
                            vNormals.push_back(glm::vec3(c1, c2, c3));
                        }
                        else // vertex
                        {
                            str >> tmp; // consume "v"
                            str >> c1 >> c2 >> c3;
                            vPositions.push_back(glm::vec3(c1, c2, c3));
                        }
                    }
                    else if (curLine[0] == 'f') // face
                    {
                        str >> tmp; // consume "f"
                        char c;
                        int vIndex[3], nIndex[3], tIndex[3];
                        str >> vIndex[0]; str >> c >> c; // consume "//"
                        str >> nIndex[0];
                        str >> vIndex[1]; str >> c >> c; // consume "//"
                        str >> nIndex[1];
                        str >> vIndex[2]; str >> c >> c; // consume "//"
                        str >> nIndex[2];

                        assert(vIndex[0] == nIndex[0] &&
                            vIndex[1] == nIndex[1] &&
                            vIndex[2] == nIndex[2]); // a limitation for now

                     // make indices start from 0
                        for (int c = 0; c < 3; ++c)
                        {
                            vIndex[c] -= 1;
                            nIndex[c] -= 1;
                            tIndex[c] -= 1;
                        }

                        vIndices.push_back(vIndex[0]);
                        vIndices.push_back(vIndex[1]);
                        vIndices.push_back(vIndex[2]);
                    }
                    else
                    {
                        cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                    }
                }

                //data += curLine;
                if (!myfile.eof())
                {
                    //data += "\n";
                }
            }

            myfile.close();
        }
        else
        {
            std::cout << "File could not open!";
            exit(-1);
        }

        cout << "TexCoord Size: " << vTexCoords.size() << " Position Size: " << vPositions.size() << endl;
        assert(vNormals.size() == vPositions.size());

        vector<Vertex> vertices;
        vector<Texture> textures;

        for (int i = 0; i < vPositions.size(); i++)
        {
            Vertex vert(vPositions[i], vNormals[i]);
            vertices.push_back(vert);
        }

        Mesh* resultMesh = new Mesh(vertices, vIndices, textures);

        return resultMesh;
    }
};

#endif