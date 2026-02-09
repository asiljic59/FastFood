#include"Model.h"

Model::Model(const char* file)
{
    std::cout << "Model constructor started for: " << file << std::endl;

    std::string text = get_file_contents(file);
    std::cout << "File contents loaded" << std::endl;

    JSON = json::parse(text);
    std::cout << "JSON parsed" << std::endl;

    Model::file = file;

    // Check if buffers exist
    if (JSON.find("buffers") == JSON.end() || JSON["buffers"].empty()) {
        std::cerr << "Error: No buffers found in GLTF file" << std::endl;
        return;
    }

    data = getData();
    std::cout << "Binary data loaded, size: " << data.size() << std::endl;

    // Check if nodes exist
    if (JSON.find("nodes") != JSON.end() && !JSON["nodes"].empty()) {
        std::cout << "Traversing nodes..." << std::endl;
        traverseNode(0);
        std::cout << "Nodes traversed successfully. Meshes loaded: " << meshes.size() << std::endl;
    }
    else {
        std::cerr << "Warning: No nodes found in GLTF file" << std::endl;
    }
}
void Model::Draw(Shader& shader, Camera& camera, glm::mat4 transform)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        // Combine global transform with the mesh's original transform
        glm::mat4 finalMatrix = transform * matricesMeshes[i];
        meshes[i].Draw(shader, camera, finalMatrix);
    }
}

void Model::loadMesh(unsigned int indMesh)
{
    std::cout << "Loading mesh " << indMesh << std::endl;

    // Check if mesh exists
    if (indMesh >= JSON["meshes"].size()) {
        std::cerr << "Error: Mesh index out of bounds" << std::endl;
        return;
    }

    auto& mesh = JSON["meshes"][indMesh];
    if (mesh.find("primitives") == mesh.end() || mesh["primitives"].empty()) {
        std::cerr << "Error: No primitives in mesh" << std::endl;
        return;
    }

    auto& primitive = mesh["primitives"][0];
    auto& attributes = primitive["attributes"];

    // Check for required attributes
    if (attributes.find("POSITION") == attributes.end()) {
        std::cerr << "Error: No POSITION attribute" << std::endl;
        return;
    }
    if (attributes.find("NORMAL") == attributes.end()) {
        std::cerr << "Error: No NORMAL attribute" << std::endl;
        return;
    }
    if (primitive.find("indices") == primitive.end()) {
        std::cerr << "Error: No indices in primitive" << std::endl;
        return;
    }

    unsigned int posAccInd = attributes["POSITION"];
    unsigned int normalAccInd = attributes["NORMAL"];
    unsigned int indAccInd = primitive["indices"];

    // Get vertex data
    std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
    std::vector<glm::vec3> positions = groupFloatsVec3(posVec);

    std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
    std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);

    // Handle optional texture coordinates
    std::vector<glm::vec2> texUVs;
    bool hasTexCoords = attributes.find("TEXCOORD_0") != attributes.end();
    if (hasTexCoords) {
        unsigned int texAccInd = attributes["TEXCOORD_0"];
        std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
        texUVs = groupFloatsVec2(texVec);
    }
    else {
        std::cout << "No texture coordinates, using defaults" << std::endl;
        texUVs.resize(positions.size(), glm::vec2(0.0f, 0.0f));
    }

    std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
    std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);

    // Skip texture loading for now
    std::vector<Texture> textures = getTextures();

    std::cout << "Creating mesh with " << vertices.size() << " vertices and " << indices.size() << " indices" << std::endl;
    meshes.push_back(Mesh(vertices, indices, textures));
    std::cout << "Mesh created successfully" << std::endl;
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
    // Check bounds
    if (nextNode >= JSON["nodes"].size()) {
        std::cerr << "Error: Node index out of bounds: " << nextNode << std::endl;
        return;
    }

    json node = JSON["nodes"][nextNode];

    // Get translation if it exists
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    if (node.find("translation") != node.end())
    {
        float transValues[3];
        for (unsigned int i = 0; i < node["translation"].size(); i++)
            transValues[i] = (node["translation"][i]);
        translation = glm::make_vec3(transValues);
    }

    // Get quaternion if it exists
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (node.find("rotation") != node.end())
    {
        float rotValues[4] =
        {
            node["rotation"][3],
            node["rotation"][0],
            node["rotation"][1],
            node["rotation"][2]
        };
        rotation = glm::make_quat(rotValues);
    }

    // Get scale if it exists
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    if (node.find("scale") != node.end())
    {
        float scaleValues[3];
        for (unsigned int i = 0; i < node["scale"].size(); i++)
            scaleValues[i] = (node["scale"][i]);
        scale = glm::make_vec3(scaleValues);
    }

    // Get matrix if it exists
    glm::mat4 matNode = glm::mat4(1.0f);
    if (node.find("matrix") != node.end())
    {
        float matValues[16];
        for (unsigned int i = 0; i < node["matrix"].size(); i++)
            matValues[i] = (node["matrix"][i]);
        matNode = glm::make_mat4(matValues);
    }

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

    // Check if the node contains a mesh
    if (node.find("mesh") != node.end())
    {
        unsigned int meshIndex = node["mesh"];

        translationsMeshes.push_back(translation);
        rotationsMeshes.push_back(rotation);
        scalesMeshes.push_back(scale);
        matricesMeshes.push_back(matNextNode);

        loadMesh(meshIndex);
    }

    // Check if the node has children
    if (node.find("children") != node.end())
    {
        for (unsigned int i = 0; i < node["children"].size(); i++)
            traverseNode(node["children"][i], matNextNode);
    }
}

std::vector<unsigned char> Model::getData()
{
    std::string bytesText;
    std::string uri = JSON["buffers"][0]["uri"];

    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
    bytesText = get_file_contents((fileDirectory + uri).c_str());

    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
    return data;
}

std::vector<float> Model::getFloats(json accessor)
{
    std::vector<float> floatVec;

    // Check if bufferView exists
    if (accessor.find("bufferView") == accessor.end()) {
        std::cerr << "Error: No bufferView in accessor" << std::endl;
        return floatVec;
    }

    unsigned int buffViewInd = accessor["bufferView"];
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    // Check bufferView bounds
    if (buffViewInd >= JSON["bufferViews"].size()) {
        std::cerr << "Error: BufferView index out of bounds" << std::endl;
        return floatVec;
    }

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView.value("byteOffset", 0);

    unsigned int numPerVert;
    if (type == "SCALAR") numPerVert = 1;
    else if (type == "VEC2") numPerVert = 2;
    else if (type == "VEC3") numPerVert = 3;
    else if (type == "VEC4") numPerVert = 4;
    else {
        std::cerr << "Error: Invalid type: " << type << std::endl;
        return floatVec;
    }

    unsigned int beginningOfData = byteOffset + accByteOffset;
    unsigned int lengthOfData = count * 4 * numPerVert;

    // Bounds check
    if (beginningOfData + lengthOfData > data.size()) {
        std::cerr << "Error: Data read would exceed buffer bounds" << std::endl;
        return floatVec;
    }

    for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
    {
        unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }

    return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
    std::vector<GLuint> indices;

    unsigned int buffViewInd = accessor.value("bufferView", 0);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];

    if (buffViewInd >= JSON["bufferViews"].size()) {
        std::cerr << "Error: BufferView index out of bounds" << std::endl;
        return indices;
    }

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView.value("byteOffset", 0);
    unsigned int beginningOfData = byteOffset + accByteOffset;

    if (componentType == 5125)
    {
        if (beginningOfData + count * 4 > data.size()) {
            std::cerr << "Error: Indices read would exceed buffer" << std::endl;
            return indices;
        }

        for (unsigned int i = beginningOfData; i < beginningOfData + count * 4; i += 4)
        {
            unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
            unsigned int value;
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back((GLuint)value);
        }
    }
    else if (componentType == 5123)
    {
        if (beginningOfData + count * 2 > data.size()) {
            std::cerr << "Error: Indices read would exceed buffer" << std::endl;
            return indices;
        }

        for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i += 2)
        {
            unsigned char bytes[] = { data[i], data[i + 1] };
            unsigned short value;
            std::memcpy(&value, bytes, sizeof(unsigned short));
            indices.push_back((GLuint)value);
        }
    }
    else if (componentType == 5122)
    {
        if (beginningOfData + count * 2 > data.size()) {
            std::cerr << "Error: Indices read would exceed buffer" << std::endl;
            return indices;
        }

        for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i += 2)
        {
            unsigned char bytes[] = { data[i], data[i + 1] };
            short value;
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back((GLuint)value);
        }
    }

    return indices;
}

std::vector<Texture> Model::getTextures()
{
    std::vector<Texture> textures;

    // Check if images exist
    if (JSON.find("images") == JSON.end() || JSON["images"].empty()) {
        std::cout << "No images/textures in model" << std::endl;
        return textures;
    }

    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

    std::cout << "Loading textures from directory: " << fileDirectory << std::endl;
    std::cout << "Number of images in GLTF: " << JSON["images"].size() << std::endl;

    for (unsigned int i = 0; i < JSON["images"].size(); i++)
    {
        std::string texPath = JSON["images"][i]["uri"];
        std::cout << "Processing texture " << i << ": " << texPath << std::endl;

        bool skip = false;
        for (unsigned int j = 0; j < loadedTexName.size(); j++)
        {
            if (loadedTexName[j] == texPath)
            {
                std::cout << "  Texture already loaded, reusing" << std::endl;
                textures.push_back(loadedTex[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            std::string fullPath = fileDirectory + texPath;
            std::cout << "  Full path: " << fullPath << std::endl;

            // Check if file exists
            std::ifstream testFile(fullPath);
            if (!testFile.good()) {
                std::cerr << "  WARNING: Texture file not found: " << fullPath << std::endl;
                continue; // Skip this texture instead of crashing
            }
            testFile.close();

            try {
                if (texPath.find("baseColor") != std::string::npos)
                {
                    std::cout << "  Loading as diffuse texture" << std::endl;
                    Texture diffuse = Texture(fullPath.c_str(), "diffuse", loadedTex.size());
                    textures.push_back(diffuse);
                    loadedTex.push_back(diffuse);
                    loadedTexName.push_back(texPath);
                    std::cout << "  Diffuse texture loaded successfully" << std::endl;
                }
                else if (texPath.find("metallicRoughness") != std::string::npos)
                {
                    std::cout << "  Loading as specular texture" << std::endl;
                    Texture specular = Texture(fullPath.c_str(), "specular", loadedTex.size());
                    textures.push_back(specular);
                    loadedTex.push_back(specular);
                    loadedTexName.push_back(texPath);
                    std::cout << "  Specular texture loaded successfully" << std::endl;
                }
                else {
                    std::cout << "  Texture type not recognized (not baseColor or metallicRoughness), skipping" << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "  ERROR loading texture: " << e.what() << std::endl;
            }
        }
    }

    std::cout << "Total textures loaded for this mesh: " << textures.size() << std::endl;
    return textures;
}

std::vector<Vertex> Model::assembleVertices
(
    std::vector<glm::vec3> positions,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> texUVs
)
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < positions.size(); i++)
    {
        vertices.push_back
        (
            Vertex
            {
                positions[i],
                normals[i],
                glm::vec3(1.0f, 1.0f, 1.0f),
                texUVs[i]
            }
        );
    }
    return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
    const unsigned int floatsPerVector = 2;
    std::vector<glm::vec2> vectors;

    for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
    {
        vectors.push_back(glm::vec2(0, 0));
        for (unsigned int j = 0; j < floatsPerVector; j++)
        {
            vectors.back()[j] = floatVec[i + j];
        }
    }
    return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
    const unsigned int floatsPerVector = 3;
    std::vector<glm::vec3> vectors;

    for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
    {
        vectors.push_back(glm::vec3(0, 0, 0));
        for (unsigned int j = 0; j < floatsPerVector; j++)
        {
            vectors.back()[j] = floatVec[i + j];
        }
    }
    return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
    const unsigned int floatsPerVector = 4;
    std::vector<glm::vec4> vectors;

    for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
    {
        vectors.push_back(glm::vec4(0, 0, 0, 0));
        for (unsigned int j = 0; j < floatsPerVector; j++)
        {
            vectors.back()[j] = floatVec[i + j];
        }
    }
    return vectors;
}