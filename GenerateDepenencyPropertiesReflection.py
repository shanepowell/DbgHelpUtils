import re
import os

def process_header_file(header_file, name, objectType, headers):
    # Read the C++ header file
    with open(header_file, 'r') as file:
        header_content = file.read()

    # Define a regular expression pattern to extract class definitions and property functions
    class_pattern = r'struct WINRT_IMPL_EMPTY_BASES (.*) : winrt::(.*)::I.*$(\s*impl::base<.*>,$\s*impl::require<.*>$)?\s*{([\w\W]*?)$\s*};$'

    if objectType == "ObjectType::Application":
        property_pattern = r'\[\[nodiscard\]\]\s*static winrt::Microsoft::UI::Xaml::DependencyProperty (\w+)Property\(\);'
    else:
        property_pattern = r'\[\[nodiscard\]\]\s*static auto (\w+)Property\(\);'

    # Find all class definitions in the header content
    print("Finding classes: ");
    class_matches = re.findall(class_pattern, header_content, re.MULTILINE)

    output = "";

    # Process each class match
    for class_name, namespace, impls, class_body in class_matches:
        property_matches = re.findall(property_pattern, class_body)
        print(f"Generating Class: {namespace}::{class_name} for Properties: {property_matches}");
        if property_matches:
            # filter out winrt::Windows::UI::Xaml::DependencyProperty properties, we can't do it here as the return tye is 'auto' so we don't know what the actual return type is 
            # in the winrt generated files, so leave it to the c++ compiler to sort out
            output +=     f"  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::{namespace}::{class_name}::{property_matches[0]}Property())>)\n"
            output +=      "  {\n"


            output +=     f"    result.insert(std::make_pair(winrt::guid_of<winrt::{namespace}::{class_name}>(),\n"
            output +=      "      ObjectData {\n"
            output +=     f"        {objectType}, \n"
            output +=      "        {\n"

            for property_name in property_matches:
                output += f"          {{ L\"{property_name.lower()}\"s, []() {{ return winrt::{namespace}::{class_name}::{property_name}Property(); }} }},\n"

            output +=      "        }\n"
            output +=      "      }\n"
            output +=      "    ));\n"
            output +=      "  }\n"

    if len(output) > 0:
        output = f"  // {name}\n" + output
        headers.add(name)

    return output

# Create the output string

headers = set()

output  = "enum class ObjectType\n{\n  Library,\n  Application\n};\n\nstruct ObjectData\n{\n  ObjectType type;\n  std::map<std::wstring, std::function<winrt::Microsoft::UI::Xaml::DependencyProperty()>> properties;\n};\n\nconst std::map<winrt::guid, ObjectData> InterfaceIdToDependencyProperties = []()\n{\n  std::map<winrt::guid, ObjectData> result;\n"

baseDirectory = "MiniDumpExplorer\\Generated Files\\winrt\\impl\\"
for root, dirs, files in os.walk(baseDirectory):
    for name in files:
        if name.endswith(".2.h"):
            file_path = os.path.join(root, name)
            print(F"Processing header file: {file_path}")
            if file_path.endswith("MiniDumpExplorer.2.h"):
                objectType = "ObjectType::Application"
            else:
                objectType = "ObjectType::Library"
            output += process_header_file(file_path, file_path[len(baseDirectory):].replace(".2.h", ".h"), objectType, headers)

output += "  return result;\n"
output += "}();\n"

header_output = "";

for header in sorted(headers):
  header_output += f"#include \"winrt/{header}\"\n"

print(f'\n\n\n  Generating Ouput:\n************************************\n{header_output}************************************\n{output}************************************\n')

# Write the output to a file
with open('MiniDumpExplorer/Helpers/WinrtDepenencyPropertiesReflectionMap.h', 'w') as file:
    file.write(output)

# Write the output to a file
with open('MiniDumpExplorer/Helpers/WinrtDepenencyPropertiesReflectionMapHeaders.h', 'w') as file:
    file.write(header_output)
