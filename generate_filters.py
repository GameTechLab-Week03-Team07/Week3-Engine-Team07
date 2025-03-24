import os
import xml.etree.ElementTree as ET
import uuid

ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

project_root = os.path.abspath(".")
project_name = "JungleEngine"  # 프로젝트 이름 입력
vcxproj_file = os.path.join(project_root, f"{project_name}.vcxproj")
filters_file = os.path.join(project_root, f"{project_name}.vcxproj.filters")

vcxproj_tree = ET.parse(vcxproj_file)
vcxproj_root = vcxproj_tree.getroot()

existing_files = set()
for item_group in vcxproj_root.findall("{http://schemas.microsoft.com/developer/msbuild/2003}ItemGroup"):
    for cl in item_group:
        existing_files.add(cl.get("Include"))

itemgroup_compile = ET.Element("ItemGroup")
itemgroup_include = ET.Element("ItemGroup")

filters_root = ET.Element("Project")
filters_root.set("ToolsVersion", "4.0")
filters_root.set("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003")

filter_group = ET.SubElement(filters_root, "ItemGroup")
compile_filter_group = ET.SubElement(filters_root, "ItemGroup")
include_filter_group = ET.SubElement(filters_root, "ItemGroup")

def new_guid():
    return "{" + str(uuid.uuid4()).upper() + "}"

def add_filter_for_path(filter_name):
    filter_elem = ET.SubElement(filter_group, "Filter")
    filter_elem.set("Include", filter_name)
    guid_elem = ET.SubElement(filter_elem, "UniqueIdentifier")
    guid_elem.text = new_guid()

def scan_and_add_entries(base_path, current_path=""):
    current_filter = os.path.join("Source", current_path).replace("/", "\\") if current_path else "Source"
    if current_filter != "Source":
        add_filter_for_path(current_filter)

    for entry in os.listdir(os.path.join(base_path, current_path)):
        full_path = os.path.join(base_path, current_path, entry)
        rel_path = os.path.join(current_path, entry).replace("\\", "/")

        if os.path.isdir(full_path):
            scan_and_add_entries(base_path, rel_path)

        elif entry.endswith(".cpp") or entry.endswith(".c"):
            file_path = os.path.join("Source", rel_path).replace("/", "\\")
            if file_path not in existing_files:
                clcompile_elem = ET.SubElement(itemgroup_compile, "ClCompile")
                clcompile_elem.set("Include", file_path)

            compile_elem = ET.SubElement(compile_filter_group, "ClCompile")
            compile_elem.set("Include", file_path)
            filter_ref = ET.SubElement(compile_elem, "Filter")
            filter_ref.text = current_filter

        elif entry.endswith(".h") or entry.endswith(".hpp"):
            file_path = os.path.join("Source", rel_path).replace("/", "\\")
            if file_path not in existing_files:
                clinclude_elem = ET.SubElement(itemgroup_include, "ClInclude")
                clinclude_elem.set("Include", file_path)

            include_elem = ET.SubElement(include_filter_group, "ClInclude")
            include_elem.set("Include", file_path)
            filter_ref = ET.SubElement(include_elem, "Filter")
            filter_ref.text = current_filter

# Source 루트 필터도 추가
add_filter_for_path("Source")

# 실행
scan_and_add_entries(os.path.join(project_root, "Source"))

# vcxproj 저장
vcxproj_root.append(itemgroup_compile)
vcxproj_root.append(itemgroup_include)
ET.indent(vcxproj_root, space="  ")
vcxproj_tree.write(vcxproj_file, encoding="utf-8", xml_declaration=True)

# filters 저장
ET.indent(filters_root, space="  ")
with open(filters_file, "wb") as f:
    f.write(ET.tostring(filters_root, encoding="utf-8", xml_declaration=True))

print(f"현재 폴더 구조를 기반으로 Source부터 시작하는 vcxproj 및 필터 파일 자동 생성 완료!")
