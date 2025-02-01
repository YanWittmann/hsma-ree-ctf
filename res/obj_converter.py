import argparse
import os


def parse_obj_file(obj_file_path):
    vertices = []
    faces = []

    with open(obj_file_path, 'r') as file:
        for line in file:
            line = line.strip()

            # vertices (v x y z)
            if line.startswith('v '):
                parts = line.split()
                x, y, z = map(float, parts[1:])
                vertices.extend([x, y, z])

            # faces (f v1 v2 v3 ...)
            elif line.startswith('f '):
                parts = line.split()
                face_data = []
                for part in parts:
                    if part == 'f':
                        continue
                    # 1/2/3
                    if '/' in part:
                        face_data.append(int(part.split('/')[0]) - 1)
                    else:
                        print("Did not find / in face part", part, "in line", line)
                faces.append(face_data)

    print("vertices", len(vertices))
    print("faces", len(faces))

    for face in faces:
        if len(face) > 3:
            print("")
            print("Error: found face with more than 3 vertices, this is not supported by the renderer.")
            print("In Blender, use the triangulate modifier or operation to convert all faces to triangles.")
            print("Exiting...")
            exit(1)

    return vertices, faces


def write_c_code(vertices, faces, output_file):
    with open(output_file, 'w') as file:
        file.write("#ifndef RENDERER_MODEL_DATA\n")
        file.write("#define RENDERER_MODEL_DATA\n\n")
        file.write("#include \"../renderer_types.h\"\n\n")
        file.write("static const int num_vertices = {}; \n".format(len(vertices) // 3))
        file.write("static const int num_faces = {}; \n\n".format(len(faces)))

        file.write("static const Vertex vertices[] = {\n")
        for i in range(0, len(vertices), 3):
            file.write(f"\t{{ {vertices[i]}f, {vertices[i + 1]}f, {vertices[i + 2]}f }},\n")
        file.write("};\n\n")

        file.write("static const Face faces[] = {\n")
        for face in faces:
            file.write("\t{")
            for idx in face:
                file.write(f"{idx}, ")
            file.write("},\n")
        file.write("};\n\n")

        file.write("Model reference_model = {(Vertex *) vertices, (Face *) faces, num_vertices, num_faces};\n\n")

        file.write("#endif // RENDERER_MODEL_DATA\n")


def obj_to_c(obj_file_path, output_file):
    print(f"Converting {obj_file_path}...")
    vertices, faces = parse_obj_file(obj_file_path)
    write_c_code(vertices, faces, output_file)
    print(f"Conversion complete. C code written to {output_file}")


def write_header_file(output_file):
    with open(output_file, 'w') as file:
        file.write("#ifndef RENDERER_MODEL_DATA_H\n")
        file.write("#define RENDERER_MODEL_DATA_H\n\n")
        file.write("#include \"../renderer_types.h\"\n\n")
        file.write("extern Model reference_model;\n\n")
        file.write("#endif // RENDERER_MODEL_DATA_H\n")
    print(f"Header file written to {output_file}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert an obj file to C code')
    parser.add_argument('obj_file', type=str, help='Path to the obj file')
    parser.add_argument('output_file', type=str, help='Path to the output C file, without extension')
    args = parser.parse_args()

    obj_file_path = args.obj_file
    output_file = args.output_file

    # make target directory
    target_dir = os.path.dirname(output_file)
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)

    obj_to_c(obj_file_path, output_file + ".c")
    write_header_file(output_file + ".h")
