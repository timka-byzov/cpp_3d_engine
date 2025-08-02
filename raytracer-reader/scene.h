#pragma once

#include <material.h>
#include <vector.h>
#include <object.h>
#include <light.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include <fstream>
#include <obj_reader.h>
#include <type_reader.h>
#include <optional>
std::unordered_map<std::string, Material> ReadMaterials(const std::filesystem::path&);

struct ObjPoint {
    int v_idx;
    std::optional<int> vn_idx;
};

struct ObjectMeta {
    std::string material_name;
    std::vector<ObjPoint> points;
};

struct SphereObjectMeta {
    std::string material_name;
    Sphere sphere;
};

struct LightObjectMeta {
    Light light;
};

int GetIndex(int idx, int size) {
    if (idx < 0) {
        return size + idx;
    }
    return idx - 1;
}

std::vector<Object> CreateObjects(std::vector<Vector>& vertices, std::vector<Vector>& normals,
                                  std::unordered_map<std::string, Material>& materials,
                                  std::vector<ObjectMeta>& objs) {

    std::vector<Object> objects;

    for (auto& obj_meta : objs) {
        for (size_t i = 1; i < obj_meta.points.size() - 1; ++i) {
            std::vector<Vector> polygon_vertices;
            std::vector<std::optional<Vector>> polygon_normals;

            std::vector<ObjPoint> points = {obj_meta.points[0], obj_meta.points[i],
                                            obj_meta.points[i + 1]};

            for (size_t j = 0; j < 3; ++j) {
                polygon_vertices.push_back(vertices[points[j].v_idx]);
                if (points[j].vn_idx) {
                    polygon_normals.push_back(normals[points[j].vn_idx.value()]);
                } else {
                    polygon_normals.push_back(std::nullopt);
                }
            }
            auto polygon = Triangle{polygon_vertices[0], polygon_vertices[1], polygon_vertices[2]};
            objects.emplace_back(polygon, &materials.at(obj_meta.material_name), polygon_normals);
        }
    }

    return objects;
}

std::vector<SphereObject> CreateSphereObjects(
    std::vector<SphereObjectMeta>& sphere_objects,
    std::unordered_map<std::string, Material>& materials) {
    std::vector<SphereObject> objects;

    for (auto& obj_meta : sphere_objects) {
        objects.emplace_back(&materials.at(obj_meta.material_name), obj_meta.sphere);
    }

    return objects;
}

std::vector<Light> CreateLights(std::vector<LightObjectMeta>& light_objects) {
    std::vector<Light> lights;

    for (auto& obj_meta : light_objects) {
        lights.push_back(obj_meta.light);
    }

    return lights;
}

class Scene {
public:
    Scene() = default;

    const std::vector<Object>& GetObjects() const {
        return objects_;
    }

    const std::vector<SphereObject>& GetSphereObjects() const {
        return sphere_objects_;
    }

    const std::vector<Light>& GetLights() const {
        return lights_;
    }

    const std::unordered_map<std::string, Material>& GetMaterials() const {
        return materials_;
    }

    void ReadMaterials(const std::filesystem::path& path) {
        materials_ = ::ReadMaterials(path);
    }

    void Create(std::vector<Vector>& vertices, std::vector<Vector>& normals,
                std::vector<ObjectMeta>& objs, std::vector<SphereObjectMeta>& sphere_objects,
                std::vector<LightObjectMeta>& lights) {

        objects_ = CreateObjects(vertices, normals, materials_, objs);
        sphere_objects_ = CreateSphereObjects(sphere_objects, materials_);
        lights_ = CreateLights(lights);
    }

private:
    std::vector<Object> objects_;
    std::vector<SphereObject> sphere_objects_;
    std::vector<Light> lights_;
    std::unordered_map<std::string, Material> materials_;
};

std::unordered_map<std::string, Material> ReadMaterials(const std::filesystem::path& path) {
    std::unordered_map<std::string, Material> materials;

    Material material;

    std::ifstream is{path};

    while (!is.eof()) {
        std::string line;
        std::getline(is, line);

        std::istringstream iss{line};
        std::string type;

        iss >> type;
        if (type == "newmtl") {

            if (!material.name.empty()) {
                materials[material.name] = material;
            }
            material = Material();

            iss >> material.name;

        } else if (type == "Ka") {
            material.ambient_color = ReadVector(iss);

        } else if (type == "Kd") {
            material.diffuse_color = ReadVector(iss);

        } else if (type == "Ks") {
            material.specular_color = ReadVector(iss);

        } else if (type == "Ke") {
            material.intensity = ReadVector(iss);

        } else if (type == "Ns") {
            material.specular_exponent = ReadDouble(iss);

        } else if (type == "Ni") {
            material.refraction_index = ReadDouble(iss);

        } else if (type == "al") {
            material.albedo = ReadVector(iss);

        } else {
            continue;
        }
    }
    is.close();

    if (!material.name.empty()) {
        materials[material.name] = material;
    }

    return materials;
}

auto ReadObjFile(const std::filesystem::path& path) {
    std::string curr_material_name;
    std::string material_file_name;

    std::vector<Vector> vertices;
    std::vector<Vector> normals;

    std::vector<ObjectMeta> objs;
    std::vector<SphereObjectMeta> sphere_objects;
    std::vector<LightObjectMeta> lights;

    std::ifstream is{path};
    while (!is.eof()) {

        std::string line;
        std::getline(is, line);

        std::istringstream iss{line};
        std::string type;

        iss >> type;
        if (type == "mtllib") {
            material_file_name = ReadString(iss);

        } else if (type == "v") {
            vertices.push_back(ReadVector(iss));

        } else if (type == "vn") {
            normals.push_back(ReadVector(iss));

        } else if (type == "f") {
            auto tuples = ReadF(iss);

            ObjectMeta om;
            om.material_name = curr_material_name;
            for (auto& [v_idx, _, vn_idx] : tuples) {

                if (vn_idx) {
                    om.points.emplace_back(GetIndex(v_idx.value(), vertices.size()),
                                           GetIndex(vn_idx.value(), normals.size()));
                } else {
                    om.points.emplace_back(GetIndex(v_idx.value(), vertices.size()), std::nullopt);
                }
            }
            objs.push_back(om);

        } else if (type == "usemtl") {
            curr_material_name = ReadString(iss);

        } else if (type == "S") {
            auto sphere = ReadSphere(iss);
            sphere_objects.push_back({curr_material_name, sphere});

        } else if (type == "P") {
            auto light = ReadLight(iss);
            lights.push_back({light});

        } else {
            continue;
        }
    }

    is.close();
    return std::make_tuple(vertices, normals, objs, sphere_objects, lights, material_file_name);
}

Scene ReadScene(const std::filesystem::path& path) {

    auto [vertices, normals, objs, sphere_objects, lights, material_file_name] = ReadObjFile(path);

    Scene scene;
    scene.ReadMaterials(path.parent_path() / material_file_name);
    scene.Create(vertices, normals, objs, sphere_objects, lights);

    return scene;
}